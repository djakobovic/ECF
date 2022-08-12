#include <ECF/ECF.h>
#include "AntEvalOp.h"
#include "Prog2.h"
#include "Prog3.h"
#include "TurnLeft.h"
#include "IfFoodAhead.h"
#include "TurnRight.h"
#include "MoveAhead.h"
#include <iostream>
#include <fstream>
#include <string.h>


int main(int argc, char **argv)
{
	StateP state (new State);

	// crate evaluation operator
	state->setEvalOp(new AntEvalOp);
	
	// create tree genotype
	TreeP tree (new Tree::Tree);
	
	// create new functions and add them to function set 
	Tree::PrimitiveP ifa (new IfFoodAhead);
	tree->addFunction(ifa);
	Tree::PrimitiveP prog2 (new Prog2);
	tree->addFunction(prog2);
	Tree::PrimitiveP prog3 (new Prog3);
	tree->addFunction(prog3);
	Tree::PrimitiveP tl (new TurnLeft);
	tree->addTerminal(tl);
	Tree::PrimitiveP tr (new TurnRight);
	tree->addTerminal(tr);
	Tree::PrimitiveP mv (new MoveAhead);
	tree->addTerminal(mv);

	// register genotype with our primitives
	state->addGenotype(tree);

	// initialize and start evaluation
	if(!state->initialize(argc, argv))
		return 1;
	state->run();


	// after the evolution: show best evolved ant's behaviour on learning trails
	std::vector<IndividualP> hof = state->getHoF()->getBest();
	IndividualP ind = hof[0];
	std::cout << ind->toString();
	std::cout << "\nBest ant's performance on learning trail(s):" << std::endl;


	// show ant movement on the trail(s)
	AntEvalOp::trace = 1;

	// optional: show movements step by step (interactive)
	//AntEvalOp::step = 1;

	state->getEvalOp()->evaluate(ind);


	// also, simulate best evolved ant on a (different) test trail!
	std::cout << "\nBest ant's performance on test trail(s):" << std::endl;
	AntEvalOp* evalOp = new AntEvalOp;

	// substitute test trails for learning trails (defined in config file):
	state->getRegistry()->modifyEntry("learning_trails", state->getRegistry()->getEntry("test_trails"));
	evalOp->initialize(state);
	evalOp->evaluate(ind);


	// optional: write best individual to 'best.txt'
//	ofstream best("./best.txt");
//	best << ind->toString();
//	best.close();

    // optional: read individual from 'best.txt' (for subsequent simulation)
//	XMLNode xInd = XMLNode::parseFile("./best.txt", "Individual");
//	IndividualP ind = (IndividualP) new Individual(state);
//	ind->read(xInd); 

	return 0;
}
