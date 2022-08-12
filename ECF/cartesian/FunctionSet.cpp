#include "FunctionSet.h"

namespace cart {

	FunctionSet::FunctionSet(string gettype)
	{
		type = gettype;
		existFunc["+"] = true;
		existFunc["-"] = true;
		existFunc["*"] = true;
		existFunc["/"] = true;
		existFunc["sin"] = true;
		existFunc["cos"] = true;
		existFunc["AND"] = true;
		existFunc["OR"] = true;
		existFunc["NOT"] = true;
		existFunc["XOR"] = true;
		existFunc["XNOR"] = true;
	}

	bool FunctionSet::addFunction(string name)
	{
		if (!existFunc[name])
		{
			cerr << "FunctionSet error: Function " << name << " isn't implemented." << endl;
			return false;
		}
		
		if (type == "double")
		{
			if (name == "+")
			{
				this->push_back((FunctionP) (new AddDouble));
			}
			else if (name == "-")
			{
				this->push_back((FunctionP) (new SubDouble));
			}
			else if (name == "*")
			{
				this->push_back((FunctionP) (new MulDouble));
			}
			else if (name == "/")
			{
				this->push_back((FunctionP) (new DivDouble));
			}
			else if (name == "sin")
			{
				this->push_back((FunctionP) (new SinDouble));
			}
			else if (name == "cos")
			{
				this->push_back((FunctionP) (new CosDouble));
			}
			else
			{
				cerr << "FunctionSet error: Unkown function name: " << name << endl;
				return false;
			}
		}
		else if (type == "int")
		{
			if (name == "+")
			{
				this->push_back((FunctionP) (new AddInt));
			}
			else if (name == "-")
			{
				this->push_back((FunctionP) (new SubInt));
			}
			else if (name == "*")
			{
				this->push_back((FunctionP) (new MulInt));
			}
			else if (name == "/")
			{
				this->push_back((FunctionP) (new DivInt));
			}
			else if (name == "sin")
			{
				this->push_back((FunctionP) (new SinInt));
			}
			else if (name == "cos")
			{
				this->push_back((FunctionP) (new CosInt));
			}
			else
			{
				cerr << "FunctionSet error: Unkown function name: " << name << endl;
				return false;
			}
		}
		else if (type == "uint")
		{
			if (name == "AND")
			{
				this->push_back((FunctionP) (new AndUint));
			}
			else if (name == "OR")
			{
				this->push_back((FunctionP) (new OrUint));
			}
			else if (name == "NOT")
			{
				this->push_back((FunctionP) (new NotUint));
			}
			else if (name == "XOR")
			{
				this->push_back((FunctionP) (new XorUint));
			}
			else if (name == "XNOR")
			{
				this->push_back((FunctionP) (new XnorUint));
			}
			else
			{
				cerr << "FunctionSet error: Unkown function name: " << name << endl;
				return false;
			}
		}
		else
		{
			cerr << "FunctionSet error: Unkown data type: " << type << endl;
			return false;
		}

		return true;
	}

	bool FunctionSet::addFunction(string name, uint numArgs)
	{
		if (!existFunc[name])
		{
			cerr << "FunctionSet error: Function " << name << " isn't implemented." << endl;
			return false;
		}

		if (type == "double")
		{
			if (name == "+")
			{
				this->push_back((FunctionP) (new AddDouble(numArgs)));
			}
			else if (name == "-")
			{
				this->push_back((FunctionP) (new SubDouble(numArgs)));
			}
			else if (name == "*")
			{
				this->push_back((FunctionP) (new MulDouble(numArgs)));
			}
			else if (name == "/")
			{
				this->push_back((FunctionP) (new DivDouble(numArgs)));
			}
			else
			{
				cerr << "FunctionSet error: Unkown function name: " << name;
				cerr << " for " << numArgs << " arguments." << endl;
				return false;
			}
		}
		else if (type == "int")
		{
			if (name == "+")
			{
				this->push_back((FunctionP) (new AddInt(numArgs)));
			}
			else if (name == "-")
			{
				this->push_back((FunctionP) (new SubInt(numArgs)));
			}
			else if (name == "*")
			{
				this->push_back((FunctionP) (new MulInt(numArgs)));
			}
			else if (name == "/")
			{
				this->push_back((FunctionP) (new DivInt(numArgs)));
			}
			else
			{
				cerr << "FunctionSet error: Unkown function name: " << name;
				cerr << " for " << numArgs << " arguments." << endl;
				return false;
			}
		}
		else if (type == "uint")
		{
			if (name == "AND")
			{
				this->push_back((FunctionP) (new AndUint(numArgs)));
			}
			else if (name == "OR")
			{
				this->push_back((FunctionP) (new OrUint(numArgs)));
			}
			else if (name == "XOR")
			{
				this->push_back((FunctionP) (new XorUint(numArgs)));
			}
			else if (name == "XNOR")
			{
				this->push_back((FunctionP) (new XnorUint(numArgs)));
			}
			else
			{
				cerr << "FunctionSet error: Unkown function name: " << name;
				cerr << " for " << numArgs << " arguments." << endl;
				return false;
			}
		}
		else
		{
			cerr << "FunctionSet error: Unkown data type: " << type << endl;
			return false;
		}

		return true;
	}

	void FunctionSet::evaluate(voidP inputs, void* result, uint funcNum)
	{
		this->at(funcNum)->evaluate(inputs, result);
	}

}