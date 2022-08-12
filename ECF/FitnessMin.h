#ifndef FITNESSMIN_H_
#define FITNESSMIN_H_

#include "Fitness.h"

/**
 * \ingroup evol main
 * \brief Fitness for minimization problems.
 *
 * Fitness is better if the fitness value is smaller.
 */
class FitnessMin: public Fitness {
public:
	FitnessMin() {}

	bool isBetterThan(FitnessP other);
	void write(XMLNode&);
	FitnessMin* copy();
};
typedef boost::shared_ptr<FitnessMin> FitnessMinP;

#endif /* FITNESSMIN_H_ */

