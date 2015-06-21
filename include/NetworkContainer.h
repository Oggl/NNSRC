#ifndef NETWORKCONTAINER_H
#define NETWORKCONTAINER_H

#include <vector>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <random>
#include <ctime>
#include <cstdlib>

template <typename NFunctor, typename Datatype>
class NetworkContainer
{
    public:


        NetworkContainer(std::vector<unsigned int> networksize, std::vector<std::tuple<unsigned int, unsigned int, Datatype>> connections, unsigned int flags)
        {
            std::cout << "Container" << std::endl;
            _ncount = 0; //Number of neurons
            _conncount=0; //Number of connections
            _inputoffset = networksize.at(0); //Input layer
            _outputoffset = networksize.at(networksize.size()-1); //Output layer
            _layers = networksize.size(); //Number of layers
            _size = networksize;
            for(int cnt = 0; cnt < networksize.size(); cnt++) //Get number of neurons
            {
                _ncount += networksize.at(cnt);
            }

            //BIAS
            _lastNID = _ncount;
            _bias.resize(_ncount); //for each neuron one bias value
            _out.resize(_ncount); //for each neuron an output value
            _z_out.resize(_ncount);
            _preCnt.resize(_bias.size()); //Count of predecessors neurons
            _layer_start.resize(networksize.size()); //for each layer, start neuron
            _layer_end.resize(networksize.size()); //end neuron for each layer
            //Calculate ids
            for(int cnt = 0; cnt < connections.size(); cnt++) //iterate over all connections
            {
                _preCnt.at(std::get<1>(connections.at(cnt)))+=1; //get number of predecessors
            }

            for(int cnt = 0; cnt < _preCnt.size(); cnt++) //iterate over all predecessors
            {
                _conncount += _preCnt.at(cnt);
            }

            _weight.resize(_conncount);
            _inputs.resize(_conncount);

            unsigned int runparam = 0;
            for(int cnt = 0; cnt < _layers; cnt++)
            {
                _layer_start.at(cnt) = runparam;
                runparam = _layer_end.at(cnt) = runparam + networksize.at(cnt);
            }

            _lastLNID = _layer_start.back();
            _connOffset.resize(_ncount);
            runparam=0;
            for(int cnt = 0; cnt < _ncount; cnt++)
            {
                _connOffset.at(cnt) = runparam;
                runparam += _preCnt.at(cnt);
            }

            /*
            int conn_insert = 0;
            for(int cnt=0; cnt<_ncount; cnt++)
            {
                int run = _preCnt.at(cnt)+_connOffset.at(cnt);
                //std::cout << cnt << " " << run <<" " << _connOffset.at(cnt) << std::endl;
                for(int cnt_i =_connOffset.at(cnt); cnt_i < run ; cnt_i++)
                {
                    std::cout << cnt<<" " <<cnt_i << " " << _preCnt.at(cnt)<<" "<<run << std::endl;
                    //_inputs.at(i)
                    for(int c = 0; c <_conncount; c++ )
                    {
                        if(std::get<1>(connections.at(c))==cnt)
                        {
                            _inputs.at(conn_insert) = std::get<0>(connections.at(c));
                            conn_insert++;
                        }
                    }
                }

            }
            */
            int conn_insert = 0;
            for(int cnt=0; cnt<_ncount; cnt++)
            {
                for(int c = 0; c <_conncount; c++ )
                {
                    if(std::get<1>(connections.at(c))==cnt)
                    {
                        _inputs.at(conn_insert) = std::get<0>(connections.at(c));
                        _weight.at(conn_insert) = std::get<2>(connections.at(c));
                        conn_insert++;
                    }
                }
            }

            //Pre-Init weights and BIAS

            //std::srand(std::time(0));
            InitNetwork(flags);
            /*unsigned int seed = time(0);
            std::mt19937 gen(seed);


            if(flags&&_FL_INIT_RAND_) //HARd_RANDOM
            {
                for(int cnt = 0; cnt < _ncount; cnt++)
                {
                    _bias.at(cnt) = 2*float(gen())/float(gen.max())- 1.0f;
                }

                for(int cnt = 0; cnt < _weight.size(); cnt++)
                {
                    _weight.at(cnt) = 2*float(gen())/float(gen.max())- 1.0f;
                }
            }

            if(flags&&_FL_INIT_GAUSS_)
            {
                std::normal_distribution<float> distribution(0.0,0.25);


                for(int cnt = 0; cnt < _ncount; cnt++)
                {
                    _bias.at(cnt) = float(distribution(gen));
                }

                for(int cnt = 0; cnt < _weight.size(); cnt++)
                {
                    _weight.at(cnt) = float(distribution(gen));
                }
            }

            if(flags&&_FL_INIT_NW_)
            {
                for(int cnt = 0; cnt < _ncount; cnt++)
                {
                    _bias.at(cnt) = 2*float(gen())/float(gen.max())- 1.0f;
                }

                for(int cnt = 0; cnt < _weight.size(); cnt++)
                {
                    _weight.at(cnt) = 2*float(gen())/float(gen.max())- 1.0f;
                }

                unsigned int hidden = _ncount-networksize.at(0) - networksize.at(networksize.size()-1);

                float beta = powf(0.7f*hidden, 1.0f/(networksize.at(0)));

                //Now per Layer
                for(int cnt =1; cnt < _layers; cnt++)
                {
                    float norm = 0;
                    for(int cnt_n=_layer_start.at(cnt); cnt_n < _layer_end.at(cnt); cnt_n++ )
                    {
                        int end_c = _connOffset.at(cnt_n) + _preCnt.at(cnt_n);
                        for(int cnt_c = _connOffset.at(cnt_n); cnt_c < end_c; cnt_c++)
                        {
                            norm+= _weight.at(cnt_c)*_weight.at(cnt_c);
                        }
                        norm = sqrtf(norm);
                        for(int cnt_c = _connOffset.at(cnt_n); cnt_c < end_c; cnt_c++)
                        {
                            _weight.at(cnt_c) = beta * _weight.at(cnt_c)/norm;
                        }
                    }

                }
            }
            */
            std::cout << "end!" << std::endl;

        };

        NetworkContainer(std::vector<unsigned int> networksize, unsigned int flags)
        {
            if(flags&&_FL_CONN_FF_)//Feed-Forward
            {
                _ncount = 0; //Number of neurons
                _conncount = 0;
                _inputoffset = networksize.at(0); //Input layer
                _outputoffset = networksize.at(networksize.size()-1); //Output layer
                _layers = networksize.size(); //Number of layers

                for(int cnt = 0; cnt < networksize.size(); cnt++) //Get number of neurons
                {
                    _ncount += networksize.at(cnt);
                }

                _bias.resize(_ncount); //for each neuron one bias value
                _out.resize(_ncount); //for each neuron an output value
                _z_out.resize(_ncount);
                _preCnt.resize(_bias.size()); //Count of predecessors neurons
                _layer_start.resize(networksize.size()); //for each layer, start neuron
                _layer_end.resize(networksize.size()); //end neuron for each layer
                _size = networksize;

                unsigned int runparam = 0;
                _layer_start.at(0) = runparam;
                runparam = _layer_end.at(0) = runparam + networksize.at(0);

                for(int n_cnt = 0; n_cnt < networksize.at(0); n_cnt++)
                {
                    _preCnt.at(n_cnt) = 0;
                }

                for(int l_cnt = 1; l_cnt < _layers; l_cnt++)
                {
                    _layer_start.at(l_cnt) = runparam;
                    runparam = _layer_end.at(l_cnt) = runparam + networksize.at(l_cnt);
                    for(int n_cnt = _layer_start.at(l_cnt); n_cnt < _layer_end.at(l_cnt); n_cnt++)
                    {
                        _conncount+=networksize.at(l_cnt-1);
                        _preCnt.at(n_cnt) = networksize.at(l_cnt-1);
                    }

                }

                _weight.resize(_conncount);
                _inputs.resize(_conncount);

                _lastLNID = _layer_start.back();
                runparam=0;
                _connOffset.resize(_ncount);
                for(int lcnt = 1; lcnt < _layers; lcnt++)
                {
                    for(int cnt = _layer_start.at(lcnt); cnt < _layer_end.at(lcnt); cnt++)
                    {
                        _connOffset.at(cnt) = runparam;
                        runparam += _size.at(lcnt-1);
                    }
                }

                for(int lcnt = 1, conn_ = 0; lcnt < _layers; lcnt++)
                {
                    for(int cnt = _layer_start.at(lcnt); cnt < _layer_end.at(lcnt); cnt++)
                    {
                        for(int cnt_p = _layer_start.at(lcnt-1); cnt_p<_layer_end.at(lcnt-1); cnt_p++, conn_++)
                        {
                            _inputs.at(conn_) =  cnt_p;
                        }
                    }
                }

                std::cout << "Feddich!" << std::endl;
            }

            InitNetwork(flags);
        }

        virtual ~NetworkContainer()
        {

        };

        NetworkContainer(const NetworkContainer& other)
        {

        };


        NetworkContainer& operator=(const NetworkContainer& other)
        {

        };


        Datatype * GetElemValPtr(unsigned int nid)
        {
            return (_out.data()+nid);
        }

        unsigned int GetResNid()
        {
            return _lastLNID;
        }

        unsigned int GetLastNid()
        {
            return _lastNID;
        }

        inline unsigned int GetLayerStart(unsigned int id)
        {
            return _layer_start.at(id);
        }

        inline unsigned int GetLayerEnd(unsigned int id)
        {
            return _layer_end.at(id);
        }

        inline unsigned int GetPreCnt(unsigned int id)
        {
            return _preCnt.at(id);
        }

        inline unsigned int GetConnOffset(unsigned int id)
        {
            return _connOffset.at(id);
        }

        inline Datatype GetOutput(unsigned int id)
        {
            return _out.at(id);
        }

        inline unsigned int GetInputs(unsigned int id)
        {
            return _inputs.at(id);
        }

        inline Datatype GetWeight(unsigned int id)
        {
            return _weight.at(id);
        }

        inline void SetWeight(Datatype val, unsigned int id)
        {
            _weight.at(id) =  val;
        }

        inline void SetNeuronValue(Datatype val, unsigned int id)
        {
            _out.at(id) = val;
        }

        inline Datatype GetBias(unsigned int id)
        {
            return _bias.at(id);
        }

        inline void SetBias(Datatype val, unsigned int id)
        {
            _bias.at(id) =  val;
        }

        inline void SetZValue(Datatype val, unsigned int id)
        {
            _z_out.at(id) = val;
        }

        inline Datatype GetNeuronValue(unsigned int id)
        {
            return _out.at(id);
        }

        inline Datatype GetZValue(unsigned int id)
        {
            return _z_out.at(id);
        }

        inline unsigned int GetConnCount()
        {
            return _conncount;
        }

        inline unsigned int GetNeuronCount()
        {
            return _ncount;
        }

        inline unsigned int GetLastLayer()
        {
            return _layer_end.back();
        }

        inline unsigned int GetLayerCnt()
        {
            return _layers;
        }

        inline void PrintNetwork()
        {
            std::cout << "PrintNetwork" << std::endl;
            std::cout << "------------------------------" << std::endl;
            std::cout << "NetworkInformation:" <<std::endl;
            std::cout << "Layers: "<< _layers <<" Layer config: ";


            for(unsigned int&l : _size )
            {
                std::cout << l <<" ";
            }

            std::cout << std::endl << "Connections: " << _weight.size() << std::endl;
            std::cout << "------------------------------" << std::endl<<"------------------------------"<< std::endl;
            std::cout << "Overview:" << std::endl << "------------------------------" << std::endl;
            std::cout << "Layer 1:" << std::endl;

            unsigned int layer_ = 0;
            for(unsigned int ncnt_ = GetLayerStart(layer_); ncnt_ < GetLayerEnd(layer_); ncnt_++)
            {
                std::cout <<"|NID " << ncnt_ << std::endl;
            }
            std::cout << std::endl;

            for(layer_ = 1; layer_ < _layers; layer_++)
            {
                std::cout << "Layer "<< layer_+1<<":" << std::endl;
                for(unsigned int ncnt_ = GetLayerStart(layer_); ncnt_ < GetLayerEnd(layer_); ncnt_++)
                {
                    int end_c = _connOffset.at(ncnt_) + _preCnt.at(ncnt_);
                    std::cout << "|PreC: ";
                    for(int cnt_c = _connOffset.at(ncnt_); cnt_c < end_c; cnt_c++)
                    {
                         std::cout << cnt_c << ":"<< _weight.at(cnt_c) <<" ";
                    }
                    std::cout <<"|NID " << ncnt_ <<" B: " << _bias.at(ncnt_)<<" || " << std::endl;
                }
                std::cout <<std::endl;
            }
            std::cout <<std::endl;
        }
    protected:
    private:
        void InitNetwork(unsigned int flags)
        {
            unsigned int seed = time(0);
            //unsigned int seed = 0;
            std::mt19937 gen(seed);

            if(flags&_FL_INIT_RAND_) //HARd_RANDOM
            {
                for(int cnt = 0; cnt < _ncount; cnt++)
                {
                    _bias.at(cnt) = 2*float(gen())/float(gen.max())- 1.0f;
                }

                for(int cnt = 0; cnt < _weight.size(); cnt++)
                {
                    _weight.at(cnt) = 2*float(gen())/float(gen.max())- 1.0f;
                }

                for(int cnt = 0; cnt < _out.size(); cnt++)
                {
                    _out.at(cnt) = 2*float(gen())/float(gen.max())- 1.0f;
                }

                for(int cnt=0; cnt < _z_out.size(); cnt++)
                {
                    _z_out.at(cnt) = 2*float(gen())/float(gen.max())- 1.0f;
                }
            }

            if(flags&_FL_INIT_GAUSS_)
            {
                std::normal_distribution<float> distribution(0.0,0.25);


                for(int cnt = 0; cnt < _ncount; cnt++)
                {
                    _bias.at(cnt) = Datatype(distribution(gen));
                }

                for(int cnt = 0; cnt < _weight.size(); cnt++)
                {
                    _weight.at(cnt) = Datatype(distribution(gen));
                }
            }

            if(flags&_FL_INIT_NW_)
            {
                for(int cnt = 0; cnt < _ncount; cnt++)
                {
                    _bias.at(cnt) = Datatype(2* float(gen())/float(gen.max())- 1.0f);
                }

                for(int cnt = 0; cnt < _weight.size(); cnt++)
                {
                    _weight.at(cnt) = Datatype(2* float(gen())/float(gen.max())- 1.0f);
                }

                unsigned int hidden = _ncount-_size.at(0) - _size.at(_size.size()-1);

                Datatype beta = 0.7f*powf(hidden, 1.0f/(_size.at(0)));

                //Now per Layer
                for(int cnt =1; cnt < _layers; cnt++)
                {
                    Datatype norm_weight = 0;
                    Datatype norm_bias = 0;
                    for(int cnt_n=_layer_start.at(cnt); cnt_n < _layer_end.at(cnt); cnt_n++ ) //per Layer calculate norm
                    {
                        int end_c = _connOffset.at(cnt_n) + _preCnt.at(cnt_n);
                        for(int cnt_c = _connOffset.at(cnt_n); cnt_c < end_c; cnt_c++)
                        {
                            norm_weight+= _weight.at(cnt_c)*_weight.at(cnt_c);
                        }

                        norm_bias += _bias.at(cnt_n)*_bias.at(cnt_n);

                    }

                    norm_weight = sqrt(norm_weight);
                    norm_bias = sqrt(norm_bias);

                    for(int cnt_n=_layer_start.at(cnt); cnt_n < _layer_end.at(cnt); cnt_n++ ) //per Layer calculate norm
                    {
                        int end_c = _connOffset.at(cnt_n) + _preCnt.at(cnt_n);
                        for(int cnt_c = _connOffset.at(cnt_n); cnt_c < end_c; cnt_c++)
                        {
                            _weight.at(cnt_c) = beta * _weight.at(cnt_c)/norm_weight;
                        }
                        _bias.at(cnt_n) =  beta * _bias.at(cnt_n)/norm_bias;
                    }

                }
            }
            std::cout << "Init done!" << std::endl;
        }
        protected:
        std::vector<unsigned int> _size; //Layersizes 784, 30,10
        std::vector<unsigned int> _inputs; //Predecessor Nodes 0....784x..0|0,1,2,...783|x30|784....813|x10
        std::vector<unsigned int> _preCnt; //Predecessor Nodecount |0....784x....0|784|x30|30|x10
        std::vector<unsigned int> _connOffset; //Predecessor Offset 0....784x..0|0|x30|784|x10
        std::vector<unsigned int> _layer_start; //0|784|814
        std::vector<unsigned int> _layer_end; //784|814|824
        std::vector<Datatype> _weight; //Weight for predecessor inputs
        std::vector<Datatype> _out; //Output value
        std::vector<Datatype> _z_out;
        std::vector<Datatype> _bias; //bias_value

        unsigned int _ncount;
        unsigned int _layers;
        unsigned int _conncount;
        unsigned int _inputoffset;
        unsigned int _outputoffset;
        unsigned int _lastLNID;
        unsigned int _lastNID;
        unsigned int _flags;
};

#endif // NETWORKCONTAINER_H
