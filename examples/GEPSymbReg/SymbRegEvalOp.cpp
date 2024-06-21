#include "SymbRegEvalOp.h"


// called only once, before the evolution – generates training data
bool SymbRegEvalOp::initialize(StateP state)
{
	x.clear();
	y.clear();
	f.clear();
	/*// check if the parameters are stated (used) in the conf. file
	// if not, we return false so the initialization fails
	if (!state->getRegistry()->isModified("x.data") || !state->getRegistry()->isModified("y.data") || !state->getRegistry()->isModified("f.data"))
			return false;
	// read from csv
	if(!csvRead(state,"x.data", &x)) return false;
	if(!csvRead(state,"y.data", &y)) return false;
	if(!csvRead(state,"f.data", &f)) return false;
	return true;
	*/
	nSamples = 10;
	double s = .1;
	double X = -10*s;
	double Y = -10*s;

	for (uint i = 0; i < nSamples; i++) {
		x.push_back(X);
		y.push_back(Y);
		Y += 2*s;
		X += 2*s;
	}

	for (uint i = 0; i < nSamples; i++) {
		X = x.at(i);
		for (uint j = 0; j < nSamples; j++) {
			Y = y.at(j);
			// F1
			//f.push_back(sin(X) + sin(Y*Y)); // F2
			//f.push_back(2*sin(X)*cos(Y)); // F3
			f.push_back(X*Y + sin((X + 1)*(Y - 1))); // F4
			//f.push_back(8 / (2+X*X+Y*Y)); // F5
			//f.push_back((X*X*X)/5+(Y*Y*Y)/2-X-Y); // F6
		}
	}
	return true;

}
bool SymbRegEvalOp::csvRead(StateP state, std::string entry, std::vector<double>* vec){
	std::ifstream stream;
	std::string line;
	voidP sptr = state->getRegistry()->getEntry(entry);
	std::string fname = *((std::string*) sptr.get());
	stream.open(fname);
	if (!stream.is_open()) return false;
	while (getline(stream, line)){
		vec->push_back(atof(line.c_str()));
	}
	return true;
}

void SymbRegEvalOp::registerParameters(StateP state){
	/*state->getRegistry()->registerEntry("x.data", (voidP)(new std::string), ECF::STRING);
	state->getRegistry()->registerEntry("y.data", (voidP)(new std::string), ECF::STRING);
	state->getRegistry()->registerEntry("f.data", (voidP)(new std::string), ECF::STRING);
	*/
}

FitnessP SymbRegEvalOp::evaluate(IndividualP individual)
{
	// we try to minimize the function value, so we use FitnessMin fitness (for minimization problems)
	FitnessP fitness(new FitnessMin);

	// get genotype
	GEPChromosomeP gep = std::static_pointer_cast<GEP::GEPChromosome> (individual->getGenotype());
	// The system is multigenic. We iterate over every gene, transform it to a tree, execute it and link it with the results of its fellow genes
	// The user specifies the linking function programatically or in the parameters

	//// a) Transform genes into subtrees and store them in a vector
	//std::vector<Tree::Tree*> tree;
	//tree.push_back(gep->makeCellTree());
	//for (uint g = 0; g < gep->genes; g++){
	//	tree.push_back(gep->toTree(g));
	////	cout << tree[g]->toString() << endl;
	//}


	// b) or just assemble the whole expression
	gep->assemble();


	double value = 0;
	double result;
	//nSamples = f.size();
	for(uint i = 0; i < nSamples; i++) {
		for (uint j = 0; j < nSamples; j++){
			result = 0;

			// execute the whole expression
			gep->setTerminalValue("X", &x.at(i));
			gep->setTerminalValue("Y", &y.at(j));
			gep->execute(&result);

			// or, execute every subtree and link manually
			/*
			for (uint g = 0; g < gep->genes; g++){
				// for each test data instance, the x value (domain) must be set
				tree.at(g)->setTerminalValue("X", &x.at(i));
				tree.at(g)->setTerminalValue("Y", &y.at(j));

				// get the f value of the current tree
				tree.at(g)->execute(&tmpResult);
				// link using addition (for example)
				result += tmpResult;
			}*/

			// add the difference
			value += pow(f.at(i*nSamples+j) - result, 2);
			 //+= fabs(f.at(i*nSamples + j) - result);
		}
	}
	value /= pow(nSamples,2);
	fitness->setValue(value);
	return fitness;
}
