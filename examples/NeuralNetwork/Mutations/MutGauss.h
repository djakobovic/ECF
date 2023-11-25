#ifndef MUTGAUSS_H_
#define MUTGAUSS_H_
#include <ecf/ECF.h>

/**
 * \class	MutGauss
 *
 * \brief	FloatingPoint (NN) genotype: Mutation adds to the randomly chosen weight a value with
 * 			Gaussian distribution.
 *
 * \author	Masa Burda and Lucija Ulaga
 * \date	18.1.2017.
 *
 * ### ingroup	genotypes flpoint.
 */

class MutGauss : public MutationOp
{
protected:
public:

	/**
	 * \fn	bool MutGauss::mutate(GenotypeP gene);
	 *
	 * \brief	Mutates the given gene. 
	 * 			Chooses a random weight. Chooses a random number x (in Gaussian distribution). 
	 * 			Adds x to the random weight.
	 *
	 * \param	gene	The gene.
	 *
	 * \return	True if it succeeds, false if it fails.
	 */

	bool mutate(GenotypeP gene);

	/**
	 * \fn	bool MutGauss::initialize(StateP);
	 *
	 * \brief	Initializes this object.
	 *
	 * \param	parameter1	The first parameter.
	 *
	 * \return	True if it succeeds, false if it fails.
	 */

	bool initialize(StateP);

	/**
	 * \fn	void MutGauss::registerParameters(StateP);
	 *
	 * \brief	Registers the parameters described by parameter1.
	 *
	 * \param	parameter1	The first parameter.
	 */

	void registerParameters(StateP);
};
typedef std::shared_ptr<MutGauss> MutGaussP;
#endif /* MUTGAUSS_H_ */
