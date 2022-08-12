#ifndef Function_h
#define Function_h
#include "Cartesian_c.h"
using namespace std;

namespace cart
{
	class Function
	{
	public:
		Function() {}
		Function(uint numArgs) {}
		virtual ~Function() {}

		/**
		Evaluate result with given inputs in one of implemented derived classes of Function class.
		*/
		virtual void evaluate(voidP inputs, void* result) = 0;

		string getName()
		{
			return name_;
		}

		uint getNumOfArgs()
		{
			return numOfArgs_;
		}

	protected:
		string name_;
		uint numOfArgs_;
	};

	typedef boost::shared_ptr<Function> FunctionP;
}


#endif /* Function_h */