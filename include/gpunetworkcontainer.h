#ifndef GPUNETWORKCONTAINER
#define GPUNETWORKCONTAINER

#include "NetworkContainer.h"
#include "gpunetworkcontextmanager.h"
#include "GPUBuffer.h"
#include <vector>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <random>
#include <ctime>
#include <cstdlib>
#include <map>

template <typename Datatype>
class GPUNetworkContainer : public NetworkContainer<LOGISTIC<float>, Datatype >
{
   //typedef NetworkContainer<Datatype, LOGISTIC<float>> BASE_CLASS;

public:
    GPUNetworkContainer(std::vector<unsigned int> networksize, std::vector<std::tuple<unsigned int, unsigned int, Datatype>> connections, unsigned int flags):
        NetworkContainer<LOGISTIC<float>,Datatype >(networksize, connections, flags)
    {
        _clContextManager = GPUNetworkContextManager::GetInstance();

        //Now Upload memory to GPU
        _GPUWeights->GetGPUBuffer() = new cl::Buffer(_clContextManager->GetCLContext(),CL_MEM_READ_WRITE, this->_weight.size()*sizeof(float), &this->_weight[0] );
        _GPUBias->GetGPUBuffer() = new cl::Buffer(_clContextManager->GetCLContext(),CL_MEM_READ_WRITE, this->_bias.size()*sizeof(float), &this->_bias[0] );
        _GPUPreCnt->GetGPUBuffer() = new cl::Buffer(_clContextManager->GetCLContext(),CL_MEM_READ_WRITE, this->_preCnt.size()*sizeof(float), &this->_preCnt[0] );
        _GPUSize->GetGPUBuffer() = new cl::Buffer(_clContextManager->GetCLContext(),CL_MEM_READ_WRITE, this->_size.size()*sizeof(float), &this->_size[0] );
        _GPUInputs->GetGPUBuffer() = new cl::Buffer(_clContextManager->GetCLContext(),CL_MEM_READ_WRITE, this->_inputs.size()*sizeof(float), &this->_inputs[0] );
        _GPULayerStart->GetGPUBuffer() = new cl::Buffer(_clContextManager->GetCLContext(),CL_MEM_READ_WRITE, this->_layer_start.size()*sizeof(float), &this->_layer_start[0] );
        _GPULayerEnd->GetGPUBuffer() = new cl::Buffer(_clContextManager->GetCLContext(),CL_MEM_READ_WRITE, this->_layer_end.size()*sizeof(float), &this->_layer_end[0] );
        _GPUConnOffset->GetGPUBuffer() = new cl::Buffer(_clContextManager->GetCLContext(),CL_MEM_READ_WRITE, this->_connOffset.size()*sizeof(float), &this->_connOffset[0] );

        //_GPUOut->GetGPUBuffer() = cl::Buffer(_clContextManager->GetCLContext(),CL_MEM_READ_WRITE, this->_out.size()*sizeof(float), &this->_out[0] );
        //_GPUZOut->GetGPUBuffer() = cl::Buffer(_clContextManager->GetCLContext(),CL_MEM_READ_WRITE, this->_z_out.size()*sizeof(float), &this->_z_out[0] );

        //_GPUPointerMap.insert(std::pair<std::vector<>*,cl::Buffer* >(&this->_weight, &_GPUWeights));


    }

    GPUNetworkContainer(std::vector<unsigned int> networksize, unsigned int flags):
        NetworkContainer<LOGISTIC<float>,Datatype >(networksize,flags)
    {
        //Now Upload memory to GPU
        _clContextManager = GPUNetworkContextManager::GetInstance();

        _GPUWeights = new GPUBuffer<Datatype>(&this->_weight, _clContextManager);
        _GPUBias = new GPUBuffer<Datatype>(&this->_bias, _clContextManager);
        _GPUOut = new GPUBuffer<Datatype>(&this->_out, _clContextManager);
        _GPUZOut = new GPUBuffer<Datatype>(&this->_z_out, _clContextManager);

        _GPUPreCnt = new GPUBuffer<unsigned int>(&this->_preCnt, _clContextManager);
        _GPUSize = new GPUBuffer<unsigned int>(&this->_size, _clContextManager);
        _GPUInputs = new GPUBuffer<unsigned int>(&this->_inputs, _clContextManager);
        _GPULayerStart = new GPUBuffer<unsigned int>(&this->_layer_start, _clContextManager);
        _GPULayerEnd = new GPUBuffer<unsigned int>(&this->_layer_end, _clContextManager);
        _GPUConnOffset = new GPUBuffer<unsigned int>(&this->_connOffset, _clContextManager);
        //Now Upload memory to GPU

        //_GPUPointerMap.insert(std::pair<std::vector<>*,cl::Buffer* >(&this->_weight, &_GPUWeights));
    }

    GPUBuffer<Datatype> * GetGPUBufferWeights()
    {
        return _GPUWeights;
    }

    GPUBuffer<Datatype> * GetGPUBufferBias()
    {
        return _GPUBias;
    }

    GPUBuffer<Datatype> * GetGPUBufferOut(unsigned int id)
    {
        return &_GPUOut->at(id);
    }

    GPUBuffer<Datatype> * GetGPUBufferOutZ(unsigned int id)
    {
        return &_GPUZOut->at(id);
    }

    GPUBuffer<unsigned int> * GetGPUBufferPreCnt()
    {
        return _GPUPreCnt;
    }

    GPUBuffer<unsigned int> * GetGPUBufferSize()
    {
        return _GPUSize;
    }

    GPUBuffer<unsigned int> * GetGPUBufferInputs()
    {
        return _GPUInputs;
    }

    GPUBuffer<unsigned int> * GetGPUBufferLayerStart()
    {
        return _GPULayerStart;
    }

    GPUBuffer<unsigned int> * GetGPUBufferLayerEnd()
    {
        return _GPULayerEnd;
    }

    GPUBuffer<unsigned int> * GetGPUBufferConnOffset()
    {
        return _GPUConnOffset;
    }

    void SetNeuronValue(Datatype val, unsigned int id)
    {
        _GPUOut->GetCPUBuffer()->at(id) = val;
    }

    void SetGPUBuffersParallel(unsigned int batch_size)
    {
        _GPUOut = new std::vector<GPUBuffer<Datatype>>(batch_size);
        _GPUZOut = new std::vector<GPUBuffer<Datatype>>(batch_size);

        for(int cnt=0; cnt < batch_size; cnt++)
        {
            _GPUOut->at(cnt) = cl::Buffer(_clContextManager->GetCLContext(),CL_MEM_READ_WRITE, this->_out.size()*sizeof(float), &this->_out[0] );
            _GPUZOut->at(cnt) = cl::Buffer(_clContextManager->GetCLContext(),CL_MEM_READ_WRITE, this->_out.size()*sizeof(float), &this->_z_out[0] );
        }
    }



    /*std::map<void *, cl::Buffer*> _GPUPointerMap; //CPU_Ptr -> GPU_Ptr
    std::map<void *, int> _GPUUpdateBufferMap; //CPU_Ptr -> int val 0 = cache not dirty, 1=cpu cache dirty, 2= gpu cache dirty
        */
    GPUNetworkContextManager * _clContextManager;

    GPUBuffer<Datatype> * _GPUWeights;//CL_Buffer_Object for Weights
    GPUBuffer<Datatype> * _GPUBias; //CL_Buffer_Object for BIAS

    std::vector<GPUBuffer<Datatype>> * _GPUOut; //CL_Buffer for Out Vals
    std::vector<GPUBuffer<Datatype>> * _GPUZOut; //CL_Buffer for ZOut Vals

    GPUBuffer<unsigned int> * _GPUPreCnt; //CL_Buffer for Predecessors
    GPUBuffer<unsigned int> * _GPUSize; //CL_Buffer for Network layer sizes
    GPUBuffer<unsigned int> * _GPUInputs; //CL_Buffer for node inputs
    GPUBuffer<unsigned int> * _GPULayerStart; //CL_Buffer for layer start
    GPUBuffer<unsigned int> * _GPULayerEnd; //CL_Buffer for layer end
    GPUBuffer<unsigned int> * _GPUConnOffset; //CL_Buffer for ConnOffset



};


#endif // GPUNETWORKCONTAINER

