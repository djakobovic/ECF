#ifndef PrimitiveSet_h
#define PrimitiveSet_h
#include <vector>
#include "Primitive.h"


namespace Tree
{

/**
 * \ingroup genotypes tree
 * \brief Primitive set class: collects all Tree Primitives.
 *
 * Each Tree genotype (if there are more Trees in the Individual) has a unique PrimitiveSet. 
 * Each unique instance is linked to the appropriate Tree object in State (the 'hometree' variable name).
 */
class PrimitiveSet
{
	friend class Tree;
public:
	PrimitiveSet();
	virtual ~PrimitiveSet();

	bool initialize(StateP state);

	//void addFunction(PrimitiveP functionPrimitive);
	bool addFunction(std::string name);
	void addTerminal(PrimitiveP terminalPrimitive);

	PrimitiveP getRandomTerminal();
	PrimitiveP getRandomFunction();
	PrimitiveP getRandomPrimitive();
	
	PrimitiveP getTerminalByName(std::string name);
	PrimitiveP getFunctionByName(std::string name);
	PrimitiveP getPrimitiveByName(std::string name);
	
	uint getFunctionSetSize();
	uint getTerminalSetSize();
	uint getPrimitivesSize();

	std::map <std::string, PrimitiveP> mAllPrimitives_;                 //!< map of all registered primitive functions
	std::map <std::string, Primitives::terminal_type> mTypeNames_;

protected:
	StateP state_;

	std::vector<PrimitiveP> terminalSet_;                               //!< vector of active (actual used) terminals
	std::map <std::string, PrimitiveP> mTerminalSet_;                   //!< map of active (actual used) terminals
	std::vector<PrimitiveP> functionSet_;                               //!< vector of active (actual used) functions
	std::map <std::string, PrimitiveP> mFunctionSet_;                   //!< map of active (actual used) functions
	std::vector<PrimitiveP> primitives_;                                //!< vector of active (actual used) primitives
	std::map <std::string, PrimitiveP> mPrimitiveSet_;                  //!< map of active (actual used) primitives
};
typedef boost::shared_ptr<PrimitiveSet> PrimitiveSetP;

typedef std::map <std::string, PrimitiveP>::iterator prim_iter;
typedef std::map <std::string, Primitives::terminal_type>::iterator type_iter;

}
#endif
