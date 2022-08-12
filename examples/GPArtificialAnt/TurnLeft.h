#include "AntEvalOp.h"


/**
 * \ingroup ant
 * \brief GP terminal, turns the ant left
 */
class TurnLeft : public Tree::Primitives::Primitive
{
public:
	TurnLeft()
	{
		nArguments_ = 0;
		name_ = "left";
	}

	void execute(void* evalOp, Tree::Tree& tree)
	{
		// get pointer to simulator from the context
		AntEvalOp* ant = (AntEvalOp*) state_->getContext()->environment;
		// turn us left
		ant->turnLeft();
	}

	~TurnLeft()
	{	}
};