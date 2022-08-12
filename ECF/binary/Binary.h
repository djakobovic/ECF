#ifndef BINARY_H_
#define BINARY_H_

#include "BinaryCrsOnePoint.h"
#include "BinaryCrsTwoPoint.h"
#include "BinaryMutSimple.h"
#include "BinaryMutMix.h"
#include "BinaryCrsUniform.h"
#include "BinaryCrsHalfUniform.h"
#include "BinaryCrsReducedSurrogate.h"
#include "BinaryCrsSegmented.h"
#include "BinaryCrsShuffle.h"
#include "BinaryCrsNonGeometric.h"
#include "BinaryCrsRandomRespectful.h"
#include "BinaryCrsMasked.h"

#include "../RealValueGenotype.h"

namespace Binary
{

/**
 * \defgroup binary Binary
 * \ingroup genotypes
 */

/**
 * \ingroup genotypes binary
 * \brief Binary class - implements genotype as a vector of binary coded real values with variable interval and precision
 *
 * Binary genotype parameters are (the first 3 are inherited from RealValueGenoptype):
 * - lbound: lower bound of represented real value
 * - ubound: upper bound of represented real value
 * - dimension: number of distinct real values, each coded with previous 3 parameters
 * - precision: desired number of digits (after the decimal point) to be represented
 * - rounding: whether the represented real value is rounded to 'precision' digits (false by default)
 */
class Binary: public RealValueGenotype {
	friend class BinaryMutSimple;	// za potrebe ad-hoc lokalnog operatora!
protected:
	uint nDecimal_;                   //!< number of digits after the decimal point
	uint nBits_;                      //!< number of bits for a single dimension
	long potention_;
	bool bRounding_;                  //!< use rounding with 'precision' digits on real values in the chromosome
	typedef std::vector<bool> v_bool;
	v_bool vBool_;                    //!< temporary bool vector 

	double logbase(double a, double base);
	double round(double, int);

public:
	std::vector<long int> decValue;    //!< integer values of bit representation
	std::vector<v_bool> variables;     //!< vector of bit vectors

	Binary ()
	{	name_ = "Binary";	}

	/// Return number of bits used in a single dimension in real domain.
	uint getNumBits()
	{	return nBits_;	}

	bool initialize (StateP state);

	void registerParameters(StateP state);

	/// Update genotype after genetic material change via genetic operators.
	bool update (void);

	Binary* copy()
	{
		Binary *newObject = new Binary(*this);
		return newObject;
	}

	std::vector<CrossoverOpP> getCrossoverOp()
	{
		std::vector<CrossoverOpP> crx;
		crx.push_back(static_cast<CrossoverOpP> (new BinaryCrsOnePoint));
		crx.push_back(static_cast<CrossoverOpP> (new BinaryCrsTwoPoint));
		crx.push_back(static_cast<CrossoverOpP> (new BinaryCrsUniform));
		crx.push_back(static_cast<CrossoverOpP> (new BinaryCrsHalfUniform));
		crx.push_back(static_cast<CrossoverOpP> (new BinaryCrsReducedSurrogate));
		crx.push_back(static_cast<CrossoverOpP> (new BinaryCrsSegmented));
		crx.push_back(static_cast<CrossoverOpP> (new BinaryCrsShuffle));
		crx.push_back(static_cast<CrossoverOpP> (new BinaryCrsNonGeometric));
		crx.push_back(static_cast<CrossoverOpP> (new BinaryCrsRandomRespectful));
		// TODO: reimplement
		//crx.push_back(static_cast<CrossoverOpP> (new BinaryCrsMasked));
		return crx;
	}

	std::vector<MutationOpP> getMutationOp()
	{	
		std::vector<MutationOpP> mut;
		mut.push_back(static_cast<MutationOpP> (new BinaryMutSimple));
		mut.push_back(static_cast<MutationOpP> (new BinaryMutMix));
		return mut;
	}

	void write(XMLNode& xBinary);

	void read(XMLNode& xBinary);
};

}
typedef boost::shared_ptr<Binary::Binary> BinaryP;
#endif
