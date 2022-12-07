#include "ECF_base.h"
#include "AlgNSGA2.h"
#include "SelRandomOp.h"
#include "SelWorstOp.h"
#include <cmath>
#include <cfloat>


AlgNSGA2::AlgNSGA2() 
{
	name_ = "NSGA2";
	// create selection operators needed
	// in this case, SelRandomOp and SelWorstOp
	selRandomOp = static_cast<SelectionOperatorP> (new SelRandomOp);

	selWorstOp = static_cast<SelectionOperatorP> (new SelWorstOp);
	this->parentPop = new std::vector <IndividualP>();
	this->fronts = boost::shared_ptr<std::vector <std::vector <IndividualP> > > (new std::vector <std::vector <IndividualP> >());

}


bool AlgNSGA2::initialize(StateP state)
{
	// initialize all operators
	selRandomOp->initialize(state);
	selWorstOp->initialize(state);

	return true;
}


void AlgNSGA2::quickSort(std::vector <IndividualP> *group, int left, int right, std::string prop, int objective = -1) 
{
	int i = left, j = right;
	
	IndividualP tmp;
	MOFitnessP pivotMO =  boost::static_pointer_cast<MOFitness> (group->at((left + right) / 2)->fitness);
	double pivot =  pivotMO->getProperty(prop, objective);
 
	/* partition */
	while (i <= j) {
		MOFitnessP moFitnessI =  boost::static_pointer_cast<MOFitness> (group->at(i)->fitness);
		while (moFitnessI->getProperty(prop, objective) <  pivot) {
			i++;
			moFitnessI =  boost::static_pointer_cast<MOFitness> (group->at(i)->fitness);
		}
		MOFitnessP moFitnessJ =  boost::static_pointer_cast<MOFitness> (group->at(j)->fitness);
		while (pivot < moFitnessJ->getProperty(prop, objective)) {				
			j--;
			moFitnessJ =  boost::static_pointer_cast<MOFitness> (group->at(j)->fitness);
		}

		if (i <= j) {
			tmp = group->at(i);
			group->at(i) = group->at(j);
			group->at(j) = tmp;
			i++;
			j--;
		}
	};
 
	/* recursion */
	if (left < j)
		quickSort(group, left, j, prop, objective);
	if (i < right)
		quickSort(group, i, right, prop, objective);
}


void AlgNSGA2::sortBasedOnProperty(std::vector <IndividualP>* deme, double* fMin, double* fMax, std::string prop, int objective = -1) 
{
	int left = 0;
	int right = deme->size()-1;
	quickSort(deme, left, right, prop, objective);

	MOFitnessP fitness = boost::static_pointer_cast<MOFitness> (deme->at(left)->fitness);
	*fMin = fitness->getProperty(prop, objective);
	fitness = boost::static_pointer_cast<MOFitness> (deme->at(right)->fitness);
	*fMax = fitness->getProperty(prop, objective);
}


int AlgNSGA2::checkDominance(MOFitnessP fitness1, MOFitnessP fitness2) 
{
	double eps = 1E-9;
	uint size = fitness1->size();
	if (size != fitness2->size()) {
		// ne smije se dogoditi
	}
	int dominance = 0;
	for (uint i = 0; i<size; i++) {
		FitnessP f1 = fitness1->at(i);
		FitnessP f2 = fitness2->at(i);
		if (fabs(f1->getValue() - f2->getValue()) > DBL_EPSILON) {
			if (dominance == 0) {
				if (f1->isBetterThan(f2)) {
					dominance = -1;
				} else {
					dominance = 1;
				}
			} else if (dominance == -1) {
				if (f1->isBetterThan(f2)) {
					// dominance ostaje -1
				} else {
					return 0;
				}
			} else {
				if (f1->isBetterThan(f2)) {
					return 0;
				} else {
					// dominance ostaje 1
				}
			}
		}
	}
	return dominance;
}


// O(M * N^2),
// M => broj funkcija ciljeva
// N velicina populacije za sortiranje
void AlgNSGA2::nonDomSorting(boost::shared_ptr<std::vector <IndividualP> > pool, int N, boost::shared_ptr<std::vector <std::vector <IndividualP> > > fronts)
{
	fronts->clear();
	std::vector <IndividualP> Q = *(new std::vector <IndividualP>());
	int collectedSoFar = 0;
	int p = 1; // najnizi (najbolji) rank

	// izracunati nc i Sp za svaku jedinku u populaciji
	// i mora ici skroz do kraja (a ne do pool->size()-1) jer 
	// u slucaju da je cijela populacija u prvoj fronti, mora se i zadnji ubaciti u prvu frontu
	for (uint i = 0; i < pool->size(); i++) {
		IndividualP ind = pool->at(i);
		MOFitnessP fitnessI =  boost::static_pointer_cast<MOFitness> (ind->fitness);

		if (i == 0) {
			fitnessI->nc = 0;
			fitnessI->Sp = new std::vector<IndividualP>();
			fitnessI->rank = 0;
		}

		for (uint j = i+1; j < pool->size(); j++) {
			IndividualP other = pool->at(j);
			MOFitnessP fitnessJ = boost::static_pointer_cast<MOFitness> (other->fitness);

			if (i == 0) {
				fitnessJ->nc = 0;
				fitnessJ->Sp = new std::vector<IndividualP>();
				fitnessJ->rank = 0;
			}

			int dominance = checkDominance(fitnessI, fitnessJ);
			if (dominance == -1) {
				fitnessI->Sp->push_back(other);
				fitnessJ->nc++;
			} else if (dominance == 1) {
				fitnessI->nc++;
				fitnessJ->Sp->push_back(ind);
			}
		}
		
		// inicijalno u skup Q ulaze pareto optimalna rjesenja
		// sva rjesenja koja imaju 'domination count' (nc) jednak 0
		if (fitnessI->nc == 0) {
			fitnessI->rank = p;
			Q.push_back(ind);
			collectedSoFar++;
		}
	}


	// odrediti rankove rjesenjima
	//while (Q.size() != 0 && collectedSoFar < N) {
	while (Q.size() != 0) {
		fronts->push_back(Q);

		
		p++;
		// ako je skup Q prazan, zanci da smo svim rjesenjima dodjelili rank
		// u skupu newQ skupljaju se rjesenja iduce fronte nedominacije
		// znaci samo one rjesenja koja su dominirana iskljucivo rjesenjima iz skupa Q
		std::vector <IndividualP> newQ; 
		for (uint i=0; i<Q.size(); i++) {
			//pool->push_back(Q.at(i));
			MOFitnessP fitnessI = boost::static_pointer_cast<MOFitness> (Q.at(i)->fitness);

			for (uint j=0; j<fitnessI->Sp->size(); j++) {
				// za svako rjesenje prolazimo po skupu rjesenja nad kojima ono dominira te azuriramo nc
				IndividualP dominated = fitnessI->Sp->at(j);
				MOFitnessP fitnessJ = boost::static_pointer_cast<MOFitness> (dominated->fitness);
				fitnessJ->nc--;
				if (fitnessJ->nc == 0) {
					fitnessJ->rank = p;
					newQ.push_back(dominated);
					collectedSoFar++;
				}
			}
		}
		Q = newQ;
	}

}


// O(M * N * logN)
void AlgNSGA2::crowdedDistanceEst(StateP state, std::vector <IndividualP> *deme) 
{
	MOFitnessP fitness =  boost::static_pointer_cast<MOFitness> (deme->at(0)->fitness);
	uint objCount = fitness->size();
	for (uint i = 0; i<objCount; i++) {

		double fMin;
		double fMax;

		// sortiramo cijelu populaciju rjesenja prema jednom cilju 'i'
		// populacije je sortirana tako da rezultati funkcije cilja 'i' budu poredani od najmanjeg prema najvecem,
		// bez obzira da li se u funkciji cilja 'i' radi o maksimizaciji ili minimizaciji
		sortBasedOnProperty(deme, &fMin, &fMax, "objective", i);

		// sad su rjesenja sortirana, na nultom indeksu nalazi se najbolje
		// na posljednjem nalazi se najlošije
		for (uint j = 0; j<deme->size(); j++) {
			fitness =  boost::static_pointer_cast<MOFitness> (deme->at(j)->fitness);


			if (i == 0) {
				// ako je ovo prva funkcija cilja koju razmatramo
				// inicijaliziramo udaljenost na nulu, pocinjemo racunati crowding_distance za rjesenje 'j'
				fitness->crowding_distance = 0;
			}

			double increment;

			if (j == 0 || j == deme->size()-1) {
				// ako je najbolje ili najlosije rjesenje, postaviti beskonacnu vrijednost u njega
				// zapravo ne stavljam beskonacno nego maksimalnu vrijednost (fMax - fMin)
				increment = fMax - fMin;

			} else {
				// ako se ne radi o najboljem ili najlosijem
				// tada je crowding distance odreden napucenoscu prostora rjesenja

				// sljedbenik (j+1) ima veci fitness od (j) s obzirom na funkciju cilja 'i'
				MOFitnessP fitnessNeighbour = boost::static_pointer_cast<MOFitness> (deme->at(j+1)->fitness);
				increment = fitnessNeighbour->getValueOfObjective(i);

				// prethodnik (j-1) ima manji fitness od (j) s obzirom na funkciju cilja 'i'
				fitnessNeighbour = boost::static_pointer_cast<MOFitness> (deme->at(j-1)->fitness);
				increment -= fitnessNeighbour->getValueOfObjective(i);
				

			}
			// normalizirati udaljenost
			increment /= fMax - fMin;
			fitness->crowding_distance += increment;



		}

	}

}


// obavlja selekciju, krizanje, mutacije te stvara novu populaciju iste velicine
// trenutno je steadyStateTournament sa turnirom velicine 3
void AlgNSGA2::makeNewPop(StateP state, DemeP deme) 
{
	for(uint iIter = 0; iIter < deme->size(); iIter++) {
		
		ECF_LOG(state, 5, "Individuals in tournament: ");

		std::vector<IndividualP> tournament = *(new std::vector<IndividualP>());
		for (uint i = 0; i < 2; ++i) {
			// select a random individual for the tournament
			tournament.push_back(selRandomOp->select(*deme));
			ECF_LOG(state, 5, uint2str(i) + ": " + tournament[i]->toString());
		}

		// select the worst from the tournament
		IndividualP worst = selWorstOp->select(tournament);
		ECF_LOG(state, 5, "The worst from the tournament: " + worst->toString());

		// remove pointer to 'worst' individual from vector 'tournament'
		removeFrom(worst, tournament);

		IndividualP myMate = selRandomOp->select(*deme);

		// crossover the first two (remaining) individuals in the tournament
		mate(tournament[0], myMate, worst);

		// perform mutation on new individual
		mutate(worst);

		// create new fitness
		evaluate(worst);
		ECF_LOG(state, 5, "New individual: " + worst->toString());
	}
}


bool AlgNSGA2::advanceGeneration(StateP state, DemeP deme)
{
	static bool firstGen = true;

	// ne prvi put
	if(!firstGen)
		this->makeNewPop(state, deme);
	firstGen = false;

	uint N = deme->size();
	bool initialGeneration = parentPop->size() == 0;
	for (uint id = 0; id<parentPop->size(); id++) {
		deme->push_back((IndividualP) parentPop->at(id)->copy());
	}

	fronts->clear();
	//int lastFront;
	//nonDomSorting(deme, N, fronts, &lastFront);
	nonDomSorting(deme, N, fronts);
	//crowdedDistanceEst(state, deme, lastFront);
	deme->clear();

	uint i = 0;
	uint size = 0;
	while (i < fronts->size() && size + fronts->at(i).size() <= N) {
		crowdedDistanceEst(state, &(fronts->at(i)));
		for (uint j = 0; j<fronts->at(i).size(); j++) {
			deme->push_back(fronts->at(i).at(j));
		}
		size += fronts->at(i).size();
		i++;
	}

	if (!initialGeneration) {
		double fMin;
		double fMax;
		crowdedDistanceEst(state, &(fronts->at(i)));
		sortBasedOnProperty(&(fronts->at(i)), &fMin, &fMax, "crowding_distance");

		int howMany = N - deme->size();
		int lastFrontSize = fronts->at(i).size();
		for (int j = lastFrontSize-1; j >= (lastFrontSize - howMany); j--) {
			deme->push_back(fronts->at(i).at(j));
		}
	}
	// deme sad ponovo ima N jedinki i mozemo krenuti sa procesom selekcije, krizanja i mutacija

/*	uint genNo = state->getGenerationNo();
	if (genNo == 499) {

		// ispisi populaciju u log file
		XMLNode xDeme;
		deme->write(xDeme);
		char *output = xDeme.createXMLString(true);
		ECF_LOG(state, 1, "\nPareto Solutions: \n" + std::string(output));
		
		// ispisati populaciju u zaseban file... 
		std::ofstream myfile;
		myfile.open ("paretoFront.txt");
		for (uint i = 0; i<deme->size(); i++) {
			FloatingPointP gen = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (deme->at(i)->getGenotype(0));
			//MOFitnessP fitness = boost::static_pointer_cast<MOFitness> (deme->at(i)->fitness);
			myfile << gen->realValue[0] << " " << gen->realValue[1] << "\n";
		}
		myfile.close();
		
	}
*/

	// trenutnu populaciju spremimo u roditeljsku populaciju
	parentPop->clear();
	for (uint i = 0; i<deme->size(); i++) {
		this->parentPop->push_back((IndividualP) deme->at(i)->copy());
	}

	// premjestio na pocetak generacije
	//this->makeNewPop(state, deme);


	return true;
}
