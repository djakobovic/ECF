#ifndef FunctionSet_h
#define FunctionSet_h
#include "Function.h"

//functions
#include "Add.h"
#include "Sub.h"
#include "Mul.h"
#include "Div.h"
#include "Sin.h"
#include "Cos.h"
#include "And.h"
#include "Or.h"
#include "Not.h"
#include "Xor.h"
#include "Xnor.h"

namespace cart
{
	class FunctionSet : public vector<FunctionP>
	{
	public:
		/**
		Gettype in constructor defines data type for evaluation.
		*/
		FunctionSet(string gettype);
		/**
		Add function defined by its name, take default number of arguments.
		*/
		bool addFunction(string name);
		/**
		Add function defined by its name and number of arguments.
		*/
		bool addFunction(string name, uint numArgs);
		/**
		Evaluate function with inputs and function defined by funcNum, return result in parameter result.
		*/
		void evaluate(voidP inputs, void* result, uint funcNum);
		~FunctionSet() {}
		
		map<string, bool> existFunc;		//<! existing (implemented) functions
	protected:
		string type;

	};	

	typedef boost::shared_ptr<FunctionSet> FunctionSetP;
}


#endif /* FunctionSet_h */