#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H

#include <iostream>
#include <limits>

template <typename Datatype, unsigned int INT, unsigned int FRAC_BITS>
class FixedPoint
{
    public:
        FixedPoint()
        {
        };

        FixedPoint(int in): _T((Datatype)in << FRAC_BITS)
        {
            //std::cout << "PRE:" << _PRE << " SUFF:" << _SUFF << std::endl;
            //std::cout << (_T>>_SUFF) << " "<<((_T <<_PRE)>>_PRE) << std::endl;
        };

        FixedPoint(double in)
        {
            double val = ((in * power2<FRAC_BITS>::val) + (in >= 0 ? .5 : -.5));

            _T = (Datatype) val;
            /*
            std::cout << "PRE:" << _PRE << " SUFF:" << _SUFF << std::endl;
            int pre = (_T>>_SUFF);
            int suff = ((_T <<_PRE)>>_PRE);
            std::cout << pre << " "<<suff << std::endl;
            */
        };

        FixedPoint(float in)
        {
            _T = (Datatype)(in * power2<FRAC_BITS>::val + (in >= 0 ? .5 : -.5));
            //std::cout << *this << std::endl;
            /*
            std::cout << "PRE:" << _PRE << " SUFF:" << _SUFF << std::endl;
            std::cout << (_T>>_SUFF) << " "<<((_T <<_PRE)>>_PRE) << std::endl;*/
        };

        FixedPoint(unsigned int in): _T((Datatype)in << FRAC_BITS)
        {
            /*
            std::cout << "PRE:" << _PRE << " SUFF:" << _SUFF << std::endl;
            std::cout << (_T>>_SUFF) << " "<<((_T <<_PRE)>>_PRE) << std::endl;
            */
        };

        FixedPoint(unsigned char in): _T((Datatype)in << FRAC_BITS)
        {
            /*
            std::cout << "PRE:" << _PRE << " SUFF:" << _SUFF << std::endl;
            std::cout << (_T>>_SUFF) << " "<<((_T <<_PRE)>>_PRE) << std::endl;
            */
        };

        FixedPoint(char in): _T((Datatype)in << FRAC_BITS)
        {
            /*
            std::cout << "PRE:" << _PRE << " SUFF:" << _SUFF << std::endl;
            std::cout << (_T>>_SUFF) << " "<<((_T <<_PRE)>>_PRE) << std::endl;
            */
        };

        FixedPoint<Datatype,INT,FRAC_BITS> & operator = (FixedPoint<Datatype, INT,FRAC_BITS> const& rhs)
        {
            _T = rhs._T;
            return *this;
        }

        FixedPoint<Datatype,INT,FRAC_BITS> & operator += (FixedPoint<Datatype, INT,FRAC_BITS> const& rhs)
        {
            _T += rhs._T;
            return *this;
        }


        FixedPoint<Datatype,INT,FRAC_BITS> & operator + (FixedPoint<Datatype, INT,FRAC_BITS> const& rhs)
        {
            FixedPoint<Datatype,INT,FRAC_BITS> temp = *this;
            temp._T += rhs._T;
            return (temp);
        }

        FixedPoint<Datatype,INT,FRAC_BITS> & operator -(const FixedPoint& rhs)
        {
            FixedPoint<Datatype,INT,FRAC_BITS> temp = *this;
            temp._T -= rhs._T;
            return (temp);
        }

        FixedPoint<Datatype,INT,FRAC_BITS> operator-()
        {
            FixedPoint<Datatype,INT,FRAC_BITS> result;
            result._T = -_T;
            return result;
        }

        FixedPoint<Datatype,INT,FRAC_BITS> & operator*(const FixedPoint& rhs)
        {
            FixedPoint<Datatype,INT,FRAC_BITS> temp = *this;
            temp._T = ((static_cast< typename FixedPoint<Datatype,INT,FRAC_BITS>::template
				promote_type<Datatype>::type> (temp._T))* rhs._T)>>FRAC_BITS;
            return (temp);
        }

        FixedPoint<Datatype,INT,FRAC_BITS> & operator/(const FixedPoint& rhs)
        {
            FixedPoint<Datatype,INT,FRAC_BITS> temp = *this;
            temp._T = (((static_cast< typename FixedPoint<Datatype,INT,FRAC_BITS>::template
				promote_type<Datatype>::type> (temp._T))<<FRAC_BITS)/rhs._T);
            return (temp);
        }

        friend FixedPoint<Datatype,INT,FRAC_BITS> floor(FixedPoint<Datatype,INT,FRAC_BITS> in)
        {
            FixedPoint<Datatype,INT,FRAC_BITS> res;
            res._T = in._T & ~(power2<FRAC_BITS>::val-1);
            return res + FixedPoint<Datatype,INT,FRAC_BITS>(
			in._T & (power2<FRAC_BITS>::val-1) ? 1 : 0);
        }

        operator unsigned int()
        {
            return (unsigned int)(_T >> _SUFF);
        }

        operator int()
        {
            return (int)(_T >> _SUFF);
        }

        operator unsigned char() const
        {
            return (unsigned char)(_T >> _SUFF);
        }

        operator char() const
        {
            return (char)(_T >> _SUFF);
        }

        operator float() const
        {
            float val = (float)_T/power2<FRAC_BITS>::val;
            return val;
        }

        operator double()
        {
            return (double) _T/power2<FRAC_BITS>::val;
        }

        bool operator <(FixedPoint<Datatype,INT,FRAC_BITS> const& rhs) const
        {
            return _T < rhs._T;
        }

        bool operator >(FixedPoint<Datatype,INT,FRAC_BITS> const& rhs) const
        {
            return _T > rhs._T;
        }

        FixedPoint<Datatype,INT,FRAC_BITS> & operator *=(
		FixedPoint<Datatype,INT,FRAC_BITS> const& rhs)
        {

            _T = (static_cast< typename FixedPoint<Datatype,INT,FRAC_BITS>::template
				promote_type<Datatype>::type>
			(_T) * rhs._T) >> FRAC_BITS;
            return *this;
        }

        FixedPoint<Datatype,INT,FRAC_BITS> & operator /=(
		FixedPoint<Datatype,INT,FRAC_BITS> const& rhs)
        {
            _T = (static_cast< typename FixedPoint<Datatype,INT,FRAC_BITS>::template
				promote_type<Datatype>::type>
			(_T) << FRAC_BITS) / rhs._T;
		return *this;
        }

        virtual ~FixedPoint(){};

        Datatype _T;
        const static unsigned int _PRE = INT;
        const static unsigned int _SUFF = FRAC_BITS;


        template<int P,typename T = void> struct power2
        {
            static const long long val = 2 * power2<P-1,T>::val;
        };

        template <typename P> struct power2<0, P>
        {
            static const long long val = 1;
        };

        friend FixedPoint<Datatype,INT,FRAC_BITS> exp(FixedPoint<Datatype,INT,FRAC_BITS> in)
        {
            FixedPoint<Datatype,INT,FRAC_BITS> a[] = {
			1.64872127070012814684865078781,
			1.28402541668774148407342056806,
			1.13314845306682631682900722781,
			1.06449445891785942956339059464,
			1.03174340749910267093874781528,
			1.01574770858668574745853507208,
			1.00784309720644797769345355976,
			1.00391388933834757344360960390,
			1.00195503359100281204651889805,
			1.00097703949241653524284529261,
			1.00048840047869447312617362381,
			1.00024417042974785493700523392,
			1.00012207776338377107650351967,
			1.00006103701893304542177912060,
			1.00003051804379102429545128481,
			1.00001525890547841394814004262,
			1.00000762942363515447174318433,
			1.00000381470454159186605078771,
			1.00000190735045180306002872525,
			1.00000095367477115374544678825,
			1.00000047683727188998079165439,
			1.00000023841860752327418915867,
			1.00000011920929665620888994533,
			1.00000005960464655174749969329,
			1.00000002980232283178452676169,
			1.00000001490116130486995926397,
			1.00000000745058062467940380956,
			1.00000000372529030540080797502,
			1.00000000186264515096568050830,
			1.00000000093132257504915938475,
			1.00000000046566128741615947508 };

            FixedPoint<Datatype,INT,FRAC_BITS> e(2.718281828459045);

            FixedPoint<Datatype,INT,FRAC_BITS> y(1);
            for (int i=FRAC_BITS-1; i>=0; --i)
            {
                if (!(in._T & 1<<i))
				y *= a[FRAC_BITS-i-1];
            }

            int x_int = (int)(floor(in));
            if (x_int<0)
            {
                for (int i=1; i<=-x_int; ++i)
                    y /= e;
            }
            else
            {
                for (int i=1; i<=x_int; ++i)
                    y *= e;
            }

            return y;
        }

        friend FixedPoint<Datatype,INT,FRAC_BITS> sqrt(
		/// The argument to the square root function, a nonnegative fixed-point
		/// value.
		FixedPoint<Datatype,INT,FRAC_BITS> in)
        {
            if (in < FixedPoint<Datatype,INT,FRAC_BITS>(0))
            {
                return 0;
            }

            typename FixedPoint<Datatype, INT, FRAC_BITS>::template
            promote_type<Datatype>::type op =
                static_cast<typename FixedPoint<Datatype, INT, FRAC_BITS>::template promote_type<Datatype>::type>(
                    in._T) << (INT - 1);


            typename FixedPoint<Datatype, INT, FRAC_BITS>::template promote_type<Datatype>::type res = 0;
            typename FixedPoint<Datatype, INT, FRAC_BITS>::template promote_type<Datatype>::type one =
                (typename FixedPoint<Datatype, INT, FRAC_BITS>::template promote_type<Datatype>::type)1 <<
				(std::numeric_limits<typename FixedPoint<Datatype, INT, FRAC_BITS>::template promote_type<Datatype>
					::type>::digits - 1);

		while (one > op)
			one >>= 2;

		while (one != 0)
		{
			if (op >= res + one)
			{
				op = op - (res + one);
				res = res + (one << 1);
			}
			res >>= 1;
			one >>= 2;
		}

		FixedPoint<Datatype, INT, FRAC_BITS> root;
		root._T = static_cast<Datatype>(res);
		return root;
	}

    friend std::ostream & operator<<(std::ostream& os, const FixedPoint& fp)
    {
        FixedPoint t = fp;
        double value = double(fp);
        os << value;
        return os;
    }

	//double _SUFF_POW2 = std::pow(2, FRAC_BITS);
    //double _SUFF_POW_NEG2 =  std::pow(2, -FRAC_BITS);

    private:
        //Type Promotion
        template<typename T, typename U=void>
        struct promote_type
		{
		};

        template<typename U>
		struct promote_type<signed int, U>
		{
			typedef signed long long type;
		};
};

#endif // FIXEDPOINT_H
