#ifndef MOFitness_h
#define MOFitness_h

#include "Fitness.h"
#include <string>

class MOFitness;
typedef boost::shared_ptr<MOFitness> MOFitnessP;


// razred nasljedjuje osnovni Fitness, ali zapravo koristimo vektor Fitness objekata!
class MOFitness : public Fitness, public std::vector<FitnessP>
{
public:


	int nc; // domination count, koliko jedinki iz populacije dominira ovom; [0..popSize-1]
	std::vector<IndividualP> *Sp; // skup rješenja nad kojima dominira
	int rank; // fronta nedominiranih rjesenja kojoj pripada ova jedinka; [1..popSize]
	double crowding_distance; // mjera napucenosti prostora rjesenja; manji crowding_distance -> veca napucenost
	
	
	std::vector<double>  convertedFitness;
	 std::vector<double>  convertedFitness2;

	// ovo je zapravo 'crowded comparison operator' iz originalnog NSGA-II algoritma
	// primjenjuje se nakon sto je fitnessima odreden rank i crowding_distance
	// zato sto prema ta dva parametra usporeduje jedinke
	bool isBetterThan(FitnessP other);

	// stvori identicnu kopiju (kopiraj i sve fitnese u vektoru)
	MOFitness* copy();

	// ispis - trenutno ispisuje samo vrijednost prvog Fitness objekta u vektoru
	void write(XMLNode &xFitness);

	// vraca rank zasad
	double getValue();

	// fja koja prima int parametar 
	// pa vraca vrijednost i-tog fitnesa
	double getValueOfObjective(int objective);

	// u stringu je naziv propertya ("objective", "rank" ili "crowding_distance")
	// u slucaju da prop == "objective" potreban je i argument objective koji specificira vrijednost koje funkcije cilja trazimo
	double getProperty(std::string prop, int objective);

};

#endif