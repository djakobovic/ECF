#ifndef SelFitnessProportionalOp_h
#define SelFitnessProportionalOp_h

#include "SelectionOperator.h"

/**
 * \ingroup evol selection
 * \brief Fitness proportional (and inverse proportional) individual selection operator.
 *
 * \par Functionality
 * The selection operator assigns a positive value 'S' to each individual, and chooses
 * an individual with proportion to the value 'S'. 
 * The selection pressure parameter 'P' states how much is the best individual 'better' than the worst, 
 * i.e. how greater chance of selection has the best individual related to the worst one. 
 * (the worst individual always has S = 1, and the best S = P) 
 *
 * \par Inverse proportional selection
 * For elimination purpose, the P may be set to a value lower than 1; in that case, the worst
 * individual will have the greatest chance of selection, and the best individual the lowest chance.
 *
 * \par Examples
 * - if P = 10, then the best individual has the selection value 10 and worst value 1
 * - if P = 0.1, then the best individual has the selection value 0.1 (i.e. has 10 times lesser chance to 
 * be selected than the worst individual!)
 */
class SelFitnessProportionalOp : public SelectionOperator
{
protected:
	double selPressure_;    //!< the ratio of selection probability of the best and worst individual in the set

public:
	bool initialize(StateP);

	IndividualP select(const std::vector<IndividualP>&);

	/// Repeatedly select from the same pool (duplicates allowed)
	std::vector<IndividualP> selectMany(const std::vector<IndividualP>&, uint);

	/// Set selection pressure value
	bool setSelPressure(double);

};
typedef boost::shared_ptr<SelFitnessProportionalOp> SelFitnessProportionalOpP;

#endif // SelFitnessProportionalOp_h

