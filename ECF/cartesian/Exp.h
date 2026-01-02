#ifndef Exp_h
#define Exp_h
#include "Function.h"
namespace Cartesian
{
	template <class T>
	class ExpT : public Function
	{
	public:
		ExpT();
		~ExpT();

		void evaluate(std::vector<T>& inputs, T& result);
	};

	typedef ExpT<double> Exp;

	template <class T>
	ExpT<T>::ExpT()
	{
		name_ = "exp";
		nArguments_ = 1;
	}


	template <class T>
	ExpT<T>::~ExpT()
	{}

	template <class T>
	void ExpT<T>::evaluate(std::vector<T>& inputs, T& result)
	{
		result = exp(inputs[0]);
	}

}
#endif //Exp_h
