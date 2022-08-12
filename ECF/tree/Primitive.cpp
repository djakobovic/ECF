#include "../ECF_base.h"
#include "Tree_c.h"
#include <iostream>


namespace Tree {
namespace Primitives {

Primitive::Primitive()
{
	// default: no complement
	complementName_ = "";
}


Primitive::~Primitive()
{	}


/**
 * \brief	Initialize the primitive (default just sets the StateP pointer).
 */
bool Primitive::initialize(StateP state)
{
	state_ = state;
	return true;
}


/**
 * \brief	Assign primitive to node (when building a tree).
 * The default behaviour just returns the same pointer (common primitives).
 * ERC's should override and generate a new object with a new value.
 * \param	sptr to this primitive
 */
PrimitiveP Primitive::assignToNode(PrimitiveP primitive)
{
	return primitive;
}


void Primitive::setValue(void* value)
{
	return;
}


void Primitive::getValue(void* value)
{
	return;
}


/**
 * \brief	Copy primitive (when copying a node, e.g. in crossover).
 * The default behaviour just returns the same pointer (common primitives).
 * (ERC's should override and make a new copy of the original object).
 * \param	sptr to this primitive
 */
PrimitiveP Primitive::copyWithNode(PrimitiveP primitive)
{
	return primitive;
}


/**
 * \brief	Execute next child node's primitive (execute next subtree).
 * \param	result result of the operation
 * \param	tree reference to current tree
 */
void Primitive::getNextArgument(void* result, Tree& tree)
{
	tree.iNode_++;
	tree[tree.iNode_]->primitive_->execute(result, tree);
}


/**
 * \brief	Skip next child subtree (doesn't execute the subtree).
 * \param	tree reference to current tree
 */
void Primitive::skipNextArgument(Tree& tree)
{
	tree.iNode_ += tree[tree.iNode_ + 1]->size_;
}


/**
 * \brief	Return primitive's number of arguments.
 */
int Primitive::getNumberOfArguments()
{
	return nArguments_;
}


/**
 * \brief	Set primitive's name.
 */
void Primitive::setName(std::string name)
{
	name_ = name;
}


/**
 * \brief	Get primitive's name.
 */
std::string Primitive::getName()
{
	return name_;
}


/**
 * \brief	Get complement's name.
 */
std::string Primitive::getComplementName()
{
	return complementName_;
}

}
}
