#ifndef FLOATINGPOINTMUTSIMPLE_H_
#define FLOATINGPOINTMUTSIMPLE_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: simple mutation where a single vector element is mutated. New value is random value from the given domain.
 */
class FloatingPointMutSimple: public MutationOp 
{
protected:
public:
	bool mutate(GenotypeP gene);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<FloatingPointMutSimple> FloatingPointMutSimpleP;
}
#endif /* FLOATINGPOINTMUTSIMPLE_H_ */

