#include "Neuron.h"


Neuron::Neuron()
{
    //ctor
}

Neuron::~Neuron()
{
    //dtor
}

Neuron::Neuron(const Neuron& other)
{
    //copy ctor
}

Neuron& Neuron::operator=(const Neuron& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}
