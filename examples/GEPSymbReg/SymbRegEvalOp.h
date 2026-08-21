#ifndef SymbRegEvalOp_h
#define SymbRegEvalOp_h


/**
 * \defgroup gepsymbreg Symbolic regression with Gene Expression Programming (GEP)
 * \ingroup examples
 * \brief GEP symbolic regression example - illustration for GEP genotype
 *
 * \par Description
 * The objective is to find a symbolic form of an unknown function (i.e. find a mapping between the given set 
 * of input and output values). 
 * The fitness is defined with total error for the given set of output values.
 * 
 * \par GEP genotype parameters (in the config file)
 * - set of functions 
 * - set of terminals (input variables and constants)
 *
 */
 
 
/**
 * \ingroup gepsymbreg
 * \brief GEP symbolic regression evaluation operator.
 */
#include <cmath>
#include <ECF/ECF.h>
#include <fstream>
class GEPSymbRegEvalOp : public EvaluateOp
{
public:
	FitnessP evaluate(IndividualP individual);
	bool initialize(StateP);
	void registerParameters(StateP);
	bool csvRead(StateP state, std::string entry, std::vector<double>* vec);
	std::vector<double> x;
	std::vector<double> y;
	std::vector<double> f;
	uint nSamples;
};
typedef std::shared_ptr<GEPSymbRegEvalOp> GEPSymbRegEvalOpP;

#endif // SymbRegEvalOp_h
