#ifndef GPUNETWORK_H
#define GPUNETWORK_H

#include "Network.h"
#include "gpunetworkcontextmanager.h"
#include "gpunetworkcontainer.h"
#include "../Neuron_Functors.h"
#include <initializer_list>
#include <memory>
#include <random>
#include <algorithm>
#include "NetworkContainer.h"
#include "NeuronDatareader.h"
#include <tuple>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

template <typename Datatype>
class GPUNetwork : private Network<Datatype, LOGISTIC<float>>
{
    public:
    typedef LOGISTIC<float> ActivationFunctionType;
    typedef GPUNetworkContainer<Datatype> nContainer;
    typedef std::tuple<unsigned int, unsigned int, Datatype> nConnection;

    GPUNetwork(std::initializer_list<unsigned int> regions, std::vector<nConnection> connections, unsigned int flags):
        Network<Datatype, LOGISTIC<float>>(regions, connections, flags),
        _networkContainer(regions, connections, flags )
    {
        _clContextManager = GPUNetworkContextManager::GetInstance();

        std::string path = "./cl/NeuralNetworkKernels.cl";

        std::ostringstream FileStream;
        std::ifstream Filereader;
        Filereader.open(_file);

        while(!Filereader.eof())
        {
            FileStream << Filereader.get();
        }

        _file = FileStream.str();



    }

    GPUNetwork(std::initializer_list<unsigned int> regions, unsigned int flags):
        Network<Datatype, LOGISTIC<float>>(regions, flags),
        _networkContainer(this->_networkRegion, flags)
        //_connect(_networkContainer.GetConnections())
    {
        try
        {
            _clContextManager = GPUNetworkContextManager::GetInstance();

            std::string path = "./cl/NeuralNetworkKernels.cl";


            std::ifstream Filereader(path);

            if(!Filereader.is_open())
            {
            }
            std::ostringstream FileStream;

            std:copy(std::istreambuf_iterator<char>(Filereader),
            std::istreambuf_iterator<char>(),
            std::ostreambuf_iterator<char>(FileStream));
            _file = FileStream.str();

            std::cout << "bla" << std::endl;

            //
            _UpdateKernelProgram = cl::Program(_clContextManager->GetCLContext(), _file, true, &_error);

            _UpdateKernelProgram.build(*_clContextManager->GetSelectedDevices(), NULL, NULL, NULL);
            _UpdateKernel = cl::Kernel(_UpdateKernelProgram,_UpdateKernelName.c_str(), &_error);

            //
            _DeltaNeuronKernelProgram = cl::Program(_clContextManager->GetCLContext(), _file, true, &_error);

            _DeltaNeuronKernelProgram.build(*_clContextManager->GetSelectedDevices(), NULL, NULL, NULL);
            _DeltaNeuronKernel = cl::Kernel(_DeltaNeuronKernelProgram, _DeltaNeuronKernelName.c_str(), &_error);

            //
            _DeltaWeightKernelProgram =  cl::Program(_clContextManager->GetCLContext(), _file, true, &_error);

            _DeltaWeightKernelProgram.build(*_clContextManager->GetSelectedDevices(), NULL, NULL, NULL);
            _DeltaWeightKernel = cl::Kernel(_DeltaWeightKernelProgram, _DeltaWeightKernelName.c_str(), &_error);

            //Create Kernel Informations

            _GlobalUpdateKernelNDRanges = std::vector<cl::NDRange>(this->_networkRegion.size());
            _GlobalDeltaNeuronKernelNDRanges = std::vector<cl::NDRange>(this->_networkRegion.size());
            _GlobalDeltaWeightKernelNDRanges = std::vector<cl::NDRange>(this->_networkRegion.size());

            _LocalUpdateKernelNDRanges = std::vector<cl::NDRange>(this->_networkRegion.size());
            _LocalDeltaNeuronKernelNDRanges = std::vector<cl::NDRange>(this->_networkRegion.size());
            _LocalDeltaWeightKernelNDRanges  = std::vector<cl::NDRange>(this->_networkRegion.size());
            _BasicWorkgroupSize = _clContextManager->GetWorkgroupSize();

            _LocalSize = new size_t(this->_networkRegion.size()); //per layer
            _GlobalSize = new size_t(this->_networkRegion.size()); //per layer
            _WorkSize = new size_t(this->_networkRegion.size()); //per layer

            for(int cnt = 0; cnt< this->_networkRegion.size();cnt++ )
            {
                _WorkSize[cnt] =  _BasicWorkgroupSize*std::ceil(float(this->_networkRegion.at(cnt)/_BasicWorkgroupSize));
                _LocalSize[cnt] =  _BasicWorkgroupSize;
                _GlobalSize[cnt] = this->_networkRegion.at(cnt)*_LocalSize[cnt];
            }

            for(int cnt = 0; cnt < this->_networkRegion.size();cnt++ )
            {
                _LocalUpdateKernelNDRanges.at(cnt) = cl::NDRange(_LocalSize[cnt]);
                _LocalDeltaNeuronKernelNDRanges.at(cnt) = cl::NDRange(_LocalSize[cnt]);
                _LocalDeltaWeightKernelNDRanges.at(cnt) = cl::NDRange(_LocalSize[cnt]);

                _GlobalUpdateKernelNDRanges = cl::NDRange(_GlobalSize[cnt]);
                _GlobalDeltaNeuronKernelNDRanges = cl::NDRange(_GlobalSize[cnt]);
                _GlobalDeltaWeightKernelNDRanges = cl::NDRange(_GlobalSize[cnt]);
            }

        }
        catch(...)
        {
            std::cout <<"Ooops! Something went wrong! " << _error << std::endl;
        }


    }

    void LoadProgramsFromFile()
    {

    }

    void GenerateKernels()
    {

    }

    void GenerateKernelInformation()
    {

    }

    void Update(cl::Kernel _UpdateKernel)
    {
        unsigned int temp_layer = 0;

        if(this->_flags&&_FL_GPU_WGEX_)
        {
            for(unsigned int layer = 1; layer<this->_networkRegion.size(); layer++ )
            {
                unsigned int cnt = 0;
                temp_layer = layer;
                _UpdateKernel.setArg(cnt++, _networkContainer.GetNeuronCount()*sizeof(float) ,_networkContainer.GetGPUBufferOut()->GetGPUBuffer() );
                _UpdateKernel.setArg(cnt++, _networkContainer.GetConnCount()*sizeof(float) ,_networkContainer.GetGPUBufferWeights()->GetGPUBuffer() );
                _UpdateKernel.setArg(cnt++, _networkContainer.GetNeuronCount()*sizeof(float) ,_networkContainer.GetGPUBufferBias()->GetGPUBuffer() );
                _UpdateKernel.setArg(cnt++, _networkContainer.GetLayerCnt()*sizeof(unsigned int) ,_networkContainer.GetGPUBufferSize()->GetGPUBuffer() );
                _UpdateKernel.setArg(cnt++, _networkContainer.GetNeuronCount()*sizeof(unsigned int) ,_networkContainer.GetGPUBufferPreCnt()->GetGPUBuffer() );
                _UpdateKernel.setArg(cnt++, _networkContainer.GetConnCount()*sizeof(unsigned int) ,_networkContainer.GetGPUBufferInputs()->GetGPUBuffer() );
                _UpdateKernel.setArg(cnt++, _networkContainer.GetNeuronCount()*sizeof(unsigned int) ,_networkContainer.GetGPUBufferConnOffset()->GetGPUBuffer() );
                _UpdateKernel.setArg(cnt++, _networkContainer.GetLayerCnt()*sizeof(unsigned int) ,_networkContainer.GetGPUBufferLayerStart()->GetGPUBuffer() );
                _UpdateKernel.setArg(cnt++, _networkContainer.GetLayerCnt()*sizeof(unsigned int) ,_networkContainer.GetGPUBufferLayerEnd()->GetGPUBuffer() );

                _UpdateKernel.setArg(cnt++, _WorkSize[cnt]*sizeof(float) ,NULL );
                _UpdateKernel.setArg(cnt++, &_WorkSize[cnt]);
                _UpdateKernel.setArg(cnt++, &temp_layer);

                _clContextManager->GetCommandQueue().enqueueNDRangeKernel(_UpdateKernel, NULL, _GlobalUpdateKernelNDRanges.at(cnt), _LocalUpdateKernelNDRanges.at(cnt), NULL, NULL );
            }
        }
        else
        {
            for(unsigned int layer = 1; layer<this->_networkRegion.size(); layer++ )
            {
                unsigned int cnt = 0;
                temp_layer = layer;
                _UpdateKernel.setArg(cnt++, _networkContainer.GetNeuronCount()*sizeof(float) ,_networkContainer.GetGPUBufferOut()->GetGPUBuffer() );
                _UpdateKernel.setArg(cnt++, _networkContainer.GetConnCount()*sizeof(float) ,_networkContainer.GetGPUBufferWeights()->GetGPUBuffer() );
                _UpdateKernel.setArg(cnt++, _networkContainer.GetNeuronCount()*sizeof(float) ,_networkContainer.GetGPUBufferBias()->GetGPUBuffer() );

                _UpdateKernel.setArg(cnt++, _networkContainer.GetNeuronCount()*sizeof(unsigned int) ,_networkContainer.GetGPUBufferPreCnt()->GetGPUBuffer() );
                _UpdateKernel.setArg(cnt++, _networkContainer.GetConnCount()*sizeof(unsigned int) ,_networkContainer.GetGPUBufferInputs()->GetGPUBuffer() );
                _UpdateKernel.setArg(cnt++, _networkContainer.GetNeuronCount()*sizeof(unsigned int) ,_networkContainer.GetGPUBufferConnOffset()->GetGPUBuffer() );
                _UpdateKernel.setArg(cnt++, _networkContainer.GetLayerCnt()*sizeof(unsigned int) ,_networkContainer.GetGPUBufferLayerStart()->GetGPUBuffer() );

                _UpdateKernel.setArg(cnt++, &temp_layer);

                _clContextManager->GetCommandQueue().enqueueNDRangeKernel(_UpdateKernel, NULL, _GlobalUpdateKernelNDRanges.at(cnt), _LocalUpdateKernelNDRanges.at(cnt), NULL, NULL );
            }
        }
    }

    void CalcDelta()
    {

    }

    void SetInput(Imagetype * input, int id)
    {
        if(input->img_size==this->_networkRegion.at(0))
        {
            int cnt = 0;
            for(; cnt < input->img_size; cnt++)
            {
                _networkContainer.SetNeuronValue(input->val[cnt].sfloat, cnt);
            }
            _networkContainer.GetGPUBufferOut(id)->SetGPUBufferDirty();
            _networkContainer.GetGPUBufferOut(id)->SyncGPUBuffer(0, cnt);
        }
    }

    void SetOutput(unsigned int output)
    {
        //Unsigned int for output -> example: value is 5, therefore set 5th output to 1. Remaining outputs to 0
        unsigned int nlimit = _networkContainer.GetNeuronCount();
        unsigned int endlayerstart = _networkContainer.GetResNid();
        for(int n_cnt =  _networkContainer.GetResNid(),run = 0; n_cnt < nlimit; n_cnt++, run++)
        {
            _networkContainer.SetNeuronValue(0, n_cnt);
        }
        unsigned int pos =  _networkContainer.GetResNid();
        pos += output - 1;
        _networkContainer.SetNeuronValue(1, pos);

        _networkContainer.GetGPUBufferOut()->SetGPUBufferDirty();
        _networkContainer.GetGPUBufferOut()->SyncGPUBuffer(endlayerstart, nlimit);
    }

    void TrainWithGD(NeuronDatareader * data, float eta, int batch_size)
    {
        //Create Buffers and Events for multiple batches
        KernelWaitEvent = std::vector<cl::Event>(this->batch_size);

        for(unsigned int cnt =0; cnt < batch_size ; cnt++)
        {
            KernelWaitEvent.at(cnt) = cl::Event();
        }

        _networkContainer.SetGPUBuffersParallel(batch_size);


        bool done = false;
        while(!done) //GD_Iter
        {
            //SetInputs
            for(int cnt = 0; cnt < batch_size; cnt++)
            {

            }

        }

    }

private:
    GPUNetworkContextManager * _clContextManager;

    cl::Program _UpdateKernelProgram;
    cl::Program _DeltaNeuronKernelProgram;
    cl::Program _DeltaWeightKernelProgram;

    cl::Kernel _UpdateKernel;
    cl::Kernel _DeltaNeuronKernel;
    cl::Kernel _DeltaWeightKernel;

    nContainer _networkContainer;

    std::string _file;

    const std::string _UpdateKernelName = "UpdateNeuralNetwork";
    const std::string _UpdateWGKernelName = "UpdateNeuralNetworkWG";
    const std::string _DeltaNeuronKernelName = "CalcDeltaNeuron";
    const std::string _DeltaWeightKernelName = "CalcDeltaWeight";

    cl_int _error;

    unsigned int _BasicWorkgroupSize;
    size_t * _WorkSize;

    size_t * _LocalSize;
    size_t * _GlobalSize;

    std::vector<cl::NDRange> _GlobalUpdateKernelNDRanges; //For each layer
    std::vector<cl::NDRange> _GlobalDeltaNeuronKernelNDRanges;
    std::vector<cl::NDRange> _GlobalDeltaWeightKernelNDRanges;

    std::vector<cl::NDRange> _LocalUpdateKernelNDRanges; //For each layer
    std::vector<cl::NDRange> _LocalDeltaNeuronKernelNDRanges;
    std::vector<cl::NDRange> _LocalDeltaWeightKernelNDRanges;

    std::vector<cl::Event> KernelWaitEvent;

};

#endif // GPUNETWORK_H

