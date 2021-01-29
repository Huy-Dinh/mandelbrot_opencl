#pragma once

#include <CL/cl.h>

#include <string>

/**
 * Converts the given OpenCL errorcode into a readable string.
 */
std::string clErrToStr(cl_int errcode);

/**
 * Prints the name and vendor of the given OpenCL device.
 */
void printDeviceName(cl_device_id device);

/**
 * CATCH_CL_ERROR is a helper macro to deal with OpenCL errors.
 * If errcode != CL_SUCCESS then this macro will print the error and
 * the location where the error was caught and abort the program.
 */
void catchClErrImpl(cl_int errcode, const char *file, int line);
#define CATCH_CL_ERROR(errcode) catchClErrImpl(errcode, __FILE__, __LINE__)

/**
 * CATCH_CL_BUILD_FAILURE is a helper macro to deal with OpenCL build failures.
 * For errcode == CL_BUILD_PROGRAM_FAILURE this macro will print the 
 * compile errors and abort the program.
 */
void catchClBuildFailureImpl(cl_int errcode, cl_device_id device, cl_program program, const char *file, int line);
#define CATCH_CL_BUILD_FAILURE(errcode, device, program) catchClBuildFailureImpl(errcode, device, program, __FILE__, __LINE__)