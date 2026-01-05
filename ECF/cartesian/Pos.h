#ifndef CGPPos_h
#define CGPPos_h
#include "Function.h"
namespace Cartesian {

	template <class T>
	class PosT : public Function
	{
	public:
		PosT();
		~PosT();

		void evaluate(std::vector<T>& inputs, T& result);
	};

	typedef PosT<double> Pos;

	template <class T>
	PosT<T>::PosT()
	{
		name_ = "pos";
		nArguments_ = 1;
	}

	template  <class T>
	PosT<T>::~PosT()
	{}

	template  <class T>
	void PosT<T>::evaluate(std::vector<T>& inputs, T& result)
	{
		result = (inputs[0] > 0 ? inputs[0] : 0);
	}
}
#endif //Pos_h
