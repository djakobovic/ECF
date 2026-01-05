#ifndef CGPStep_h
#define CGPStep_h
#include "Function.h"
namespace Cartesian {

	template <class T>
	class StepT : public Function
	{
	public:
		StepT();
		~StepT();

		void evaluate(std::vector<T>& inputs, T& result);
	};

	typedef StepT<double> Step;

	template <class T>
	StepT<T>::StepT()
	{
		name_ = "step";
		nArguments_ = 1;
	}

	template  <class T>
	StepT<T>::~StepT()
	{}

	template  <class T>
	void StepT<T>::evaluate(std::vector<T> &inputs, T &result)
	{
		result = (inputs[0] > 0 ? 1 : 0);
	}
}
#endif //Step_h
