#ifndef TreeMutGauss_h
#define TreeMutGauss_h

#include "boost/random/lagged_fibonacci.hpp"


namespace Tree
{

/**
 * \ingroup genotypes tree
 * \brief Tree genotype: standard normal distribution noise mutation operator.
 * Applicable only on ephemereal random constants (ERC) of type 'double'.
 */
class TreeMutGauss : public MutationOp
{
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);

protected:
	boost::lagged_fibonacci607 engine_;
};
typedef boost::shared_ptr<TreeMutGauss> TreeMutGaussP;
}
#endif // TreeMutGauss_h
