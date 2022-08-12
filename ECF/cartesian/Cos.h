#ifndef Cos_h
#define Cos_h
#include "Function.h"
#include <cmath>

namespace cart
{
	template <class T>
	class Cos : public Function
	{
	public:
		Cos();
		~Cos();

		void evaluate(voidP inputs, void* result);
	};

	typedef Cos<double> CosDouble;
	typedef Cos<int> CosInt;

	template <class T>
	Cos<T>::Cos()
	{
		name_ = "cos";
		numOfArgs_ = 1;
	}

	template <class T>
	Cos<T>::~Cos()
	{
	}

	template <class T>
	void Cos<T>::evaluate(voidP inputs, void* result)
	{
		T& data = *(T*) result;
		stringstream ss;
		ss << *((string*) inputs.get());
		vector<T> readInputs;
		T input;
		uint i = 0;
		while (ss >> input)
		{
			readInputs.push_back(input);
			++i;
			if (i == numOfArgs_)
			{
				break;
			}
		}

		data = (T)cos((double)readInputs.at(0));
	}

}

#endif /* Cos_h */