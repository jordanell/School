/* MyAlloc.c */

/*
   All memory allocation and deallocation is performed in this module.
   
   There are two families of functions, one just for managing the Java heap
   and a second for other memory requests.  This second family of functions
   provides wrappers for standard C library functions but checks that memory
   is not exhausted and zeroes out any returned memory.
   
   Java Heap Management Functions:
   * InitMyAlloc  -- initializes the Java heap before execution starts
   * MyHeapAlloc  -- returns a block of memory from the Java heap
   * gc           -- the System.gc garbage collector
   * MyHeapFree   -- to be called only by gc()!!
   * PrintHeapUsageStatistics  -- does as the name suggests

   General Storage Functions:
   * SafeMalloc  -- used like malloc
   * SafeCalloc  -- used like calloc
   * SafeStrdup  -- used like strdup
   * SafeFree    -- used like free
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "ClassFileFormat.h"
#include "TraceOptions.h"
#include "MyAlloc.h"
#include "jvm.h"
#include "VerifyPointer.h"

typedef struct FreeStorageBlock {
    uint32_t size;  /* size in bytes of this block of storage */
    int32_t  offsetToNextBlock;
    uint8_t  restOfBlock[1];   /* the actual size has to be determined from the size field */
} FreeStorageBlock;

/* these three variables are externally visible */
uint8_t *HeapStart, *HeapEnd;
HeapPointer MaxHeapPtr;

static int offsetToFirstBlock = -1;
static long totalBytesRequested = 0;
static int numAllocations = 0;
static int gcCount = 0;
static long totalBytesRecovered = 0;
static int totalBlocksRecovered = 0;
static int searchCount = 0;

static void *maxAddr = NULL;    // used by SafeMalloc, etc
static void *minAddr = NULL;


/* Allocate the Java heap and initialize the free list */
void InitMyAlloc( int HeapSize ) {
    FreeStorageBlock *FreeBlock;

    HeapSize &= 0xfffffffc;   /* force to a multiple of 4 */
    HeapStart = calloc(1,HeapSize);
    if (HeapStart == NULL) {
        fprintf(stderr, "unable to allocate %d bytes for heap\n", HeapSize);
        exit(1);
    }
    HeapEnd = HeapStart + HeapSize;
    MaxHeapPtr = (HeapPointer)HeapSize;
    
    FreeBlock = (FreeStorageBlock*)HeapStart;
    FreeBlock->size = HeapSize;
    FreeBlock->offsetToNextBlock = -1;  /* marks end of list */
    offsetToFirstBlock = 0;
    
    // Used by SafeMalloc, SafeCalloc, SafeFree below
    maxAddr = minAddr = malloc(4);  // minimal small request to get things started
}

void Mark(void *p);
void Sweep();

/* Returns a pointer to a block with at least size bytes available,
   and initialized to hold zeros.
   Notes:
   1. The result will always be a word-aligned address.
   2. The word of memory preceding the result address holds the
      size in bytes of the block of storage returned (including
      this size field).
   3. A block larger than that requested may be returned if the
      leftover portion would be too small to be useful.
   4. The size of the returned block is always a multiple of 4.
   5. The implementation of MyAlloc contains redundant tests to
      verify that the free list blocks contain plausible info.
*/
void *MyHeapAlloc( int size ) {
    /* we need size bytes plus more for the size field that precedes
       the block in memory, and we round up to a multiple of 4 */
    int offset, diff, blocksize;
    FreeStorageBlock *blockPtr, *prevBlockPtr, *newBlockPtr;
    int minSizeNeeded = (size + sizeof(blockPtr->size) + 3) & 0xfffffffc;

    if (tracingExecution & TRACE_HEAP)
        fprintf(stdout, "* heap allocation request of size %d (augmented to %d)\n",
            size, minSizeNeeded);
    blockPtr = prevBlockPtr = NULL;
    offset = offsetToFirstBlock;
    while(offset >= 0) {
        searchCount++;
        blockPtr = (FreeStorageBlock*)(HeapStart + offset);
        /* the following check should be quite unnecessary, but is
           a good idea to have while debugging */
        if ((offset&3) != 0 || (uint8_t*)blockPtr >= HeapEnd) {
            fprintf(stderr,
                "corrupted block in the free list -- bad next offset pointer\n");
            exit(1);
        }
        blocksize = blockPtr->size;
        /* the following check should be quite unnecessary, but is
           a good idea to have while debugging */
        if (blocksize < MINBLOCKSIZE || (blocksize&3) != 0) {
            fprintf(stderr,
                "corrupted block in the free list -- bad size field\n");
            exit(1);
        }
        diff = blocksize - minSizeNeeded;
        if (diff >= 0) break;
        offset = blockPtr->offsetToNextBlock;
        prevBlockPtr = blockPtr;
    }
    if (offset < 0) {
        static int gcAlreadyPerformed = 0;
        void *result;
        if (gcAlreadyPerformed) {
            /* we are in a recursive call to MyAlloc after a gc */
            fprintf(stderr,
                "\nHeap exhausted! Unable to allocate %d bytes\n", size);
            exit(1);
        }
        gc();
        gcAlreadyPerformed = 1;
        result = MyHeapAlloc(size);
        /* control never returns from the preceding call if the gc
           did not obtain enough storage */
        gcAlreadyPerformed = 0;
        return result;
    }
    /* we have a sufficiently large block of free storage, now determine
       if we will have a significant amount of storage left over after
       taking what we need */
    if (diff < MINBLOCKSIZE) {
        /* we will return the entire free block that we found, so
           remove the block from the free list  */
        if (prevBlockPtr == NULL)
            offsetToFirstBlock = blockPtr->offsetToNextBlock;
        else
            prevBlockPtr->offsetToNextBlock = blockPtr->offsetToNextBlock;
        if (tracingExecution & TRACE_HEAP)
            fprintf(stdout, "* free list block of size %d used\n", blocksize);
    } else {
        /* we split the free block that we found into two pieces;
           blockPtr refers to the piece we will return;
           newBlockPtr will refer to the remaining piece */
        blockPtr->size = minSizeNeeded;
        newBlockPtr = (FreeStorageBlock*)((uint8_t*)blockPtr + minSizeNeeded);
        /* replace the block in the free list with the leftover piece */
        if (prevBlockPtr == NULL)
            offsetToFirstBlock += minSizeNeeded;
        else
            prevBlockPtr->offsetToNextBlock += minSizeNeeded;
        newBlockPtr->size = diff;
        newBlockPtr->offsetToNextBlock = blockPtr->offsetToNextBlock;
        if (tracingExecution & TRACE_HEAP)
            fprintf(stdout, "* free list block of size %d split into %d + %d\n",
                diff+minSizeNeeded, minSizeNeeded, diff);

    }
    blockPtr->offsetToNextBlock = 0;  /* remove this info from the returned block */
    totalBytesRequested += minSizeNeeded;
    numAllocations++;
    return (uint8_t*)blockPtr + sizeof(blockPtr->size);
}


/* When garbage collection is implemented, this function should never
   be called from outside the current file.
   This implementation checks that p is plausible and that the block of
   memory referenced by p holds a plausible size field.
*/
static void MyHeapFree(void *p) {
    uint8_t *p1 = (uint8_t*)p;
    int blockSize;
    FreeStorageBlock *blockPtr;

    if (p1 < HeapStart || p1 >= HeapEnd || ((p1-HeapStart) & 3) != 0) {
        fprintf(stderr, "bad call to MyHeapFree -- bad pointer\n");
        exit(1);
    }
    /* step back over the size field */
    p1 -= sizeof(blockPtr->size);
    /* now check the size field for validity */
    blockSize = *(uint32_t*)p1;
    if (blockSize < MINBLOCKSIZE || (p1 + blockSize) >= HeapEnd || (blockSize & 3) != 0) {
        fprintf(stderr, "bad call to MyHeapFree -- invalid block\n");
        exit(1);
    }
    /* link the block into the free list at the front */
    blockPtr = (FreeStorageBlock*)p1;
    blockPtr->offsetToNextBlock = offsetToFirstBlock;
    offsetToFirstBlock = p1 - HeapStart;
}


/* This implements garbage collection.
   It should be called when
   (a) MyAlloc cannot satisfy a request for a block of memory, or
   (b) when invoked by the call System.gc() in the Java program.
*/
void gc() {
    fprintf(stdout, "Garbage collecting.\n");
    gcCount++;
    DataItem* JVMBasePtr = JVM_Stack;
    DataItem* CurrentPtr;
    while(JVMBasePtr != NULL && JVMBasePtr <= JVM_Top) {
        CurrentPtr = JVMBasePtr++;
        void* Ptr = VerifyPointer(REAL_HEAP_POINTER(CurrentPtr->pval));
        
        // Mark if needed
        if (Ptr != NULL) {
            Mark(Ptr);
        }
    }
    Sweep();
    PrintHeapUsageStatistics();
    
    // Leave this in for now
    // The following lines of code exist only so that the compiler does not
    // generate a warning message that MyHeapFree is defined but not used.
    if (0) {
    	MyHeapFree(NULL);
    }
}

uint8_t isMarked(void *p);

void Mark(void *p) {  
    // Return if the pointer is marked already
    if(isMarked(p))
        return;
    
    uint8_t *p1 = (uint8_t*)p;
    FreeStorageBlock *blockPtr;
    uint8_t* currentPtr = (uint8_t*)VerifyPointer((uint8_t*)p1);
    
    // Return if it's not a pointer
    if (currentPtr == NULL) return;
    
    // Step back over the size field
    p1 -= sizeof(blockPtr->size);
    uint32_t size = *(p1);
    *(uint32_t*)p1 |= 1 << MARKBIT;

    // reset to the memory in the block
    p1 += sizeof(blockPtr->size);
    u1 numElements;
    int i;
    ArrayOfRef* ARRAPtr;
    StringInstance* STRGPtr;
    StringBuilderInstance* SBLDPtr;
    ClassType* CLASPtr;
    ClassInstance* INSTPtr;
    if (currentPtr != NULL) {
        switch((*(uint32_t*)currentPtr)) {
            case (CODE_ARRA):
                // ARRA case
                ARRAPtr = (ArrayOfRef*)currentPtr;
                assert(ARRAPtr->kind == CODE_ARRA);
                                      
                for (i = 0;i < ARRAPtr->size;i++) {
                    Mark(&ARRAPtr->elements[i]);
                }
                Mark(&ARRAPtr->classRef);
                break;
            case (CODE_CLAS):
                // CLASSTYPE CASE
                CLASPtr = (ClassType*)currentPtr;
                assert(CLASPtr->kind == CODE_CLAS);
                               
                Mark(CLASPtr->typeDescriptor);
                Mark(CLASPtr->nextClass);
                
                if(CLASPtr->isArrayType) {
                    Mark(CLASPtr->elementType);
                }
                else {
                    // Do we need to do something about CLASPtr->cf ?
                    Mark(CLASPtr->parent);
                    numElements = (size - ((u1*)&CLASPtr->classField - (u1*)p));
                    for (i = 0;i < numElements;i++){
                        Mark(&CLASPtr->classField[i]);
                    }
                }
                break;
            case(CODE_INST):
                // INSTANCE CASE
                INSTPtr = (ClassInstance*)currentPtr;
                assert(INSTPtr->kind == CODE_INST);
                
                numElements = (sizeof(INSTPtr->instField) / sizeof(DataItem));
                
                for (i = 0;i < numElements;i++) {
                    Mark(&INSTPtr->instField[i]);
                }
                Mark(&INSTPtr->thisClass);
                break;
            case(CODE_STRG):
                // STRING CASE
                STRGPtr = (StringInstance*)currentPtr;
                assert(STRGPtr->kind == CODE_STRG);
                
                Mark(STRGPtr->sval);
                break;
            case(CODE_SBLD):
                // STRINGBUILDER CASE
                SBLDPtr = (StringBuilderInstance*)currentPtr;
                assert(SBLDPtr->kind == CODE_SBLD);
                
                Mark(SBLDPtr->buffer);
                break;
            default:
                // whatever else
                break;
        }
    }
    
}

void unMark(void *p) {
    uint8_t *p1 = (uint8_t*)p;
    FreeStorageBlock *blockPtr;
    
    // Step back over the size field
    p1 -= sizeof(blockPtr->size);
    *(uint32_t*)p1 &= ~(1 << MARKBIT);
    p1 += sizeof(blockPtr->size);
}

uint8_t isMarked(void *p) {
    uint8_t *p1 = (uint8_t*)p;
    FreeStorageBlock *blockPtr;
    
    // Step back over the size field
    p1 -= sizeof(blockPtr->size);
    return !!( *(uint32_t*)p1 & (1 << MARKBIT));
}

void clearMemory(uint8_t *blockStart, uint8_t *blockEnd) {
    uint8_t *clearer = blockStart;
    while(clearer < blockEnd) {
        *clearer = 0;
        clearer++;
    }
}

void addFreeBlock(uint8_t *blockStart, uint8_t *blockEnd) {
    FreeStorageBlock *endBlock, *FreeBlock;
    uint32_t diff = blockEnd - blockStart;
    
    if(diff == 0)
        return;
    
    int offset = offsetToFirstBlock;
    
    while(offset >= 0) {
        endBlock = (FreeStorageBlock*)(HeapStart + offset);
        offset = endBlock->offsetToNextBlock;
    }
    // Clear the memory to avoid metric errors in recounting
    clearMemory(blockStart, blockEnd);
    
    FreeBlock = (FreeStorageBlock*)blockStart;
    FreeBlock->size = diff;
    FreeBlock->offsetToNextBlock = -1;  /* marks end of list */
    if(offsetToFirstBlock == -1)
        offsetToFirstBlock = blockStart - HeapStart;
    else
        endBlock->offsetToNextBlock = blockStart - HeapStart;
}

uint8_t isLiveObject(uint8_t *p) {
    uint8_t* ptr = VerifyPointer(p);
    return (((*(uint32_t*)p ^ CODE_ARRA) ==  0 || 
             (*(uint32_t*)p ^ CODE_ARRS) == 0 ||  
             (*(uint32_t*)p ^ CODE_INST) == 0 || 
             (*(uint32_t*)p ^ CODE_STRG) == 0 || 
             (*(uint32_t*)p ^ CODE_SBLD) == 0) && 
             isMarked(p) && 
            ptr != NULL) ||
            ((*(uint32_t*)p ^ CODE_CLAS) == 0 &&
             ptr != NULL);
}

uint8_t isDeadObject(uint8_t *p)  {
    uint8_t* ptr = VerifyPointer(p);
    return (((*(uint32_t*)p ^ CODE_ARRA) ==  0 || 
             (*(uint32_t*)p ^ CODE_ARRS) == 0 ||  
             (*(uint32_t*)p ^ CODE_INST) == 0 || 
             (*(uint32_t*)p ^ CODE_STRG) == 0 || 
             (*(uint32_t*)p ^ CODE_SBLD) == 0) && 
             !isMarked(p) && 
             ptr != NULL);
}

void Sweep() {
    FreeStorageBlock *blockPtr;
    uint8_t *p = HeapStart;
    uint8_t *fBlockStart = HeapStart;
    uint8_t *fBlockEnd = HeapStart;
    
    // Clear the free list
    offsetToFirstBlock = -1;
    
    while(p < HeapEnd) {
        // Metrics
        if(isDeadObject(p)) {
            totalBytesRecovered += *(uint32_t*)(p-sizeof(blockPtr->size));
            totalBlocksRecovered++;
        }
        // Check if we are at a marked object
        if(isLiveObject(p)) {
            fBlockEnd = (p-sizeof(blockPtr->size));
            // Add Free block to free list
            addFreeBlock(fBlockStart, fBlockEnd);
            // Move pointers up
            unMark(p);
            int size = *(uint32_t*)fBlockEnd;
            p += (size - 4);
            fBlockStart = p;
        }
        else
            p++;
    }
    
    // Add the last block found to the free list
    if(isDeadObject(fBlockStart+4)) {
        totalBytesRecovered += *(uint32_t*)fBlockStart;
        totalBlocksRecovered++;
    }
    addFreeBlock(fBlockStart, HeapEnd);
    
    int offset = offsetToFirstBlock;
    while (offset >= 0) {
        blockPtr = (FreeStorageBlock*)(HeapStart + offset);
        fprintf(stdout, "Free storage block at offset %u and size %u\n",
                offset, blockPtr->size);
        offset = blockPtr->offsetToNextBlock;
    }
}


/* Report on heap memory usage */
void PrintHeapUsageStatistics() {
    printf("\nHeap Usage Statistics\n=====================\n\n");
    printf("  Number of blocks allocated = %d\n", numAllocations);
    if (numAllocations > 0) {
        float avgBlockSize = (float)totalBytesRequested / numAllocations;
        float avgSearch = (float)searchCount / numAllocations;
        printf("  Average size of allocated blocks = %.2f\n", avgBlockSize);
        printf("  Average number of blocks checked = %.2f\n", avgSearch);
    }
    printf("  Number of garbage collections = %d\n", gcCount);
    if (gcCount > 0) {
        float avgRecovery = (float)totalBytesRecovered / gcCount;
        printf("  Total storage reclaimed = %ld\n", totalBytesRecovered);
        printf("  Total number of blocks reclaimed = %d\n", totalBlocksRecovered);
        printf("  Average bytes recovered per gc = %.2f\n", avgRecovery);
    }
}


static void *trackHeapArea( void *p ) {
    if (p > maxAddr)
        maxAddr = p;
    if (p < minAddr)
        minAddr = p;
    return p;
}


void *SafeMalloc( int size ) {
    return SafeCalloc(1,size);
}


void *SafeCalloc( int ncopies, int size ) {
    void *result;
    result = calloc(ncopies,size);
    if (result == NULL) {
        fprintf(stderr, "Fatal error: memory request cannot be satisfied\n");
        exit(1);
    }
    trackHeapArea(result);
    return result;    
}


char *SafeStrdup( char *s ) {
    char *r;
    int len;

    len = (s == NULL)? 0 : strlen(s);
    r = SafeMalloc(len+1);
    if (len > 0)
        strcpy(r,s);
    return r;
}

void SafeFree( void *p ) {
    if (p == NULL || ((int)p & 0x7) != 0) {
        fprintf(stderr, "Fatal error: invalid parameter passed to SafeFree\n");
        fprintf(stderr, "    The address was NULL or misaligned\n");
        abort();
    }
    if (p >= minAddr && p <= maxAddr)
        free(p);
    else {
        fprintf(stderr, "Fatal error: invalid parameter passed to SafeFree\n");
        fprintf(stderr, "     The memory was not allocated by SafeMalloc\n");
        abort();
    }
}