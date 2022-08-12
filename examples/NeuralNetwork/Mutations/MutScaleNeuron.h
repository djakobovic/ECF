#ifndef MUTSCALENEURON_H_
#define MUTSCALENEURON_H_
#include <ecf/ECF.h>

/**
 * \class	MutScaleNeuron
 *
 * \brief	NeuralNetwork genotype: Mutation where all weights of a randomly chosen neuron are
 * 			scaled by a factor from 0.5 to 1.5 so that the weights don't change as radically.
 *
 * \author	Masa Burda and Lucija Ulaga
 * \date	18.1.2017.
 *
 * ### ingroup	genotypes flpoint.
 */

class MutScaleNeuron : public MutationOp
{
protected:
public:

	/**
	 * \fn	bool MutScaleNeuron::mutate(GenotypeP gene);
	 *
	 * \brief	Mutates the given gene. 
	 * 			Picks a random neuron and multiplies
	 * 			all it's weights by a random number from [0.5, 1.5] interval
	 *
	 * \param	gene	The gene.
	 *
	 * \return	True if it succeeds, false if it fails.
	 */

	bool mutate(GenotypeP gene);

	/**
	 * \fn	bool MutScaleNeuron::initialize(StateP);
	 *
	 * \brief	Initializes this object.
	 *
	 * \param	parameter1	The first parameter.
	 *
	 * \return	True if it succeeds, false if it fails.
	 */

	bool initialize(StateP);

	/**
	 * \fn	void MutScaleNeuron::registerParameters(StateP);
	 *
	 * \brief	Registers the parameters described by parameter1.
	 *
	 * \param	parameter1	The first parameter.
	 */

	void registerParameters(StateP);
};
typedef boost::shared_ptr<MutScaleNeuron> MutScaleNeuronP;

#endif /* MUTSCALENEURON_H_ */

