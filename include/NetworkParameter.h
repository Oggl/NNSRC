#ifndef NETWORKREGION_H
#define NETWORKREGION_H

#include <cstdarg>
#include <initializer_list>
#include <iostream>
#include <vector>

class NetworkParameter
{
    public:
        NetworkParameter(std::initializer_list<unsigned int> regions);
        virtual ~NetworkParameter();
        NetworkParameter(const NetworkParameter& other);
        NetworkParameter& operator=(const NetworkParameter& other);
        void SetLayerCount(unsigned int cnt);
        void SetNeuronCntLayer(unsigned int layer, unsigned int ncnt);
        void ResizeNetworkRegion(std::initializer_list<unsigned int> args);
        unsigned int GetLayerCount();
        unsigned int GetNeuronsLayer(unsigned int layer);
    protected:
    private:
        unsigned int _layers;
        std::vector <unsigned int> _NeuronsPerLayer;

};

#endif // NETWORKREGION_H
