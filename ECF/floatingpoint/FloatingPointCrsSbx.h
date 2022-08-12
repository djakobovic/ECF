#ifndef FLOATINGPOINTCRSSBX_H_
#define FLOATINGPOINTCRSSBX_H_


namespace FloatingPoint
{

/**
 * \ingroup genotypes flpoint
 * \brief FloatingPoint genotype: SBX crossover (http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.33.7291&rep=rep1&type=pdf,
 * http://www.iitk.ac.in/kangal/papers/k2011017.pdf)
 */
class FloatingPointCrsSbx: public CrossoverOp
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);

	uint ni;
};
typedef boost::shared_ptr<FloatingPointCrsSbx> FloatingPointCrsSbxP;
}
#endif /* FLOATINGPOINTCRSSBX_H_ */

