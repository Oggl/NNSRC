#ifndef GPUNETWORKCONTEXTMANAGER_H
#define GPUNETWORKCONTEXTMANAGER_H

#include "cl/OpenCL.h"
#include "cl/cl.h"
#include "cl/cl.hpp"
#include <vector>
#include <iostream>
#include <string>
#include "../Neuron_Functors.h"


class GPUNetworkContextManager
{
public:
    GPUNetworkContextManager();
    void DestroyInstance();
    static GPUNetworkContextManager* GetInstance();
    cl::Context GetCLContext();
    cl::CommandQueue GetCommandQueue();
    std::vector<cl::Device>* GetSelectedDevices();
    cl::Device GetSelectedDevice();
    size_t GetWorkgroupSize();

private:
    static GPUNetworkContextManager* _Instance;

    ~GPUNetworkContextManager();

    cl::Platform    _SelectedPlatform;
    cl::Context       _SelectedContext;
    cl::CommandQueue _SelectedQueue;
    std::vector<cl::Device>     _SelectedDevices;
    cl::Device _SelectedDevice;
    cl::CommandQueue* m_CommandQueue;
    cl_uint m_NumberOfDevices, m_NumberOfPlatforms;

    size_t _workgroupsize[3];


    std::vector<cl::Platform> cl_platforms;
    std::vector<cl::Device> cl_devices;

};

#endif // GPUNETWORKCONTEXTMANAGER_H
