#ifndef Div_h
#define Div_h
#include "Function.h"

namespace cart
{
	template <class T>
	class Div : public Function
	{
	public:
		Div();
		Div(uint numArgs);
		~Div();

		void evaluate(voidP inputs, void* result);
	};

	typedef Div<double> DivDouble;
	typedef Div<int> DivInt;

	template <class T>
	Div<T>::Div()
	{
		name_ = "/";
		numOfArgs_ = 2;
	}

	template <class T>
	Div<T>::Div(uint numArgs)
	{
		name_ = "/";
		numOfArgs_ = numArgs;
	}

	template <class T>
	Div<T>::~Div()
	{
	}

	template <class T>
	void Div<T>::evaluate(voidP inputs, void* result)
	{
		T& divs = *(T*) result;
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

		divs = readInputs.at(0);
		for (int i = 1; i < (int)numOfArgs_; i++)
		{
			if (readInputs.at(i))
			{
				divs /= readInputs.at(i);
			}
			else
			{
				divs = 0;
				break;
			}
		}
	}

}

#endif /* Div_h */