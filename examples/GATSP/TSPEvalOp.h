#ifndef TSPEVALOP_H_
#define TSPEVALOP_H_

#include <cmath>
#include <iostream>
#include <string>
#include <fstream>


/**
 * \defgroup tsp Traveling Salesperson Problem (TSP)
 * \ingroup examples
 * \brief TSP example - illustration for Permutation genotype (see more on this example in help/tutorial.html)
 *
 * \par Description
 * The TSP is a classic combinatorial problem which represents a large number of problem instances.
 * The objective is to find a route connecting all the 'cities' that minimizes the total traveled distance.
 * The test examples are copied from the TSPLIB repository at http://www.iwr.uni-heidelberg.de/groups/comopt/software/TSPLIB95/.
 * (pick additional examples as needed!)
 * 
 * \par Parameters (in the config file)
 * - path to a TSP file containing distances between cities (e.g. 'bays29.tsp') must be given
 * - for Permutation genotype, you only need to define its size (number of cities in this case)
 * (update: in current revision the size is auto-adjusted to the number of cities in the TSP file!)
 *
 */

/**
 * \ingroup tsp
 * \brief TSP evaluation operator.
 */
class TSPEvalOp : public EvaluateOp 
{
private:
	int dimension;
	std::vector< std::vector<int> > weights;
	std::vector< std::vector<double> > coordinates;
public:
	void registerParameters(StateP);
	bool initialize(StateP);
	FitnessP evaluate(IndividualP individual);
};
typedef boost::shared_ptr<TSPEvalOp> TSPEvalOpP;

#endif /* TSPEVALOP_H_ */
