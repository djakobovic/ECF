pkginclude_maindir = $(includedir)/ECF
pkginclude_treedir = $(includedir)/ECF/tree
pkginclude_binarydir = $(includedir)/ECF/binary
pkginclude_floatingpointdir = $(includedir)/ECF/floatingpoint
pkginclude_bitstringdir = $(includedir)/ECF/bitstring
pkginclude_permutationdir = $(includedir)/ECF/permutation
pkginclude_apgenotypedir = $(includedir)/ECF/apgenotype
pkginclude_xmldir = $(includedir)/ECF/xml
pkginclude_xcsdir = $(includedir)/ECF/xcs

pkginclude_main_HEADERS = Algorithm.h \
	AlgRouletteWheel.h \
	AlgSteadyStateTournament.h \
	AlgParticleSwarmOptimization.h \
	AlgElimination.h \
	AlgGeneticAnnealing.h \
	AlgGenHookeJeeves.h \
	AlgRandomSearch.h \
	AlgDifferentialEvolution.h \
	AlgArtificialBeeColony.h \
	AlgClonalg.h \
	AlgOptIA.h \
	AlgEvolutionStrategy.h \
	AlgNSGA2.h \
	AlgCuckooSearch.h \
	Communicator.h \
	Context.h \
	Crossover.h \
	Deme.h \
	ECF.h \
	ECF_base.h \
	ECF_derived.h \
	ECF_macro.h \
	EvaluateOp.h \
	Fitness.h \
	FitnessMax.h \
	FitnessMin.h \
	MOFitness.h \
	Genotype.h \
	RealValueGenotype.h \
	HallOfFame.h \
	Individual.h \
	Logger.h \
	Migration.h \
	Mutation.h \
	Operator.h \
	Population.h \
	Randomizer.h \
	Registry.h \
	SelBestOp.h \
	SelectionOperator.h \
	SelFitnessProportionalOp.h \
	SelRandomOp.h \
	SelWorstOp.h \
	SimpleRandomizer.h \
	StatCalc.h \
	State.h \
	TermFitnessValOp.h \
	TermMaxGenOp.h \
	TermMaxTimeOp.h \
	TermMaxEvalOp.h \
	TermStagnationOp.h
pkginclude_tree_HEADERS = tree/Add.h \
	tree/Cos.h \
	tree/Div.h \
	tree/Mul.h \
	tree/Node.h \
	tree/Pos.h \
	tree/Max.h \
	tree/Min.h \
	tree/Neg.h \
	tree/Primitive.h \
	tree/PrimitiveSet.h \
	tree/Sin.h \
	tree/Sub.h \
	tree/Terminal.h \
	tree/Tree.h \
	tree/Tree_c.h \
	tree/TreeCrxSimple.h \
	tree/TreeCrxContextPreserved.h \
	tree/TreeCrxOnePoint.h \
	tree/TreeCrxSizeFair.h \
	tree/TreeCrxUniform.h \
	tree/TreeMutPermutation.h \
	tree/TreeMutGauss.h \
	tree/TreeMutHoist.h \
	tree/TreeMutNodeComplement.h \
	tree/TreeMutNodeReplace.h \
	tree/TreeMutShrink.h \
	tree/TreeMutSubtree.h
pkginclude_binary_HEADERS = binary/Binary.h \
	binary/BinaryCrsHalfUniform.h \
	binary/BinaryCrsMasked.h \
	binary/BinaryCrsNonGeometric.h \
	binary/BinaryCrsOnePoint.h \
	binary/BinaryCrsRandomRespectful.h \
	binary/BinaryCrsReducedSurrogate.h \
	binary/BinaryCrsSegmented.h \
	binary/BinaryCrsShuffle.h \
	binary/BinaryCrsTwoPoint.h \
	binary/BinaryCrsUniform.h \
	binary/BinaryMutSimple.h \
	binary/BinaryMutMix.h
pkginclude_floatingpoint_HEADERS = floatingpoint/FloatingPoint.h \
	floatingpoint/FloatingPointCrsArithmetic.h \
	floatingpoint/FloatingPointCrsArithmeticSimple.h \
	floatingpoint/FloatingPointCrsArithmeticSingle.h \
	floatingpoint/FloatingPointCrsAverage.h \
	floatingpoint/FloatingPointCrsBga.h \
	floatingpoint/FloatingPointCrsBlx.h \
	floatingpoint/FloatingPointCrsBlxAlpha.h \
	floatingpoint/FloatingPointCrsBlxAlphaBeta.h \
	floatingpoint/FloatingPointCrsDiscrete.h \
	floatingpoint/FloatingPointCrsFlat.h \
	floatingpoint/FloatingPointCrsHeuristic.h \
	floatingpoint/FloatingPointCrsLocal.h \
	floatingpoint/FloatingPointCrsOnePoint.h \
	floatingpoint/FloatingPointCrsRandom.h \
	floatingpoint/FloatingPointCrsSbx.h \
	floatingpoint/FloatingPointMutSimple.h
pkginclude_bitstring_HEADERS = bitstring/BitString.h \
	bitstring/BitStringCrsOnePoint.h \
	bitstring/BitStringMutSimple.h \
	bitstring/BitStringMutMix.h \
	bitstring/BitStringCrsUniform.h
pkginclude_permutation_HEADERS = permutation/Permutation.h \
	permutation/PermutationCrsCOSA.h \
	permutation/PermutationCrsCyclic.h \
	permutation/PermutationCrsCyclic2.h \
	permutation/PermutationCrsDPX.h \
	permutation/PermutationCrsOBX.h \
	permutation/PermutationCrsOPX.h \
	permutation/PermutationCrsOX.h \
	permutation/PermutationCrsOX2.h \
	permutation/PermutationCrsPBX.h \
	permutation/PermutationCrsPMX.h \
	permutation/PermutationCrsSPX.h \
	permutation/PermutationCrsULX.h \
	permutation/PermutationCrsUPMX.h \
	permutation/PermutationMutIns.h \
	permutation/PermutationMutInv.h \
	permutation/PermutationMutToggle.h
pkginclude_apgenotype_HEADERS = apgenotype/APGenotype.h \
	apgenotype/PrimitiveSetAP.h
pkginclude_xml_HEADERS = xml/xmlParser.h
pkginclude_xcs_HEADERS = xcs/AlgXCS.h \
	xcs/Classifier.h \
	xcs/ClassifierParams.h \
	xcs/Environment.h \
	xcs/XCSParams.h

lib_LTLIBRARIES = libecf.la
if MPI
libecf_la_SOURCES = AlgAEliGPEA2.cpp \
	AlgAEliGPEA.cpp \
	AlgSGenGPEA.cpp \
	Communicator.cpp 
else
libecf_la_SOURCES = 
endif
libecf_la_SOURCES += Algorithm.cpp \
	AlgRouletteWheel.cpp \
	AlgSteadyStateTournament.cpp \
	AlgParticleSwarmOptimization.cpp \
	AlgElimination.cpp \
	AlgRandomSearch.cpp \
	AlgGeneticAnnealing.cpp \
	AlgGenHookeJeeves.cpp \
	AlgDifferentialEvolution.cpp \
	AlgArtificialBeeColony.cpp \
	AlgClonalg.cpp \
	AlgOptIA.cpp \
	AlgEvolutionStrategy.cpp \
	AlgNSGA2.cpp \
	AlgCuckooSearch.cpp \
	Crossover.cpp \
	Deme.cpp \
	FitnessMax.cpp \
	FitnessMin.cpp \
	MOFitness.cpp \
	Genotype.cpp \
	HallOfFame.cpp \
	Individual.cpp \
	Logger.cpp \
	Migration.cpp \
	Mutation.cpp \
	Population.cpp \
	Registry.cpp \
	SelBestOp.cpp \
	SelFitnessProportionalOp.cpp \
	SelRandomOp.cpp \
	SelWorstOp.cpp \
	SimpleRandomizer.cpp \
	StatCalc.cpp \
	State.cpp \
	TermFitnessValOp.cpp \
	TermMaxGenOp.cpp \
	TermMaxTimeOp.cpp \
	TermStagnationOp.cpp \
	TermMaxEvalOp.cpp \
	binary/Binary.cpp \
	binary/BinaryCrsHalfUniform.cpp \
	binary/BinaryCrsMasked.cpp \
	binary/BinaryCrsNonGeometric.cpp \
	binary/BinaryCrsOnePoint.cpp \
	binary/BinaryCrsRandomRespectful.cpp \
	binary/BinaryCrsReducedSurrogate.cpp \
	binary/BinaryCrsSegmented.cpp \
	binary/BinaryCrsShuffle.cpp \
	binary/BinaryCrsTwoPoint.cpp \
	binary/BinaryCrsUniform.cpp \
	binary/BinaryMutMix.cpp \
	binary/BinaryMutSimple.cpp \
	floatingpoint/FloatingPoint.cpp \
	floatingpoint/FloatingPointCrsArithmetic.cpp \
	floatingpoint/FloatingPointCrsArithmeticSimple.cpp \
	floatingpoint/FloatingPointCrsArithmeticSingle.cpp \
	floatingpoint/FloatingPointCrsAverage.cpp \
	floatingpoint/FloatingPointCrsBga.cpp \
	floatingpoint/FloatingPointCrsBlx.cpp \
	floatingpoint/FloatingPointCrsBlxAlpha.cpp \
	floatingpoint/FloatingPointCrsBlxAlphaBeta.cpp \
	floatingpoint/FloatingPointCrsDiscrete.cpp \
	floatingpoint/FloatingPointCrsFlat.cpp \
	floatingpoint/FloatingPointCrsHeuristic.cpp \
	floatingpoint/FloatingPointCrsLocal.cpp \
	floatingpoint/FloatingPointCrsOnePoint.cpp \
	floatingpoint/FloatingPointCrsRandom.cpp \
	floatingpoint/FloatingPointCrsSbx.cpp \
	floatingpoint/FloatingPointMutSimple.cpp \
	bitstring/BitStringCrsOnePoint.cpp \
	bitstring/BitStringMutSimple.cpp \
	bitstring/BitStringCrsUniform.cpp \
	bitstring/BitStringMutMix.cpp \
	tree/Cos.cpp \
	tree/Node.cpp \
	tree/Primitive.cpp \
	tree/PrimitiveSet.cpp \
	tree/Sin.cpp \
	tree/Tree.cpp \
	tree/TreeCrxSimple.cpp \
	tree/TreeCrxContextPreserved.cpp \
	tree/TreeCrxOnePoint.cpp \
	tree/TreeCrxSizeFair.cpp \
	tree/TreeCrxUniform.cpp \
	tree/TreeMutPermutation.cpp \
	tree/TreeMutGauss.cpp \
	tree/TreeMutHoist.cpp \
	tree/TreeMutNodeComplement.cpp \
	tree/TreeMutNodeReplace.cpp \
	tree/TreeMutShrink.cpp \
	tree/TreeMutSubtree.cpp \
	permutation/Permutation.cpp \
	permutation/PermutationCrsCOSA.cpp \
	permutation/PermutationCrsCyclic.cpp \
	permutation/PermutationCrsCyclic2.cpp \
	permutation/PermutationCrsDPX.cpp \
	permutation/PermutationCrsOBX.cpp \
	permutation/PermutationCrsOPX.cpp \
	permutation/PermutationCrsOX.cpp \
	permutation/PermutationCrsOX2.cpp \
	permutation/PermutationCrsPBX.cpp \
	permutation/PermutationCrsPMX.cpp \
	permutation/PermutationCrsSPX.cpp \
	permutation/PermutationCrsULX.cpp \
	permutation/PermutationCrsUPMX.cpp \
	permutation/PermutationMutIns.cpp \
	permutation/PermutationMutInv.cpp \
	permutation/PermutationMutToggle.cpp \
	apgenotype/APGenotype.cpp \
	apgenotype/PrimitiveSetAP.cpp \
	xml/xmlParser.cpp \
	xcs/AlgXCS.cpp \
	xcs/Classifier.cpp \
	xcs/Environment.cpp \
	xcs/XCSParams.cpp

libecf_la_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/binary -I$(top_srcdir)/floatingpoint -I$(top_srcdir)/bitstring -I$(top_srcdir)/tree -I$(top_srcdir)/permutation -I$(top_srcdir)/apgenotype -I$(top_srcdir)/xml -I$(top_srcdir)/xcs
