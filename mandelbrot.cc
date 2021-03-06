#include "cl_utils.h"
#include "lodepng.h"

/**
 * OpenCL 1.2 is more than enough for our purposes.
 * This define also allows using clCreateCommandQueue 
 * without the compiler throwing a warning.
 */
#include <CL/cl.h>

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

/**
 * Helper function to store an image as PNG. You know the drill...
 */
void saveImageAsPNG(const unsigned char *image, int width, int height, const std::string &fname) {
    std::cout << "Encoding image..." << std::endl;
    int error = lodepng_encode32_file(fname.c_str(), image, width, height);
    if (error) {
        std::cout << "error " << error << " : " << lodepng_error_text(error) << std::endl;
        std::abort();
    }
}

int main( int argc, char ** argv )
{
    /**
     * Some constants to use  
     */
    constexpr int width = 2048, height = 2048;
    constexpr int max_iter = 350;
    constexpr float max_abs = 2.0;
    constexpr size_t count = width * height * 4;
    
    std::vector<unsigned char> resultVector(count);
    /**
     * Check for a OpenCL capable device  
     */
    cl_int err;

    /* Identify a platform */
    cl_platform_id platform;
    err = clGetPlatformIDs(1, &platform, NULL);
    CATCH_CL_ERROR(err);

    /*  Access a device in our case we are looking for a GPU but a CPU is also possible 
        This might be helpful for students who don't have access to a GPU capable of OpenCL */
    cl_device_id device;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err == CL_DEVICE_NOT_FOUND) {
        // If we cannot find a GPU we take the CPU
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
    }
    CATCH_CL_ERROR(err);

    /* Lets check what device we are using */
    printDeviceName(device);

    /**
     * extend this template to compute the mandelbrot set with a OpenCL kernel.
     * Then transfer the resulting image back to the host and store it as png.
     */
    /* Create a context */
    cl_context context = clCreateContext(0, 1, &device, NULL, NULL, &err);
    CATCH_CL_ERROR(err);
    /* Create a command queue */
    cl_command_queue commandQueue = clCreateCommandQueue(context, device, 0, &err);
    CATCH_CL_ERROR(err);

    /* Read the program source */
    std::ifstream sourceFile("mandelbrot.cl");
    std::string sourceCodeString( std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
    const char* sourceCode = sourceCodeString.c_str();
    std::cout << sourceCodeString << std::endl;

    /* Create the program from the source code */
    cl_program program = clCreateProgramWithSource(context, 1, (const char **) &sourceCode, NULL, &err);
    CATCH_CL_ERROR(err);

    /* Build the program executable */
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    CATCH_CL_ERROR(err);

    /* Create kernel */
    cl_kernel kernel = clCreateKernel(program, "solve_mandelbrot", &err);
    CATCH_CL_ERROR(err);

    /* Create output buffer */
    cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, count, NULL, NULL);
    if (!outputBuffer)
    {
        std::cout << "Failed to allocate output memory for the kernel" << std::endl;
        exit(1);
    }

    /* Pass arguments onto the program */
    err = 0;
    err |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &outputBuffer);
    err |= clSetKernelArg(kernel, 1, sizeof(int), &height);
    err |= clSetKernelArg(kernel, 2, sizeof(int), &width);
    err |= clSetKernelArg(kernel, 3, sizeof(int), &max_iter);
    err |= clSetKernelArg(kernel, 4, sizeof(float), &max_abs);
    CATCH_CL_ERROR(err);

    size_t localWorkSize, globalWorkSize;
    /* Get maximum work group size, these work items can run concurrently */
    err = clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(localWorkSize), &localWorkSize, NULL);
    CATCH_CL_ERROR(err);

    std::cout << "Maximum number of work items in a work group: " << localWorkSize << std::endl;

    /* Execute the kernel */
    globalWorkSize = count / 4;
    err = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
    CATCH_CL_ERROR(err);

    /* Wait for the command queue to finish */
    clFinish(commandQueue);
    std::cout << "Done" << std::endl;

    /* Read back the result to the array */
    err = clEnqueueReadBuffer( commandQueue, outputBuffer, CL_TRUE, 0, count, resultVector.data(), 0, NULL, NULL );  
    CATCH_CL_ERROR(err);

    /* Encode the image and cleanup */
    saveImageAsPNG(resultVector.data(), width, height, "mandelbrot.png");

    /* Be a good memory manager and clean up after myself */
    clReleaseMemObject(outputBuffer);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commandQueue);
    clReleaseContext(context);

    return 0;
}
