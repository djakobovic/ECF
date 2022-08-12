
/**
 * \ingroup ant
 * \brief GP function, executes 3 subtrees in sequence
 */
class Prog3 : public Tree::Primitives::Primitive
{
public:
	// 3 subtrees
	Prog3()
	{
		nArguments_ = 3;
		name_ = "p3";
	}

	void execute(void* evalOp, Tree::Tree& tree)
	{
		// execute all subtrees
		getNextArgument(evalOp, tree);
		getNextArgument(evalOp, tree);
		getNextArgument(evalOp, tree);
	}

	~Prog3()
	{	}
};