#ifndef BINARYREDUCEDSURROGATECRS_H_
#define BINARYREDUCEDSURROGATECRS_H_


namespace Binary
{

/**
 * \ingroup genotypes binary
 * \brief Binary genotype: reduced surrogate crossover operator
 */
class BinaryCrsReducedSurrogate: public CrossoverOp 
{
public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
	bool initialize(StateP);
	void registerParameters(StateP);
};
typedef boost::shared_ptr<BinaryCrsReducedSurrogate> BinaryCrsReducedSurrogateP;
}

#endif /* BINARYREDUCEDSURROGATECRS_H_ */

