#ifndef And_h
#define And_h
#include "Function.h"

namespace cart
{
	template <class T>
	class And : public Function
	{
	public:
		And();
		And(uint numArgs);
		~And();

		void evaluate(voidP inputs, void* result);
	};

	typedef And<uint> AndUint;

	template <class T>
	And<T>::And()
	{
		name_ = "AND";
		numOfArgs_ = 2;
	}

	template <class T>
	And<T>::And(uint numArgs)
	{
		name_ = "AND";
		numOfArgs_ = numArgs;
	}

	template <class T>
	And<T>::~And()
	{
	}

	template <class T>
	void And<T>::evaluate(voidP inputs, void* result)
	{
		T& and_ = *(T*) result;
		stringstream ss;
		ss << *((string*) inputs.get());
		vector<T> readInputs;
		T input, maxSize = 0;
		uint i = 0;
		//received inputs are in format: input1 sizeOfInput1 input2 sizeOfInput2 ...
		//size of inputs are not important in AND-ing because AND will not produce leading 1's instead of
		//leading 0's like NOT or XNOR function
		while (ss >> input)
		{
			readInputs.push_back(input);
			ss >> input;
			if (input > maxSize)
			{
				maxSize = input;
			}
			i += 2;
			if (i == 2 * numOfArgs_)
			{
				break;
			}
		}

		and_ = readInputs.at(0);
		for (int i = 1; i < (int)numOfArgs_; i++)
		{
			and_ &= readInputs.at(i);
		}
	}

}

#endif /* And_h */
