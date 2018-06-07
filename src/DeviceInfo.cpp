#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

#if defined(__APPLE__) || defined(__MACOSX)
    #include <OpenCL/opencl.h>
#else
    #include <CL/cl.h>
#endif

// More details: https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clGetDeviceInfo.html

cl_kernel createKernelFromSource(cl_device_id device_id, cl_context context,
        const char *source, const char *name) {
    int err;
    // Load the source
    cl_program program = clCreateProgramWithSource(context, 1, &source , NULL, &err);
    if (err != CL_SUCCESS) { perror("Unable to create program"); }

    // Compile it.
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[4096];
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
                                          sizeof(buffer), buffer, &len);
        fprintf(stderr, "%s\n", buffer);
        perror("Unable to build program");
    }
    // Load it.
    cl_kernel kernel = clCreateKernel(program, name, &err);
    if (!kernel || err != CL_SUCCESS) { perror("Unable to create kernel"); }
    clReleaseProgram(program);

    return kernel;
}

int main() {

    int i, j, k;
    char* value;
    size_t valueSize;
    cl_uint platformCount;
    cl_platform_id* platforms;
    cl_uint deviceCount;
    cl_device_id* devices;
    cl_uint maxComputeUnits;
    size_t maxWorkGroupSize;
    cl_uint maxWorkItemDimensions;
    size_t preferredWorkGroupSizeMultiple;
    cl_kernel kernel;
    cl_context context;
    cl_ulong local_mem_size;

    // get all platforms
    clGetPlatformIDs(0, NULL, &platformCount);
    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
    clGetPlatformIDs(platformCount, platforms, NULL);

    for (i = 0; i < platformCount; i++) {

        // get all devices
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
        devices = (cl_device_id*) malloc(sizeof(cl_device_id) * deviceCount);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);

        // for each device print critical attributes
        for (j = 0; j < deviceCount; j++) {

            // print device name
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
            printf("%d. Device: %s\n", j+1, value);
            free(value);

            // print hardware device version
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
            printf(" %d.%d Hardware version: %s\n", j+1, 1, value);
            free(value);

            // print software driver version
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
            printf(" %d.%d Software version: %s\n", j+1, 2, value);
            free(value);

            // print c version supported by compiler for device
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
            printf(" %d.%d OpenCL C version: %s\n", j+1, 3, value);
            free(value);

            // print parallel compute units
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,
                    sizeof(maxComputeUnits), &maxComputeUnits, NULL);
            printf(" %d.%d Parallel compute units: %d\n", j+1, 4, maxComputeUnits);

            // print max work group size
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE,
                    sizeof(maxWorkGroupSize), &maxWorkGroupSize, NULL);
            printf(" %d.%d Max work group size: %zu\n", j+1, 5, maxWorkGroupSize);

            // print max work item dimensions
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
                    sizeof(maxWorkItemDimensions), &maxWorkItemDimensions, NULL);
            printf(" %d.%d Max work item dimensions: %d\n", j+1, 6, maxWorkItemDimensions);

            size_t maxWorkItemSizes[maxWorkItemDimensions];
            // print max work item sizes
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES,
                    sizeof(maxWorkItemSizes), &maxWorkItemSizes, NULL);
            printf(" %d.%d Max work item sizes: (", j+1, 7);
            for (k = 0; k < maxWorkItemDimensions; k++) {
                if (k == maxWorkItemDimensions - 1) {
                    printf("%zu)\n", maxWorkItemSizes[k]);
                } else {
                    printf("%zu, ", maxWorkItemSizes[k]);
                }
            }

            clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &local_mem_size, 0);
            printf(" %d.%d Local memory size: %llu\n", j+1, 8, local_mem_size);

            // print preferred work group size multiple
            context = clCreateContext(0, 1, &devices[j], NULL, NULL, NULL);
            std::ifstream kernel_file{"../kernels/info/simple_kernel.cl"};
            std::string kernel_source{std::istreambuf_iterator<char>(kernel_file), std::istreambuf_iterator<char>()};
            kernel = createKernelFromSource(devices[j], context, kernel_source.c_str(), "vector_add");
            clGetKernelWorkGroupInfo(kernel, devices[j], CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
                sizeof(preferredWorkGroupSizeMultiple), &preferredWorkGroupSizeMultiple, NULL);
            printf(" %d.%d Preferred work group size multiple: %zu\n", j+1, 9, preferredWorkGroupSizeMultiple);

            printf("\n");
        }

        free(devices);

    }

    free(platforms);
    return 0;
}
