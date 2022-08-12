#include "ECF_base.h"
#include "floatingpoint/FloatingPoint.h"
#include "AlgCuckooSearch.h"
#include <boost/random/normal_distribution.hpp>
#include <boost/random.hpp>
#include <ctime>
#include <cstdlib>
#include <vector>


CuckooSearch::CuckooSearch()
{
	name_ = "CuckooSearch";
	selBestOp = static_cast<SelectionOperatorP> (new SelBestOp);
}


void CuckooSearch::registerParameters(StateP state)
{
	registerParameter(state, "pa", (voidP) new double(0.75), ECF::DOUBLE);
}


bool CuckooSearch::initialize(StateP state)
{
	selBestOp->initialize(state);

	voidP pDiscovery = getParameterValue(state, "pa");
	pa = *((double*)pDiscovery.get());
	if (pa < 0 || pa > 1)
	{
		ECF_LOG_ERROR(state, "Error - pa must be in interval [0,1]");
		throw "";
	}

	// reading boudaries and problem dimension
	voidP lBound = state->getGenotypes()[0]->getParameterValue(state, "lbound");
	lbound = *((double*)lBound.get());
	voidP uBound = state->getGenotypes()[0]->getParameterValue(state, "ubound");
	ubound = *((double*)uBound.get());
	voidP sptr = state->getGenotypes()[0]->getParameterValue(state, "dimension");
	numDimension = *((uint*)sptr.get());

	// algorithm accepts a single FloatingPoint or Binary genotype 
	// or a genotype derived from the abstract RealValueGenotype class
	GenotypeP activeGenotype = state->getGenotypes()[0];
	RealValueGenotypeP rv = boost::dynamic_pointer_cast<RealValueGenotype> (activeGenotype);
	if(!rv) {
		ECF_LOG_ERROR(state, "Error: Cuckoo Search algorithm accepts only a RealValueGenotype derived genotype! (FloatingPoint or Binary)");
		throw ("");
	}

	return true;
}


bool CuckooSearch::advanceGeneration(StateP state, DemeP deme)
{
	double sigma = 0.696574502;
	boost::mt19937 rng;
	boost::normal_distribution<> nd(0.0, 1.0);
	boost::variate_generator<boost::mt19937&,
	boost::normal_distribution<> > var_nor(rng, nd);

	IndividualP best = selBestOp->select(*deme);
	FloatingPointP bestFp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (best->getGenotype(0));

	// cuckoos via Levy flights (by Mantegna's algorithm)
	// new individual is added to population only if it is better than original individual
	for (uint i = 0; i < deme->size(); i++) {
		IndividualP trial = (IndividualP)deme->at(i)->copy();
		FloatingPointP trialFp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (trial->getGenotype(0));
		for (uint j = 0; j < numDimension; j++)	{
			double u = var_nor() * sigma;
			double v = var_nor();
			double step = u / pow(fabs(v), 2 / (double)3);
			double randn = var_nor();
			double diff = trialFp->realValue[j] - bestFp->realValue[j];
			double stepsize = 0.01 * step * diff;
			trialFp->realValue[j] = trialFp->realValue[j] + stepsize*randn;
			if (trialFp->realValue[j] > ubound)
				trialFp->realValue[j] = ubound;
			if (trialFp->realValue[j] < lbound)
				trialFp->realValue[j] = lbound;
		}
		evaluate(trial);
		if (trial->fitness->isBetterThan(deme->at(i)->fitness))
			replaceWith(deme->at(i), trial);
	}

	// copy all individuals
	std::vector<IndividualP> nest1;
	std::vector<IndividualP> nest2;
	for (uint i = 0; i < deme->size(); i++) {
		IndividualP indCp = (IndividualP)deme->at(i)->copy();
		nest1.push_back(indCp);
		nest2.push_back(indCp);
	}

	// replace some individuals/nests by constructing new nests
	// nest is replaced only if it is better than original
	random_shuffle(nest1.begin(), nest1.end());
	random_shuffle(nest2.begin(), nest2.end());
	double randNum = (double)rand() / RAND_MAX;
	for (uint i = 0; i < deme->size(); i++) {
		IndividualP trial = (IndividualP)deme->at(i)->copy();
		FloatingPointP trialFp1 = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (nest1.at(i)->getGenotype(0));
		FloatingPointP trialFp2 = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (nest2.at(i)->getGenotype(0));
		FloatingPointP trialFp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (trial->getGenotype(0));

		for (uint j = 0; j < numDimension; j++) {
			if ((double)rand() / RAND_MAX < pa) {
				double stepsize = (trialFp1->realValue[j] - trialFp2->realValue[j])*randNum;
				trialFp->realValue[j] += stepsize;
				if (trialFp->realValue[j] > ubound)
					trialFp->realValue[j] = ubound;
				if (trialFp->realValue[j] < lbound)
					trialFp->realValue[j] = lbound;
			}
		}

		evaluate(trial);
		if (trial->fitness->isBetterThan(deme->at(i)->fitness))
			replaceWith(deme->at(i), trial);
	}
	return true;

}

