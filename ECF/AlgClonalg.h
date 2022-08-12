#ifndef Clonalg_h
#define Clonalg_h

#include "Algorithm.h"
#include "floatingpoint/FloatingPoint.h"

/**
 * \brief Clonal Selection Algorithm (see e.g. http://en.wikipedia.org/wiki/Clonal_Selection_Algorithm).
 * \ingroup algorithms serial
 * 
 *  This CLONALG implements: 
 *			- cloning Versions:	
 *								- static cloning :  n of the best antibodies are cloned beta*populationSize times
 *								- proportional cloning: number of clones per antibody is proportional to that ab's fitness
 *			- inversely proportional hypermutation : better antibodies are mutated less
 *			- selectionSchemes:	
 *								- CLONALG1 - at new generation each antibody will be substituded by the best individual of its set of beta*population clones
 *								- CLONALG2 - new generation will be formed by the best (1-d)*populationSize clones ( or all if the number of clones is less than that )
 *			- birthPhase: where d * populationSize of new antibodies are randomly created and added to the population for diversification
 *							
 * CLONALG algorithm accepts only a single FloatingPoint genotype.
 * Additionally, if chosen, selectionScheme CLONALG1 adds a FloatingPoint genotype (parentAntibody) to mark which clone came from which antibodies.
 */
class Clonalg : public Algorithm
{
protected:

	double ubound;
	double lbound;
	uint dimension;
	bool areGenotypesAdded_;

	uint n;					//!< number of antibodies cloned every generation
	double beta;			//!< parameter which determines the number of clones for every antibody
	double c;				//!< mutation parameter
	double d;				//!< fraction of population regenerated every generation
	std::string cloningVersion;	//!< specifies whether to use static or proportional cloning
	std::string selectionScheme;	//!< specifies which selection scheme to use CLONALG1 or CLONALG2

	//! sort vector of antibodies in regards to their fitness
	static bool sortPopulationByFitness (IndividualP ab1,IndividualP ab2) { return ( ab1->fitness->isBetterThan(ab2->fitness)); }

	//! sort vector of antibodies first by their antibody parents and then by their fitness
	static bool sortPopulationByParentAndFitness (IndividualP ab1,IndividualP ab2) 
	{ 
		FloatingPointP flp = boost::dynamic_pointer_cast<FloatingPoint::FloatingPoint> (ab1->getGenotype(1));
		double &parentAb1 = flp->realValue[0];
		flp = boost::dynamic_pointer_cast<FloatingPoint::FloatingPoint> (ab2->getGenotype(1));
		double &parentAb2 = flp->realValue[0];

		if (parentAb1 <parentAb2) return true;
		if (parentAb1 == parentAb2) return (ab1->fitness->isBetterThan(ab2->fitness));

		return false;
	}

public:

    Clonalg();
    void registerParameters(StateP state);
    bool initialize(StateP state);
    bool advanceGeneration(StateP state, DemeP deme);
	
	bool markAntibodies(DemeP deme);
	bool cloningPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones);
	bool hypermutationPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones);
	bool selectionPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones);
	bool birthPhase(StateP state, DemeP deme, std::vector<IndividualP> &clones);
	bool replacePopulation(StateP state, DemeP deme, std::vector<IndividualP> &clones);

};
typedef boost::shared_ptr<Clonalg> ClonalgP;


#endif // Clonalg_h

