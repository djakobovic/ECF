#include "PrimitiveSetAP.h"
#include "../tree/Tree.h"
#include "../tree/Primitive.h"


namespace Tree
{

	PrimitiveSetAP::PrimitiveSetAP()
	{	}


	PrimitiveSetAP::~PrimitiveSetAP()
	{	}


	bool PrimitiveSetAP::initialize(StateP state)
	{
		state_ = state;

		// register existing primitives
		PrimitiveP prim = (PrimitiveP)(new Primitives::Add);
		mAllPrimitives_[prim->getName()] = prim;
		prim = (PrimitiveP)(new Primitives::Sub);
		mAllPrimitives_[prim->getName()] = prim;
		prim = (PrimitiveP)(new Primitives::Mul);
		mAllPrimitives_[prim->getName()] = prim;
		prim = (PrimitiveP)(new Primitives::Div);
		mAllPrimitives_[prim->getName()] = prim;
		prim = (PrimitiveP)(new Primitives::Sin);
		mAllPrimitives_[prim->getName()] = prim;
		prim = (PrimitiveP)(new Primitives::Cos);
		mAllPrimitives_[prim->getName()] = prim;
		prim = (PrimitiveP)(new Primitives::Pos);
		mAllPrimitives_[prim->getName()] = prim;
		prim = (PrimitiveP)(new Primitives::Neg);
		mAllPrimitives_[prim->getName()] = prim;
		prim = (PrimitiveP)(new Primitives::Max);
		mAllPrimitives_[prim->getName()] = prim;
		prim = (PrimitiveP)(new Primitives::Min);
		mAllPrimitives_[prim->getName()] = prim;

		prim_iter primIter;
		for (primIter = mAllPrimitives_.begin(); primIter != mAllPrimitives_.end(); ++primIter)
			primIter->second->initialize(state);

		// register terminal types
		mTypeNames_.insert(std::pair<std::string, Primitives::terminal_type>("DOUBLE", Primitives::Double));
		mTypeNames_.insert(std::pair<std::string, Primitives::terminal_type>("INT", Primitives::Int));
		mTypeNames_.insert(std::pair<std::string, Primitives::terminal_type>("BOOL", Primitives::Bool));
		mTypeNames_.insert(std::pair<std::string, Primitives::terminal_type>("CHAR", Primitives::Char));
		mTypeNames_.insert(std::pair<std::string, Primitives::terminal_type>("STRING", Primitives::String));

		return true;
	}


	/// Access function by name (active functions only).
	PrimitiveP PrimitiveSetAP::getGFSOneByName(std::string name)
	{
		prim_iter iter = mGFSOneSet_.find(name);
		if (iter == mGFSOneSet_.end())
			return PrimitiveP();

		return iter->second;
	}

	/// Access function by name (active functions only).
	PrimitiveP PrimitiveSetAP::getGFSTwoByName(std::string name)
	{
		prim_iter iter = mGFSTwoSet_.find(name);
		if (iter == mGFSTwoSet_.end())
			return PrimitiveP();

		return iter->second;
	}


	/// Access terminal by name (active terminals only).
	PrimitiveP PrimitiveSetAP::getTerminalByName(std::string name)
	{
		prim_iter iter = mTerminalSet_.find(name);
		if (iter == mTerminalSet_.end())
			return PrimitiveP();

		return iter->second;
	}


	/// Access primitive by name (active functions or terminals only).
	PrimitiveP PrimitiveSetAP::getPrimitiveByName(std::string name)
	{
		prim_iter iter = mPrimitiveSet_.find(name);
		if (iter == mPrimitiveSet_.end())
			return PrimitiveP();

		return iter->second;
	}

	PrimitiveP PrimitiveSetAP::getTerminalByIndex(uint index) {
		return terminalSet_[index % terminalSet_.size()];
	}

	PrimitiveP PrimitiveSetAP::getGFSOneByIndex(uint index) {
		return GFSOneSet_[index % GFSOneSet_.size()];
	}

	PrimitiveP PrimitiveSetAP::getGFSTwoByIndex(uint index) {
		return GFSTwoSet_[index % GFSTwoSet_.size()];
	}

	PrimitiveP PrimitiveSetAP::getPrimitiveByIndex(uint index) {
		return primitives_[index % primitives_.size()];
	}


	/**
	 * \brief	Add a function primitive to the set of active primitives - if found by name in collection of all primitives.
	 * If a function takes 0 arguments, it is added to the terminal set.
	 */
	bool PrimitiveSetAP::addFunction(std::string name)
	{
		prim_iter iter = mAllPrimitives_.find(name);
		if (iter == mAllPrimitives_.end())
			return false;

		int argNum = iter->second->getNumberOfArguments();

		if (argNum == 0) {
			terminalSet_.push_back(iter->second);
			mTerminalSet_[iter->first] = iter->second;
		}
		else if (argNum == 1) {
			GFSOneSet_.push_back(iter->second);
			mGFSOneSet_[iter->first] = iter->second;
		}
		else if (argNum == 2) {
			GFSTwoSet_.push_back(iter->second);
			mGFSTwoSet_[iter->first] = iter->second;
		}

		primitives_.push_back(iter->second);
		mPrimitiveSet_[iter->first] = iter->second;

		return true;
	}


	/**
	 * \brief	Add a terminal primitive to the set of active primitives.
	 */
	void PrimitiveSetAP::addTerminal(PrimitiveP terminalPrimitive)
	{
		terminalSet_.push_back(terminalPrimitive);
		mTerminalSet_[terminalPrimitive->getName()] = terminalPrimitive;

		primitives_.push_back(terminalPrimitive);
		mPrimitiveSet_[terminalPrimitive->getName()] = terminalPrimitive;
	}


	uint PrimitiveSetAP::getFunctionSetSize()
	{
		return getGFSOneSetSize() + getGFSTwoSetSize();
	}

	uint PrimitiveSetAP::getGFSOneSetSize()
	{
		return (uint)GFSOneSet_.size();
	}

	uint PrimitiveSetAP::getGFSTwoSetSize()
	{
		return (uint)GFSTwoSet_.size();
	}

	/// Get the number of active terminals.
	uint PrimitiveSetAP::getTerminalSetSize()
	{
		return (uint)terminalSet_.size();
	}

	/// Get the number of active primitives (functions and terminals).
	uint PrimitiveSetAP::getPrimitivesSize()
	{
		return (uint)primitives_.size();
	}

}