#ifndef Sin_h
#define Sin_h
#include "Function.h"
#include <cmath>

namespace cart
{
	template <class T>
	class Sin : public Function
	{
	public:
		Sin();
		~Sin();

		void evaluate(voidP inputs, void* result);
	};

	typedef Sin<double> SinDouble;
	typedef Sin<int> SinInt;

	template <class T>
	Sin<T>::Sin()
	{
		name_ = "sin";
		numOfArgs_ = 1;
	}

	template <class T>
	Sin<T>::~Sin()
	{
	}

	template <class T>
	void Sin<T>::evaluate(voidP inputs, void* result)
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

		data = (T)sin((double)readInputs.at(0));
	}

}

#endif /* Sin_h */