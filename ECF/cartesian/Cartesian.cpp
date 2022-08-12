#include "Cartesian.h"
#include <cctype>
#include <map>
#include <vector>

namespace cart {

Cartesian::Cartesian(void)
{
	name_ = "Cartesian";
}

Cartesian::~Cartesian(void)
{
}

bool Cartesian::initialize(StateP state)
{
	state_ = state;

	stringstream names;
	string name;
	voidP sptr;

	sptr = getParameterValue(state, "type");
	names << *((string*) sptr.get());
	name = names.str();

	//type defines data type (from domain and codomain), if not defined default is double
	if (name.length() == 0)
	{
		name = "double";
	}
	else if (name != "double" && name != "int" && name != "uint")
	{
		cerr << "Genoype initialization error:: Has to be one of types: double, int, uint" << endl;
		return false;
	}
	else
	{ }

	//function nodes must be able to work with defined data type
	funcSet = static_cast<FunctionSetP> (new FunctionSet(name));

	sptr = getParameterValue(state, "numinputconns");
	name = *((string*) sptr.get());
	if (name.length() == 0)
	{
		cerr << "Genotype initialization error:: Number of input connections is undefined." << endl;
		return false;
	}
	inputConns = str2uint(name);
	if (inputConns == 0)
	{
		cerr << "Genotype initialization error:: Number of input connections can't be zero." << endl;
		return false;
	}

	vector<string> functionset;
	sptr = getParameterValue(state, "functionset");
	//one function can be defined with multiple (but different) number of arguments
	uint arg;
	map<std::string, std::vector<uint> > args;

	//extract function names and number of their arguments, if number of arguments isn't defined, then
	//it's default (some function nodes can't have variable number of arguments, e.g. sin or cos)
	names.str("");
	names.clear();
	names << *((string*) sptr.get());
	while (names >> name)
	{
		//if it isn't function name then it can be number of arguments for the last read function name
		if (!funcSet->existFunc[name])
		{
			char *c = (char *)name.c_str();
			for (int i = 0; i < (int)name.length(); i++)
			{
				if (!isdigit(c[i]))
				{
					cerr << "Genoype initialization error:: Function " << name << " isn't implemented." << endl;
					return false;
				}
			}
			//some functions may go with many number of arguments, all numbers of arguments must be defined
			//after stating the name of that function (in configuration file)
			if (args[functionset.back()].empty())
			{
				vector<uint> vec;
				vec.push_back(str2uint(name));
				args[functionset.back()] = vec;
			}
			else
			{
				arg = str2uint(name);
				//there can't be defined two or more functions with the same name and the same number
				//of arguments
				for (int i = 0; i < args[functionset.back()].size(); i++)
				{
					if (arg == args[functionset.back()].at(i))
					{
						cerr << "Genotype initialization error:: Number of arguments " << arg;
						cerr << " for function " << functionset.back() << " already taken." << endl;
						return false;
					}
				}
				args[functionset.back()].push_back(arg);
			}
		}
		else
		{
			functionset.push_back(name);
		}
	}

	if (functionset.size() == 0)
	{
		cerr << "Genotype initialization error:: Must be defined at least one function in function set." << endl;
		return false;
	}

	//create function objects
	for (int i = 0; i < (int)functionset.size(); i++)
	{
		if (!args[functionset.at(i)].empty())
		{
			for (int j = 0; j < args[functionset.at(i)].size(); j++)
			{
				if (args[functionset.at(i)].at(j) > inputConns)
				{
					cerr << "Genoype initialization error:: Number of arguments for " << functionset.at(i);
					cerr << " greater than number of input connections." << endl;
					return false;
				}
				if (!funcSet->addFunction(functionset.at(i), args[functionset.at(i)].at(j)))
				{
					return false;
				}
			}
		}
		else
		{
			if (!funcSet->addFunction(functionset.at(i)))
			{
				return false;
			}
		}
	}

	//number of possible functions
	numFunc = (uint)funcSet->size();

	//extract constants, constants aren't necessary
	sptr = getParameterValue(state, "constantset");
	constantset = sptr;
	names.str("");
	names.clear();
	names << *((string*) sptr.get());
	uint numCons = 0;
	while (names >> name)
	{
		++numCons;
	}

	//num of variables = num of inputs to be replaced by elements from domain
	sptr = getParameterValue(state, "numvariables");
	name = *((string*) sptr.get());
	numVars = str2uint(name);

	inputs = numVars + numCons;
	if (inputs == 0)
	{
		cerr << "Genotype initialization error:: Number of inputs can't be zero." << endl;
		return false;
	}

	sptr = getParameterValue(state, "numoutputs");
	name = *((string*) sptr.get());
	if (name.length() == 0)
	{
		cerr << "Genotype initialization error:: Number of outputs is undefined." << endl;
		return false;
	}
	outputs = str2uint(name);
	if (outputs == 0)
	{
		cerr << "Genotype initialization error:: Number of outputs can't be zero." << endl;
		return false;
	}

	sptr = getParameterValue(state, "numrows");
	name = *((string*) sptr.get());
	if (name.length() == 0)
	{
		cerr << "Genotype initialization error:: Number of rows is undefined." << endl;
		return false;
	}
	rows = str2uint(name);
	if (rows == 0)
	{
		cerr << "Genotype initialization error:: Number of rows can't be zero." << endl;
		return false;
	}

	sptr = getParameterValue(state, "numcols");
	name = *((string*) sptr.get());
	if (name.length() == 0)
	{
		cerr << "Genotype initialization error:: Number of columns is undefined." << endl;
		return false;
	}
	cols = str2uint(name);
	if (cols == 0)
	{
		cerr << "Genotype initialization error:: Number of columns can't be zero." << endl;
		return false;
	}


	sptr = getParameterValue(state, "levelsback");
	name = *((string*) sptr.get());
	//if not defined, default is 1
	if (name.length() == 0)
	{
		levelsBack = 1;
	}
	else
	{
		levelsBack = str2uint(name);
	}

	makeGenotype();

	//printGenotype();

	return true;
}

Cartesian* Cartesian::copy()
{
	Cartesian *newObject = new Cartesian(*this);

	//create new copy of existing genotype
	for(int i = 0; i < (int) this->size(); i++) {
		(*newObject)[i] = this->at(i);
	}
	return newObject;
}

vector<CrossoverOpP> Cartesian::getCrossoverOp()
{
	vector<CrossoverOpP> crossops;
	crossops.push_back((CrossoverOpP) (new CartesianCrsOnePoint));
	return crossops;
}

vector<MutationOpP> Cartesian::getMutationOp()
{
	vector<MutationOpP> mutops;
	mutops.push_back((MutationOpP) (new CartesianMutOnePoint));
	return mutops;
}

void Cartesian::registerParameters(StateP state)
{
	registerParameter(state, "type", (voidP) (new string), ECF::STRING);
	registerParameter(state, "numoutputs", (voidP) (new string), ECF::STRING);
	registerParameter(state, "numinputconns", (voidP) (new string), ECF::STRING);
	registerParameter(state, "numrows", (voidP) (new string), ECF::STRING);
	registerParameter(state, "numcols", (voidP) (new string), ECF::STRING);
	registerParameter(state, "levelsback", (voidP) (new string), ECF::STRING);
	// variableset + constantset = terminalset
	registerParameter(state, "numvariables", (voidP) (new string), ECF::STRING);
	registerParameter(state, "constantset", (voidP) (new string), ECF::STRING);
	registerParameter(state, "functionset", (voidP) (new string), ECF::STRING);
}

void Cartesian::read(XMLNode &xCart)
{
}

void Cartesian::write(XMLNode &xCart)
{
	xCart = XMLNode::createXMLTopNode("Cartesian");
	stringstream sValue;
	sValue << this->size();
	xCart.addAttribute("size", sValue.str().c_str());

	sValue.str("");
	for (int i = 0; i < (int)(rows * cols * (inputConns + 1)); i++)
	{
		sValue << this->at(i) << " ";
		if (((i + 1) % (inputConns + 1)) == 0)
		{
			sValue << "   ";
		}
	}
	for (int i = 0; i < (int)outputs; i++)
	{
		sValue << this->at((rows * cols * (inputConns + 1)) + i) << " ";
	}

	xCart.addText(sValue.str().c_str());
}

uint Cartesian::getGenomeSize()
{
	return 0;
}

void Cartesian::makeGenotype()
{
	uint currCol = 0;
	//generate nodes - input connections + one function
	for (int i = 0; i < (int)(rows * cols); i++)
	{
		if ((i % rows) == 0 && i != 0)
		{
			++currCol;
		}
		for (int j = 0; j < (int)inputConns; j++)
		{
			this->push_back(randInputConn(currCol));
		}
		this->push_back(randFunction());
	}
	//generate outputs
	for (int i = 0; i < (int)outputs; i++)
	{
		this->push_back(randOutput());
	}
}

uint Cartesian::randInputConn(uint currCol)
{
	int emax = inputs + (currCol * rows);

	if (currCol < levelsBack)
	{
		return (uint)(state_->getRandomizer()->getRandomInteger(0, emax - 1));
	}

	int emin = inputs + ((currCol - levelsBack) * rows);
	return (uint)(state_->getRandomizer()->getRandomInteger(emin, emax - 1));
}

uint Cartesian::randOutput()
{
	int hmin = inputs + ((cols - levelsBack) * rows);
	int hmax = inputs + (cols * rows);
	return (uint)(state_->getRandomizer()->getRandomInteger(hmin, hmax - 1));
}

uint Cartesian::randFunction()
{
	return (uint)(state_->getRandomizer()->getRandomInteger(0, numFunc - 1));
}

void Cartesian::evaluate(voidP inputs, void* result, uint funcNum)
{
	funcSet->evaluate(inputs, result, funcNum);
}

void Cartesian::printGenotype()
{
	for (int i = 0; i < (int)(rows * cols * (inputConns + 1)); i++)
	{
		cout << this->at(i) << " ";
		if (((i + 1) % (inputConns + 1)) == 0)
		{
			cout << "   ";
		}
	}
	for (int i = 0; i < (int)outputs; i++)
	{
		cout << this->at((rows * cols * (inputConns + 1)) + i) << " ";
	}
	cout << endl;
}

uint Cartesian::getNumOfInputs()
{
	return inputs;
}

uint Cartesian::getNumOfOutputs()
{
	return outputs;
}

uint Cartesian::getNumOfInputConn()
{
	return inputConns;
}

voidP Cartesian::getConstantNames()
{
	return constantset;
}

uint Cartesian::getNumOfRows()
{
	return rows;
}

uint Cartesian::getNumOfCols()
{
	return cols;
}

uint Cartesian::getLevelsBack()
{
	return levelsBack;
}

uint Cartesian::getNumVars()
{
	return numVars;
}

}





