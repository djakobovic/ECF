#ifndef Not_h
#define Not_h
#include "Function.h"

namespace cart
{
	template <class T>
	class Not : public Function
	{
	public:
		Not();
		~Not();

		void evaluate(voidP inputs, void* result);
	};

	typedef Not<uint> NotUint;

	template <class T>
	Not<T>::Not()
	{
		name_ = "NOT";
		numOfArgs_ = 1;
	}

	template <class T>
	Not<T>::~Not()
	{
	}

	template <class T>
	void Not<T>::evaluate(voidP inputs, void* result)
	{
		T& neg = *(T*) result;
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

		neg = ~((T)readInputs.at(0));
		//masking is important in NOT operation because instead of leading 0's there would be 
		//leading 1's as most significant bits
		neg &= mask;
	}

}

#endif /* Not_h */