#ifndef PERMUTATION_H_
#define PERMUTATION_H_

#include "PermutationCrsPMX.h"
#include "PermutationMutToggle.h"
#include "PermutationMutIns.h"
#include "PermutationMutInv.h"
#include "PermutationCrsPBX.h"
#include "PermutationCrsOX.h"
#include "PermutationCrsCOSA.h"
#include "PermutationCrsCyclic.h"
#include "PermutationCrsCyclic2.h"
#include "PermutationCrsOBX.h"
#include "PermutationCrsULX.h"
#include "PermutationCrsOX2.h"
#include "PermutationCrsOPX.h"
#include "PermutationCrsSPX.h"
#include "PermutationCrsDPX.h"
#include "PermutationCrsUPMX.h"


#include "../Genotype.h"


namespace Permutation
{

/**
 * \defgroup permutation Permutation
 * \ingroup genotypes
 */

/**
 * \ingroup genotypes permutation
 * \brief Permutation class - implements genotype as a vector of indices 0..(n-1) (permutation of indices)
 */
class Permutation: public Genotype {
protected:
	uint size_;	//!< permutation vector size
public:
	typedef std::vector<int> var;
	var variables;

	Permutation () {name_ = "Permutation";}

	uint getSize()	//!< return size of indices vector
	{	return size_;	}

	bool initialize (StateP state);

	void registerParameters(StateP state);

	Permutation* copy()
	{
		Permutation *newObject = new Permutation(*this);
		return newObject;
	}

	std::vector<CrossoverOpP> getCrossoverOp()
	{
		std::vector<CrossoverOpP> crx;
		crx.push_back(static_cast<CrossoverOpP> (new PermutationCrsPMX));
		crx.push_back(static_cast<CrossoverOpP> (new PermutationCrsPBX));
		crx.push_back(static_cast<CrossoverOpP> (new PermutationCrsOX));
		crx.push_back(static_cast<CrossoverOpP> (new PermutationCrsCOSA));
		crx.push_back(static_cast<CrossoverOpP> (new PermutationCrsCyclic));
		crx.push_back(static_cast<CrossoverOpP> (new PermutationCrsCyclic2));
		crx.push_back(static_cast<CrossoverOpP> (new PermutationCrsOBX));
		crx.push_back(static_cast<CrossoverOpP> (new PermutationCrsULX));
		crx.push_back(static_cast<CrossoverOpP> (new PermutationCrsOX2));
		crx.push_back(static_cast<CrossoverOpP> (new PermutationCrsOPX));
		crx.push_back(static_cast<CrossoverOpP> (new PermutationCrsSPX));
		crx.push_back(static_cast<CrossoverOpP> (new PermutationCrsDPX));
		crx.push_back(static_cast<CrossoverOpP> (new PermutationCrsUPMX));
		return crx;
	}

	std::vector<MutationOpP> getMutationOp()
	{
		std::vector<MutationOpP> mut;
		mut.push_back(static_cast<MutationOpP> (new PermutationMutToggle));
		mut.push_back(static_cast<MutationOpP> (new PermutationMutIns));
		mut.push_back(static_cast<MutationOpP> (new PermutationMutInv));
		return mut;

	}

	void write(XMLNode&);
	void read(XMLNode&);
};
}

typedef std::shared_ptr<Permutation::Permutation> PermutationP;
#endif
