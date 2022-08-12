#ifndef CartesianMutOnePoint_h
#define CartesianMutOnePoint_h

#include "../ECF_base.h"
#include "Cartesian.h"

namespace cart {
/**
 * \brief Cartesian genotype: one point mutation operator
 */
class CartesianMutOnePoint: public MutationOp 
{
protected:
	double mutProb_;
	bool useMutProb_;

	void mutOneValue(Cartesian* mut, int mutPoint);

public:
	/**
	Mutation point is randomly chosen between all integer values in genotype.
	Chosen integer for mutation might be input connection, function or output. Considering those three
	cases, predetermined rules are applied in order to make valid genotype by mutation.
	*/
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<CartesianMutOnePoint> CartesianMutOnePointP;

}

#endif /* CartesianMutOnePoint_h */