#ifndef NEURON_H
#define NEURON_H

#include "../Neuron_Functors.h"

class Neuron
{
    public:
        Neuron();
        virtual ~Neuron();
        Neuron(const Neuron& other);
        Neuron& operator=(const Neuron& other);
    protected:
    private:
        unsigned int _id;
};

#endif // NEURON_H
