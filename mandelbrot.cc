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
     * TODO extend this template to compute the mandelbrot set with a OpenCL kernel.
     * Then transfer the resulting image back to the host and store it as png.
     */

    /* TODO: Encode the image and cleanup */
    // saveImageAsPNG(image, width, height, "mandelbrot.png");

    return 0;
}
