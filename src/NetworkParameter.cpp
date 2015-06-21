#include "../include/NetworkParameter.h"

NetworkParameter::NetworkParameter(std::initializer_list<unsigned int> regions): _layers(regions.size()), _NeuronsPerLayer(regions)
{

}

NetworkParameter::~NetworkParameter()
{
    //dtor
}


NetworkParameter::NetworkParameter(const NetworkParameter& other)
{
    //copy ctor
}


NetworkParameter & NetworkParameter::operator=(const NetworkParameter& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void NetworkParameter::SetLayerCount(unsigned int cnt)
{
    _layers = cnt;
    _NeuronsPerLayer.resize(cnt);
}

void NetworkParameter::SetNeuronCntLayer(unsigned int layer, unsigned int cnt)
{
    if(layer < _layers)
        _NeuronsPerLayer.at(layer) = cnt;

}

void NetworkParameter::ResizeNetworkRegion(std::initializer_list<unsigned int> region)
{
    _layers =  region.size();
    _NeuronsPerLayer = region;
}
