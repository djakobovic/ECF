#pragma once

namespace Cartesian {

	class Function;
	typedef std::shared_ptr<Function> FunctionP;

	/**
	 * A node is an element of a CGP genotype. 
	 * 
	 * Each node points to a Function primitive and has a vector of indexes of argument nodes.
	 */
	class Node
	{
	public:
		Node()
		{	}
		Node(FunctionP primitive)
		{
			primitive_ = primitive;
		}
		~Node()
		{	}
		void setPrimitive(FunctionP primitive)
		{
			primitive_ = primitive;
		}

		FunctionP primitive_;           //< pointer to the Primitive belonging to this node
		std::vector<uint> arguments_;    //< indexes of argument nodes
	};
	typedef std::shared_ptr<Node> NodeP;

}