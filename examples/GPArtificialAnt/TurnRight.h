#include "AntEvalOp.h"


/**
 * \ingroup ant
 * \brief GP terminal, turns the ant right
 */
class TurnRight : public Tree::Primitives::Primitive
{
public:
	TurnRight()
	{
		nArguments_ = 0;
		name_ = "right";
	}

	void execute(void* evalOp, Tree::Tree& tree)
	{
		// get pointer to simulator from the context
		AntEvalOp* ant = (AntEvalOp*) state_->getContext()->environment;
		// turn us right
		ant->turnRight();
	}

	~TurnRight()
	{	}
};