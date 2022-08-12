#ifndef Population_h
#define Population_h

#include "Deme.h"

/**
 * \ingroup evol population
 * \brief Population class - inherits a vector of Deme objects.
 *
 * Population may consist of one or more demes. The demes evolve independently of each other.
 *
 * Parallel ECF: Population object of each process contains only one Deme (called the local Deme)!
 */
class Population : public std::vector <DemeP>
{
protected:
	uint nDemes_;              //!< total number of demes in the population
	uint myDemeIndex_;         //!< global index of local deme (at local process) - PECF only
	uint nIndividuals_;        //!< number of individuals (in each deme!)
	StateP state_;
	HallOfFameP hof_;          //!< population HallOfFame
	StatCalcP stats_;          //!< population statistics

public:
	Population();
	bool initialize(StateP state);
	void registerParameters(StateP state);        //!< register population specific parameters    
	void write(XMLNode&);
	void read(XMLNode&);
	void updateDemeStats();
	uint getNoDemes()                             //! return total number of demes
	{	return nDemes_;	}
	uint getLocalDemeId()                         //! return local deme index (local process in parallel ECF)
	{	return myDemeIndex_;	}
	DemeP getLocalDeme()                          //! get first deme (local deme in parallel ECF)
	{	return this->at(0);	}
	HallOfFameP getHof()                          //! get population HallofFame
	{	return hof_;	}
	StatCalcP getStats()                          //! get population stats
	{	return stats_;	}
};
typedef boost::shared_ptr<Population> PopulationP;

#endif // Population_h

