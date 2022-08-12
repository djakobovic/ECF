#ifndef Or_h
#define Or_h
#include "Function.h"

namespace cart
{
	template <class T>
	class Or : public Function
	{
	public:
		Or();
		Or(uint numArgs);
		~Or();

		void evaluate(voidP inputs, void* result);
	};

	typedef Or<uint> OrUint;

	template <class T>
	Or<T>::Or()
	{
		name_ = "OR";
		numOfArgs_ = 2;
	}

	template <class T>
	Or<T>::Or(uint numArgs)
	{
		name_ = "OR";
		numOfArgs_ = numArgs;
	}

	template <class T>
	Or<T>::~Or()
	{
	}

	template <class T>
	void Or<T>::evaluate(voidP inputs, void* result)
	{
		T& or_ = *(T*) result;
		stringstream ss;
		ss << *((string*) inputs.get());
		vector<T> readInputs;
		T input, maxSize = 0;
		uint i = 0;
		//received inputs are in format: input1 sizeOfInput1 input2 sizeOfInput2 ...
		//size of inputs are not important in OR-ing because OR will not produce leading 1's instead of
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

		or_ = readInputs.at(0);
		for (int i = 1; i < (int)numOfArgs_; i++)
		{
			or_ |= readInputs.at(i);
		}
	}

}

#endif /* Or_h */
