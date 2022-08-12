#include <cmath>
#include "../../ECF.h"
#include "SchedulingEvalOp.h"

#include "readpar.h"
#include "matrice.h"


// prilagodba za ECF
// trenutno radi:
//	single:
//		staticki
//			setup
//			constraints (?)



// makroi za uvjetnu provjeru
#define CHECKMSG(condition, text) \
if(!(condition)) {fprintf(stderr,"file: " __FILE__ "\nline: %d\nmsg:  " text "\n",__LINE__); exit(1);}
#define CHECK(condition) \
if(!(condition)) {fprintf(stderr,"Nesto ne valja!\nfile: " __FILE__ "\nline: %d\n" ,__LINE__); exit(1);}
// fja max{x,0}
#define POS(x)		(x>0 ? x : 0)
#define MIN(x,y)	(x<y ? x : y)
#define MAX(x,y)	(x>y ? x : y)

// fitness_types
const int Twt = 0;
const int Nwt = 1;
const int FTwt = 2;
const int ETwt = 3;
const int Fwt = 4;
const int Cmax = 5;
const int FUNCTIONS = 6;


void SchedulingEvalOp::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("test_cases", (voidP) (new std::string), ECF::STRING);
	state->getRegistry()->registerEntry("normalized", (voidP) (new uint(1)), ECF::UINT);
}


bool SchedulingEvalOp::initialize(StateP state)
{
	std::string configFile;

	// check if the parameters are defined in the conf. file
	if(!state->getRegistry()->isModified("test_cases"))
		return false;

	voidP sptr = state->getRegistry()->getEntry("test_cases"); // get parameter value
	configFile = *((std::string*) sptr.get()); // convert from voidP to user defined type

	sptr = state->getRegistry()->getEntry("normalized"); // get parameter value
	m_Normalized = (bool) *((uint*) sptr.get()); // convert from voidP to user defined type

// originalni dio iz BEAGLE implementacije:
	std::string input,sp,duration,deadline,weightT,weightF,weightE,weightN,term,ready,cons,speed;
	char pom[256];
	ReadPar p;
	unsigned int i,j;
	double d_niz[2][1000];

	// inicijalizacija default vrijednosti
	input = configFile;	// glavni ulazni fajl, mora ga biti

	// ucitavanje parametara
	p.OpenFile(input.c_str());
	if(p.ReadParameter("single",ReadPar::INTEGER,&i))
		m_Environment = SINGLE;
	else if(p.ReadParameter("uniform",ReadPar::INTEGER,&i))
		m_Environment = UNIFORM;
	else if(p.ReadParameter("unrelated",ReadPar::INTEGER,&i))
		m_Environment = UNRELATED;
	else if(p.ReadParameter("jobshop",ReadPar::INTEGER,&i))
		m_Environment = JOBSHOP;
	p.ReadParameter("sets",ReadPar::INTEGER,&sets);
	p.ReadParameter("max_jobs",ReadPar::INTEGER,&max_jobs);
	if(!p.ReadParameter("max_machines",ReadPar::INTEGER,&max_machines))
		max_machines = 1;
	p.ReadParameter("max_length",ReadPar::INTEGER,&max_length);
	p.ReadParameter("duration",ReadPar::STRING,pom); duration = pom;
	p.ReadParameter("deadline",ReadPar::STRING,pom); deadline = pom;
	p.ReadParameter("weight_T",ReadPar::STRING,pom); weightT = pom;
	p.ReadParameter("weight_F",ReadPar::STRING,pom); weightF = pom;
	p.ReadParameter("weight_E",ReadPar::STRING,pom); weightE = pom;
	p.ReadParameter("weight_N",ReadPar::STRING,pom); weightN = pom;
	p.ReadParameter("SP",ReadPar::STRING,pom); sp = pom;
	p.ReadParameter("machine_file",ReadPar::STRING,pom); speed = pom;
	// dinamicki dolasci poslova
	if(p.ReadParameter("ready",ReadPar::STRING,pom))
	{	ready = pom;
		m_dynamic = true;
	}
	else
		m_dynamic = false;
	// limited error fitness izracunavanje
	if(p.ReadParameter("LEF",ReadPar::INTEGER,&i))
	{	if(i==1)
		{	m_LEF = true;
			if(!p.ReadParameter("LEF_value",ReadPar::DOUBLE,&m_LEFVal))
				CHECKMSG(0,"LEF vrijednost nije zadana!");
		}
		else
			m_LEF = false;
	}
	// evaluacija - ispis rjesenja za svaku jedinku
	if(p.ReadParameter("evaluation",ReadPar::INTEGER,&i))
		if(i==1) m_Evaluation = true;
		else m_Evaluation = false;
	// fitness - mora biti definiran
	if(p.ReadParameter("fitness",ReadPar::STRING,pom))
	{	input = pom;
		if(input == "Twt")
			m_fitness = Twt;
		else if(input == "Nwt")
			m_fitness = Nwt;
		else if(input == "FTwt")
			m_fitness = FTwt;
		else if(input == "ETwt")
			m_fitness = ETwt;
		else if(input == "Cmax")
			m_fitness = Cmax;
		else if(input == "Fwt")
			m_fitness = Fwt;
		else
			CHECKMSG(0,"Nepoznata fitnes funkcija!");
	}
	else CHECKMSG(0,"Nije definirana fitnes funkcija!");
	// editiranje jedinke
	if(p.ReadParameter("editing",ReadPar::INTEGER,&i))
		if(i==1) m_editing = true;
	// stochastic sampling, koliko
	if(p.ReadParameter("stsampling",ReadPar::DOUBLE,&m_sampling))
		m_stsampling = true;
	else
		m_stsampling = false;
	// ogranicenja u rasporedu
	if(p.ReadParameter("constraints",ReadPar::STRING,pom))
	{	cons = pom;
		m_constrained = true;
	}
	else
		m_constrained = false;
	// trajanja postavljanja
	if(p.ReadParameter("setup",ReadPar::DOUBLE,&m_setup_faktor))
		m_setup = true;
	else
		m_setup = false;

	if(p.ReadParameter("idleness",ReadPar::INTEGER, &i))
		if(i == 1)	m_Idleness = true;

	//duration = path + duration;
	//deadline = path + deadline;
	//sp = path + sp;
	//weightT = path + weightT;
	//weightF = path + weightF;
	//weightE = path + weightE;
	//weightN = path + weightN;
	//ready = path + ready;
	N.Init(sets);
	SP.Init(sets);
	SD.Init(sets);
	Machines.Init(sets);
	MachineReady.Init(max_machines);
	Speed.Init(sets,max_jobs);
	Duration.Init(sets,max_jobs);
	Deadline.Init(sets,max_jobs);
	Durations.Init(max_jobs, max_machines);
	MachineIndex.Init(max_jobs, max_machines);
	WeightT.Init(sets,max_jobs);
	WeightF.Init(sets,max_jobs);
	WeightE.Init(sets,max_jobs);
	WeightN.Init(sets,max_jobs);
	Fitness.Init(sets+1,FUNCTIONS);
	Values.Init(max_machines,max_jobs);
	Precedence.Reset(max_jobs,max_jobs);	// prazna matrica znaci da nema ogranicenja!
	Setup.Init(max_jobs+1,max_jobs);
	if(m_Environment == JOBSHOP)
	{	Schedule.Init(sets, max_machines*max_jobs);
		PTimeAvg.Reset(sets, max_machines);
	}
	else
	{	Schedule.Init(sets,max_jobs);
		PTimeAvg.Init(sets,max_jobs);
		PTimeMinMachine.Init(sets,max_jobs);
	}
	SortedReady.Init(sets,max_jobs);

	pVrijednosti = new double[max_jobs];
	pRasporedjen = new bool[max_jobs];
	pIndex = new unsigned int[max_jobs];
	pUsed = new unsigned int[max_jobs];
	pArray = new double[max_jobs];
	pSlack = new double[max_jobs];
	pSlackSpeed = new double[max_jobs];
	pArrival = new double[max_jobs];
	pLevel = new double[max_jobs];
	pSetupAvg = new double[max_jobs + 1];
	pSamples = new bool[sets];
	pLastJob = new unsigned int[max_machines];
	pMachineScheduled = new unsigned int[max_machines];
	pOperationReady = new double[max_machines];
	pJobReady = new double[max_jobs];
	pOperationsScheduled = new unsigned int[max_jobs];
	pTotalWorkRemaining = new double[max_jobs];
	pTotalWorkDone = new double[max_jobs];
	pTotalMachineWork = new double[max_machines];
	pOperationsWaiting = new unsigned int[max_machines];
	pMachineWorkRemaining = new double[max_machines];
	pMachineValues = new double[max_machines];
	p.ReadParameter("jobs",ReadPar::DOUBLE,&d_niz[0][0],sets);
	p.ReadParameter("machines",ReadPar::DOUBLE,&d_niz[1][0],sets);
	total_jobs = 0;
	for(i=0; i<sets; i++)
	{	N[i][0] = d_niz[0][i];
		total_jobs += (int) d_niz[0][i];
		Machines[i][0] = d_niz[1][i];
	}
	Duration.Load(duration.c_str());
	Deadline.Load(deadline.c_str());
	if(m_Environment==UNIFORM)
	{	Speed.Load(speed.c_str());
	}
	WeightT.Load(weightT.c_str());
	WeightF.Load(weightF.c_str());
	WeightE.Load(weightE.c_str());
	WeightN.Load(weightN.c_str());
	SP.Load(sp.c_str());
	if(m_dynamic) Ready.Load(ready.c_str());
	else Ready.Reset(sets,max_jobs);
	if(m_constrained) Constraints.Load(cons.c_str());
	// racunamo sumu deadline-a
	Level.Init(sets,max_jobs);
	for(i=0; i<sets; i++)
	{	SD.Set(i,0);
		for(j=0; j<(unsigned int)N.Get(i); j++)
		{	SD.data[i][0] += Deadline.data[i][j];
			Level[i][j] = -1;	// oznacimo da je neizracunato
		}
	}
	// racunamo level cvorova u grafu ovisnosti
	for(i=0; i<sets; i++)
	{	if(m_constrained)
			ReadConstraints(Constraints,i,(unsigned int)N.Get(i),Precedence);
		for(j=0; j<(unsigned int)N.Get(i); j++)
			Level[i][j] = NodeLevel(i,j);
	}
	// racunamo prosjek i minimalno trajanje izvodjenja za UNRELATED
	if(m_Environment == UNRELATED)
	{	for(uint set=0; set<sets; set++)
		{	uint jobs = (uint) N[set][0];
			uint machines = (uint) Machines[set][0];
			for(j=0; j<jobs; j++)
			{	PTimeAvg[set][j] = 0;
				uint min_machine = 0;
				for(uint machine=0; machine<machines; machine++)
				{	PTimeAvg[set][j] += Duration[set][j*machines + machine];
					if(Duration[set][j*machines + machine] < Duration[set][j*machines + min_machine])
						min_machine = machine;
				}
				PTimeAvg[set][j] /= machines;
				PTimeMinMachine[set][j] = min_machine;
			}
		}
	}
	if(m_Environment == JOBSHOP)	// prosjek trajanja operacija po strojevima
	{	for(uint set=0; set<sets; set++)
		{	uint jobs = (uint) N[set][0];
			uint machines = (uint) Machines[set][0];
			for(j=0; j<jobs; j++)
			{	uint operations = machines;
				for(uint op=0; op<operations; op++)
				{	double dur = Duration[set][j*operations + op];
					uint machine = (uint) dur / 1000;
					dur = (int)dur % 1000;	// dobijemo trajanje
					PTimeAvg[set][machine] += dur;
				}
			}
			for(uint m=0; m<machines; m++)
				PTimeAvg[set][m] /= jobs;
		}
	}

	// sortiramo indekse poslova po dolascima - treba za ubrzano izracunavanje
	//for(i=0; i<sets; i++)
	//{	::pVal = Ready[i];
	//	uint jobs = (uint) N[i][0];
	//	for(j=0; j<jobs; j++)
	//		pIndex[j] = j;
	//	qsort(pIndex,jobs,sizeof(unsigned int),::Before);
	//	for(j=0; j<jobs; j++)
	//		SortedReady[i][j] = pIndex[j];
	//}
				
	p.CloseFile();

	return true;
}


// rekurzivno racunanje 'level' vrijednosti svakog posla
// ima smisla samo u problemima s ogranicenjima
// promjena 27.07: level cvora ukljucuje i trajanje cvora
double SchedulingEvalOp::NodeLevel(int set, int node)
{	double value,level;
	int succ,i,next;
	if(Level[set][node] > -1)	// ako je vec izracunato
		return Level[set][node];
	if(Precedence[node][1] == 0)	// ako nema sljedbenika
		return Duration[set][node];
	succ = (int)Precedence[node][1];	// koliko sljedbenika
	next = (int)Precedence[node][2];	// prvi sljedbenik
	level = NodeLevel(set,next) + Duration[set][node];	// level zbog prvog sljedbenika
	for(i=1; i<succ; i++)
	{	next = (int)Precedence[node][i+2];
		value = NodeLevel(set,next) + Duration[set][node];
		if(value > level)
			level = value;
	}
	return level;
}




SchedulingEvalOp::~SchedulingEvalOp()
{
	delete [] pRasporedjen;
	delete [] pVrijednosti;
	delete [] pIndex;
	delete [] pUsed;
	delete [] pArray;
	delete [] pSlack;
	delete [] pSlackSpeed;
	delete [] pSamples;
	delete [] pArrival;
	delete [] pLevel;
	delete [] pSetupAvg;
	delete [] pLastJob;
	delete [] pMachineScheduled;
	delete [] pOperationReady;
	delete [] pJobReady;
	delete [] pOperationsScheduled;
	delete [] pTotalWorkRemaining;
	delete [] pTotalWorkDone;
	delete [] pTotalMachineWork;
	delete [] pMachineWorkRemaining;
	delete [] pOperationsWaiting;
	delete [] pMachineValues;
}


// dekodira matricu Constraints i definira matricu Precedence
void SchedulingEvalOp::ReadConstraints(Matrica &Constraints, int set, int jobs, Matrica &Precedence)
{
	int i,j,prethodnika,prethodnik,pom;
	unsigned int podatak;
	//Precedence.Init(jobs,jobs);
	// prvo ocistimo prva dva stupca! gdje su brojevi prethodnika i sljedbenika
	for(i=0; i<jobs; i++)
		for(j=0; j<2; j++)
			Precedence[i][j] = 0;
	for(i=0; i<jobs; i++)
	{	podatak = (unsigned int) Constraints[set][i];
		prethodnik = 1;	// koji po redu posao iza mene
		prethodnika = 0;
		while(podatak != 0)
		{	if(podatak%2 != 0)
			{	prethodnika++;	// povecam broj svojih prethodnika
				pom = (int) Precedence[i-prethodnik][1] + 1;
				Precedence[i-prethodnik][pom+1] = i;
				Precedence[i-prethodnik][1] = pom;	// i broj sljedbenika od moga prethodnika
			}
			prethodnik++;
			podatak /= 2;
		}
		Precedence[i][0] = prethodnika;
	}
}


// generira matricu sequence dependant setup trajanja
// i polje prosjecnih trajanja postavljanja za svaki posao prema ostalima
void SchedulingEvalOp::MakeSetup(Matrica &Duration, int set, int jobs, double faktor, Matrica &Setup)
{
	int i,j;
	pSetupAvg[jobs] = 0;
	if(m_Environment == JOBSHOP)
	{	srand(set);
		for(i=0; i<jobs; i++)
		{	Setup[jobs][i] = (int) ((rand()%max_length+1) * faktor);	// polazno trajanje postavljanja
			pSetupAvg[jobs] += Setup[jobs][i];
			for(j=0; j<=i; j++)
			{	Setup[i][j] = (int) ((rand()%max_length+1) * faktor);
				Setup[j][i] = (int) ((rand()%max_length+1) * faktor);
				pSetupAvg[i] += Setup[i][j];
				pSetupAvg[j] += Setup[j][i];
			}
		}
	}
	else
		for(i=0; i<jobs; i++)
		{	pSetupAvg[i] = 0;
			Setup[jobs][i] = Duration[set][(i+1) % jobs];	// polazno trajanje postavljanja
			pSetupAvg[jobs] += Setup[jobs][i];
			for(j=0; j<=i; j++)
			{	Setup[i][j] = ceil( fabs( Duration[set][i] - Duration[set][j] ) * faktor);
				Setup[j][i] = ceil( fabs( Duration[set][(i+1) % jobs] - Duration[set][(j+1) % jobs] ) * faktor);
				pSetupAvg[i] += Setup[i][j];
				pSetupAvg[j] += Setup[j][i];
			}
		}
	pSetupAvg[jobs] /= jobs;
	for(i=0; i<jobs; i++)
		pSetupAvg[i] /= (jobs-1);
}


FitnessP SchedulingEvalOp::evaluate(IndividualP individual)
{
	// stvaranje zeljenog Fintess objekta:
	FitnessP fitness = static_cast<FitnessP> (new FitnessMin);

	// dohvat genotipa jedinke
	TreeP tree = boost::dynamic_pointer_cast<Tree::Tree> (individual->getGenotype());

// oroginalni kod iz BEAGLE implementacije
	unsigned int i;
	double dRawFitness, dFitness;

	// stochastic sampling?
	if(m_stsampling)
	{	int koliko = (int) (m_sampling*sets);
		int razmak = sets / koliko;
		int pocetni = rand()%razmak;
		for(i=0; i<sets; i++)
			pSamples[i] = false;
		for(i=pocetni; i<sets; i+=razmak)
			pSamples[i] = true;
	}

	switch(m_Environment)
	{	case SINGLE:
			dRawFitness = EvaluateSingle(tree);
		break;
		//case UNIFORM:
		//	EvaluateUniform(dRawFitness);
		//break;
		//case UNRELATED:
		//	EvaluateUnrelated(dRawFitness);
		//break;
		//case JOBSHOP:
		//	EvaluateJobShop(dRawFitness);
		//break;
	}

	//dFitness = dRawFitness /= nJobS*SETS;	// prosjek
	//double lRMSE = sqrt(sqrt(dRawFitness));		// irelevantno za turnir selekciju
	//dFitness = (1.0 / (lRMSE + 1.0));	
	//dFitness = -dRawFitness;	// obrnemo kriterij (trazimo minimum)
	dFitness = dRawFitness;

	//if(m_Evaluation)	// samo za usporedbu heuristika! pise rezultate svih skupova u fajl
	//{	Fitness.Save("rezultat_GP.txt");
	//	std::ostream *file = new std::ofstream("rezultat_GP.txt", std::ios_base::app);
	//	Evaluator.write();
	//	*file << std::endl << "-- infix: " << Evaluator.m_output << " --" << std::endl;
	//	*file << "Editirano: " << edited << ", ukupno: " << total << std::endl;
	//	*file << std::flush;
	//	delete file;
	//	Schedule.Save("raspored_GP.txt");
	//}

	fitness->setValue(dFitness);

	return fitness;
}


// obrada za SINGLE okruzenje
double SchedulingEvalOp::EvaluateSingle(TreeP tree)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dAvgDuration, dAvgDueDate;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSDr;
	unsigned int nPoslova,nNiz,nJob,i,j,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nOdabrani;

	double dRawFitness=0;

// vrtimo sve skupove
	for(nNiz = 0; nNiz < sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);

		// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		dAvgDueDate = 0;
		for(i=0; i<nPoslova; i++)
		{	dAvgDueDate += Deadline[nNiz][i];
			pRasporedjen[i] = false;	// svi nerasporedjeni
			pIndex[i] = i;	// inicijalni poredak
		}
		dAvgDueDate /= nPoslova;
		dSPr = SP.Get(nNiz);
		dSDr = SD.Get(nNiz);

	// postavljanje pocetnih vrijednosti terminala - nepromjenjivi terminali
/*		Evaluator.m_pTermValues[T_N] = N.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			Evaluator.m_dTermValuesArray[T_TF][i] = 1 - dAvgDueDate / SP[nNiz][0];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_pt], Duration.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));
*/

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa jedan po jedan
		{	
			// terminali neovisni o poslu (ali ovisni o rasporedjenim poslovima!)
			tree->setTerminalValue("SPr", &dSPr);    // suma trajanja preostalih
			tree->setTerminalValue("Nr", &nNr);      // broj preostalih

			// dinamicka okolina; + uzeta u obzir eventualna ogranicenja
			if(m_dynamic)
			{	unsigned int raspolozivi = nJob, prvi = nJob;
				unsigned int najkraci;	// najkraci raspolozivi
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				double najdulje = 0, najkrace = 0;
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	int job = pIndex[raspolozivi];
					if(Ready.data[nNiz][job] < kada && Precedence[job][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][job];
						prvi = raspolozivi;
					}
				}
				if(kada > dClock)	// ako nema raspolozivih 
				{	dClock = kada;	// sat postavimo na najblize vrijeme dolaska
				}
				// pronadjimo najduljeg i najkraceg raspolozivog
				najdulje = najkrace = Duration[nNiz][pIndex[prvi]];
				najkraci = prvi;
				for(i = nJob; i < nPoslova; i++)
				{	int job = pIndex[i];
					if(dClock < Ready[nNiz][job] || Precedence[job][0] > 0)
						continue;
					if(Duration[nNiz][job] < najkrace)	// najkrace
					{	najkrace = Duration[nNiz][job];
						najkraci = i;
					}
				}
				// sad pogledamo najduljega koji pocinje <= zavrsetka najkraceg raspolozivog
				for(i = nJob; i < nPoslova; i++)
				{	int job = pIndex[i];
					if(Precedence[job][0] > 0)
						continue;
					if(Duration[nNiz][job] > najdulje && Ready[nNiz][job] <= (dClock+najkrace))	// gledamo najdulje trajanje
						najdulje = Duration[nNiz][job];
				}
				// sada je (dClock + najkrace + najdulje) limit za gledanje u buducnost!

				// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
				double dCurrent;
				for(i = nJob; i<nPoslova; i++)
				{	j = pIndex[i];
					// terminali ovisni o poslu
					pSlack[j] = POS(Deadline[nNiz][j] - (dClock + Duration[nNiz][j]));
					tree->setTerminalValue("SL", &pSlack[j]);
					tree->setTerminalValue("pt", &Duration[nNiz][j]);
					tree->setTerminalValue("w", &WeightT[nNiz][j]);
					tree->setTerminalValue("dd", &Deadline[nNiz][j]);

					// terminali za trajanja postavljanja
					if(m_setup) {
						tree->setTerminalValue("STP", &Setup[nLastJob][j]);   // trajanje postavljanja
						tree->setTerminalValue("Sav", &pSetupAvg[nLastJob]);  // prosjecno t.p.
					}

					// terminali za ogranicenja u redoslijedu
					if(m_constrained) {
						tree->setTerminalValue("SC", &Precedence[j][1]);      // broj sljedbenika
						tree->setTerminalValue("LVL", &Level[nNiz][j]);       // razina posla (level)
					}

					tree->execute(&dCurrent);
					pVrijednosti[j] = dCurrent;
				}

				// druga verzija (jednostavnija)
				// samo gledamo najboljega koji moze poceti prije zavrsetka najkraceg raspolozivog
				dBest = pVrijednosti[pIndex[najkraci]]; // poc. vrijednost za usporedbu
				nOdabrani = najkraci;
				for(i=nJob; i<nPoslova; i++)	// trazimo najbolju vrijednost unutar < (dClock + najkrace)
				{	if((pVrijednosti[pIndex[i]] < dBest) && (Ready[nNiz][pIndex[i]] < dClock + najkrace) \
						&& Precedence[pIndex[i]][0] == 0)
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
				kada = Ready[nNiz][pIndex[nOdabrani]] - dClock;	// za koliko pocinje odabrani?

				// redovni nastavak (iza 1. i 2. verzije)
				if(kada > 0)	// odabrali smo cekati
					dClock += kada;	// ili: dClock = Ready[nNiz][pIndex[nOdabrani]];
			}	// endif - m_dynamic

			// staticki
			else
			{	//CalcTimedTerminals(nNiz,nPoslova,nJob,dClock);
				
				double dCurrent;

				// ocijeni sve nerasporedjene poslove
				for(i = nJob; i<nPoslova; i++)
				{	j = pIndex[i];
					// terminali ovisni o poslu
					pSlack[j] = POS(Deadline[nNiz][j] - (dClock + Duration[nNiz][j]));
					tree->setTerminalValue("SL", &pSlack[j]);
					tree->setTerminalValue("pt", &Duration[nNiz][j]);
					tree->setTerminalValue("w", &WeightT[nNiz][j]);
					tree->setTerminalValue("dd", &Deadline[nNiz][j]);

					// terminali za trajanja postavljanja
					if(m_setup) {
						tree->setTerminalValue("STP", &Setup[nLastJob][j]);   // trajanje postavljanja
						tree->setTerminalValue("Sav", &pSetupAvg[nLastJob]);  // prosjecno t.p.
					}

					// terminali za ogranicenja u redoslijedu
					if(m_constrained) {
						tree->setTerminalValue("SC", &Precedence[j][1]);      // broj sljedbenika
						tree->setTerminalValue("LVL", &Level[nNiz][j]);       // razina posla (level)
					}

					tree->execute(&dCurrent);

					pVrijednosti[j] = dCurrent;
				}

				dBest = pVrijednosti[pIndex[nJob]];	// pretpostavimo da je neki najbolji
				nOdabrani = nJob;

				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;

				for(i = nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					int index = pIndex[i];
					if(pVrijednosti[index] < dBest && Precedence[index][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[index];
						nOdabrani = i;
					}
				}
			}

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;

			// azuriramo vrijednosti promjenjivih terminala
			dClock += Duration[nNiz][pIndex[nJob]];   // update vremena
			dSPr -= Duration[nNiz][pIndex[nJob]];     // update trajanja preostalih
			dSDr -= Deadline[nNiz][pIndex[nJob]];     // update deadlinea
			nNr--;                                    // update broja preostalih
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[pIndex[nJob]][1]; i++)
				{	j = (int) Precedence[pIndex[nJob]][i+2];
					Precedence[j][0] -= 1;
				}
			if(m_setup)	// trajanje postavljanja
			{	dClock += Setup[nLastJob][pIndex[nJob]];
				nLastJob = pIndex[nJob];	// sljedeci prethodni posao
			}
			Schedule[nNiz][nJob] = pIndex[nJob];	// zapisemo posao u raspored
		} // kraj petlje koja vrti poslove u skupu


		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = dAvgDuration = 0;
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = pIndex[nJob];
				dAvgWeights += WeightT[nNiz][index];
				dAvgDuration += Duration[nNiz][index];
				if(m_dynamic && dClock < Ready[nNiz][index])	// ako jos nije raspoloziv
					dClock = Ready[nNiz][index];
				if(m_setup)
					dClock += Setup[nLastJob][index];
				nLastJob = index;
				dClock += Duration.data[nNiz][index];	// update vremena
				dRez = dClock - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
			}
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			// promijenjeno (valjda konacno) 04.09.
			if(m_Normalized) {
				dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
				dAvgDuration /= nPoslova;
				dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
				dNwt /= (nPoslova * dAvgWeights);
			}
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				default: exit(1);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
		}
	}
// kraj petlje koja vrti skupove poslova

	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;

	return dRawFitness;
}