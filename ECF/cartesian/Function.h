#ifndef Function_h
#define Function_h
#include "Cartesian_c.h"
#include <math.h>


namespace Cartesian
{
	class Function
	{
	public:
		Function() {}
		virtual ~Function() {}

		/**
		Evaluate result with given inputs in one of implemented derived classes.
		*/
		virtual void evaluate(std::vector<double>& inputs, double& result) = 0;

		std::string getName()
		{
			return name_;
		}

		uint getNumberOfArguments()
		{
			return nArguments_;
		}

	protected:
		std::string name_;
		uint nArguments_;
	};

	typedef std::shared_ptr<Function> FunctionP;
}

#endif /* Function_h */