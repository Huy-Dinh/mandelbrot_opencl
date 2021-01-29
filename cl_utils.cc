#include "cl_utils.h"

#include <iostream>
#include <unordered_map>

std::string clErrToStr(cl_int errcode) {
    static const std::unordered_map<int, std::string> mapErrToStr = {
        #define ERR_STR_PAIR(err) {err, #err}
        ERR_STR_PAIR(CL_SUCCESS),
        ERR_STR_PAIR(CL_DEVICE_NOT_FOUND),
        ERR_STR_PAIR(CL_DEVICE_NOT_AVAILABLE),
        ERR_STR_PAIR(CL_COMPILER_NOT_AVAILABLE),
        ERR_STR_PAIR(CL_MEM_OBJECT_ALLOCATION_FAILURE),
        ERR_STR_PAIR(CL_OUT_OF_RESOURCES),
        ERR_STR_PAIR(CL_OUT_OF_HOST_MEMORY),
        ERR_STR_PAIR(CL_PROFILING_INFO_NOT_AVAILABLE),
        ERR_STR_PAIR(CL_MEM_COPY_OVERLAP),
        ERR_STR_PAIR(CL_IMAGE_FORMAT_MISMATCH),
        ERR_STR_PAIR(CL_IMAGE_FORMAT_NOT_SUPPORTED),
        ERR_STR_PAIR(CL_BUILD_PROGRAM_FAILURE),
        ERR_STR_PAIR(CL_MAP_FAILURE),
        ERR_STR_PAIR(CL_MISALIGNED_SUB_BUFFER_OFFSET),
        ERR_STR_PAIR(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST),
        ERR_STR_PAIR(CL_COMPILE_PROGRAM_FAILURE),
        ERR_STR_PAIR(CL_LINKER_NOT_AVAILABLE),
        ERR_STR_PAIR(CL_LINK_PROGRAM_FAILURE),
        ERR_STR_PAIR(CL_DEVICE_PARTITION_FAILED),
        ERR_STR_PAIR(CL_KERNEL_ARG_INFO_NOT_AVAILABLE),
        ERR_STR_PAIR(CL_INVALID_VALUE),
        ERR_STR_PAIR(CL_INVALID_DEVICE_TYPE),
        ERR_STR_PAIR(CL_INVALID_PLATFORM),
        ERR_STR_PAIR(CL_INVALID_DEVICE),
        ERR_STR_PAIR(CL_INVALID_CONTEXT),
        ERR_STR_PAIR(CL_INVALID_QUEUE_PROPERTIES),
        ERR_STR_PAIR(CL_INVALID_COMMAND_QUEUE),
        ERR_STR_PAIR(CL_INVALID_HOST_PTR),
        ERR_STR_PAIR(CL_INVALID_MEM_OBJECT),
        ERR_STR_PAIR(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR),
        ERR_STR_PAIR(CL_INVALID_IMAGE_SIZE),
        ERR_STR_PAIR(CL_INVALID_SAMPLER),
        ERR_STR_PAIR(CL_INVALID_BINARY),
        ERR_STR_PAIR(CL_INVALID_BUILD_OPTIONS),
        ERR_STR_PAIR(CL_INVALID_PROGRAM),
        ERR_STR_PAIR(CL_INVALID_PROGRAM_EXECUTABLE),
        ERR_STR_PAIR(CL_INVALID_KERNEL_NAME),
        ERR_STR_PAIR(CL_INVALID_KERNEL_DEFINITION),
        ERR_STR_PAIR(CL_INVALID_KERNEL),
        ERR_STR_PAIR(CL_INVALID_ARG_INDEX),
        ERR_STR_PAIR(CL_INVALID_ARG_VALUE),
        ERR_STR_PAIR(CL_INVALID_ARG_SIZE),
        ERR_STR_PAIR(CL_INVALID_KERNEL_ARGS),
        ERR_STR_PAIR(CL_INVALID_WORK_DIMENSION),
        ERR_STR_PAIR(CL_INVALID_WORK_GROUP_SIZE),
        ERR_STR_PAIR(CL_INVALID_WORK_ITEM_SIZE),
        ERR_STR_PAIR(CL_INVALID_GLOBAL_OFFSET),
        ERR_STR_PAIR(CL_INVALID_EVENT_WAIT_LIST),
        ERR_STR_PAIR(CL_INVALID_EVENT),
        ERR_STR_PAIR(CL_INVALID_OPERATION),
        ERR_STR_PAIR(CL_INVALID_GL_OBJECT),
        ERR_STR_PAIR(CL_INVALID_BUFFER_SIZE),
        ERR_STR_PAIR(CL_INVALID_MIP_LEVEL),
        ERR_STR_PAIR(CL_INVALID_GLOBAL_WORK_SIZE),
        ERR_STR_PAIR(CL_INVALID_PROPERTY),
        ERR_STR_PAIR(CL_INVALID_IMAGE_DESCRIPTOR),
        ERR_STR_PAIR(CL_INVALID_COMPILER_OPTIONS),
        ERR_STR_PAIR(CL_INVALID_LINKER_OPTIONS),
        ERR_STR_PAIR(CL_INVALID_DEVICE_PARTITION_COUNT),
#if CL_TARGET_OPENCL_VERSION >= 200
        ERR_STR_PAIR(CL_INVALID_PIPE_SIZE),
        ERR_STR_PAIR(CL_INVALID_DEVICE_QUEUE),
#endif
        #undef ERR_STR_PAIR
    };

    auto got = mapErrToStr.find(errcode);

    if (got == mapErrToStr.end()) {
        return "UNKNOWN_ERROR";
    }

    return got->second;
}

void catchClErrImpl(cl_int err, const char *file, int line) {
    if (err == CL_SUCCESS) {
        return;
    }
    std::cout << file << ":" << line << ":" << "Caught OpenCL error '" << clErrToStr(err) << "(" << err << ")'!" << std::endl;
    std::abort();
}

void catchClBuildFailureImpl(cl_int err, cl_device_id device, cl_program program, const char *file, int line) {
    if (err != CL_BUILD_PROGRAM_FAILURE) {
        catchClErrImpl(err, file, line);
        return;
    }
    
    /* Find size of log and print to std output */
    std::cout << file << ":" << line << ":" << "Caught OpenCL build error. Log:" << std::endl;
    size_t log_size;
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    char *program_log = (char*) malloc(log_size + 1);
    program_log[log_size] = '\0';
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
    printf("%s\n", program_log);
    free(program_log);
    std::abort();
}

void printDeviceName(cl_device_id device) {
    cl_int err;

    char* devicevendor;
    size_t devicevendorSize;
    err = clGetDeviceInfo(device, CL_DEVICE_VENDOR, 0, NULL, &devicevendorSize);
    CATCH_CL_ERROR(err);
    devicevendor = (char*) malloc(devicevendorSize);
    err = clGetDeviceInfo(device, CL_DEVICE_VENDOR, devicevendorSize, devicevendor, NULL);
    CATCH_CL_ERROR(err);

    char* devicename;
    size_t devicenameSize;
    err = clGetDeviceInfo(device, CL_DEVICE_NAME, 0, NULL, &devicenameSize);
    CATCH_CL_ERROR(err);
    devicename = (char*) malloc(devicenameSize);
    err = clGetDeviceInfo(device, CL_DEVICE_NAME, devicenameSize, devicename, NULL);
    CATCH_CL_ERROR(err);

    printf("Using device %s from vendor %s.\n", devicename, devicevendor);

    free(devicename);
    free(devicevendor);
}