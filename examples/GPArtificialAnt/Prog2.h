
/**
 * \ingroup ant
 * \brief GP function, executes 2 subtrees in sequence
 */
class Prog2 : public Tree::Primitives::Primitive
{
public:
	// 2 subtrees
	Prog2()
	{
		nArguments_ = 2;
		name_ = "p2";
	}

	void execute(void* evalOp, Tree::Tree& tree)
	{
		// execute all subtrees
		getNextArgument(evalOp, tree);
		getNextArgument(evalOp, tree);
	}

	~Prog2()
	{	}
};