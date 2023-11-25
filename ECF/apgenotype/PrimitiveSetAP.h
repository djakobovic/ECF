#ifndef PrimitiveSetAP_h
#define PrimitiveSetAP_h
#include <vector>
#include "../ECF_base.h"
#include "../tree/Primitive.h"


namespace Tree
{

	/**
	 * \ingroup genotypes tree
	 * \brief Primitive set class for APGenotype: collects all Tree Primitives.
	 *
	 * Each Tree genotype (if there are more Trees in the Individual) has a unique PrimitiveSet.
	 * Each unique instance is linked to the appropriate Tree object in State (the 'hometree' variable name).
	 */
	class PrimitiveSetAP
	{
		
	public:
		PrimitiveSetAP();
		virtual ~PrimitiveSetAP();

		bool initialize(StateP state);

		//void addFunction(PrimitiveP functionPrimitive);
		bool addFunction(std::string name);
		void addTerminal(PrimitiveP terminalPrimitive);

		PrimitiveP getTerminalByName(std::string name);
		PrimitiveP getGFSOneByName(std::string name);
		PrimitiveP getGFSTwoByName(std::string name);
		PrimitiveP getPrimitiveByName(std::string name);

		PrimitiveP getTerminalByIndex(uint index);
		PrimitiveP getGFSOneByIndex(uint index);
		PrimitiveP getGFSTwoByIndex(uint index);
		PrimitiveP getPrimitiveByIndex(uint index);

		uint getGFSOneSetSize();
		uint getGFSTwoSetSize();
		uint getFunctionSetSize();
		uint getTerminalSetSize();
		uint getPrimitivesSize();

		std::map <std::string, PrimitiveP> mAllPrimitives_;                 //!< map of all registered primitive functions
		std::map <std::string, Primitives::terminal_type> mTypeNames_;

	protected:
		StateP state_;

		std::vector<PrimitiveP> terminalSet_;                               //!< vector of active (actual used) terminals
		std::map <std::string, PrimitiveP> mTerminalSet_;                   //!< map of active (actual used) terminals
		std::vector<PrimitiveP> GFSOneSet_;                               //!< vector of active (actual used) functions
		std::map <std::string, PrimitiveP> mGFSOneSet_;                   //!< map of active (actual used) functions
		std::vector<PrimitiveP> GFSTwoSet_;                               //!< vector of active (actual used) functions
		std::map <std::string, PrimitiveP> mGFSTwoSet_;                   //!< map of active (actual used) functions
		std::vector<PrimitiveP> primitives_;                                //!< vector of active (actual used) primitives
		std::map <std::string, PrimitiveP> mPrimitiveSet_;                  //!< map of active (actual used) primitives
	};
	typedef std::shared_ptr<PrimitiveSetAP> PrimitiveSetAPP;

	typedef std::map <std::string, PrimitiveP>::iterator prim_iter;
	typedef std::map <std::string, Primitives::terminal_type>::iterator type_iter;

}
#endif