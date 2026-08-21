#ifndef CGPNLog_h
#define CGPNLog_h
#include "Function.h"
namespace Cartesian {

	template <class T>
	class NlogT : public Function
	{
	public:
		NlogT();
		~NlogT();

		void evaluate(std::vector<T>& inputs, T& result);
	};

	typedef NlogT<double> Ln;

	template <class T>
	NlogT<T>::NlogT()
	{
		name_ = "ln";
		nArguments_ = 1;
	}

	template  <class T>
	NlogT<T>::~NlogT()
	{}

	template  <class T>
	void NlogT<T>::evaluate(std::vector<T> &inputs, T &result)
	{
		result = inputs[0];
		if(inputs[0] > 0)
			result = log(inputs[0]);
	}
}
#endif //NLog_h
