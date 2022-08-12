#include <ECF/ECF.h>
#include "OneMaxEvalOp.h"

// evaluate() receives a smart pointer to the individual to evaluate
FitnessP OneMaxEvalOp::evaluate(IndividualP individual)
{
        // evaluation creates a new fitness object using a smart pointer
        // in our case, we try to maximize the number of ones, so we use FitnessMax fitness (for maximization problems)
        FitnessP fitness (new FitnessMax);

        // Each individual is a vector of genotypes (defined in the configuration file).
        // We'll use BitString, and put it as the first and only genotype
		BitString::BitString* bitstr = (BitString::BitString*) individual->getGenotype().get();
		//BitStringP bitstr = boost::static_pointer_cast<BitString::BitString> (individual->getGenotype(0)); // don't need zero for the first one
        
        // count the ones; where are they?
        // BitString genotype contains a std::vector of bool's named 'bits'
        uint ones = 0;
        for(uint i = 0; i<bitstr->bits.size(); i++){
                if(bitstr->bits[i] == true)
                        ones++ ;
        }
        fitness->setValue(ones);

        // return the smart pointer to new fitness
        return fitness;
}
