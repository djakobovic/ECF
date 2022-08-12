#include "Algorithm.h"
#include "MOFitness.h"
#include <iostream>
#include <fstream>



class AlgNSGA2 : public Algorithm
{
protected:
	std::vector <IndividualP> *parentPop;
	boost::shared_ptr<std::vector <std::vector <IndividualP> > > fronts;
	SelectionOperatorP selRandomOp, selWorstOp;

public:
	AlgNSGA2();

	bool initialize(StateP state);

	bool advanceGeneration(StateP state, DemeP deme);

	// sortira se uvijek od najmanje vrijednosti prema najvecoj, 
	// bez obzira na to da li se u konkretnom slucaju radi o maksimizaciji ili minimizaciji
	void quickSort(std::vector <IndividualP> *group, int left, int right, std::string prop, int objective);

	// sortira populaciju prema fitness-u jedinki s obzirom na jednu odabranu fukciju cilja
	void sortBasedOnProperty(std::vector <IndividualP>* deme, double* fMin, double* fMax, std::string prop, int objective);

	// provjerava dominantnost izmedu dva multi-objective fitnessa
	// vraca -1 ako fitness1 dominira fitnessom2
	// vraca 0 ako niti jedan ne dominira drugim
	// vraca 1 ako fitness2 dominira fitnessom1
	// ne uzima u obzir nikakve druge stvari poput 'nc' ili 'crowding-distance', gledaju se samo vrijednosti funkcijâ ciljeva
	int checkDominance(MOFitnessP fitness1, MOFitnessP fitness2);

	// azurira rank polje u MOFitness-u jedinki
	void nonDomSorting(boost::shared_ptr<std::vector <IndividualP> > pool, int N, boost::shared_ptr<std::vector <std::vector <IndividualP> > > fronts);

	// azurira crowding_distance polje u MOFitness-u jedinki
	void crowdedDistanceEst(StateP state, std::vector <IndividualP> *deme);

	void makeNewPop(StateP state, DemeP deme);

};
typedef boost::shared_ptr<AlgNSGA2> AlgNSGA2P;