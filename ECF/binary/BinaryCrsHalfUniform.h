#ifndef BINARYHALFUNIFORMCRS_H_
#define BINARYHALFUNIFORMCRS_H_


namespace Binary
{

/**
 * \ingroup genotypes binary
 * \brief Binary genotype: half uniform crossover operator
 */
class BinaryCrsHalfUniform: public CrossoverOp 
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BinaryCrsHalfUniform> BinaryCrsHalfUniformP;
}

#endif /* BINARYHALFUNIFORMCRS_H_ */

