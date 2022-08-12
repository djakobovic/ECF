#ifndef Xnor_h
#define Xnor_h
#include "Function.h"

namespace cart
{
	template <class T>
	class Xnor : public Function
	{
	public:
		Xnor();
		Xnor(uint numArgs);
		~Xnor();

		void evaluate(voidP inputs, void* result);
	};

	typedef Xnor<uint> XnorUint;

	template <class T>
	Xnor<T>::Xnor()
	{
		name_ = "XNOR";
		numOfArgs_ = 2;
	}

	template <class T>
	Xnor<T>::Xnor(uint numArgs)
	{
		name_ = "XNOR";
		numOfArgs_ = numArgs;
	}

	template <class T>
	Xnor<T>::~Xnor()
	{
	}

	template <class T>
	void Xnor<T>::evaluate(voidP inputs, void* result)
	{
		T& xnor = *(T*) result;
		stringstream ss;
		ss << *((string*) inputs.get());
		vector<T> readInputs;
		T input, maxSize = 0;
		uint i = 0;
		//received inputs are in format: input1 sizeOfInput1 input2 sizeOfInput2 ...
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
		T mask = (T)pow(2.0, maxSize + 1.0) - 1;

		xnor = readInputs.at(0);
		for (int i = 1; i < (int)numOfArgs_; i++)
		{
			xnor = ~(xnor ^ readInputs.at(i));
			//masking is important in XNOR operation because instead of leading 0's there would be 
			//leading 1's as most significant bits
			xnor &= mask;
		}
	}

}

#endif /* Xnor_h */