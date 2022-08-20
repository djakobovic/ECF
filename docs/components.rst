
6. Adding components to ECF
===========================

#. :ref:`Adding a genotype <addgenotype>`
#. :ref:`Adding an algorithm <addalgorithm>`
#. :ref:`Adding an operator <addoperator>`


General rules:

-  Components must be added *before* the initialization phase (before
   'State::initialize').

-  Adding a component does not mean the component \ *will* be used;
   adding only makes it possible to choose that component in the
   configuration file (e.g. you may add an Algorithm, but it will only
   be used if stated in the configuration file).

-  All components may register their parameters ('registerParameters'
   method) and initialize themselves ('initialize' method).

 
.. _addgenotype:

1. Adding a genotype
~~~~~~~~~~~~~~~~~~~~

New genotype classes may be added with 'State::addGenotype(GenotypeP)'
function before 'State::initialize', e.g.:

.. code-block:: cpp

   NewGenotypeP gen = (NewGenotypeP) new NewGenotype;
   state->addGenotype(gen);

New genotype class must inherit from the abstract *Genotype* class (see
'Genotype.h'). A genotype *should* define its own crossover and mutation
classes so it can be used in an evolutionary algorithm (see example
below). If it does not, it will not change during crossover and mutation
of individuals (other genotypes in the individual may change). This
might be useful for additional data structures you may want to put in
the individual, but don't want them to evolve. Alternatively, you can
use the genotype protection option to exclude it from mutation and/or
crossover (see the :doc:`parameters section <parameters>`).

Adding a genotype with the same name as an existing one will replace the
original one (if you need to change the genotype's functionality).

An example: a genotype with a vector of double variables (and a pair of
operators which do nothing):

.. code-block:: cpp

	// empty crx op
	class MyGenotypeCrxOp : public CrossoverOp
	{
		public:
		bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child)
		{
			// does nothing! (see existing operators)
			return true;
		}
	};
	typedef boost::shared_ptr<MyGenotypeCrxOp> MyGenotypeCrxOpP;

	// empty mut op
	class MyGenotypeMutOp : public MutationOp
	{
		public:
		bool mutate(GenotypeP gene)
		{
			// does nothing! (see existing operators)
			return true;
		}
	};
	typedef boost::shared_ptr<MyGenotypeMutOp> MyGenotypeMutOpP;


	class MyGenotype : public Genotype
	{
		public:
		std::vector<double> realValues_;

		// mandatory: define name, other things as needed
		MyGenotype()
		{
			name_ = "MyGenotype";
		}

		// mandatory: must provide copy method
		MyGenotype* copy()
		{
			MyGenotype *newObject = new MyGenotype(*this);
			return newObject;
		}

		// optional: declare crx operators (if not, no crossover will be performed)
		std::vector<CrossoverOpP> getCrossoverOp()
		{
			std::vector<CrossoverOpP> crx;
			crx.push_back(static_cast<CrossoverOpP> (new MyGenotypeCrxOp));
			return crx;
		}

		// optional: declare mut operators (if not, no mutation will be performed)
		std::vector<MutationOpP> getMutationOp()
		{
			std::vector<MutationOpP> mut;
			mut.push_back(static_cast<MutationOpP> (new MyGenotypeMutOp));
			return mut;
		}

		// optional: register any parameters
		void registerParameters(StateP state)
		{
			registerParameter(state, "size", (voidP) (new uint(1)), ECF::UINT);
		}

		// mandatory: build initial genotype structure
		bool initialize(StateP state)
		{
			if(!isParameterDefined(state, "size")) {
				state->getLogger()->log(1, "Error: MyGenotype size not defined");
				throw("");
			}

			voidP sizep = getParameterValue(state, "size");
			uint size = *((uint*) sizep.get());

			realValues_.resize(size);
			// generate random doubles in [0, 1]
			for(uint i = 0; i < size; i++)
				realValues_[i] = state->getRandomizer()->getRandomDouble();

			return true;
		}

		// mandatory: write to XMLNode
		void write(XMLNode &xMyGenotype)
		{
			xMyGenotype = XMLNode::createXMLTopNode("MyGenotype");
			std::stringstream sValue;
			sValue << realValues_.size();
			xMyGenotype.addAttribute("size", sValue.str().c_str());

			sValue.str("");
			for(uint iVar = 0; iVar < realValues_.size(); iVar++)
			sValue << "\t" << realValues_[iVar];
			xMyGenotype.addText(sValue.str().c_str());
		}

		// read from XMLNode
		// mandatory if running parallel ECF or reading population from a milestone file
		void read(XMLNode& xMyGenotype)
		{
			XMLCSTR values = xMyGenotype.getText();
			std::stringstream sValues;
			sValues << values;

			for(uint iVar = 0; iVar < realValues_.size(); iVar++)
				sValues >> realValues_[iVar];
		}
	};
	typedef boost::shared_ptr<MyGenotype> MyGenotypeP;

 

.. _addalgorithm:

2. Adding an algorithm
~~~~~~~~~~~~~~~~~~~~~~

Algoritm must inherit from the abstract *Algorithm* class (see
'Algorithm.h'). Algorithm may be genotype independent or may define a
mandatory genotype structure. It may use genotype's crossover and
mutation operators or may define its own (algorithm specific) operators.
As an example, see below or e.g. 'AlgSteadyStateTournament.cpp' or
'AlgRouletteWheel.cpp'.

An example: the following algorithm illustrates the basic mechanisms and
contains **HOW TO** code snippets (note that the code is longer than it
could be because of optional explanatory parts). The application of a
new algorithm is done in 3 steps:

Step 1: First, write an algorithm class:

.. code-block:: cpp

	class MyAlg : public Algorithm
	{
		protected:

		// declare all available selection operators (not all get used)
		SelFitnessProportionalOpP selFitOp_;
		SelRandomOpP selRandomOp_;
		SelBestOpP selBestOp_;
		SelWorstOpP selWorstOp_;
		// what individual to replace (worst or random)
		bool replaceWorst_;

		public:

		// mandatory: define name, construct selection operators
		MyAlg()
		{
			// the algorithm name will be used in config file (see below)
			name_ = "MyAlg";
			selFitOp_ = (SelFitnessProportionalOpP) (new SelFitnessProportionalOp);
			selRandomOp_ = (SelRandomOpP) (new SelRandomOp);
			selBestOp_ = (SelBestOpP) (new SelBestOp);
			selWorstOp_ = (SelWorstOpP) (new SelWorstOp);
		}


		// optional: register any parameters
		void registerParameters(StateP state)
		{
			// HOW TO: define a parameter
			// string parameter, options: random, worst
			registerParameter(state, "replace", (voidP) (new std::string("random")), ECF::STRING);
		}


		// optional: initialize components, read parameters
		bool initialize(StateP state)
		{
			// selection operators must be initialized if used!
			selFitOp_->initialize(state);
			// optional: set ratio between the best and the worst individual's selection probability
			selFitOp_->setSelPressure(10);
			// if the ratio is < 1, the selection favours worse over better individuals
			//selFitOp_->setSelPressure(0.1);
			selRandomOp_->initialize(state);
			selBestOp_->initialize(state);
			selWorstOp_->initialize(state);

			// HOW TO: read a parameter value
			// get parameter, decide what to replace
			voidP par = getParameterValue(state, "replace");
			std::string replace = *((std::string*) par.get());
			replaceWorst_ = false;
			if(replace == "worst")
				 replaceWorst_ = true;

			// HOW TO: check if genotype is of a specific kind
			// suppose we only accept FloatingPoint
			FloatingPointP flp (new FloatingPoint::FloatingPoint);
			if(state->getGenotypes()[0]->getName() != flp->getName()) {
				ECF_LOG_ERROR(state, "Error: this algorithm accepts only a single FloatingPoint genotype!");
				throw ("");
			}

			// HOW TO: read the dimension and domain boundaries
			voidP sptr = state->getGenotypes()[0]->getParameterValue(state, "dimension");
			uint numDimension = *((uint*) sptr.get());
			voidP lBound = state->getGenotypes()[0]->getParameterValue(state, "lbound");
			double lbound = *((double*) lBound.get());
			voidP uBound = state->getGenotypes()[0]->getParameterValue(state, "ubound");
			double ubound = *((double*) uBound.get());

			// HOW TO: add another genotype in all individuals (if algorithm requires)
			// new FloatingPoint genotype with same parameters
			FloatingPointP fp (static_cast<FloatingPoint::FloatingPoint*> (state->getGenotypes()[0]->copy()));
			//state->setGenotype(fp);
			fp->setParameterValue(state, "dimension", (voidP) new uint(numDimension));
			fp->setParameterValue(state, "lbound", (voidP) new double(lbound));
			fp->setParameterValue(state, "ubound", (voidP) new double(ubound));

			// HOW TO: read population (local deme) size
			voidP psizep = state->getRegistry()->getEntry("population.size"); 
			uint popSize = *((uint*) psizep.get()); 

			return true;
		}


		// mandatory: perform single 'generation' (however the algorithm defines it)
		bool advanceGeneration(StateP state, DemeP deme)
		{
			// HOW TO: select parents
			IndividualP first = selFitOp_->select(*deme);
			IndividualP second = selBestOp_->select(*deme);

			// select child (random or worst)
			IndividualP child;
			if(replaceWorst_)
				child = selWorstOp_->select(*deme);
			else
				child = selRandomOp_->select(*deme);

			// HOW TO: cross two individuals
			mate(first, second, child);

			// HOW TO: mutate an individual
			// mutation probability defined in Registry!
			mutate(child);              
			// to explicitly mutate an individual:
			//mutation_->mutate(child);

			// HOW TO: evaluate an individual
			evaluate(child);

			// HOW TO: create a trial individual (e.g. a copy of an existing individual)
			IndividualP trial = (IndividualP) deme->at(0)->copy();

			// HOW TO: access individual data
			// get FloatingPoint genotype from individual
			FloatingPointP fp = boost::static_pointer_cast<FloatingPoint::FloatingPoint> (trial->getGenotype(0));
			// or use ordinary pointers:
			//FloatingPoint::FloatingPoint* fp = static_cast<FloatingPoint::FloatingPoint*> (trial->getGenotype().get());

			// HOW TO: change individual data
			fp->realValue[0] = 3.14;

			// HOW TO: replace an individual in deme
			// evaluate and compare with another individual
			evaluate(trial);
			if(trial->fitness->isBetterThan(child->fitness))
				// replace first with second:
				replaceWith(child, trial);

			// some other helper functions (see existing algorithms):
			// copy, replaceWith, removeFrom, isMember

			return true;
		}
	};
	typedef boost::shared_ptr<MyAlg> MyAlgP;

Step 2: then, a new algorithm can be added to ECF with
'State::addAlgorithm(AlgorithmP)' function, invoked before
'State::initialize' in your main() function:

.. code-block:: cpp

	NewAlgorithmP alg = (NewAlgorithmP) new NewAlgorithm;
	state->addAlgorithm(alg);

Step 3: finally, the algorithm will only get used if specified in the
configuration, e.g.:

.. code-block:: xml

   <Algorithm>
           <MyAlg>
                   <!-- optional parameter -->
                   <Entry key="replace">worst</Entry>
           </MyAlg>
   </Algorithm>


 
.. _addoperator:

3. Adding an operator
~~~~~~~~~~~~~~~~~~~~~

A new general purpose operator may be added with
'State::addOperator(OperatorP)' function:

.. code-block:: cpp

	MyOpP myOp = (MyOpP) new MyOp;
	state->addOperator(myOp);

| Operator must inherit from the abstract *Operator* class (see
  'Operator.h'). The operator will be used only if its 'initialize'
  method returns TRUE (which is the default if not reimplemented).
| Currently every operator defined in this way is called *once each
  generation* (before the generation ends). Further development will
  allow operators to be called within the algorithm (i.e. before/after
  mutation, evaluation, crossover)

An example: the following operator simply stops the evolution if the
generation number is specified in the configuration file (e.g. with
<Entry key="my.stopgen">10</Entry>):

.. code-block:: cpp

	class MyOp : public Operator
	{
		public:
		uint myStopGen_;

		// optional: register any parameters
		void registerParameters(StateP state)
		{
			state->getRegistry()->registerEntry("my.stopgen", (voidP) (new uint(0)), ECF::UINT);
		}

		// optional: initialize (with parameter check)
		bool initialize(StateP state)
		{
			// if parameter not defined in config, return false (inactive operator)
			if(!state->getRegistry()->isModified("my.stopgen"))
				return false;
			// otherwise, read param. value and return true (active operator)
			voidP sptr = state->getRegistry()->getEntry("my.stopgen");
			myStopGen_ = *((uint*) sptr.get());
			return true;
		}

		// mandatory: actual operation
		bool operate(StateP state)
		{
			// read generation no, stop if needed
			if(state->getGenerationNo() == myStopGen_)
				state->setTerminateCond();
			return true;
		}
	};
	typedef boost::shared_ptr<MyOp> MyOpP;

| 

**TODO**: setInitMethod, addMutationOp, addCrossoverOp, addTerminationOp

| 

