#ifndef Tree_c_h
#define Tree_c_h
#include "Node.h"
#include "../Genotype.h"
#include <vector>
#define MAX_ARGS 10


namespace Tree
{

/**
 * \defgroup tree Tree
 * \ingroup genotypes
 */

class PrimitiveSet;
typedef std::shared_ptr<PrimitiveSet> PrimitiveSetP;

/**
 * \ingroup genotypes tree
 * \brief Tree class - implements genotype as a tree.
 *
 * Tree inherits a vector of Node objects. Each node points to a Primitive object. 
 * Nodes in tree are aligned in a prefix notation.
 * Some primitives are shared among all the trees, but some (such as ERC primitives) belong to a single Tree.
 */
class Tree : public std::vector<NodeP>, public Genotype
{
protected:
	uint growBuild(PrimitiveSetP primitiveSet, uint myDepth);
	uint fullBuild(PrimitiveSetP primitiveSet, uint myDepth);
	uint setSize(uint);
	void setDepth(uint myDepth);
	virtual void initializeFirst(Tree* hometree);

	std::vector<PrimitiveP> userFunctions_;	// programatically added functions
	std::vector<PrimitiveP> userTerminals_;	// programatically added terminals
	StateP state_;

public:
	Tree();
	~Tree();
	Tree* copy();

	void growBuild(PrimitiveSetP primitiveSet);
	void fullBuild(PrimitiveSetP primitiveSet);
	void update();
	void execute(void*);
	void addNode(Node* node);
	void addNode(NodeP node);
	void setTerminalValue(std::string, void*);
	void getTerminalValue(std::string, void*);
	void write(XMLNode&);
	void read(XMLNode&);
	void registerParameters(StateP);
	bool initialize (StateP state);

	std::vector<MutationOpP> getMutationOp();
	std::vector<CrossoverOpP> getCrossoverOp();

	bool addFunction(PrimitiveP);
	bool addTerminal(PrimitiveP);

	uint getMaxDepth()
	{	return maxDepth_;	}

	uint getMinDepth()
	{	return minDepth_;	}

	uint getStartDepth()
	{	return startDepth_;	}

	PrimitiveSetP primitiveSet_;
	uint iNode_;                 ///< current node index (when parsing the tree)
	uint startDepth_;            ///< start depth of the first node (0 by default)
	uint maxDepth_;              ///< max allowed Tree depth
	uint minDepth_;              ///< min allowed Tree depth
	uint initMaxDepth_;          ///< max allowed Tree depth at initialization (maxDepth_ used if not defined)
	uint initMinDepth_;          ///< min allowed Tree depth at initialization (minDepth_ used if not defined)
};
}
typedef std::shared_ptr<Tree::Tree> TreeP;

#endif

