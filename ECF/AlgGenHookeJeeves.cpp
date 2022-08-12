#include "ECF_base.h"
#include "ECF_macro.h"
#include "AlgGenHookeJeeves.h"
#include "SelFitnessProportionalOp.h"
#include "SelRandomOp.h"
#include "floatingpoint/FloatingPoint.h"


GenHookeJeeves::GenHookeJeeves()
{
	// define algorithm name (for referencing in config file)
	name_ = "GenHookeJeeves";

	// inital state
	areGenotypesAdded_ = false;

	// create selection operators needed
	selFitPropOp_ = static_cast<SelectionOperatorP> (new SelFitnessProportionalOp);
	selBestOp_    = static_cast<SelectionOperatorP> (new SelBestOp);
	selRandomOp_  = static_cast<SelectionOperatorP> (new SelRandomOp);
}


void GenHookeJeeves::registerParameters(StateP state)
{
	// preciznost: minimalni delta_x
	registerParameter(state, "precision", (voidP) new double(0.000001), ECF::DOUBLE);
	// pocetni pomak (pocetni delta_x)
	registerParameter(state, "delta", (voidP) new double(1.), ECF::DOUBLE);
	// samo lokalna pretraga
	registerParameter(state, "localonly", (voidP) new uint(0), ECF::UINT);
}


bool GenHookeJeeves::initialize(StateP state)
{
	// algorithm accepts a single FloatingPoint or Binary genotype 
	// or a genotype derived from the abstract RealValueGenotype class
	GenotypeP activeGenotype = state->getGenotypes()[0];
	RealValueGenotypeP rv = boost::dynamic_pointer_cast<RealValueGenotype> (activeGenotype);
	if(!rv) {
		ECF_LOG_ERROR(state, "Error: This algorithm accepts only a RealValueGenotype derived genotype! (FloatingPoint or Binary)");
		throw ("");
	}

	// initialize all operators
	selFitPropOp_->initialize(state);
	selBestOp_->initialize(state);
	selRandomOp_->initialize(state);

	// read parameter values 
	voidP sptr = getParameterValue(state, "precision");
	precision_ = *((double*) sptr.get());
	sptr = getParameterValue(state, "delta");
	initialMove_ = *((double*) sptr.get());
	sptr = getParameterValue(state, "localonly");
	localOnly_ =  *((uint*) sptr.get()) ? true : false;

	// init pomake i zastavice postupka
	sptr = state->getRegistry()->getEntry("population.size");
	uint size = *((uint*) sptr.get());
	for(uint i = 0; i < size; i++) {
		delta_.push_back(initialMove_);
		changed_.push_back(true);
		converged_.push_back(false);
	}

	convergedTotal_ = 0;

	// batch run check
	if(areGenotypesAdded_)
		return true;

	// procitaj parametre prvog genotipa i prepisi u novi genotip
	// (drugi genotip nam treba za operaciju pretrazivanja)
	// to sve moze i jednostavnije (npr. s privatnim vektorom genotipa), ali ovako mozemo koristiti i milestone!
	sptr = state->getGenotypes()[0]->getParameterValue(state, "dimension");
	uint numDimension = *((uint*) sptr.get());
	sptr = state->getGenotypes()[0]->getParameterValue(state, "lbound");
	lbound_ = *((double*) sptr.get());
	sptr = state->getGenotypes()[0]->getParameterValue(state, "ubound");
	ubound_ = *((double*) sptr.get());

	// stvori i dodaj novi genotip
	FloatingPointP fp (static_cast<FloatingPoint::FloatingPoint*> (new FloatingPoint::FloatingPoint));
	state->setGenotype(fp);

	// ako je lokalna pretraga, stvori i dodaj novi genotip za broj koraka do konvergencije svake jedinke
	if(localOnly_) {
		FloatingPointP fp2 (static_cast<FloatingPoint::FloatingPoint*> (new FloatingPoint::FloatingPoint));
		state->setGenotype(fp2);
		fp2->setParameterValue(state, "dimension", (voidP) new uint(1));
		fp2->setParameterValue(state, "lbound", (voidP) new double(0));
		fp2->setParameterValue(state, "ubound", (voidP) new double(1));
	}

	// postavi jednake parametre
	fp->setParameterValue(state, "dimension", (voidP) new uint(numDimension));
	fp->setParameterValue(state, "lbound", (voidP) new double(lbound_));
	fp->setParameterValue(state, "ubound", (voidP) new double(ubound_));

	// mark adding of genotypes
	areGenotypesAdded_ = true;

	return true;
}


bool GenHookeJeeves::advanceGeneration(StateP state, DemeP deme)
{
	// fitnesi i pomocna jedinka za postupak pretrazivanja (koristimo Fitness objekte tako da radi i za min i max probleme)
	FitnessP neighbor[2];
	neighbor[0] = (FitnessP) (*deme)[0]->fitness->copy();
	neighbor[1] = (FitnessP) (*deme)[0]->fitness->copy();
	IndividualP temp (new Individual(state));

	uint mutacija = 0;

	// vrti isto za sve jedinke
	for(uint i = 0; i < deme->size(); i++) {

		if(localOnly_ && converged_[i])
			continue;

		IndividualP ind = deme->at(i);
		// bazna tocka:
		FloatingPointP x = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (ind->getGenotype(0));
		// pocetna tocka pretrazivanja:
		FloatingPointP xn = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (ind->getGenotype(1));

		FitnessP finalFit;
		// je li prva iteracija uz ovaj deltax?
		if(changed_[i]) {
			xn = (FloatingPointP) x->copy();
			changed_[i] = false;
			finalFit = (FitnessP) ind->fitness->copy();
		}
		// ako nije, trebamo evaluirati i pocetnu tocku pretrazivanja
		else {
			(*temp)[0] = xn;
			evaluate(temp);
			finalFit = temp->fitness;
		}

		// pretrazivanje
		for(uint dim = 0; dim < x->realValue.size(); dim++) {
			xn->realValue[dim] += delta_[i];    // pomak vektora

			// ogranicenja: provjeri novu tocku samo ako zadovoljava
			if(xn->realValue[dim] <= ubound_) {
				(*temp)[0] = xn;                    // stavimo u pomocnu jedinku
				evaluate(temp);                     // evaluiramo jedinku
				neighbor[0] = temp->fitness;        // zabiljezimo fitnes

				// VARIJANTA A: originalni postupak za unimodalne fje
				// ako je drugi bolji, treceg niti ne gledamo
				if(neighbor[0]->isBetterThan(finalFit)) {
					finalFit = neighbor[0];
					continue;
				}

			}

			// onda idemo na drugu stranu
			xn->realValue[dim] -= 2 * delta_[i];

			// ogranicenja: provjeri novu tocku samo ako zadovoljava
			if(xn->realValue[dim] >= lbound_) {
				(*temp)[0] = xn;
				evaluate(temp);
				neighbor[1] = temp->fitness;

				// je li treci bolji?
				if(neighbor[1]->isBetterThan(finalFit)) {
					finalFit = neighbor[1];
					continue;
				}
			}

			// ako nije, vrati u sredinu
			xn->realValue[dim] += delta_[i];     // vrati u sredinu
			continue;


			// VARIJANTA B: gledamo sve tri tocke (za visemodalni slucaj)
			// odredi najbolju od 3
			if(finalFit->isBetterThan(neighbor[0]) && finalFit->isBetterThan(neighbor[1]))
				;
			else if(neighbor[0]->isBetterThan(neighbor[1])) {
				xn->realValue[dim] += delta_[i];
				finalFit = neighbor[0];
			}
			else {
				xn->realValue[dim] -= delta_[i];
				finalFit = neighbor[1];
			}

		}	// kraj pretrazivanja


		// je li tocka nakon pretrazivanja bolja od bazne tocke?
		if(finalFit->isBetterThan(ind->fitness)) {
			FloatingPointP xnc (xn->copy());
			// preslikavanje:
			for(uint dim = 0; dim < x->realValue.size(); dim++)
				xn->realValue[dim] = 2 * xn->realValue[dim] - x->realValue[dim];

			// ogranicenja: pomakni na granicu
			for(uint dim = 0; dim < xn->realValue.size(); dim++) {
				if(xn->realValue[dim] < lbound_)
					xn->realValue[dim] = lbound_;
				if(xn->realValue[dim] > ubound_)
					xn->realValue[dim] = ubound_;
			}

			x = xnc;                  // nova bazna tocka
			ind->fitness = finalFit;  // ne zaboravimo i novi fitnes
		}
		// nije, smanji deltax i resetiraj tocku pretrazivanja
		else {
			delta_[i] /= 2;
			xn = (FloatingPointP) x->copy();
			changed_[i] = true;
		}

		// azuriraj genotipe (pointeri u jedinki):
		(*ind)[0] = x;
		(*ind)[1] = xn;


		// dio koji obradjuje samo Hooke-Jeeves (localonly)
		if(localOnly_) {
			if(converged_[i] == false && changed_[i] == true && delta_[i] < precision_) {
				converged_[i] = true;
				convergedTotal_++;

				// zapisi generaciju u kojoj je jedinka konvergirala
				FloatingPointP fp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (ind->getGenotype(2));
				fp->realValue[0] = (double) state->getGenerationNo();
			}


			if(convergedTotal_ == converged_.size()) {
				state->setTerminateCond();
				std::cout << "svi konvergirali!" << std::endl;
			}

			continue;	// sljedeca jedinka

		}

		// ako je jedinka konvergirala (i ako nije trenutno najbolja), stvori novu krizanjem + mutacijom
		if(changed_[i] == true && delta_[i] < precision_ && (selBestOp_->select(*deme) != ind)) {
			IndividualP first, second;
			first = second = selFitPropOp_->select(*deme);
			while(second == first)
				second = selFitPropOp_->select(*deme);

			// VARIJANTA C: slucajni odabir roditelja (umjesto fitness proportional)
//			first = second = selRandomOp_->select(*deme);
//			while(second == first)
//				second = selRandomOp_->select(*deme);

			// krizanje, mutacija, evaluacija
			mate(first, second, ind);
			mutate(ind);
			evaluate(ind);
			delta_[i] = initialMove_;	// ponovo pocetni pomak
		}

	}

	return true;
}
