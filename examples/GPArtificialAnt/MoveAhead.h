#include "AntEvalOp.h"


/**
 * \ingroup ant
 * \brief GP terminal, moves the ant one square ahead
 */
class MoveAhead : public Tree::Primitives::Primitive
{
public:
	MoveAhead()
	{
		nArguments_ = 0;
		name_ = "move";
	}

	void execute(void* evalOp, Tree::Tree& tree)
	{
		// get pointer to simulator from the context
		AntEvalOp* ant = (AntEvalOp*) state_->getContext()->environment;
		ant->moveAhead();
	}

	~MoveAhead()
	{	}
};