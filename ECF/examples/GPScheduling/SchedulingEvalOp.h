#ifndef SchedulingEvalOp_h
#define SchedulingEvalOp_h

#include "matrice.h"

class SchedulingEvalOp : public EvaluateOp
{
public:
	FitnessP evaluate(IndividualP individual);
	void registerParameters(StateP);
	bool initialize(StateP);
	~SchedulingEvalOp();

protected:

	enum environment
	{	SINGLE,
		UNIFORM,
		UNRELATED,
		JOBSHOP
	};

	unsigned int edited,total,nItems;
//  RPN Evaluator;
  //void write(std::string &output);
  //void ReadTerminals(void);
  //void DefineNodeNames(void);
  void ReadConstraints(Matrica &Constraints, int set, int jobs, Matrica &Precedence);
  void MakeSetup(Matrica &Duration, int set, int jobs, double faktor, Matrica &Setup);
  double NodeLevel(int set, int node);
  //void StartTerminalStatistic(Beagle::GP::Context& ioContext, double dSubsetSize=0);
  //void GetTerminalStatistic(double *pValues);
  //void UpdateTerminalStatistic(double &dFitness);
  //void ReadIndividual(Beagle::GP::Individual& inIndividual);
  //void CalcTimedTerminals(uint &nNiz, uint &nPoslova, uint &nJob, double &dClock, uint nMachine=0, uint nMachines=1);
  double EvaluateSingle(TreeP);
  //void EvaluateUniform(double &dRawFitness);
  //void EvaluateUnrelated(double &dRawFitness);
  //void EvaluateJobShop(double &dRawFitness);
  //int Before(const void *arg1, const void *arg2);

protected:
  unsigned int sets,max_jobs,total_jobs,max_machines,max_length;
  unsigned int m_fitness;	// koja fitnes funkcija? definicije u nodes.h
  unsigned int m_BestSubset;	// koliko najboljih jedinki gledamo za statistiku terminala
  unsigned int m_SubsetSize;	// vlastita proracunata velicina (default vrijednost)
  unsigned int m_InSubset;		// koliko ih trenutno ima u matrici
  unsigned int m_WorstInSubset;	// koji je najlosiji
  double m_WorstSubsetFitness;	// fitnes najlosije jedinke u podskupu najboljih
  double m_BestSubsetFitness;	// treba li komentar?
  environment m_Environment;	// okruzenje: single, uniform, unrelated, job shop
  unsigned int m_SortSet;		// oznaka skupa za potrebe qsort-a
  unsigned int m_PopSize;		// ukupna velicina populacije

  bool m_Normalized;		// je li fitnes normiran brojem i trajanjem poslova
  bool m_Evaluation;		// pisanje rezultata za svaki skup u datoteku
  bool m_TermUsage;			// prikupljanje statistike koristenja terminala
  bool m_editing;			// editiranje?
  bool m_LEF;				// limited error fitness (1 ili 0)
  double m_LEFVal;			// vrijednost za LEF
  bool m_setup;				// postoje li trajanja postavljanja izmedyu poslova
  double m_setup_faktor;	// prosjecni odnos izmedju trajanja postavljanja i trajanja posla
  bool m_dynamic;			// dinamicka okolina?
  bool m_constrained;		// ogranicenja u rasporedu?
  bool m_stsampling;		// stochastic sampling?
  double m_sampling;		// koliki postotak test skupova se uzima
  bool *pSamples;			// koji tocno skupovi idu
  bool m_Idleness;			// uzimamo li u obzir i cekanje operacija (job shop)
  bool *pRasporedjen;
  double *pVrijednosti, *pArray, *pSlack, *pSlackSpeed, *pArrival, *pLevel, *pSetupAvg;
  double *pOperationReady, *pJobReady, *pTotalWorkRemaining, *pTotalWorkDone, *pTotalMachineWork;
  double *pMachineWorkRemaining, *pMachineValues;
  unsigned int *pIndex, *pUsed, *pLastJob, *pPTimeMinMachine, *pMachineScheduled, *pOperationsScheduled;
  unsigned int *pOperationsWaiting;
  Matrica N,Duration,Deadline,WeightT,WeightF,WeightE,WeightN,Ready,Constraints,Machines,Speed;
  Matrica SP,SD,Fitness,Schedule,Precedence,Level,Setup,Terminals,MachineReady,PTimeAvg,PTimeMinMachine;
  Matrica SortedReady, Values, Durations, MachineIndex;
  std::string in_file;
};
typedef boost::shared_ptr<SchedulingEvalOp> SchedulingEvalOpP;

#endif // SchedulingEvalOp_h
