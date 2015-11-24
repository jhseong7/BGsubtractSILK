#include "CL/cl.hpp"

extern cl_device_id device_id;
extern cl_context context;
extern cl_command_queue command_queue;
extern cl_mem memobj_kernel_BG_Model;
extern cl_program program;
extern cl_kernel kernel_BG_Model;
extern cl_kernel kernel_ImageAbsSubtract;
extern cl_platform_id platform_id;
extern cl_uint ret_num_devices;
extern cl_uint ret_num_platforms;
extern cl_int ret;

extern size_t global_work_size;
extern size_t local_work_size;