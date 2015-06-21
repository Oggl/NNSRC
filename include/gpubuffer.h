#ifndef GPUBUFFER_H
#define GPUBUFFER_H

#include <CL/cl.hpp>
#include "gpunetworkcontextmanager.h"

template <typename Datatype>
class GPUBuffer
{
public:
    GPUBuffer(std::vector<Datatype> * cpubuffer, GPUNetworkContextManager * gpucontext)
    {
        _clContextManager = gpucontext;
        _CPUMemObj.assign(cpubuffer->begin(), cpubuffer->end());
        _GPUMemObj = cl::Buffer(_clContextManager->GetCLContext(),CL_MEM_READ_WRITE, _CPUMemObj.size()*sizeof(float), &_CPUMemObj.at(0) );
        _CPUBufferDirty = false;
        _GPUBufferDirty = false;
        _size = cpubuffer->size()*sizeof(Datatype);
    }

    cl::Buffer *  GetGPUBuffer()
    {
        if(_GPUBufferDirty)
        {
            SyncGPUBuffer();
        }
        return _GPUMemObj;
    }

    std::vector<Datatype> * GetCPUBuffer()
    {
        if(_CPUBufferDirty)
        {
            SyncCPUBuffer();
        }
        return _CPUMemObj;
    }

    void SyncGPUBuffer() //->Upload to GPU
    {
        if(_GPUBufferDirty)
        {
            _clContextManager->GetCommandQueue().enqueueWriteBuffer(_GPUMemObj,CL_FALSE, 0, _size, &_CPUMemObj[0], NULL, NULL );
        }
    }

    void SyncGPUBuffer(unsigned int start, unsigned int end) //->Upload to GPU
    {
        if(_GPUBufferDirty)
        {
            _clContextManager->GetCommandQueue().enqueueWriteBuffer(_GPUMemObj,CL_FALSE, start*sizeof(Datatype), end*sizeof(Datatype), &_CPUMemObj[0], NULL, NULL );
        }
    }

    void SyncCPUBuffer() //Download to CPU
    {
        if(_CPUBufferDirty)
        {
            _clContextManager->GetCommandQueue().enqueueReadBuffer(_GPUMemObj,CL_FALSE, 0, _size, &_CPUMemObj[0], NULL, NULL );
        }
    }

    void SyncCPUBuffer(unsigned int start, unsigned int end) //Download to CPU
    {
        if(_CPUBufferDirty)
        {
            _clContextManager->GetCommandQueue().enqueueReadBuffer(_GPUMemObj,CL_FALSE, start*sizeof(Datatype), end*sizeof(Datatype), &_CPUMemObj[0], NULL, NULL );
        }
    }

    void SetGPUBufferDirty()
    {
        _GPUBufferDirty = true;
    }

    void SetCPUBufferDirty()
    {
        _CPUBufferDirty = false;
    }

private:
    cl::Buffer _GPUMemObj;
    std::vector<Datatype> _CPUMemObj;
    bool _CPUBufferDirty;
    bool _GPUBufferDirty;
    unsigned int _size;


    GPUNetworkContextManager * _clContextManager;

};

#endif // GPUBUFFER_H
