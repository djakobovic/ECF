#ifndef FitnessMax_h
#define FitnessMax_h

#include "Fitness.h"

/**
 * \ingroup evol main
 * \brief Fitness for maximization problems.
 *
 * Fitness is better if the fitness value is greater.
 */
class FitnessMax : public Fitness
{
public:
	FitnessMax()
	{	}
	bool isBetterThan(FitnessP other);
	void write(XMLNode&);
	FitnessMax* copy();
};
typedef boost::shared_ptr<FitnessMax> FitnessMaxP;

#endif // FitnessMax_h