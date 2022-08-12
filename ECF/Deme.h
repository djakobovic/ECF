#ifndef Deme_h
#define Deme_h

#include "Individual.h"

class HallOfFame;
typedef boost::shared_ptr<HallOfFame> HallOfFameP;
class StatCalc;
typedef boost::shared_ptr<StatCalc> StatCalcP;

/**
 * \ingroup evol population
 * \brief Deme class - inherits a vector of Individual objects.
 *
 * A deme is a set of individuals that may interact (compete, mate) during an Algorithm execution. 
 * Each deme also contains a HallOfFame and a StatCalc object.
 */
class Deme : public std::vector <IndividualP>
{
public:
	HallOfFameP hof_;
	StatCalcP stats_;

	bool initialize(StateP state);	//!< initialize deme
	bool replace(uint index, IndividualP newInd);	//!< replace ind. at index with newInd
	void write(XMLNode&);
	void read(XMLNode&);
	
	/// get number of indiviuals (deme size)
	uint& getSize()
	{	return nIndividuals_;	}

protected:
	uint nIndividuals_;
};
typedef boost::shared_ptr<Deme> DemeP;

#endif // Deme_h

