#include "../ECF_base.h"
#include "Node.h"
#include <iostream>


namespace Tree
{

Node::Node(void)
{
	size_ = 1;
}


/**
 * \brief	Create a copy of an existing Node.
 * The associated primitive (pointer) is copied.
 */
Node::Node(NodeP node)
{
	size_ = node->size_;
	depth_ = node->depth_;
	primitive_ = node->primitive_->copyWithNode(node->primitive_);
}


/**
 * \brief	Create a copy of an existing Node with its Primitive pointer.
 * The associated primitive (pointer) is copied.
 */
Node::Node(PrimitiveP primitive)
{
	primitive_ = primitive->copyWithNode(primitive);
	size_ = 1;
}


Node::~Node(void)
{	}


/**
 * \brief	Set the primitive this node points to (when creating a new tree node).
 * In case of an ephemereal random constant primitive, new primitive object is created.
 */
void Node::setPrimitive(PrimitiveP primitive)
{
	primitive_ = primitive->assignToNode(primitive);
}

}