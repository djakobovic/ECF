#ifndef ALGGEP_H
#define ALGGEP_H

#include "../ECF_base.h"
#include "../Algorithm.h"
#include "../SelRandomOp.h"
#include "../SelBestOp.h"
#include "../SelFitnessProportionalOp.h"
#include "GEPChromosome.h"

/**
* \brief Generational algorithm with roulette wheel selection operator and unique operators and chromosome representation
* \ingroup algorithms serial
*
* This algorithm requires one GEPChromosome genotype.
*
* The algorithm flow:
*
\verbatim
single generation {
select individuals to form the new generation (fitness proportional selection operator);
create new generation (make copies);
noCrx = (deme size) * <crxRate_> / 2;
repeat(<noCrx> times) {
randomly select two parents;
perform crossover, _replace_ parents with their children;
}
perform mutation on new generation;
perform transposition on new generation;
perform inversion on new generation
}
\endverbatim
*
*/
class AlgGEP : public Algorithm{
public:
	AlgGEP();
	void registerParameters(StateP state);
	bool initialize(StateP state);
	bool advanceGeneration(StateP state, DemeP deme);
	void invert(StateP state, const std::vector<IndividualP>& pool);
	void invertDc(StateP state, const std::vector<IndividualP>& pool);
	void transpose(StateP state, const std::vector<IndividualP>& pool);
	void transposeIS(StateP state, const std::vector<IndividualP>& pool);
	void transposeRIS(StateP state, const std::vector<IndividualP>& pool);
	void transposeGene(StateP state, const std::vector<IndividualP>& pool);
	void transposeDc(StateP state, const std::vector<IndividualP>& pool);

protected:
	double crxRate_;	//!< crossover rate
	double selPressure_;	//!< selection pressure
	double invRate_; //!< inversion rate
	double invDcRate_; //!< Constant domain inversion rate
	double invMaxLength_; //!< inversion maximum sequence length
	double transISRate_; //!< IS transposition rate
	double transRISRate_; //!< RIS transposition rate
	double transGeneRate_; //!< gene transposition rate
	double transDcRate_; //!< constant domain transposition rate
	double transMaxLength_; //!< maximum sequence transposition length

	SelRandomOpP selRandomOp;
	SelBestOpP selBestOp;
	SelFitnessProportionalOpP selFitPropOp;
};
typedef boost::shared_ptr<AlgGEP> AlgGEPP;

#endif // AlgGEP_h