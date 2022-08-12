#ifndef HALLOFFAME_H_
#define HALLOFFAME_H_

#include "SelBestOp.h"
#include "Operator.h"

/**
 * \ingroup evol population 
 * \brief Records a set of best-so-far individuals.
 *
 * Currently only one individual is kept. Each Deme contains a HoF object.
 */
class HallOfFame : public Operator
{
protected:
	bool bEmpty_;         //!< is HoF empty
	StateP state_;
	uint lastChangeGen_;  //!< generation of last update
	uint hofSize_;        //!< no. of individuals in HoF
	std::vector<IndividualP> bestIndividuals_;	///< vector of individuals in HoF
	std::vector<uint> bestGenerations_;
	SelBestOpP selectBest_;

public:
	bool initialize(StateP);
	bool operate(StateP);
	bool operate(const std::vector<IndividualP>&);
	void write(XMLNode&);
	void read(XMLNode&);
	HallOfFame();

	/// Get HoF contents (vector of best-so-far individuals)
	std::vector<IndividualP> getBest()
	{	return bestIndividuals_;	}

	/// Return generation of last update (of newest individual in HoF)
	uint getLastChange()
	{	return lastChangeGen_;	}
};
typedef boost::shared_ptr<HallOfFame> HallOfFameP;
#endif /* HALLOFFAME_H_ */
