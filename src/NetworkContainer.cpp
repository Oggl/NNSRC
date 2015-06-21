/*
#include "NetworkContainer.h"

template
NetworkContainer::NetworkContainer(std::vector<unsigned int> networksize)
{
    std::cout << "Container" << std::endl;

    _ncount = 0;
    for(int c = 0; c <  networksize.size(); c++)
        _ncount += networksize.at(c);

    _out.resize(_ncount);

    //ctor
}

NetworkContainer::~NetworkContainer()
{
    //dtor
}

NetworkContainer::NetworkContainer(const NetworkContainer& other)
{
    std::cout << "Container" << std::endl;
    //copy ctor
}

NetworkContainer& NetworkContainer::operator=(const NetworkContainer& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

*/
