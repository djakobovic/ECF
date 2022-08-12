#ifndef Mul_h
#define Mul_h
#include "Function.h"

namespace cart
{
	template <class T>
	class Mul : public Function
	{
	public:
		Mul();
		Mul(uint numArgs);
		~Mul();

		void evaluate(voidP inputs, void* result);
	};

	typedef Mul<double> MulDouble;
	typedef Mul<int> MulInt;

	template <class T>
	Mul<T>::Mul()
	{
		name_ = "*";
		numOfArgs_ = 2;
	}

	template <class T>
	Mul<T>::Mul(uint numArgs)
	{
		name_ = "*";
		numOfArgs_ = numArgs;
	}

	template <class T>
	Mul<T>::~Mul()
	{
	}

	template <class T>
	void Mul<T>::evaluate(voidP inputs, void* result)
	{
		T& prod = *(T*) result;
		prod = 0;
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

		prod = readInputs.at(0);
		for (int i = 1; i < (int)numOfArgs_; i++)
		{
			prod *= readInputs.at(i);
		}
	}

}

#endif /* Mul_h */