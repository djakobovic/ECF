#ifndef Sub_h
#define Sub_h
#include "Function.h"

namespace cart
{
	template <class T>
	class Sub : public Function
	{
	public:
		Sub();
		Sub(uint numArgs);
		~Sub();

		void evaluate(voidP inputs, void* result);
	};

	typedef Sub<double> SubDouble;
	typedef Sub<int> SubInt;

	template <class T>
	Sub<T>::Sub()
	{
		name_ = "-";
		numOfArgs_ = 2;
	}

	template <class T>
	Sub<T>::Sub(uint numArgs)
	{
		name_ = "-";
		numOfArgs_ = numArgs;
	}

	template <class T>
	Sub<T>::~Sub()
	{
	}

	template <class T>
	void Sub<T>::evaluate(voidP inputs, void* result)
	{
		T& diff = *(T*) result;
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

		diff = readInputs.at(0);
		for (int i = 1; i < (int)numOfArgs_; i++)
		{
			diff -= readInputs.at(i);
		}
	}

}

#endif /* Sub_h */