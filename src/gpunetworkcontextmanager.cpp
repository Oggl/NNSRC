#include "../include/gpunetworkcontextmanager.h"

GPUNetworkContextManager* GPUNetworkContextManager::_Instance = NULL;

GPUNetworkContextManager::GPUNetworkContextManager()
{
    cl_int errid;
    errid = clGetPlatformIDs(0, NULL, &m_NumberOfPlatforms);
    if(errid != CL_SUCCESS)
    {

    }
    cl::Platform::get(&cl_platforms);

    cl_device_type devType = CL_DEVICE_TYPE_GPU;//CL_DEVICE_TYPE_CPU;//
    //Get platform infos
    std::string platformstring;
    std::string devicestring;
    std::cout << "----------" << std::endl;
    std::cout << "List of available platforms:" << std::endl;
    std::cout << "----------" << std::endl;
    for(int pc = 0; pc < cl_platforms.size(); pc++)
    {
        std::cout << "----------" << std::endl;
        cl_platforms[pc].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformstring);
        cl_platforms[pc].getDevices(CL_DEVICE_TYPE_GPU, &cl_devices);
        std::cout << "Platform Number " << (pc+1) << std::endl;
        std::cout << platformstring <<" Number of devices: "<<cl_devices.size() << std::endl;
        std::vector<size_t> wgsize;
        std::string dev_info;
        for(int dc = 0; dc < cl_devices.size(); dc++ )
        {
            cl_devices[dc].getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &wgsize);
            std::cout << "Workgroupsize: " <<wgsize[0]<<" "<<wgsize[1]<<" "<< wgsize[2] << std::endl;
            cl_devices[dc].getInfo(CL_DEVICE_NAME, &dev_info);
            std::cout << "CL_DEVICE_NAME: "<< dev_info <<std::endl;
            cl_devices[dc].getInfo(CL_DEVICE_VENDOR, &dev_info);
            std::cout << "CL_DEVICE_VENDOR: "<< dev_info <<std::endl;
            cl_devices[dc].getInfo(CL_DEVICE_PROFILE, &dev_info);
            std::cout << "CL_DEVICE_PROFILE: "<< dev_info <<std::endl;
            cl_devices[dc].getInfo(CL_DEVICE_VERSION, &dev_info);
            std::cout << "CL_DEVICE_VERSION: "<< dev_info <<std::endl;
            cl_devices[dc].getInfo(CL_DRIVER_VERSION, &dev_info);
            std::cout << "CL_DRIVER_VERSION: "<< dev_info <<std::endl;
            cl_devices[dc].getInfo(CL_DEVICE_OPENCL_C_VERSION, &dev_info);
            std::cout << "CL_DEVICE_OPENCL_C_VERSION: "<< dev_info <<std::endl;
            cl_devices[dc].getInfo(CL_DEVICE_EXTENSIONS, &dev_info);
            std::cout << "CL_DEVICE_EXTENSIONS: "<< dev_info <<std::endl;
            std::cout << "----------" << std::endl;
        }
    }

    cl_platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &_SelectedDevices);
    _SelectedContext = cl::Context(_SelectedDevices, NULL, NULL, NULL, NULL);
    std::string dev_info;
    _SelectedContext.getInfo(CL_CONTEXT_DEVICES, &cl_devices);
    cl_devices[0].getInfo(CL_DEVICE_NAME,&dev_info);
    cl_devices[0].getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &_workgroupsize);
    std::cout << "Selected Device is : " << dev_info << std::endl;
    _SelectedDevice = _SelectedDevices[0];
    _SelectedQueue = cl::CommandQueue(_SelectedContext,_SelectedDevice, 0, NULL);


}

GPUNetworkContextManager * GPUNetworkContextManager::GetInstance()
{
    if(_Instance == NULL)
      {
      _Instance = new GPUNetworkContextManager();
      }
    return _Instance;
}

GPUNetworkContextManager::~GPUNetworkContextManager()
{


}

cl::Context GPUNetworkContextManager::GetCLContext()
{
    return this->_SelectedContext;
}

cl::CommandQueue GPUNetworkContextManager::GetCommandQueue()
{
    return this->_SelectedQueue;
}

std::vector<cl::Device>* GPUNetworkContextManager::GetSelectedDevices()
{
    return &_SelectedDevices;
}

cl::Device GPUNetworkContextManager::GetSelectedDevice()
{
    return _SelectedDevice;
}

size_t GPUNetworkContextManager::GetWorkgroupSize()
{
    return _workgroupsize[0];
}
