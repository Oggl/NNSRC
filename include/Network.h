#ifndef NETWORK_H
#define NETWORK_H

#include "Neuron.h"
#include "NetworkParameter.h"
#include <vector>
#include <tuple>
#include "../Neuron_Functors.h"
#include "NetworkContainer.h"
#include <initializer_list>
#include <memory>
#include <random>
#include <algorithm>
#include "NeuronDatareader.h"

template <typename Datatype, typename NFunctor>
class Network
{
    public:
		typedef NFunctor ActivationFunctionType;
        typedef NetworkContainer<ActivationFunctionType,Datatype> nContainer;
		typedef std::tuple<unsigned int, unsigned int, Datatype> nConnection;

        Network(std::initializer_list<unsigned int> regions, std::vector<nConnection> connections, unsigned int flags):
            _networkRegion(regions),
            _connect(connections),
            _nFunction(),
            _networkContainer(_networkRegion, _connect, flags)
        {

            _flags = flags;
            _output.resize(_networkRegion.back());
            Datatype test = 5.0;
            _nFunction(5.0);
            std::cout << _nFunction(5);
        }

        Network(std::initializer_list<unsigned int> regions, unsigned int flags):
            _networkRegion(regions),
            _nFunction(),
            _networkContainer(_networkRegion, flags)
            //_connect(_networkContainer.GetConnections())
        {
            _flags = flags;
            _output.resize(_networkRegion.back());
        }

        virtual ~Network(){};
        Network(const Network& other)
        {


        };
        Network& operator=(const Network& rhs){
            if (this == &rhs) return *this; // handle self assignment
            //assignment operator
            return *this;
        };

        void SetInput(std::initializer_list<unsigned int> regions, std::initializer_list<nConnection> connections)
        {
        }

        void Allocate()
        {
            try
            {
                //_networkContainer.reset(new NetworkContainer<NFunctor>(_networkRegion, _connect));
            }
            catch(std::exception _exp)
            {
                std::cout << exp <<std::endl;
            }

            //_networkContainer(_networkRegion, _connect);
            //_networkContainer.get()->
        };

        void PrintOutput() const
        {
            for(const auto & elem : _output)
            {
                std::cout << elem << " ";
            }
            std::cout << std::endl;
        }

        void Update()
        {
            try
            {
                int nsize = _networkRegion.size();
                for(int lcnt_=1; lcnt_ <_networkRegion.size() ;lcnt_++ )
                {
                    int layers = _networkContainer.GetLayerStart(lcnt_);
                    int layere = _networkContainer.GetLayerEnd(lcnt_);
                    for(int ncnt_ = _networkContainer.GetLayerStart(lcnt_); ncnt_ < _networkContainer.GetLayerEnd(lcnt_);ncnt_++)
                    {
                        int connOffset_ = _networkContainer.GetConnOffset(ncnt_);
                        int run_ = connOffset_+_networkContainer.GetPreCnt(ncnt_);
                        Datatype val = 0;

                        for(int cnt_i =connOffset_; cnt_i < run_ ; cnt_i++)
                        {
                            int in = _networkContainer.GetInputs(cnt_i);
                            Datatype out = _networkContainer.GetOutput(in);
                            Datatype w = _networkContainer.GetWeight(cnt_i);

                           val += out*w ;

                            //std::cout <<
                            //val += _networkContainer.GetOutput(cnt_i)*_networkContainer.GetWeight(cnt_i);
                        }
                        val+=_networkContainer.GetBias(ncnt_);
                        _networkContainer.SetZValue(val, ncnt_);
                        _networkContainer.SetNeuronValue(_nFunction(val),ncnt_);
                    }
                }



                for(int ncnt_ = _networkContainer.GetLayerStart(_networkRegion.size()-1),run=0; ncnt_ < _networkContainer.GetLayerEnd(_networkRegion.size()-1); ncnt_++, run++)
                {
                    _output.at(run) = _networkContainer.GetNeuronValue(ncnt_);
                }
            }
            catch(std::exception & exp)
            {
                std::cout << exp.what() << std::endl;
            }
        };

        Datatype * GetResult()
        {
            return _networkContainer.GetElemValPtr(_networkContainer.GetResNid());
        };

        void SetInput(std::initializer_list<Datatype> input)
        {
            if(input.size()==_networkRegion.at(0))
            {
                int cnt = 0;
                for (const auto& elem : input)
                {
                    _networkContainer.SetNeuronValue(elem, cnt);
                    cnt++;
                }
            }
        }

        void SetInput(Imagetype * input)
        {
            if(input->img_size==_networkRegion.at(0))
            {
                int cnt = 0;
                int s =  input->img_size;
                for(; cnt < input->img_size; cnt++)
                {
                    Datatype bla = input->val[cnt].sfloat;
                    //std::cout << bla <<" " <<_nFunction(input->val[cnt].sfloat)<<std::endl;
                    _networkContainer.SetNeuronValue(input->val[cnt].sfloat, cnt);
                }
            }
        }

        void SetOutput(unsigned int output)
        {
            //Unsigned int for output -> example: value is 5, therefore set 5th output to 1. Remaining outputs to 0
            unsigned int nlimit = _networkContainer.GetNeuronCount();
            for(int n_cnt =  _networkContainer.GetResNid(),run = 0; n_cnt < nlimit; n_cnt++, run++)
            {
                _networkContainer.SetNeuronValue(0, n_cnt);
            }
            unsigned int pos =  _networkContainer.GetResNid();
            pos += output - 1;
            _networkContainer.SetNeuronValue(1, pos);
        }

        void TrainWithGD(NeuronDatareader * data, float eta, int batch_size) //Gradient Descent
        {

            try
            {
                _alpha =  0.4f;
                _data =  data;
                _eta = eta;
                bool done = false;
                _batch_size = batch_size;
                _batch_val = batch_size;
                //auto data_elem = data->
                _delta.resize(_networkContainer.GetNeuronCount()); //Neuron
                _nabla_w.resize(_networkContainer.GetConnCount());
                _nabla_b.resize(_networkContainer.GetNeuronCount());
                _delta_nabla_w.resize(_networkContainer.GetConnCount());
                _delta_nabla_b.resize(_networkContainer.GetNeuronCount());
                _mom_nabla_w.resize(_networkContainer.GetConnCount());
                _mom_nabla_b.resize(_networkContainer.GetNeuronCount());
                std::fill(_mom_nabla_b.begin(), _mom_nabla_b.end(), 0.0f);
                std::fill(_mom_nabla_w.begin(), _mom_nabla_w.end(), 0.0f);
                unsigned int iter = 0;
                while(!done) //GD_Iter
                {
                    std::fill(_delta_weight.begin(), _delta_weight.end(), 0.0f);

                    //int batch_size = data->GetIndex()->size()/data->GetIndex()->size();

                    std::fill(_nabla_b.begin(), _nabla_b.end(), 0.0f);
                    std::fill(_nabla_w.begin(), _nabla_w.end(), 0.0f);
                    iter++;
                    std::cout << data->GetIndex()->size() << std::endl;
                    for(int c_in = 0; c_in < data->GetIndex()->size(); c_in++ ) //for_each input
                    {
                        std::cout << std::dec <<iter << "------"<<std::endl;
                        std::fill(_delta.begin(), _delta.end(), 0.0f);
                        std::fill(_delta_nabla_b.begin(), _delta_nabla_b.end(), 0.0f);
                        std::fill(_delta_nabla_w.begin(), _delta_nabla_w.end(), 0.0f);

                        std::cout << "Input: ";
                        data->PrintValue(data->GetData()->at(c_in));
                        SetInput(& data->GetData()->at(c_in));//SEt Train Input
                        Update(); //Execute NN for Output
                        std::cout << "Output: ";
                        PrintOutput();

                        CalcDelta(data->GetLabelVector()->at(c_in));
                        //for each neuron in last layer
                        for(int lcnt_next = _networkRegion.size()-1, lcnt_this = _networkRegion.size()-2; lcnt_this>0; lcnt_next--, lcnt_this--)
                        {
                            for(int next_n = _networkContainer.GetLayerStart(lcnt_next); next_n < _networkContainer.GetLayerEnd(lcnt_next); next_n++)
                            {
                                int connOffset_ = _networkContainer.GetConnOffset(next_n);
                                int run_ = connOffset_+_networkContainer.GetPreCnt(next_n);

                                for(int this_n = _networkContainer.GetLayerStart(lcnt_this), cnt_i = connOffset_; this_n < _networkContainer.GetLayerEnd(lcnt_this); this_n++, cnt_i++)
                                {
                                    _delta.at(this_n) += _nFunction.prime(_networkContainer.GetZValue(this_n))*
                                                        _networkContainer.GetWeight(_networkContainer.GetInputs(cnt_i))*_delta.at(next_n);
                                }
                            }
                        }

                        for(int run_l=1; run_l <_networkRegion.size() ;run_l++ )
                        {
                            for(int this_n = _networkContainer.GetLayerStart(run_l); this_n < _networkContainer.GetLayerEnd(run_l); this_n++)
                            {
                                _delta_nabla_b.at(this_n) = _delta.at(this_n);
                            }
                        }

                        //std::cout << std::endl;
                        //Back_Iteration
                        for(int run_l=1; run_l <_networkRegion.size() ;run_l++ )
                        {
                            for(int this_n = _networkContainer.GetLayerStart(run_l); this_n < _networkContainer.GetLayerEnd(run_l); this_n++)
                            {
                                int connOffset_ = _networkContainer.GetConnOffset(this_n);
                                int run_ = connOffset_+_networkContainer.GetPreCnt(this_n);
                                //Get each connection to pre layer
                                for(int this_i =connOffset_; this_i < run_; this_i++)
                                {
                                    _delta_nabla_w.at(this_i) = _delta.at(this_n)*_networkContainer.GetNeuronValue(_networkContainer.GetInputs(this_i));
                                }
                            }
                        }

                        for(int cnt_n = 0; cnt_n < _networkContainer.GetNeuronCount(); cnt_n++)
                        {
                            _nabla_b.at(cnt_n) +=_delta_nabla_b.at(cnt_n);// std::fabs(_delta_nabla_b.at(cnt_n));
                        }

                        for(int cnt_i = 0; cnt_i  < _networkContainer.GetConnCount(); cnt_i++)
                        {
                            _nabla_w.at(cnt_i) += _delta_nabla_w.at(cnt_i);//std::fabs(_delta_nabla_w.at(cnt_i));
                        }

                        if(!((c_in+1)%_batch_size))
                        {

                            for(int cnt_n = 0; cnt_n < _networkContainer.GetNeuronCount(); cnt_n++)
                            {
                                Datatype val = _networkContainer.GetBias(cnt_n);
                                Datatype change = _eta/_batch_val;
                                Datatype nabla_b_val = _nabla_b.at(cnt_n);
                                _networkContainer.SetBias(_networkContainer.GetBias(cnt_n) + change*_nabla_b.at(cnt_n),cnt_n);
                                _mom_nabla_b.at(cnt_n) = _nabla_b.at(cnt_n);
                            }

                            for(int cnt_i = 0; cnt_i  < _networkContainer.GetConnCount(); cnt_i++)
                            {
                                Datatype val = _networkContainer.GetWeight(cnt_i);
                                _networkContainer.SetWeight(_networkContainer.GetWeight(cnt_i) + ((_eta/_batch_val)*_nabla_w.at(cnt_i)), cnt_i);
                                _mom_nabla_w.at(cnt_i) = _nabla_w.at(cnt_i);
                            }
                            iter++;
                        }
                    //_eta = (1.0f -_alpha)*std::pow(0.99f, iter/100.0f);
                    }
                }
            }
            catch(std::exception & exp)
            {
                std::cout << exp.what();
            }
        }



        inline void PrintNetwork()
        {
            _networkContainer.PrintNetwork();
        }

        void Backpropagation()
        {

        }

        void CalcDelta(Imagetype label)
        {
            /*
            std::vector<float> _label_vec;
            _label_vec.resize(_delta.size());
            std::fill(_label_vec.begin(),_label_vec.end(), 0);

            _label_vec.at(_networkContainer.GetResNid()+ label) = 1.0f;
            */

            std::vector<Datatype> _label_vec;
            _label_vec.resize(_delta.size());
            std::cout << "Desired: ";
            for(int  ncnt_ = _networkContainer.GetResNid(), run = 0; ncnt_ < _delta.size(); ncnt_++, run++)
            {
                std::cout << (_label_vec.at(ncnt_) = label.val[run].sfloat) << " ";
            }
            std::cout << std::endl;
            std::cout << "Delta: ";
            for(int ncnt_ = _networkContainer.GetResNid(); ncnt_ < _delta.size(); ncnt_++)
            {

                Datatype nval = _networkContainer.GetNeuronValue(ncnt_);
                Datatype lval = _label_vec.at(ncnt_);
                //Datatype zval = _networkContainer.GetZValue(ncnt_);
                Datatype diff = lval - nval;
                Datatype endv = diff * _nFunction.prime(_networkContainer.GetNeuronValue(ncnt_));
                _delta.at(ncnt_) =  (_label_vec.at(ncnt_) - _networkContainer.GetNeuronValue(ncnt_))
                                   *_nFunction.prime(_networkContainer.GetNeuronValue(ncnt_));//(_networkContainer.GetNeuronValue(ncnt_));
                Datatype dval =  endv;//_delta.at(ncnt_);
                std::cout << "NVal: " << nval <<" LVal:"<< lval <<" endv:" << endv << std::endl;
                std::cout << dval << " ";
            }
            std::cout << std::endl;
        }

        void ExportNeuralNetwork(std::string filename)
        {

        }

        void ImportNeuralNetwork(std::string filename)
        {

        }



        std::vector<unsigned int> _networkRegion;
        std::vector<Datatype> _output;
        std::vector<Datatype> _delta_nabla_b;
        std::vector<Datatype> _delta_nabla_w;
        std::vector<Datatype> _nabla_b;
        std::vector<Datatype> _nabla_w;
        std::vector<Datatype> _delta;
        std::vector<Datatype> _delta_weight;
        std::vector<Datatype> _mom_nabla_w;
        std::vector<Datatype> _mom_nabla_b;
        std::vector<nConnection> _connect;
        std::vector<unsigned int> _layercnt;
        Datatype _eta;
        std::vector<Neuron> _neurons;
        nContainer _networkContainer;
        ActivationFunctionType _nFunction;
        NeuronDatareader * _data;
        float _alpha;
        int _batch_size;
        Datatype _batch_val;
        unsigned int _flags;
    private:
    protected:

};


#endif // NETWORK_H

