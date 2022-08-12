#ifndef FLOATINGPOINTCRSHEURISTIC_H_
#define FLOATINGPOINTCRSHEURISTIC_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: value on allele i smaller gene value + rand value * (greater - smaller value) 
 */
class FloatingPointCrsHeuristic: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<FloatingPointCrsHeuristic> FloatingPointCrsHeuristicP;
}
#endif /* FLOATINGPOINTCRSHEURISTIC_H_ */

