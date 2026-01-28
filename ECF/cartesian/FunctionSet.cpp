#include "FunctionSet.h"
#include "Cartesian.h"

namespace Cartesian {

FunctionSet::FunctionSet()
{ }


bool FunctionSet::initialize(StateP state)
{
	// TODO: initialize map with all functions

	state_ = state;

	// register existing functions
	FunctionP func = (FunctionP) (new Add);
	mAllFunctions[func->getName()] = func;

	func = (FunctionP) (new Sub);
	mAllFunctions[func->getName()] = func;

	func = (FunctionP) (new Mul);
	mAllFunctions[func->getName()] = func;

	func = (FunctionP) (new Div);
	mAllFunctions[func->getName()] = func;

	func = (FunctionP) (new Sqrt);
	mAllFunctions[func->getName()] = func;

	func = (FunctionP) (new Exp);
	mAllFunctions[func->getName()] = func;

	func = (FunctionP) (new Ln);
	mAllFunctions[func->getName()] = func;

	func = (FunctionP) (new Sin);
	mAllFunctions[func->getName()] = func;

	func = (FunctionP) (new Cos);
	mAllFunctions[func->getName()] = func;

	func = (FunctionP) (new Step);
	mAllFunctions[func->getName()] = func;

	func = (FunctionP)(new Pos);
	mAllFunctions[func->getName()] = func;

	func = (FunctionP)(new Min);
	mAllFunctions[func->getName()] = func;

	func = (FunctionP)(new Max);
	mAllFunctions[func->getName()] = func;

	return true;

}


bool FunctionSet::addFunction(std::string name)
{
	// mark given function as active (add to vector of used functions)

	func_iter iter = mAllFunctions.find(name);
	// if not found, return false
	if(iter == mAllFunctions.end())
		return false;

	vFunctions.push_back(iter->second);
	mFunctionSet[iter->first] = vFunctions.size() - 1;

	return true;
}


}
