#include "../include/NeuronDatareader.h"

NeuronDatareader::NeuronDatareader(char * file)
{
    std::cout << file << std::endl;
    //ctor
}

std::vector<std::string> & _split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        std::cout << item << std::endl;
        elems.push_back(item);
    }
    return elems;
}

NeuronDatareader::NeuronDatareader(std::initializer_list<std::initializer_list< int>> data, std::initializer_list<std::initializer_list< int>> labels)
{
    try
    {
        if(data.size() != labels.size())
        {
            throw _EXP_FLAG_DATA;
        }

        _nmb_items = data.size();
        _lnmb_items = labels.size();

        _ldimensions = 1;
        _dimensions = 1;
        _size[0]  = _size[1] =_size[2] = 1;
        _inputsize = _size[0]  = (*data.begin()).size();
        _lsize[0] = _lsize[1] =_lsize[2] = 1;
        _linputsize = _lsize[0] = (*labels.begin()).size();
        _item_size = 1;
        _litem_size = 1;
        _item_type = 1;
        _litem_type = 1;

        Filetype value;
        Imagetype img;
        for(std::initializer_list<std::initializer_list< int>>::iterator it_data_=data.begin(), it_label_=labels.begin(); it_data_ !=data.end(); it_data_++, it_label_++)
        {
            img.val = new Filetype[_inputsize];
            int run_=0;
            for(std::initializer_list< int>::iterator it_data_in = (*it_data_).begin(); it_data_in != (*it_data_).end(); it_data_in++)
            {
                value.sfloat = (float)*it_data_in;
                img.val[run_] = value;
                run_++;
            }
            img.img_size = _size[0]*_size[1]*_size[2];
            _data.push_back(img);

            PrintValue(_data.back());


            img.val = new Filetype[_linputsize];
            run_=0;
            int label_val = 0;
            for(std::initializer_list< int>::iterator it_label_in = (*it_label_).begin(); it_label_in != (*it_label_).end(); it_label_in++)
            {
                value.sfloat = (float)*it_label_in;
                img.val[run_] = value;
                run_++;
                //label_val += (*it_label_in)?(run_):(0);
            }
            img.img_size = _lsize[0]*_lsize[1]*_lsize[2];

            _ldata_vec.push_back(img);
            //_ldata.push_back(label_val-1);
        }



        _indexes.resize(_ldata_vec.size());
        for (unsigned int i = 0; i < _indexes.size(); ++i)
            _indexes.at(i) = i;

        std::random_shuffle(_indexes.begin(), _indexes.end());

    }
    catch(std::exception & exp)
    {
        std::cout << exp.what() << std::endl;
    }

}


NeuronDatareader::NeuronDatareader(char * file, char * labels)
{
    try
    {
        _file = _split(file, '.', _file);
        _label = _split(labels, '.', _label);

        if(!std::strcmp(_file.at(1).c_str(),"idx3-ubyte"))
        {
            if(!std::strcmp(_label.at(1).c_str(),"idx1-ubyte"))
            {
                _filetype = Fileformat::idx;
            }
        }
        _dimensions = 0;
        _size[0]=_size[1]=_size[2] = 1;
        _lsize[0]=_lsize[1]=_lsize[2] = 1;
        std::fstream filestream;
        std::fstream labelstream;
        filestream.open(file, std::ios::in|std::ios::binary);
        labelstream.open(labels, std::ios::in|std::ios::binary);
        bool open = filestream.is_open()&&labelstream.is_open();
        std::cout <<"IsOpen? "<< open<<" " <<file <<" " << labels<<std::endl;
        std::cout <<"Stream Read!"<<std::endl;
        std::cout <<"Get Header Information!"<< std::endl;
        _magicnumber =  (filestream.get()<<24)|(filestream.get()<<16)|(filestream.get()<<8)|filestream.get();
        _nmb_items = (filestream.get()<<24)|(filestream.get()<<16)|(filestream.get()<<8)|filestream.get();
        _dimensions = _magicnumber&(0x000000ff);
        _item_type = (_magicnumber&(0x0000ff00))>>8;

        _lmagicnumber = (labelstream.get()<<24)|(labelstream.get()<<16)|(labelstream.get()<<8)|labelstream.get();
        _lnmb_items = (labelstream.get()<<24)|(labelstream.get()<<16)|(labelstream.get()<<8)|labelstream.get();
        _ldimensions = _lmagicnumber&(0x000000ff);
        _litem_type = (_lmagicnumber&(0x0000ff00))>>8;

        switch(_dimensions)
        {
            case 0: //scalar
            _dimensions = 0;
            break;
            case 1: //1D
            _dimensions = 1;
            break;
            case 2: //2D
            _dimensions = 2;
            break;
            case 3: //3D
            _dimensions = 2;
            break;
        }

        switch(_ldimensions)
        {
            case 0: //scalar
            _ldimensions = 0;
            break;
            case 1: //1D
            _ldimensions = 0;
            break;
            case 2: //2D
            _ldimensions = 2;
            break;
            case 3: //3D
            _ldimensions = 2;
            break;
        }

        switch(_item_type)
        {
            case 0x08:
                //unsigned byte
                _item_size = 8;
                break;
            case 0x09:
                //signed byte
                _item_size = 8;
                break;
            case 0x0b:
                //short
                _item_size = 16;
                break;
            case 0x0c:
                //int
                _item_size = 32;
                break;
            case 0x0d:
                //float
                _item_size = 32;
                break;
            case 0x0e:
                //double
                _item_size = 64;
                break;
        }

        switch(_litem_type)
        {
            case 0x08:
                //unsigned byte
                _litem_size = 8;
                break;
            case 0x09:
                //signed byte
                _litem_size = 8;
                break;
            case 0x0b:
                //short
                _litem_size = 16;
                break;
            case 0x0c:
                //int
                _litem_size = 32;
                break;
            case 0x0d:
                //float
                _litem_size = 32;
                break;
            case 0x0e:
                //double
                _litem_size = 64;
                break;
        }


        for(int c=0; c <_dimensions; c++)
        {
            _size[c] = (filestream.get()<<24)|(filestream.get()<<16)|(filestream.get()<<8)|filestream.get();
        }

        for(int c=0; c <_ldimensions; c++)
        {
            _lsize[c] = (filestream.get()<<24)|(filestream.get()<<16)|(filestream.get()<<8)|filestream.get();
        }

        _inputsize = _size[0]*_size[1]*_size[2];
        Filetype value;
        Imagetype img;
        int cnt=0;
        int img_index = 0;
        for(int dataset_cnt = 0; dataset_cnt < _nmb_items; dataset_cnt++,cnt++)
        {
            img.val = new Filetype[_inputsize];
            memset(img.val, 0, _inputsize*sizeof(Filetype));//Set Image to 0

            for(int dep_c=0; dep_c < _size[2]; dep_c++) //Z
            {
                for(int col_c=0; col_c < _size[1]; col_c++) //Y
                {
                    for(int row_c = 0; row_c <_size[0]; row_c++) //X
                    {
                        memset(&value, 0, sizeof(Filetype)); //SetPixel to 0
                        int shift = 0;
                        unsigned char bla = 0;
                        for(int val_c = 0; val_c < _item_size/8 ; val_c++)//Read Byte-wise
                        {
                            shift = (_item_size-8-(val_c*8));
                            bla = filestream.get();
                            value.ulong |= ((unsigned long)bla)<< shift;  //SetPixel
                        }
                        img_index = row_c+col_c*_size[0]+dep_c*_size[0]*_size[1];
                        value.sfloat = value.ulong/(std::pow(2, _item_size)-1.0f);//norm
                        img.val[img_index] = value; //Save Pixel in image
                    }
                }
            }
            img.img_size = _size[0]*_size[1]*_size[2];
            _data.push_back(img);//Pushback in data-array
        }
        /*
        for(int c2 = 0; c2 < _size[1]; c2++)
        {
            for(int c1=0; c1 < _size[0]; c1++)
            {
                int val = (_data.at(0).val[c1+c2*_size[0]].uchar);
                std::cout << std::setw(2) << std::setfill('0') << std::hex << (val);
            }
            std::cout << std::endl;
        }
        */
        _linputsize = 10;
        unsigned char label_data = 0;
        Imagetype label_img;
        for(int dataset_cnt = 0; dataset_cnt < _lnmb_items; dataset_cnt++)
        {
            label_img.val =  new Filetype[_linputsize];
            label_img.img_size = _linputsize;
            label_data=labelstream.get();
            _ldata.push_back(label_data);
            //label_vec
            for(int index = 0; index < _linputsize; index++)
            {

                value.sfloat = (label_data == index)?(1.0f):(0.0f);
                label_img.val[index] = value;
            }

            _ldata_vec.push_back(label_img);
        }

        _indexes.resize(_ldata.size());
        for (unsigned int i = 0; i < _indexes.size(); ++i)
            _indexes.at(i) = i;

        std::random_shuffle(_indexes.begin(), _indexes.end());

        std::cout << "Feddich!" << std::endl;
    }
    catch(std::exception & exp)
    {
        std::cout << exp.what() << std::endl;
        throw _EXP_FLAG_READ;
    }
    //ctor
}


int NeuronDatareader::power(int number, int index) {
    if (index == 0) {
        return 1;
    }
    int num = number;
    for (int i = 1; i < index; i++) {
        number = number * num;
    }
    return number;
}

NeuronDatareader::~NeuronDatareader()
{
    //dtor
}

