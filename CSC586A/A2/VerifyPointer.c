#include "VerifyPointer.h"
#include "ClassFileFormat.h"
#include "MyAlloc.h"
#include "jvm.h"
#include <stdint.h>
#include <stdio.h>

/**
 * Verifies a pointer value as an actual reference to the heap
 * Uses conservative garbage collection from
 * http://ow.ly/mNKoQ
 * @param ptr
 * @return 1 for real pointer, 0 for not
 */
void* VerifyPointer(uint8_t* RealHeapPtr) {
    // First check if it is Real pointer within the heap bounds.
    if (RealHeapPtr > HeapEnd || RealHeapPtr < HeapStart) {
        return 0;
    }
    
    // First check if the value is divisible by 4
    if (((uint32_t)RealHeapPtr & 3) != 0) {
        return 0;
    }
    
    RealHeapPtr -= 4;
    uint32_t blockSize = *RealHeapPtr;
    if (blockSize < MINBLOCKSIZE || (RealHeapPtr + blockSize) >= HeapEnd || (blockSize & 3) != 0) {
        RealHeapPtr += 4;
        return 0;
    }
    
    // Restore the ptr
    RealHeapPtr += 4;
    
    if(((*(uint32_t*)RealHeapPtr ^ CODE_ARRA) !=  0 && 
        (*(uint32_t*)RealHeapPtr ^ CODE_ARRS) != 0 && 
        (*(uint32_t*)RealHeapPtr ^ CODE_CLAS) != 0 && 
        (*(uint32_t*)RealHeapPtr ^ CODE_INST) != 0 && 
        (*(uint32_t*)RealHeapPtr ^ CODE_STRG) != 0 && 
        (*(uint32_t*)RealHeapPtr ^ CODE_SBLD) != 0)) {
        // Then it's not a real allocated object
        return 0;
    }
    
   // else {
//        fprintf(stderr, "KIND: %s", (RealHeapPtr));
//        HeapReference = (uint8_t*)MAKE_HEAP_REFERENCE(RealHeapPtr);
//        if (HeapReference < HeapEnd || HeapReference > HeapStart) {
//            // A valid reference
//            return 1;
//        }
//        else {
//            // Not a valid reference
//            return 0;
//        }
        // It's a valid reference
        //return RealHeapPtr;
    //}
    // Mark it always if we get to here
    return RealHeapPtr;
}