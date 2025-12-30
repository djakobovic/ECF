#ifndef Log_h
#define Log_h
#include "Primitive.h"
#include <cmath>

namespace Tree {
namespace Primitives {

class Log : public Primitive
{
	public:
		Log::Log(void)
		{
			nArguments_ = 1;
			name_ = "log";
		}


		Log::~Log(void)
		{ }


		void Log::execute(void* result, Tree& tree)
		{
			double& arg = *(double*)result;
			getNextArgument(&arg, tree);
			if (arg > 0) {
				arg = log10(arg);
			}
			else {
				arg = 0;
			}
		}
};

}
}

#endif 