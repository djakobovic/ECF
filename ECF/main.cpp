#include "ECF.h"

// Za svaki primjer je potrebno:
// a) definirati odgovarajuci EvaluateOp objekt koji se primjenjuje za evaluaciju jedinke
// b) definirati genotip (po zelji i druge parametre) u konf fajlu
//
// Svaki primjer ima posebnu funkciju main() - otkomentirati



//
// primjer dodavanja novog algoritma
//
class MyAlg : public Algorithm
{
protected:

	// declare all available selection operators (not all get used)
	SelFitnessProportionalOpP selFitOp_;
	SelRandomOpP selRandomOp_;
	SelBestOpP selBestOp_;
	SelWorstOpP selWorstOp_;
	// what individual to replace (worst or random)
	bool replaceWorst_;

public:

	// mandatory: define name, construct selection operators
	MyAlg()
	{
		// the algorithm name will be used in config file (see below)
		name_ = "MyAlg";
		selFitOp_ = (SelFitnessProportionalOpP) (new SelFitnessProportionalOp);
		selRandomOp_ = (SelRandomOpP) (new SelRandomOp);
		selBestOp_ = (SelBestOpP) (new SelBestOp);
		selWorstOp_ = (SelWorstOpP) (new SelWorstOp);
	}


	// optional: register any parameters
	void registerParameters(StateP state)
	{
		// HOW TO: define a parameter
		// string parameter, options: random, worst
		registerParameter(state, "replace", (voidP) (new std::string("random")), ECF::STRING);
	}


	// optional: initialize components, read parameters
	bool initialize(StateP state)
	{
		// selection operators must be initialized if used!
		selFitOp_->initialize(state);
		// optional: set ratio between the best and the worst individual's selection probability
		selFitOp_->setSelPressure(10);
		// if the ratio is < 1, the selection favours worse over better individuals
		//selFitOp_->setSelPressure(0.1);
		selRandomOp_->initialize(state);
		selBestOp_->initialize(state);
		selWorstOp_->initialize(state);

		// HOW TO: read a parameter value
		// get parameter, decide what to replace
		voidP par = getParameterValue(state, "replace");
		std::string replace = *((std::string*) par.get());
		replaceWorst_ = false;
		if(replace == "worst")
				replaceWorst_ = true;

		// HOW TO: check if genotype is of a specific kind
		// suppose we only accept FloatingPoint
		FloatingPointP flp (new FloatingPoint::FloatingPoint);
		if(state->getGenotypes()[0]->getName() != flp->getName()) {
			ECF_LOG_ERROR(state, "Error: this algorithm accepts only a single FloatingPoint genotype!");
			throw ("");
		}

		// HOW TO: read the dimension and domain boundaries
		voidP sptr = state->getGenotypes()[0]->getParameterValue(state, "dimension");
		uint numDimension = *((uint*) sptr.get());
		voidP lBound = state->getGenotypes()[0]->getParameterValue(state, "lbound");
		double lbound = *((double*) lBound.get());
		voidP uBound = state->getGenotypes()[0]->getParameterValue(state, "ubound");
		double ubound = *((double*) uBound.get());

		// HOW TO: add another genotype in all individuals (if algorithm requires)
		// new FloatingPoint genotype with same parameters
		FloatingPointP fp (static_cast<FloatingPoint::FloatingPoint*> (state->getGenotypes()[0]->copy()));
		//state->setGenotype(fp);
		fp->setParameterValue(state, "dimension", (voidP) new uint(numDimension));
		fp->setParameterValue(state, "lbound", (voidP) new double(lbound));
		fp->setParameterValue(state, "ubound", (voidP) new double(ubound));

		// HOW TO: read population (local deme) size
		uint popSize = state->getPopulation()->getLocalDeme()->getSize();

		return true;
	}


	// mandatory: perform single 'generation' (however the algorithm defines it)
	bool advanceGeneration(StateP state, DemeP deme)
	{
		// HOW TO: select parents
		IndividualP first = selFitOp_->select(*deme);
		IndividualP second = selBestOp_->select(*deme);

		// select child (random or worst)
		IndividualP child;
		if(replaceWorst_)
				child = selWorstOp_->select(*deme);
		else
				child = selRandomOp_->select(*deme);

		// HOW TO: cross two individuals
		mate(first, second, child);

		// HOW TO: mutate an individual
		// mutation probability defined in Registry!
		mutate(child);				
		// to explicitly mutate an individual:
		//mutation_->mutate(child);

		// HOW TO: evaluate an individual
		evaluate(child);

		// HOW TO: create a trial individual (e.g. a copy of an existing individual)
		IndividualP trial = (IndividualP) deme->at(0)->copy();

		// HOW TO: access individual data
		// get FloatingPoint genotype from individual
		FloatingPointP fp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (trial->getGenotype(0));
		// or use ordinary pointers:
		//FloatingPoint::FloatingPoint* fp = static_cast<FloatingPoint::FloatingPoint*> (trial->getGenotype().get());

		// HOW TO: change individual data
		fp->realValue[0] = 3.14;

		// HOW TO: replace an individual in deme
		// evaluate and compare with another individual
		evaluate(trial);
		if(trial->fitness->isBetterThan(child->fitness))
			// replace first with second:
			replaceWith(child, trial);

		// some other helper functions (see existing algorithms):
		// copy, replaceWith, removeFrom, isMember

		return true;
	}
};
typedef boost::shared_ptr<MyAlg> MyAlgP;








// 1. primjer: GA OneMax problem

#include "examples/GAonemax/OneMaxEvalOp.h"
int main(int argc, char **argv)
{
	argc = 2;	// hard coded za lakse isprobavanje :)
	//argv[1] = "./examples/GAOneMax/parametri.txt";

	StateP state (new State);

	//state->setEvalOp(static_cast<EvaluateOpP> (new OneMaxEvalOp));
	state->setEvalOp(new OneMaxEvalOp);

	state->initialize(argc, argv);
	state->run();

	return 0;
}



#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
namespace pt = boost::property_tree;

// 2. primjer: GA minimizacija funkcije
/*
#include "examples/GAFunctionMin/FunctionMinEvalOp.h"
int main(int argc, char **argv)
{
	argc = 2;
	argv[1] = "./examples/GAFunctionMin/parametri.txt";

	StateP state (new State);
	state->setEvalOp(new FunctionMinEvalOp);

	state->initialize(argc, argv);
	state->run();

	
	try {
		pt::ptree tree;
		pt::read_xml("./examples/GAFunctionMin/parametri.txt", tree);
		pt::ptree ecf = tree.get_child("ECF");
		pt::ptree genotype = ecf.get_child("Genotype");

	//	BOOST_FOREACH(pt::ptree::value_type &v, genotype.get_child("")) {
		for(pt::ptree::iterator it = genotype.begin(); it != genotype.end(); it++) {
			pt::ptree::value_type &v = *it;
			if(v.first == "FloatingPoint")
				;
		}
	}
	catch(const pt::ptree_error& er){
		std::cout << er.what() << std::endl;
	}





	return 0;

//
//	ispis populacije u txt fajl, za potrebe landscape analysis
//
	ofstream fajl("popis.txt");
	for(uint i = 0; i < state->getPopulation()->getLocalDeme()->getSize(); i++) {
		IndividualP ind = state->getPopulation()->getLocalDeme()->at(i);
		fajl << ind->fitness->getValue() << "\t";
		FloatingPointP fp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (ind->getGenotype());
		for(uint dim = 0; dim < fp->realValue.size(); dim++)
			fajl << fp->realValue[dim] << "\t";
		fp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (ind->getGenotype(2));
		fajl << fp->realValue[0];
		fajl << "\n";
	}

	return 0;
}
*/


// 2a. primjer: MO NSGA minimizacija funkcije
/*
#include "examples/MOFunctionMin/MOFunctionMinEvalOp.h"
int main(int argc, char **argv)
{
	argc = 2;
	argv[1] = "./examples/MOFunctionMin/parameters.txt";

	StateP state (new State);
	state->setEvalOp(new MOEvalOp);

	state->initialize(argc, argv);
	state->run();

	// ispis populacije na kraju
	std::ofstream myfile;
	myfile.open ("paretoFront.txt");
	DemeP deme = state->getPopulation()->getLocalDeme();
	for (uint i = 0; i<deme->size(); i++) {
		MOFitnessP fitness = boost::static_pointer_cast<MOFitness> (deme->at(i)->fitness);
		for (uint f = 0; f < fitness->size(); f++)
			myfile << fitness->at(f)->getValue() << "\t";
		myfile << "\n";
	}
	myfile.close();

	return 0;
}
*/



namespace Tree {

// primjer tipa podataka
struct my_type
{
	double v;
	bool b;
};

// terminal za doticni tip
class MyTerminal : public Primitives::Primitive
{
public:
	my_type value_;

	MyTerminal()
	{
		nArguments_ = 0;
	}
	void execute(void* result, Tree& tree)
	{
		my_type& res = *(my_type*)result;
		res = value_;
	}
	void setValue(void* value)
	{
		value_ = *(my_type*)value;
	}
	~MyTerminal()
	{	}
};

// primjer funkcije za korisnicki tip podataka
class MyFunc : public Primitives::Primitive
{
public:
	MyFunc()
	{
		nArguments_ = 2;
		name_ = "func";
	}
	void execute(void* result, Tree& tree)
	{
		my_type first, second;
		my_type& func = *(my_type*)result;

		getNextArgument(&first, tree);
		getNextArgument(&second, tree);
		
		func.b = first.b && second.b;
		func.v = first.v + second.v;
	}
	~MyFunc()
	{	}
};

}



// 3. primjer: GP simbolicka regresija
/*
#include "examples/GPSymbReg/SymbRegEvalOp.h"
#include "examples/GPSymbReg/zbr.h"
int main(int argc, char **argv)
{
//	argc = 2;
//	argv[1] = "./examples/GPSymbReg/parametri.txt";

	StateP state = static_cast<StateP> (new State);

	state->setEvalOp(static_cast<EvaluateOpP> (new SymbRegEvalOp));

	// primjer: dodavanje korisnickog operatora
//	MyOpP myOp = (MyOpP) (new MyOp);
//	state->addOperator(myOp);

	// primjer: dodavanje korisnickog algoritma
//	MyAlgP myAlg = (MyAlgP) (new MyAlg);
//	state->addAlgorithm(myAlg);

	// primjer: dodavanje korisnickog genotipa
//	MyGenotypeP myGenotype = (MyGenotypeP) (new MyGenotype);
//	state->addGenotype(myGenotype);

	// primjer: dodavanje korisnicke funkcije za stablo
	TreeP tree = (TreeP) new Tree::Tree;
	Tree::PrimitiveP zbr = (Tree::PrimitiveP) new Tree::Ad;
	tree->addFunction(zbr);

Tree::PrimitiveP myFunc = (Tree::PrimitiveP) new Tree::MyFunc;
tree->addFunction(myFunc);

Tree::PrimitiveP myTerm = (Tree::PrimitiveP) new Tree::MyTerminal;
myTerm->setName("term");
tree->addTerminal(myTerm);


	state->addGenotype(tree);

	if (!state->initialize(argc, argv))
		return 1;
	state->run();

	//std::vector<IndividualP> vec = state->getPopulation()->hof_->getBest();
	//IndividualP ind = vec[0];
	//state->getAlgorithm()->evaluate(ind);
	//XMLNode out;
	//ind->write(out);
	//std::cout << out.createXMLString() << std::endl;

	std::vector<IndividualP> vec = state->getPopulation()->getHof()->getBest();
	IndividualP ind = vec[0];
	XMLNode xml2 = XMLNode::parseString("<Individual size=""1"" gen=""77""><FitnessMin value=""4.26326e-14""/><Tree size=""36"">+ + * * X * X X - * X X + X X + sin X * X * X X / - * X X X / / X X + X X </Tree></Individual>", "Individual");
	ind->read(xml2);
	state->getAlgorithm()->evaluate(ind);
	std::cout << ind->toString();

	return 0;
}
*/


//4. primjer: GA problem trgovackog putnika, 29 gradova
/*
#include "examples/GATSP/TSPEvalOp.h"
int main(int argc, char **argv)
{
//	argc = 2;
//	argv[1] = "./examples/GATSP/parameters.txt";

	StateP state = static_cast<StateP> (new State);

	state->setEvalOp(static_cast<EvaluateOpP> (new TSPEvalOp));

	state->initialize(argc, argv);
	//state->getFitnessObject();

	state->run();

	return 0;
}
*/



//5. primjer: GA problem aproksimacije funkcije
/*
#include "examples/GAApprox/ApproxEvalOp.h"
int main(int argc, char **argv)
{
//	argc = 2;
//	argv[1] = "./examples/GAApprox/parameters.txt";

	StateP state(new State);

	state->setEvalOp(EvaluateOpP (new AproxEvalOp));

	state->initialize(argc, argv);
	state->run();

	return 0;
}
*/



//6. primjer: GP evolucija pravila rasporedjivanja
/*
#include "examples/GPScheduling/SchedulingEvalOp.h"
int main(int argc, char **argv)
{
	argc = 2;
	//argv[1] = "./examples/GPScheduling/parameters.txt";
	argv[1] = "./parameters.txt";

	StateP state = static_cast<StateP> (new State);

	state->setEvalOp(static_cast<EvaluateOpP> (new SchedulingEvalOp));

	state->initialize(argc, argv);
	state->run();

//	XMLNode xInd = XMLNode::parseFile("./ind.txt", "Individual");
//	IndividualP ind = (IndividualP) new Individual(state);
//	ind->read(xInd);
//	state->getAlgorithm()->evaluate(ind);
//	std::cout << ind->toString();

	return 0;
}
*/


//7. primjer: XCS
/*
#include "examples/XCSmux/MuxEvalOp.h"
#include "examples/XCSmaze/SingleObjMazeEnv.h"
#include "examples/XCSmaze/SeqObjMazeEnv.h"
#include "examples/XCSmaze/TwoObjMazeEnv.h"
#include "examples/XCSmaze/ThreeObjMazeEnv.h"

int main(int argc, char **argv)
{
	argc = 2;
	StateP state = static_cast<StateP> (new State);
	MazeEnvP maze;

	//Multistep:

	//	- sigle-objective maze:
		//argv[1] = "examples/XCSmaze/single-obj params.txt";
		//maze = static_cast<MazeEnvP> (new SingleObjMazeEnv(state));
		//maze->setMazeFile("examples/XCSmaze/Environments/single-objective/Maze1.txt");

	//	- multi-objective maze:
		//argv[1] = "examples/XCSmaze/seq-obj params.txt";
		//maze = static_cast<MazeEnvP> (new SeqObjMazeEnv(state));
		//maze->setMazeFile("examples/XCSmaze/Environments/multi-objective/Maze1k.txt");
	
		argv[1] = "examples/XCSmaze/three-obj params.txt";
		maze = static_cast<MazeEnvP> (new ThreeObjMazeEnv(state,0));
		maze->setMazeFile("examples/XCSmaze/Environments/multi-objective/Maze1em.txt");
	
	maze->setResultsFile("examples/XCSmaze/Maze1k results.txt");
	state->setEvalOp(maze);
	
	//Singlestep:

	//	- 6-multiplexor problem:
	//	argv[1] = "./examples/XCSmux/parametri.txt";
	//	state->setEvalOp(static_cast<EvaluateOpP> (new MuxEvalOp(state)));

	state->initialize(argc, argv);

	state->run();

	int a;
	cin >> a;
	return 0;
}
*/



//8. primjer: Kartezijski GP - feedforward
/*
#include "examples/CGPFeedForward/FeedForwardEvalOp.h"
#include "examples/CGPFeedForward/CGPEvalOp.h"
#include "cartesian/Cartesian.h"
using namespace cart;

int main(int argc, char **argv)
{
	argc = 2;
	argv[1] = "./examples/CGPFeedForward/parameters.txt";

	StateP state (new State);

	CartesianP cart (new Cartesian);

	state->addGenotype(cart);

	state->setEvalOp(new FunctionMinEvalOp);


	// izabrati koji tip
	//state->setEvalOp(new cart::FeedForwardEvalOpInt);
	//state->setEvalOp(static_cast<EvaluateOpP> (new cart::FeedForwardEvalOpDouble));
	//state->setEvalOp(static_cast<EvaluateOpP> (new cart::CircuitEvalOpUint));

	state->initialize(argc, argv);
	state->run();

	return 0;
}
*/
