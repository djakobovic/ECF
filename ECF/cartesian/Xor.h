#ifndef Xor_h
#define Xor_h
#include "Function.h"

namespace cart
{
	template <class T>
	class Xor : public Function
	{
	public:
		Xor();
		Xor(uint numArgs);
		~Xor();

		void evaluate(voidP inputs, void* result);
	};

	typedef Xor<uint> XorUint;

	template <class T>
	Xor<T>::Xor()
	{
		name_ = "XOR";
		numOfArgs_ = 2;
	}

	template <class T>
	Xor<T>::Xor(uint numArgs)
	{
		name_ = "XOR";
		numOfArgs_ = numArgs;
	}

	template <class T>
	Xor<T>::~Xor()
	{
	}

	template <class T>
	void Xor<T>::evaluate(voidP inputs, void* result)
	{
		T& xor_ = *(T*) result;
		stringstream ss;
		ss << *((string*) inputs.get());
		vector<T> readInputs;
		T input, maxSize = 0;
		uint i = 0;
		//received inputs are in format: input1 sizeOfInput1 input2 sizeOfInput2 ...
		//size of inputs are not important in XOR-ing because XOR will not produce leading 1's instead of
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

		xor_ = readInputs.at(0);
		for (int i = 1; i < (int)numOfArgs_; i++)
		{
			xor_ ^= readInputs.at(i);
		}
	}

}

#endif /* Xor_h */
