//
// File:       hello.c
//
// Abstract:   A simple "Hello World" compute example showing basic usage of OpenCL which
//             calculates the mathematical square (X[i] = pow(X[i],2)) for a buffer of
//             floating point values.
//
//
// Version:    <1.0>
//
// Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc. ("Apple")
//             in consideration of your agreement to the following terms, and your use,
//             installation, modification or redistribution of this Apple software
//             constitutes acceptance of these terms.  If you do not agree with these
//             terms, please do not use, install, modify or redistribute this Apple
//             software.
//
//             In consideration of your agreement to abide by the following terms, and
//             subject to these terms, Apple grants you a personal, non - exclusive
//             license, under Apple's copyrights in this original Apple software ( the
//             "Apple Software" ), to use, reproduce, modify and redistribute the Apple
//             Software, with or without modifications, in source and / or binary forms;
//             provided that if you redistribute the Apple Software in its entirety and
//             without modifications, you must retain this notice and the following text
//             and disclaimers in all such redistributions of the Apple Software. Neither
//             the name, trademarks, service marks or logos of Apple Inc. may be used to
//             endorse or promote products derived from the Apple Software without specific
//             prior written permission from Apple.  Except as expressly stated in this
//             notice, no other rights or licenses, express or implied, are granted by
//             Apple herein, including but not limited to any patent rights that may be
//             infringed by your derivative works or by other works in which the Apple
//             Software may be incorporated.
//
//             The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
//             WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
//             WARRANTIES OF NON - INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
//             PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION
//             ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
//
//             IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
//             CONSEQUENTIAL DAMAGES ( INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//             SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//             INTERRUPTION ) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION
//             AND / OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER
//             UNDER THEORY OF CONTRACT, TORT ( INCLUDING NEGLIGENCE ), STRICT LIABILITY OR
//             OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright ( C ) 2008 Apple Inc. All Rights Reserved.
//

////////////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <OpenCL/opencl.h>
#include <mach/mach.h>
#include <mach/mach_time.h>

////////////////////////////////////////////////////////////////////////////////

// Use a static data size for simplicity
//
#define DATA_SIZE (262144)

////////////////////////////////////////////////////////////////////////////////

// Simple compute kernel which computes the formula for A3
//
const char *KernelSourceOne = "\n" \
"__kernel void formula(                                                 \n" \
"   __global float* input_a,                                            \n" \
"   __global float* input_b,                                            \n" \
"   __global float* input_c,                                            \n" \
"   __global float* output,                                             \n" \
"   const unsigned int count)                                           \n" \
"{                                                                      \n" \
"   int i = get_global_id(0);                                           \n" \
"   if(i < count)                                                       \n" \
"       output[i] = (input_a[i] * input_b[i]) + input_c[i];             \n" \
"}                                                                      \n" \
"\n";

////////////////////////////////////////////////////////////////////////////////

// Two stage reduction
// http://developer.amd.com/resources/documentation-articles/articles-whitepapers/opencl-optimization-case-study-simple-reductions/
//
const char *KernelSourceTwo = "\n" \
"__kernel void reduce(                                                  \n" \
"    __global float* buffer,                                            \n" \
"    __global float* scratch,                                           \n" \
"    const unsigned int length,                                         \n" \
"    __global float* result) {                                          \n" \
"                                                                       \n" \
"  int global_index = get_global_id(0);                                 \n" \
"  float accumulator = INFINITY;                                        \n" \
"  // Loop sequentially over chunks of input vector                     \n" \
"  while (global_index < length) {                                      \n" \
"    float element = buffer[global_index];                              \n" \
"    accumulator = (accumulator < element) ? accumulator : element;     \n" \
"    global_index += get_global_size(0);                                \n" \
"  }                                                                    \n" \
"                                                                       \n" \
"  // Perform parallel reduction                                        \n" \
"  int local_index = get_local_id(0);                                   \n" \
"  scratch[local_index] = accumulator;                                  \n" \
"  barrier(CLK_LOCAL_MEM_FENCE);                                        \n" \
"  for(int offset = get_local_size(0) / 2;                              \n" \
"      offset > 0;                                                      \n" \
"      offset = offset / 2) {                                           \n" \
"    if (local_index < offset) {                                        \n" \
"      float other = scratch[local_index + offset];                     \n" \
"      float mine = scratch[local_index];                               \n" \
"      scratch[local_index] = (mine < other) ? mine : other;            \n" \
"    }                                                                  \n" \
"    barrier(CLK_LOCAL_MEM_FENCE);                                      \n" \
"  }                                                                    \n" \
"  if (local_index == 0) {                                              \n" \
"    result[get_group_id(0)] = scratch[0];                              \n" \
"  }                                                                    \n" \
"}                                                                      \n" \
"\n";

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    int err;                            // error code returned from api calls
    
    float a[DATA_SIZE];                 // original data sets given to device
    float b[DATA_SIZE];
    float c[DATA_SIZE];
    float results[DATA_SIZE];           // results returned from device
    float scratch[DATA_SIZE];
    float reduction_results[DATA_SIZE];
    unsigned int correct;               // number of correct results returned
    
    size_t global;                      // global domain size for our calculation
    size_t local;                       // local domain size for our calculation
    
    cl_platform_id platform_id;         // platform id
    cl_device_id device_id;             // compute device id
    cl_context context;                 // compute context
    cl_command_queue commands;          // compute command queue
    cl_program program;                 // compute program
    cl_kernel kernel;                   // compute kernel
    
    cl_mem input_a;                     // device memory used for the input array
    cl_mem input_b;                     // device memory used for the input array
    cl_mem input_c;                     // device memory used for the input array
    cl_mem output;                      // device memory used for the output array
    
    cl_mem input_buffer;
    cl_mem input_scratch;
    cl_mem result;
    
    uint64_t        start;
    uint64_t        end;
    uint64_t        elapsed;
    uint64_t        elapsedNano;
    static mach_timebase_info_data_t    sTimebaseInfo;
    
    // Fill our data sets with random float values
    //
    int i = 0;
    unsigned int count = DATA_SIZE;
    for(i = 0; i < count; i++)
    {
        a[i] = rand() / (float)RAND_MAX;
        b[i] = rand() / (float)RAND_MAX;
        c[i] = rand() / (float)RAND_MAX;
        scratch[i] = 0.0f;
    }
    
    // Connect to a compute device
    //
    int gpu = 1;
    clGetPlatformIDs(1, &platform_id, NULL);
    err = clGetDeviceIDs(platform_id, gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to create a device group!\n");
        return EXIT_FAILURE;
    }
    
    // Create a compute context
    //
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    if (!context)
    {
        printf("Error: Failed to create a compute context!\n");
        return EXIT_FAILURE;
    }
    
    // Create a command commands
    //
    commands = clCreateCommandQueue(context, device_id, 0, &err);
    if (!commands)
    {
        printf("Error: Failed to create a command commands!\n");
        return EXIT_FAILURE;
    }
    
    // Start the timing
    //
    start = mach_absolute_time();
    
    ////////////////////////////////// FORMULA KERNEL ////////////////////////////////////////////////
    
    // Create the compute program from the source buffer
    //
    program = clCreateProgramWithSource(context, 1, (const char **) & KernelSourceOne, NULL, &err);
    if (!program)
    {
        printf("Error: Failed to create compute program!\n");
        return EXIT_FAILURE;
    }
    
    // Build the program executable
    //
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];
        
        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(1);
    }
    
    // Start the timing
    //
    
    
    // Create the compute kernel in the program we wish to run
    //
    kernel = clCreateKernel(program, "formula", &err);
    if (!kernel || err != CL_SUCCESS)
    {
        printf("Error: Failed to create compute kernel!\n");
        exit(1);
    }
    
    // Create the input and output arrays in device memory for our calculation
    //
    input_a = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(float) * count, NULL, NULL);
    input_b = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(float) * count, NULL, NULL);
    input_c = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(float) * count, NULL, NULL);
    output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * count, NULL, NULL);
    if (!a || !b || !c || !output)
    {
        printf("Error: Failed to allocate device memory!\n");
        exit(1);
    }
    
    // Write our data sets into the input array in device memory
    //
    err  = clEnqueueWriteBuffer(commands, input_a, CL_TRUE, 0, sizeof(float) * count, a, 0, NULL, NULL);
    err |= clEnqueueWriteBuffer(commands, input_b, CL_TRUE, 0, sizeof(float) * count, b, 0, NULL, NULL);
    err |= clEnqueueWriteBuffer(commands, input_c, CL_TRUE, 0, sizeof(float) * count, c, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to write to source array!\n");
        exit(1);
    }
    
    // Set the arguments to our compute kernel
    //
    err = 0;
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_a);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &input_b);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &input_c);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &output);
    err |= clSetKernelArg(kernel, 4, sizeof(unsigned int), &count);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to set kernel arguments! %d\n", err);
        exit(1);
    }
    
    // Get the maximum work group size for executing the kernel on the device
    //
    err = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to retrieve kernel work group info! %d\n", err);
        exit(1);
    }
    
    // Execute the kernel over the entire range of our 1d input data sets
    // using the maximum number of work group items for this device
    //
    global = count;
    err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
    if (err)
    {
        printf("Error: Failed to execute kernel!\n");
        return EXIT_FAILURE;
    }
    
    // Wait for the command commands to get serviced before reading back results
    //
    clFinish(commands);
    
    // Read back the results from the device to verify the output
    //
    err = clEnqueueReadBuffer( commands, output, CL_TRUE, 0, sizeof(float) * count, results, 0, NULL, NULL );
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to read output array! %d\n", err);
        exit(1);
    }
    
    ////////////////////////////////// REDUCTION KERNEL ////////////////////////////////////////////////
    
    // Create the compute program from the source buffer
    //
    program = clCreateProgramWithSource(context, 1, (const char **) & KernelSourceTwo, NULL, &err);
    if (!program)
    {
        printf("Error: Failed to create compute program!\n");
        return EXIT_FAILURE;
    }
    
    // Build the program executable
    //
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];
        
        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(1);
    }
    
    // Create the compute kernel in the program we wish to run
    //
    kernel = clCreateKernel(program, "reduce", &err);
    if (!kernel || err != CL_SUCCESS)
    {
        printf("Error: Failed to create compute kernel!\n");
        exit(1);
    }
    
    // Create the input and output arrays in device memory for our calculation
    //
    input_buffer = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(float) * count, NULL, NULL);
    input_scratch = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(float) * count, NULL, NULL);
    result = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * count, NULL, NULL);
    if (!input_buffer || !input_scratch || !result)
    {
        printf("Error: Failed to allocate device memory!\n");
        exit(1);
    }
    
    // Write our data sets into the input array in device memory
    //
    err  = clEnqueueWriteBuffer(commands, input_buffer, CL_TRUE, 0, sizeof(float) * count, results, 0, NULL, NULL);
    err |= clEnqueueWriteBuffer(commands, input_scratch, CL_TRUE, 0, sizeof(float) * count, scratch, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to write to source array!\n");
        exit(1);
    }
    
    // Set the arguments to our compute kernel
    //
    err = 0;
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &input_scratch);
    err |= clSetKernelArg(kernel, 2, sizeof(int), &count);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &result);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to set kernel arguments! %d\n", err);
        exit(1);
    }
    
    // Get the maximum work group size for executing the kernel on the device
    //
    err = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to retrieve kernel work group info! %d\n", err);
        exit(1);
    }
    
    // Execute the kernel over the entire range of our 1d input data sets
    // using the maximum number of work group items for this device
    //
    global = count;
    err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
    if (err)
    {
        printf("Error: Failed to execute kernel!\n");
        return EXIT_FAILURE;
    }
    
    // Wait for the command commands to get serviced before reading back results
    //
    clFinish(commands);
    
    // Read back the results from the device to verify the output
    //
    err = clEnqueueReadBuffer( commands, output, CL_TRUE, 0, sizeof(float) * count, reduction_results, 0, NULL, NULL );
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to read output array! %d\n", err);
        exit(1);
    }
    
    // End the timing
    //
    end = mach_absolute_time();
    elapsed = end - start;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }
    elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
    printf("Elspased time: %fs\n", (float)elapsedNano / 1000000000.0f);
    
    // Validate our results
    //
    correct = 0;
    for(i = 0; i < count; i++)
    {
        if(floorf(results[i] * 10000) == floorf(((a[i] * b[i]) + c[i]) * 10000))
            correct++;
    }
    
    // Print a brief summary detailing the results
    //
    printf("Computed '%d/%d' correct values!\n", correct, count);
    
    // Shutdown and cleanup
    //
    clReleaseMemObject(input_a);
    clReleaseMemObject(input_b);
    clReleaseMemObject(input_c);
    clReleaseMemObject(output);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);
    
    return 0;
}