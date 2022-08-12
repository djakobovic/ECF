#ifndef BINARYUNIFORMCRS_H_
#define BINARYUNIFORMCRS_H_


namespace Binary
{

/**
 * \ingroup genotypes binary
 * \brief Binary genotype: uniform crossover operator
 */
class BinaryCrsUniform: public CrossoverOp 
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BinaryCrsUniform> BinaryCrsUniformP;
}

#endif /* BINARYUNIFORMCRS_H_ */

