#ifndef RVG_H_
#define RVG_H_

#include "./Genotype.h"

/**
 * \ingroup genotypes
 */

/**
 * \ingroup genotypes
 * \brief RealValueGenotype class - abstract genotype class for genotypes that represent a vector of real values (Binary, FloatingPoint)
 *
 * parameters:
 * - lbound: lower bound of represented real value
 * - ubound: upper bound of represented real value
 * - dimension: number of distinct real values
 */
class RealValueGenotype: public Genotype {

protected:
	double minValue_;                 //!< lower bound
	double maxValue_;                 //!< upper bound
	uint nDimension_;                 //!< dimensionality


public:
	std::vector<double> realValue;     //!< vector of floating point values

	/// return lower bound of the defined interval
	double getLBound ()
	{	return minValue_;	}

	/// return upper bound of the defined interval
	double getUBound ()
	{	return maxValue_;	}

	/// Update genotype after genetic material change via genetic operators.
	virtual bool update (void)
	{	return true;	}

};
typedef boost::shared_ptr<RealValueGenotype>  RealValueGenotypeP;

#endif