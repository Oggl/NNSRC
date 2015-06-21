#ifndef NEURONDATAREADER_H
#define NEURONDATAREADER_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include "../Neuron_Functors.h"
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <climits>

enum Fileformat
{
    idx = 0
};

union Filetype
{
    unsigned char uchar;
    signed char schar;
    short sshort;
    int sint;
    float sfloat;
    double sdouble;
    long slong;
    unsigned long ulong;
};

struct Imagetype
{
    Filetype * val;
    unsigned int img_size;
};


class NeuronDatareader
{
    public:
        NeuronDatareader(std::initializer_list<std::initializer_list< int>> data, std::initializer_list<std::initializer_list< int>> label);
        NeuronDatareader(char * file, char * label);
        NeuronDatareader(char * file);

        inline void PrintValue(unsigned int val)
        {
            int id = 0;
            unsigned char bla=0;
            //unsigned int blabla = 0;
            for(int z = 0; z < _size[2]; z++)
            {
                for(int y = 0; y <_size[1]; y++)
                {
                    for(int x = 0; x < _size[0]; x++)
                    {
                        id = x + y*_size[0] + z*_size[0]*_size[1];
                        bla = _data.at(val).val[id].uchar;
                        //blabla = bla;
                        std::cout << std::setw(3) << std::setfill('0') << std::hex  << int(bla);
                    }
                    std::cout << std::endl;
                }
            }
        }

        inline void PrintValue(const Imagetype & img)
        {
            int id = 0;
            unsigned char bla=0;
            for(int z = 0; z < _size[2]; z++)
            {
                for(int y = 0; y <_size[1]; y++)
                {
                    for(int x = 0; x < _size[0]; x++)
                    {
                        id = x + y*_size[0] + z*_size[0]*_size[1];
                        bla = img.val[id].sfloat * UCHAR_MAX;
                        //blabla = bla;
                        std::cout << std::setw(1) << std::setfill('0') << /*std::dec<< */int(bla)/UCHAR_MAX;
                    }
                    std::cout << std::endl;
                }
            }
        }

		int power(int base, int pow);

        inline std::vector<unsigned int> * GetLabelData()
        {
            return & _ldata;
        };

        inline std::vector<Imagetype> * GetLabelVector()
        {
            return & _ldata_vec;
        }

        inline std::vector<Imagetype> * GetData()
        {
            return & _data;
        };

        inline std::vector<unsigned int> * GetIndex()
        {
            return & _indexes;
        };

        inline unsigned int GetInputSize()
        {
            return _inputsize;
        };

        virtual ~NeuronDatareader();
    protected:
    private:
        std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
        Fileformat _filetype;
        int _dimensions;
        int _ldimensions;
        int _size[3];
        int _lsize[3];
        int _magicnumber;
        int _lmagicnumber;
        int _nmb_items;
        int _lnmb_items;
        int _inputsize;
        int _linputsize;
        int _item_size;
        int _litem_size;
        int _item_type;
        int _litem_type;

        std::vector<std::string> _file;
        std::vector<std::string> _label;
        std::vector<Imagetype> _data;
        std::vector<Imagetype> _ldata_vec;
        std::vector<unsigned int> _ldata;
        std::vector<unsigned int> _indexes;
};

#endif // NEURONDATAREADER_H
