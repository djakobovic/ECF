#ifndef Steady_State_Tournament_h
#define Steady_State_Tournament_h

#include "Algorithm.h"

/**
 * \brief Steady state algorithm with tournament elimination operator
 * \ingroup algorithms serial
 *
 * This algorithm is genotype independent (it can be used with any Genotype).
 *
 * The algorithm flow:
 *
 \verbatim
  single generation {
   repeat(deme size times) {
    randomly add <nTournament_> individuals to the tournament;
    select the worst one in the tournament;
    randomly select two parents from remaining ones in the tournament;
    replace the worst with crossover child;
    perform mutation on child;
   }
  }
 \endverbatim
 *  
 */
class SteadyStateTournament : public Algorithm
{
public:
	SteadyStateTournament();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);

protected:
	uint nTournament_;	//!< tournament size
	SelectionOperatorP selRandomOp, selWorstOp;

};
typedef boost::shared_ptr<SteadyStateTournament> SteadyStateTournamentP;

#endif // Steady_State_Tournament_h

