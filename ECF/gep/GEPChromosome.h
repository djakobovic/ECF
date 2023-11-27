#ifndef GEPCHROMOSOME_H
#define GEPCHROMOSOME_H

#include "../ECF.h"
#include "../tree/Tree.h"
#include "GEPChromosomeCrsOnePoint.h"
#include "GEPChromosomeCrsTwoPoint.h"
#include "GEPChromosomeCrsGene.h"
#include "GEPChromosomeMutOp.h"
#include "GEPChromosomeMutGauss.h"
#include <vector>
#define GEP_GENE_PREFIX "GENE_"

namespace GEP{

	/**
	* \defgroup gep GEP
	* \ingroup genotypes
	*/
	/**
	* \ingroup genotypes gep
	* \brief GEPChromosome class - implements genotype as a Gene Expression Programming chromosome.
	*
	* This representation consists of one or more genes, each consisting of two main parts and an optional third component.
	* The first part is called the Head. It has a size specified by the user and may contain any primitive.
	* The second part is called the Tail. It may contain only terminals and has a fixed size (depending on the maximum arity of the primitive set and the head length).  These two properties, along with the expression procedure, guarantee that every GEP chromosome encodes a valid expression.
	* The optional third part is called the "Dc domain". It contains the values of random constants and has the same size as the Tail. If used, a constant placeholder primitive "?" is added to the primitive set.
	* GEPChromosome inherits a fixed-size vector of Node objects. Each node points to a Primitive object.
	* Nodes in tree are aligned in a breadth-first notation.
	* Being fixed-size and breadth-first-expressed, this means that non-coding neutral node regions can exist in any given individual.
	* Some primitives are shared among all the trees, but some (such as ERC primitives) belong to a single GEPChromosome.
	*/
	class GEPChromosome : public std::vector<Tree::NodeP>, public Genotype
	{
	protected:
		StateP state_;
		std::vector<Tree::PrimitiveP> userFunctions_;	// programatically added functions
		std::vector<Tree::PrimitiveP> userTerminals_;	// programatically added terminals
	public:
		uint headLength; ///< length of the head. User-specified 
		uint tailLength; ///< length of the tail. Automatically calculated.
		uint dcLength;  ///< length of the constant values domain
		uint geneLength; ///< total length of each gene
		uint linkHeadLength; ///< length of the linking function gene's head
		uint linkTailLength; ///< length of the linking function gene's tail
		uint genes; ///< number of genes
		Tree::PrimitiveSetP primitiveSet_; // contains all functions, terminals and the ERC placeholder (if needed)
		Tree::PrimitiveSetP linkFunctionSet_; // contains all linking functions and genic terminals (integers)
		Tree::PrimitiveSetP ercSet_; // contains only the ERC ranges
		bool usesERC; ///< whether or not the chromosome uses random constants
		bool staticLink; ///< whether we are using a static linking function or if it should be allowed to evolve
		Tree::Tree *cellTree;
		std::vector<Tree::Tree*> subtrees;

		GEPChromosome();
		GEPChromosome* copy();
		std::vector<CrossoverOpP> getCrossoverOp();
		std::vector<MutationOpP> getMutationOp();
		void registerParameters(StateP state);
		bool initialize(StateP state);
		void initializeFirst(GEPChromosome* home);
		void write(XMLNode &xGEPChromosome);
		void read(XMLNode& xGEPChromosomeInd);
		void execute(void*);
		Tree::Tree* toTree(uint gene); 
		Tree::Tree* makeCellTree();
		void assemble();
		void generateChromosome();
		void setTerminalValue(std::string name, void* value);
	};
}
typedef std::shared_ptr<GEP::GEPChromosome> GEPChromosomeP;
#endif // GEPChromosome_h
