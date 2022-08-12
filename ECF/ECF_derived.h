#ifndef ECF_derived_h
#define ECF_derived_h

// derived:
#include "RealValueGenotype.h"
#include "SelRandomOp.h"
#include "SelWorstOp.h"
#include "SelBestOp.h"
#include "SelFitnessProportionalOp.h"
#include "AlgSteadyStateTournament.h"
#include "AlgRouletteWheel.h"
#include "AlgElimination.h"
#include "AlgParticleSwarmOptimization.h"
#include "AlgRandomSearch.h"
#include "AlgGeneticAnnealing.h"
#include "AlgDifferentialEvolution.h"
#include "AlgArtificialBeeColony.h"
#include "AlgGenHookeJeeves.h"
#include "AlgClonalg.h"
#include "AlgOptIA.h"
#include "AlgEvolutionStrategy.h"
#include "AlgNSGA2.h"
#include "AlgCuckooSearch.h"
#include "xcs/AlgXCS.h"
#include "FitnessMin.h"
#include "FitnessMax.h"
#include "MOFitness.h"
#include "HallOfFame.h"
#include "StatCalc.h"
#include "SimpleRandomizer.h"
#include "TermMaxGenOp.h"
#include "TermFitnessValOp.h"
#include "TermMaxTimeOp.h"
#include "TermStagnationOp.h"
#include "TermMaxEvalOp.h"

#ifdef _MPI
#include "AlgSGenGPEA.h"
#include "AlgAEliGPEA.h"
#include "AlgAEliGPEA2.h"
#endif

#endif // ECF_derived_h

