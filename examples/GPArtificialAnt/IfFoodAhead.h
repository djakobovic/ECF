#include "AntEvalOp.h"


/**
 * \ingroup ant
 * \brief GP function, checks if the food is ahead
 *
 * If the food is ahead, execute the first subtree, else the second one
 */
class IfFoodAhead : public Tree::Primitives::Primitive
{
public:
	IfFoodAhead()
	{
		nArguments_ = 2;
		name_ = "ifa";
	}

	void execute(void* evalOp, Tree::Tree& tree)
	{
		// get pointer to simulator from the context
		AntEvalOp* ant = (AntEvalOp*) state_->getContext()->environment;

		// query the simulator: are we facing food?
		if(ant->facingFood()) {
			// if yes, execute the first subtree
			getNextArgument(evalOp, tree);
			// and skip the second one
			skipNextArgument(tree);
		} else {
			// otherwise...
			skipNextArgument(tree);
			getNextArgument(evalOp, tree);
		}
	}

	~IfFoodAhead()
	{	}
};