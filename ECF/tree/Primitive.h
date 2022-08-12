#ifndef Primitive_h
#define Primitive_h


namespace Tree {

#define DBL_PREFIX "D_" 
#define INT_PREFIX "I_" 
#define BOOL_PREFIX "B_"
#define CHR_PREFIX "C_" 
#define STR_PREFIX "S_" 

class Tree;
namespace Primitives {


/**
 * \defgroup primitives Tree Primitives
 * \ingroup tree
 */


enum terminal_type
{ Double, Int, Bool, Char, String };

class Primitive;
typedef boost::shared_ptr<Primitive> PrimitiveP;

/**
 * \ingroup tree genotypes primitives
 * \brief Base primitive class (Tree genotype).
 *
 * Each node of a tree points to a single primitive, which may be a function or a variable (of Terminal class).
 * Some primitives are shared among all the trees (i.e. have the same value and functionality), but some (like ERC) are owned by a single Tree.
 */
class Primitive
{
public:
	Primitive();
	virtual ~Primitive();

	/**
	 * \brief	Execute the primitive.
	 *
	 * Function primitives gather arguments and return a value; terminal primitives return current value.
	 */
	virtual void execute(void* result, Tree& tree) = 0; 
	virtual bool initialize(StateP state);
	virtual void setValue(void* value);
	virtual void getValue(void* value);
	virtual PrimitiveP copyWithNode(PrimitiveP);
	virtual PrimitiveP assignToNode(PrimitiveP);
	void getNextArgument(void* result, Tree& tree);
	void skipNextArgument(Tree& tree);
	int getNumberOfArguments();
	void setName(std::string name);
	std::string getName();
	std::string getComplementName();

	StateP state_;

protected:
	std::string name_;
	int nArguments_;
	std::string complementName_;
};

}
typedef boost::shared_ptr<Primitives::Primitive> PrimitiveP;
}

#endif 