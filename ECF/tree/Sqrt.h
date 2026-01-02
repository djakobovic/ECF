#ifndef Sqrt_h
#define Sqrt_h
#include "Primitive.h"
#include <cmath>

namespace Tree {
namespace Primitives {

	class Sqrt : public Primitive
	{
	public:
		Sqrt(void)
		{
			nArguments_ = 1;
			name_ = "sqrt";
		}


		~Sqrt(void)
		{ }


		void execute(void* result, Tree& tree)
		{
			double& arg = *(double*)result;
			getNextArgument(&arg, tree);
			if (arg > 0) {
				arg = sqrt(arg);
			} else {
				arg = 0;
			}
		}
	};

}
}

#endif 