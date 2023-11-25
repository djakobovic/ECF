#ifndef Node_h
#define Node_h
#include <vector>
#include "Primitive.h"


namespace Tree
{

class Node;
typedef std::shared_ptr<Node> NodeP;

/**
 * \ingroup genotypes tree
 * \brief Node base class (Tree genotype)
 *
 * A node is an element of a Tree. Each node points to a Primitive object.
 */
class Node
{
public:
	Node();
	Node(PrimitiveP primitive);
	Node(NodeP node);
	~Node(void);
	void setPrimitive(PrimitiveP primitive);

	uint size_;       ///< size of the subtree of this node (including the node)
	uint depth_;      ///< depth of this node
	PrimitiveP primitive_;    ///< pointer to the Primitive belonging to this node
};
typedef std::shared_ptr<Node> NodeP;
}

#endif
