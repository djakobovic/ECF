#ifndef CGPMul_h
#define CGPMul_h
#include "Function.h"

namespace Cartesian
{
	template <class T>
	class MulT : public Function
	{
	public:
		MulT();
		~MulT();

		void evaluate(std::vector<T> &inputs, T &result);
	};

	typedef MulT<double> Mul;
	template <class T>
	MulT<T>::MulT(){
		name_ = "*";
		nArguments_ = 2;
	}

	template <class T>
	MulT<T>::~MulT()
	{}

	template <class T>
	void MulT<T>::evaluate(std::vector<T> &inputs, T &result)
	{
		double product = 1;
		for(uint i = 0; i < nArguments_; ++i)
		{
		   product *= inputs[i];
		}
		result = product;
	}





}
#endif  //Mul_h
