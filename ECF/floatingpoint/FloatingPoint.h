#ifndef FLOATINGPOINT_H_
#define FLOATINGPOINT_H_

#include "../RealValueGenotype.h"
#include "FloatingPointMutSimple.h"
#include "FloatingPointCrsOnePoint.h"
#include "FloatingPointCrsArithmetic.h"
#include "FloatingPointCrsDiscrete.h"
#include "FloatingPointCrsArithmeticSimple.h"
#include "FloatingPointCrsArithmeticSingle.h"
#include "FloatingPointCrsAverage.h"
#include "FloatingPointCrsFlat.h"
#include "FloatingPointCrsBlx.h"
#include "FloatingPointCrsHeuristic.h"
#include "FloatingPointCrsRandom.h"
#include "FloatingPointCrsSbx.h"
#include "FloatingPointCrsBga.h"
#include "FloatingPointCrsLocal.h"
#include "FloatingPointCrsBlxAlpha.h"
#include "FloatingPointCrsBlxAlphaBeta.h"

namespace FloatingPoint
{

/**
 * \defgroup flpoint FloatingPoint
 * \ingroup genotypes
 */

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint class - implements genotype as a vector of floating point values
 *
 * FloatingPoint genotype parameters are (inherited from RealValueGenoptype):
 * - lbound: lower bound of represented real value
 * - ubound: upper bound of represented real value
 * - dimension: number of distinct real values
 */
class FloatingPoint: public RealValueGenotype {

public:

	FloatingPoint ()
	{	name_="FloatingPoint";	}

	bool initialize (StateP state);

	void registerParameters(StateP state);

	/// return lower bound of the defined interval
	double getLBound ()
	{	return minValue_;	}

	/// return upper bound of the defined interval
	double getUBound ()
	{	return maxValue_;	}

	FloatingPoint* copy()
	{
		FloatingPoint *newObject = new FloatingPoint(*this);
		return newObject;
	}

	/// return usable crx operators
	std::vector<CrossoverOpP> getCrossoverOp()
	{
		std::vector<CrossoverOpP> crx;
		crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsOnePoint));
		crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsArithmetic));
		crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsDiscrete));
		crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsArithmeticSimple));
		crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsArithmeticSingle));
		crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsAverage));
		crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsFlat));
		crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsHeuristic));
		crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsSbx));
		crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsBga));
		crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsLocal));
		crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsBlxAlpha));
		crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsBlxAlphaBeta));
		// control operator - not to be used in optimization
		//crx.push_back(static_cast<CrossoverOpP> (new FloatingPointCrsRandom));
		return crx;
	}

	std::vector<MutationOpP> getMutationOp()
	{
		std::vector<MutationOpP> mut;
		mut.push_back(static_cast<MutationOpP> (new FloatingPointMutSimple));
		return mut;
	}

	void write(XMLNode& xFloatingPoint);
	void read(XMLNode& xFloatingPoint);
};
}

typedef boost::shared_ptr<FloatingPoint::FloatingPoint> FloatingPointP;
#endif