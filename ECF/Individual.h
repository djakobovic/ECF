#ifndef Individual_h
#define Individual_h

#include "Genotype.h"
#include "Fitness.h"

/**
 * \ingroup evol population
 * \brief Individual class - inherits a vector of Genotype objects.
 */
class Individual : public std::vector <GenotypeP>
{
protected:
	StateP state_;

public:
	Individual()
	{	
		cid = 0;
	}

	Individual(StateP state);	//!< create AND initialize individual

	bool initialize(StateP state);	//!< initialize individual

	void write(XMLNode&);	//!< write individual to XML node

	void read(XMLNode&);	//!< read individual from XML node

	std::string toString();	//!< output individual to string

	Individual* copy();		//!< create a copy of the individual

	GenotypeP getGenotype(uint index = 0);	//!< return genotype with given index

	FitnessP getFitness();	//!< return sptr to individual's fitness object

	FitnessP fitness;	//!< sptr to individual's fitness object

	uint index;	//!< individual's index in Deme

	uint cid;	//!< coherence index, used in asynchronous parallel algoritmhs
};
typedef boost::shared_ptr<Individual> IndividualP;

#endif // Individual_h

