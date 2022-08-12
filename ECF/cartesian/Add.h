#ifndef Add_h
#define Add_h
#include "Function.h"

namespace cart
{
	template <class T>
	class Add : public Function
	{
	public:
		Add();
		Add(uint numArgs);
		~Add();

		void evaluate(voidP inputs, void* result);
	};

	typedef Add<double> AddDouble;
	typedef Add<int> AddInt;

	template <class T>
	Add<T>::Add()
	{
		name_ = "+";
		numOfArgs_ = 2;
	}

	template <class T>
	Add<T>::Add(uint numArgs)
	{
		name_ = "+";
		numOfArgs_ = numArgs;
	}

	template <class T>
	Add<T>::~Add()
	{
	}

	template <class T>
	void Add<T>::evaluate(voidP inputs, void* result)
	{
		T& sum = *(T*) result;
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

		sum = readInputs.at(0);
		for (int i = 1; i < (int)numOfArgs_; i++)
		{
			sum += readInputs.at(i);
		}
	}

}

#endif /* Add_h */