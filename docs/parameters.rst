
4. Using the parameters
=======================

A file with complete parameter list may be obtained by running the ECF
executable with '-pardump <file>' option, e.g. '<executable> -pardump
par.txt'.

#. The configuration file 
#. The Algorithm section 
#. The Registry section 
#. Choosing crossover and mutation 
#. Using multiple genotypes 
#. Saving and restoring evolution state
#. Performing multiple ECF runs (batch mode) 
#. User defined parameters 
#. A complete parameter list 


1. The configuration file
-------------------------

The name of the configuration file is given as a command line argument.

The file consists of three XML blocks inside the ECF block: Algorithm,
Genotype and Registry. **Only the Genotype has to be stated** in the
configuration. An example structure:

.. code-block:: xml

   <ECF>
     <Algorithm>
       ...
     </Algorithm>

     <Genotype>
       ...
     </Genotype>

     <Registry>
       ...
     </Registry>
   </ECF>

If no other parameters are stated, ECF uses the following setting:

-  algorithm: SteadyStateTournament (tournament size 3)
-  population size: 100
-  individual mutation probability: 0.3
-  termination: 50 consecutive generations without improvement of the
   best individual ("term.stagnation" parameter)

2. The Algorithm section
------------------------

Only one algorithm should be stated in the configuration file (otherwise
the first one is used). Currently supported sequential algorithms and
their parameters are:

.. code-block:: xml

   <Algorithm>     <!-- should be only one inside -->
       <SteadyStateTournament>
           <Entry key="tsize">3</Entry>
           <!-- tournament size (individuals selected randomly, worst one eliminated) -->
       </SteadyStateTournament>
       <ArtificialBeeColony>
           <Entry key="elitism">1</Entry>
           <!-- Elitism: the current best food source is preserved (default: 1) -->
           <Entry key="limit">300</Entry>
           <!-- Maximum number of cycles for each individual (default: 300) -->
       </ArtificialBeeColony>
       <Clonalg>
           <Entry key="beta">1</Entry>
           <!-- number of clones (percentage) for every antibody (default: 1.0) -->
           <Entry key="c">0.2</Entry>
           <!-- mutation rate (default: 0.2) -->
           <Entry key="cloningVersion">proportional</Entry>
           <!-- cloning version, static or proportional (default: proportional) -->
           <Entry key="d">0</Entry>
           <!-- fraction of population regenerated every generation (default: 0) -->
           <Entry key="n">100</Entry>
           <!-- number of antibodies cloned every generation (default: 100) -->
           <Entry key="selectionScheme">CLONALG1</Entry>
           <!-- which selection scheme to use, CLONALG1 or CLONALG2 (default: CLONALG1) -->
       </Clonalg>
       <DifferentialEvolution>
           <Entry key="CR">0.9</Entry>
           <!-- crossover rate -->
           <Entry key="F">1</Entry>
           <!-- scaling constant -->
           <Entry key="bounded">0</Entry>
           <!-- should the algorithm respect the bounds defined in the genotype or not (default: 0) -->
       </DifferentialEvolution>
       <Elimination>
           <Entry key="gengap">0.6</Entry>
           <!-- generation gap (percentage of population to be eliminated) -->
           <Entry key="selpressure">10</Entry>
           <!-- selection pressure: how much is the worst individual 'worse' than the best -->
       </Elimination>
       <EvolutionStrategy>
           <Entry key="lambda">4</Entry>
           <!-- number of offspring created in each iteration (default: 4) -->
           <Entry key="mu">1</Entry>
           <!-- the size of parent population (default: 1) -->
           <Entry key="rho">1</Entry>
           <!-- number of parents used to create an offspring; may be 1 or 2 (default: 1) -->
           <Entry key="selection">plus</Entry>
           <!-- selection scheme: "plus", uses both parents and offspring) or "comma", uses just offspring (default: plus) -->
       </EvolutionStrategy>
       <GeneticAnnealing>
           <Entry key="coolingfactor">0.7</Entry>
           <!-- simulated annealing cooling factor -->
           <Entry key="elitism">0</Entry>
           <!-- is the best individual preserved -->
           <Entry key="energybank">200</Entry>
           <!-- total starting energy bank (fitness dependant) -->
       </GeneticAnnealing>
       <OptIA>
           <Entry key="c">0.2</Entry>
           <!-- mutation rate (default: 0.2) -->
           <Entry key="dup">5</Entry>
           <!-- number of clones for each individual in clone population (dafault: 5) -->
           <Entry key="elitism">0</Entry>
           <!-- use elitism (default: 0) -->
           <Entry key="tauB">100</Entry>
           <!-- maximum number of generations to keep an individual without improvement (default: 100) -->
       </OptIA>
       <ParticleSwarmOptimization>
           <Entry key="bounded">0</Entry>
           <!-- should the algorithm respect the bounds defined in the genotype or not (default: 0) -->
           <Entry key="maxVelocity">10</Entry>
           <!-- max particle velocity -->
           <Entry key="weight">0.8</Entry>
           <!-- initial inertia weight (either constant or time dependant) -->
           <Entry key="weightType">0</Entry>
           <!-- weight type update: 0 - constant, 1 - time dependant (based on max generations) -->
       </ParticleSwarmOptimization>
       <RandomSearch/>
       <RouletteWheel>
           <Entry key="crxprob">0.5</Entry>
           <!-- crossover rate -->
           <Entry key="selpressure">10</Entry>
           <!-- selection pressure: how much is the best individual 'better' than the worst -->
       </RouletteWheel>
   </Algorithm>

If the algorithm block is not stated, the default is used:
*SteadyStateTournament*.

 

3. The Registry section
-----------------------

The Registry collects all the parameters that are not a part of the
algorithm or the genotype. Currently supported parameters and their
default values are stated below. If the default value is "none", then
the parameter is not used unless stated in the configuration file!

.. code-block:: xml

   <Registry>
       <!-- population -->
       <Entry key="randomizer.seed">0</Entry> <!-- 0 uses time(NULL) (default: 0) -->
       <Entry key="population.size">100</Entry> <!-- number of individuals (default: 100) -->
       <Entry key="population.demes">1</Entry> <!-- number of demes (default: 1) -->
       <Entry key="migration.freq">10</Entry> <!-- individuals are exchanged each 'freq' generations (default: none) -->
       <Entry key="migration.number">5</Entry> <!-- number of individuals to be sent to another deme (default: 1) -->

       <!-- operators -->
       <Entry key="mutation.indprob">0.3</Entry> <!-- individual mutation probability (regardless of the algorithm!) (default: 0.3) -->
       <Entry key="mutation.genotypes">random</Entry> <!-- if there are multiple genotypes, which to mutate? 'random': a random one, 'all': mutate all (default: random) -->
       <Entry key="mutation.protected"></Entry> <!-- indexes of genotypes that are excluded (protected) from mutation (default: none) -->
       <Entry key="crossover.genotypes">random</Entry> <!-- if there are multiple genotypes, which to cross? 'random': a random pair, 'all': all pairs (default: random) -->
       <Entry key="crossover.protected"></Entry> <!-- indexes of genotypes that are excluded (protected) from crossover (default: none) -->

       <!-- termination conditions -->
       <Entry key="term.stagnation">100</Entry> <!-- max number of consecutive generations without improvement (default: 5000 / pop_size) -->
       <Entry key="term.maxgen">10</Entry> <!-- max number of generations (default: none) -->
       <Entry key="term.maxtime">1000</Entry> <!-- max number of seconds to run (default: none) -->
       <Entry key="term.fitnessval">1</Entry> <!-- fitness value for termination (default: none) -->
       <Entry key="term.eval">9999</Entry> <!-- max number of fitness evaluations (default: none) -->

       <!-- logging and checkpointing -->
       <Entry key="log.level">3</Entry> <!-- log level; valid values are 1 (minimal) to 5 (verbose) (default: 3) -->
       <Entry key="log.filename">log.txt</Entry> <!-- log filename (default: none) -->
       <Entry key="log.frequency">1</Entry> <!-- log only every 'frequency' generations (default: 1) -->
       <Entry key="milestone.filename">out.txt</Entry> <!-- milestone file (if stated) stores all the population (default: none) -->
       <Entry key="milestone.interval">0</Entry> <!-- milestone saving interval in generations; 0: save only at the end (default: 0) -->

       <!-- multiple runs (batch mode) -->
       <Entry key="batch.repeats">1</Entry> <!-- number of independent runs to perform (default: 1) -->
       <Entry key="batch.singlemilestone">0</Entry> <!-- use only one milestone file for all the runs (1) or one for each run (0) (default: 0) -->
       <Entry key="batch.statsfile">stats.txt</Entry> <!-- output batch end of run stats in a single file (default: none) -->

       <!-- parallel ECF parameters: -->
       <Entry key="parallel.type">eval</Entry> <!-- implicit parallelization method: eval - evaluation, mut - mutation + eval (default: none) -->
       <Entry key="parallel.sync">0</Entry> <!-- implicit parallelization synchronicity: 0 - async, 1 - sync (default: none) -->
       <Entry key="parallel.jobsize">10</Entry> <!-- implicit parallelization jobsize (individuals per job) (default: none) -->
   </Registry>

 

4. Choosing crossover and mutation
----------------------------------

Each genotype may define more than one crossover and/or mutation
operator. If no parameters are given, the operator is selected randomly
each time crossover or mutation is performed. Otherwise, the user may
specify the probability of using a single or more operators.

An example: if there are two mutation operators and one is given the
probability of 0.3 and the other of 0.7, they will be used in 30% and
70% of the cases, respectively. If the probability is defined only for
**one of them** (with *any* probability value), then only that one will
be used.

Currently supported genotypes and associated operators are:

.. code-block:: xml

   <Binary>
       <Entry key="crx.halfuniform">0</Entry>
       <Entry key="crx.nongeometric">0</Entry>
       <Entry key="crx.onepoint">0</Entry>
       <Entry key="crx.randomrespectful">0</Entry>
       <Entry key="crx.reducedsurrogate">0</Entry>
       <Entry key="crx.segmented">0</Entry>
       <Entry key="crx.shuffle">0</Entry>
       <Entry key="crx.twopoint">0</Entry>
       <Entry key="crx.uniform">0</Entry>
       <Entry key="dimension">1</Entry>
       <!-- number of real valued variables (mandatory) -->
       <Entry key="lbound">0</Entry>
       <!-- lower bound for each variable (mandatory) -->
       <Entry key="mut.mix">0</Entry>
       <Entry key="mut.simple">0</Entry>
       <Entry key="mut.simple.bitprob">0.001</Entry>
       <Entry key="precision">1</Entry>
       <!-- number of digits after the decimal point (mandatory) -->
       <Entry key="rounding">0</Entry>
       <!-- should the real value be rounded to the 'precision'-th decimal (default: no) -->
       <Entry key="ubound">10</Entry>
       <!-- upper bound for each variable (mandatory) -->
   </Binary>

   <BitString>
       <Entry key="crx.onepoint">0</Entry>
       <Entry key="crx.uniform">0</Entry>
       <Entry key="mut.mix">0</Entry>
       <Entry key="mut.simple">0</Entry>
       <Entry key="mut.simple.bitprob">0.001</Entry>
       <Entry key="size">10</Entry>
       <!-- number of bits (mandatory) -->
   </BitString>

   <FloatingPoint>
       <Entry key="crx.arithmetic">0</Entry>
       <Entry key="crx.arithmeticsimple">0</Entry>
       <Entry key="crx.arithmeticsingle">0</Entry>
       <Entry key="crx.average">0</Entry>
       <Entry key="crx.bga">0</Entry>
       <Entry key="crx.blxalpha">0</Entry>
       <Entry key="crx.blxalphabeta">0</Entry>
       <Entry key="crx.discrete">0</Entry>
       <Entry key="crx.flat">0</Entry>
       <Entry key="crx.heuristic">0</Entry>
       <Entry key="crx.local">0</Entry>
       <Entry key="crx.onepoint">0</Entry>
       <Entry key="crx.sbx">0</Entry>
       <Entry key="dimension">1</Entry>
       <!-- number of real valued variables (mandatory) -->
       <Entry key="lbound">-10</Entry>
       <!-- lower bound for each variable (mandatory) -->
       <Entry key="mut.simple">0</Entry>
       <Entry key="ubound">10</Entry>
       <!-- upper bound for each variable (mandatory) -->
   </FloatingPoint>

   <Permutation>
       <Entry key="crx.COSA">0</Entry>
       <Entry key="crx.DPX">0</Entry>
       <Entry key="crx.OBX">0</Entry>
       <Entry key="crx.OPX">0</Entry>
       <Entry key="crx.OX">0</Entry>
       <Entry key="crx.OX2">0</Entry>
       <Entry key="crx.PBX">0</Entry>
       <Entry key="crx.PMX">0</Entry>
       <Entry key="crx.SPX">0</Entry>
       <Entry key="crx.ULX">0</Entry>
       <Entry key="crx.UPMX">0</Entry>
       <Entry key="crx.cyclic">0</Entry>
       <Entry key="crx.cyclic2">0</Entry>
       <Entry key="mut.ins">0</Entry>
       <Entry key="mut.inv">0</Entry>
       <Entry key="mut.toggle">0</Entry>
       <Entry key="size">1</Entry>
       <!-- genotype size: number of indices (mandatory) -->
   </Permutation>

   <Tree>
       <Entry key="crx.context">0</Entry>
       <Entry key="crx.onepoint">0</Entry>
       <Entry key="crx.simple">0</Entry>
       <Entry key="crx.simple.functionprob">0.9</Entry>
       <Entry key="crx.sizefair">0</Entry>
       <Entry key="crx.uniform">0</Entry>
       <Entry key="functionset"></Entry>
       <!-- set of functional tree elements (mandatory) -->
       <Entry key="initmaxdepth">5</Entry>
       <!-- maximum initial tree depth (default: 5) -->
       <Entry key="initmindepth">1</Entry>
       <!-- minimum initial tree depth (default: 1) -->
       <Entry key="maxdepth">5</Entry>
       <!-- maximum tree depth (default: 5) -->
       <Entry key="mindepth">1</Entry>
       <!-- minimum tree depth (default: 1) -->
       <Entry key="mut.gauss">0</Entry>
       <Entry key="mut.hoist">0</Entry>
       <Entry key="mut.nodecomplement">0</Entry>
       <Entry key="mut.nodereplace">0</Entry>
       <Entry key="mut.permutation">0</Entry>
       <Entry key="mut.shrink">0</Entry>
       <Entry key="mut.subtree">0</Entry>
       <Entry key="terminalset"></Entry>
       <!-- set of terminal tree elements (mandatory) -->
   </Tree>

*4.1 Protected genotypes*
~~~~~~~~~~~~~~~~~~~~~~~~~

Some genotypes in the individual may also be *protected*, i.e. excluded
from either mutation and/or crossover (this is useful if you need to
store additional information in the individual, but don't want it to
evolve). To do so, just state **the indexes** of protected genotypes in
parameters "crossover.protected" or "mutation.protected", e.g.

.. code-block:: xml

    <Entry key="mutation.protected">1 2 </Entry>
    <Entry key="crossover.protected">0 2 </Entry>

 

5. Using multiple genotypes
---------------------------

In case you want to use more than one genotype (say multiple trees or
Binary's), you only have to state them in the configuration file, e.g.:

.. code-block:: xml

   <Genotype>
           <Binary>             <!-- genotype with index 0 -->
                  <Entry key="lbound">0</Entry>
                  <Entry key="ubound">50</Entry>
                  <Entry key="precision">1</Entry>
                  <Entry key="dimension">3</Entry>
           </Binary>
           <Binary>             <!-- genotype with index 1 -->
                  <Entry key="lbound">-10</Entry>
                  <Entry key="ubound">10</Entry>
                  <Entry key="precision">3</Entry>
                  <Entry key="dimension">2</Entry>
           </Binary>
           <Permutation>        <!-- genotype with index 2 -->
                  <Entry key="size">30</Entry>
           </Permutation>
   </Genotype>

Of course, the evaluation function should take this into account and
read all the relevant genotypes.

 

6. Saving and restoring the evolution state
-------------------------------------------

With "milestone.filename" parameter stated in the configuration, ECF
will save the population and current evolution state in the milestone
every "milestone.interval" generations. If the interval is 0 (or the
parameter is not stated), the milestone will be saved at the end of the
run.

| Restoration of the saved population is performed simply by running the
  ECF executable with the milestone file as its  command line argument,
  e.g.:  **./<executable> <milestone_file>**.
| The evolution process will then continue, depending on the active
  termination criteria. You can manually edit the Registry section in
  the milestone file and change the termination conditions (e.g.
  increase the value of "term.maxgen" parameter).

 

**7. Performing multiple runs (batch mode**)
--------------------------------------------

ECF can perform multiple runs in a sequence (e.g. for statistical
analysis). Multiple runs are controlled with the following parameters:

.. code-block:: xml

    <Entry key="batch.repeats">1</Entry> <!-- number of independent runs to perform (default: 1) -->
    <Entry key="batch.singlemilestone">0</Entry> <!-- use only one milestone file for all the runs (1) or one for each run (0) (default: 0) -->
    <Entry key="batch.statsfile">stats.txt</Entry> <!-- output batch end of run stats in a single file (default: none) -->

| A separate log file is generated for every run with the run number as
  extension: <log>_01, <log>_02 etc. A separate milestone is also saved,
  unless the "singlemilestone" option is set to 1.
| Note that if the batch run is interrupted before all the runs are
  performed, it can also be continued with the last milestone file (ECF
  will perform the remaining runs).

If the "batch.statsfile" option is included, the final fitness
statistics from the end of every run will be output in the file, e.g.:

| runId    fit_min    fit_max    fit_avg    fit_std
| \*        \*          \*          \*          \*
| ...

**In implementation**: changing parameter values in different runs

 

8. User defined parameters
--------------------------

It is possible to define parameters programatically so they can be used
in the configuration file. Suppose you want to load the symbolic
regression data (see the :doc:`example in tutorial
section <tutorial>`) from the configuration file instead of
generating it in the *initialize* function (that way you can train on
different data without recompilation). Say you want the parameters to
look like this:

.. code-block:: xml

   <Entry key="domain">0 2 4 6 8 10</Entry>
   <Entry key="codomain">1 3 5 7 9 11</Entry>

(Let's ignore the fact that it is probably better to define only one
text parameter as a filename of the separate data file, since the data
may be large.) In order to use these parameters, the evaluation operator
needs to *register* them. Parameter registratoin is performed simply by
implementing the virtual function *registerParameters* that gets called
once before the *initialize* function:

.. code-block:: cpp

   void SymbRegEvalOp::registerParameters(StateP state)
   {
        state->getRegistry()->registerEntry("domain", (voidP) (new std::string), ECF::STRING);
        state->getRegistry()->registerEntry("codomain", (voidP) (new std::string), ECF::STRING);
   }

In the registration, you need to reserve the space for parameter storage
and tell the framework what type is it (the types can be ECF::STRING,
ECF::UINT, ECF::INT, ECF::DOUBLE, ECF::FLOAT, ECF::CHAR). After the
parameters are read from the configuration file, you can interpret them
in *initialize* function:

.. code-block:: cpp

   bool SymbRegEvalOp::initialize(StateP state)
   {
           // check if the parameters are stated (used) in the conf. file
           // if not, we return false so the initialization fails
           if(!state->getRegistry()->isModified("domain") || !state->getRegistry()->isModified("codomain"))
                   return false;

           double datum;
           std::stringstream ss;

           voidP sptr = state->getRegistry()->getEntry("domain"); // get parameter value
           ss << *((std::string*) sptr.get());                    // convert from voidP to user defined type
           while(ss >> datum) {                                   // read all the data from string
                   domain.push_back(datum);
           }
           ss.str(""); ss.clear();                                // reset stringstream object
           sptr = state->getRegistry()->getEntry("codomain");     // get codomain parameter
           ss << *((std::string*) sptr.get());
           while(ss >> datum) {                                   // read values
                   codomain.push_back(datum);
           }

           if(domain.size() != codomain.size())                   // if the parameters are ill defined, return false
                   return false;
           nSamples = (uint) domain.size();
           return true;
   }

And the *evaluate* function stays the same. The choice of parameter
names must be such that all parameter names are unique (otherwise the
registration will fail).

 

Setting the parameters programmatically
---------------------------------------

**todo**: setParameter, setGenotype, setAlgorithm

 

9. A complete parameter list
----------------------------

A file with complete parameter list may be obtained by running the ECF
executable with '-pardump <file>' option, e.g. '<executable> -pardump
par.txt'.

.. code-block:: xml

   <ECF>
   <Algorithm>     <!-- should be only one inside -->
     <SteadyStateTournament>
           <Entry key="tsize">3</Entry>
           <!-- tournament size (individuals selected randomly, worst one eliminated) -->
       </SteadyStateTournament>
       <ArtificialBeeColony>
           <Entry key="elitism">1</Entry>
           <!-- Elitism: the current best food source is preserved (default: 1) -->
           <Entry key="limit">300</Entry>
           <!-- Maximum number of cycles for each individual (default: 300) -->
       </ArtificialBeeColony>
       <Clonalg>
           <Entry key="beta">1</Entry>
           <!-- number of clones (percentage) for every antibody (default: 1.0) -->
           <Entry key="c">0.2</Entry>
           <!-- mutation rate (default: 0.2) -->
           <Entry key="cloningVersion">proportional</Entry>
           <!-- cloning version, static or proportional (default: proportional) -->
           <Entry key="d">0</Entry>
           <!-- fraction of population regenerated every generation (default: 0) -->
           <Entry key="n">100</Entry>
           <!-- number of antibodies cloned every generation (default: 100) -->
           <Entry key="selectionScheme">CLONALG1</Entry>
           <!-- which selection scheme to use, CLONALG1 or CLONALG2 (default: CLONALG1) -->
       </Clonalg>
       <DifferentialEvolution>
           <Entry key="CR">0.9</Entry>
           <!-- crossover rate -->
           <Entry key="F">1</Entry>
           <!-- scaling constant -->
           <Entry key="bounded">0</Entry>
           <!-- should the algorithm respect the bounds defined in the genotype or not (default: 0) -->
       </DifferentialEvolution>
       <Elimination>
           <Entry key="gengap">0.6</Entry>
           <!-- generation gap (percentage of population to be eliminated) -->
           <Entry key="selpressure">10</Entry>
           <!-- selection pressure: how much is the worst individual 'worse' than the best -->
       </Elimination>
       <EvolutionStrategy>
           <Entry key="lambda">4</Entry>
           <!-- number of offspring created in each iteration (default: 4) -->
           <Entry key="mu">1</Entry>
           <!-- the size of parent population (default: 1) -->
           <Entry key="rho">1</Entry>
           <!-- number of parents used to create an offspring; may be 1 or 2 (default: 1) -->
           <Entry key="selection">plus</Entry>
           <!-- selection scheme: "plus", uses both parents and offspring) or "comma", uses just offspring (default: plus) -->
       </EvolutionStrategy>
       <GeneticAnnealing>
           <Entry key="coolingfactor">0.7</Entry>
           <!-- simulated annealing cooling factor -->
           <Entry key="elitism">0</Entry>
           <!-- is the best individual preserved -->
           <Entry key="energybank">200</Entry>
           <!-- total starting energy bank (fitness dependant) -->
       </GeneticAnnealing>
       <OptIA>
           <Entry key="c">0.2</Entry>
           <!-- mutation rate (default: 0.2) -->
           <Entry key="dup">5</Entry>
           <!-- number of clones for each individual in clone population (dafault: 5) -->
           <Entry key="elitism">0</Entry>
           <!-- use elitism (default: 0) -->
           <Entry key="tauB">100</Entry>
           <!-- maximum number of generations to keep an individual without improvement (default: 100) -->
       </OptIA>
       <ParticleSwarmOptimization>
           <Entry key="bounded">0</Entry>
           <!-- should the algorithm respect the bounds defined in the genotype or not (default: 0) -->
           <Entry key="maxVelocity">10</Entry>
           <!-- max particle velocity -->
           <Entry key="weight">0.8</Entry>
           <!-- initial inertia weight (either constant or time dependant) -->
           <Entry key="weightType">0</Entry>
           <!-- weight type update: 0 - constant, 1 - time dependant (based on max generations) -->
       </ParticleSwarmOptimization>
       <RandomSearch/>
       <RouletteWheel>
           <Entry key="crxprob">0.5</Entry>
           <!-- crossover rate -->
           <Entry key="selpressure">10</Entry>
           <!-- selection pressure: how much is the best individual 'better' than the worst -->
       </RouletteWheel>
   </Algorithm>


   <Genotype> <!-- mandatory parameters are commented -->

   <Binary>
       <Entry key="crx.halfuniform">0</Entry>
       <Entry key="crx.nongeometric">0</Entry>
       <Entry key="crx.onepoint">0</Entry>
       <Entry key="crx.randomrespectful">0</Entry>
       <Entry key="crx.reducedsurrogate">0</Entry>
       <Entry key="crx.segmented">0</Entry>
       <Entry key="crx.shuffle">0</Entry>
       <Entry key="crx.twopoint">0</Entry>
       <Entry key="crx.uniform">0</Entry>
       <Entry key="dimension">1</Entry>
       <!-- number of real valued variables (mandatory) -->
       <Entry key="lbound">0</Entry>
       <!-- lower bound for each variable (mandatory) -->
       <Entry key="mut.mix">0</Entry>
       <Entry key="mut.simple">0</Entry>
       <Entry key="mut.simple.bitprob">0.001</Entry>
       <Entry key="precision">1</Entry>
       <!-- number of digits after the decimal point (mandatory) -->
       <Entry key="rounding">0</Entry>
       <!-- should the real value be rounded to the 'precision'-th decimal (default: no) -->
       <Entry key="ubound">10</Entry>
       <!-- upper bound for each variable (mandatory) -->
   </Binary>

   <BitString>
       <Entry key="crx.onepoint">0</Entry>
       <Entry key="crx.uniform">0</Entry>
       <Entry key="mut.mix">0</Entry>
       <Entry key="mut.simple">0</Entry>
       <Entry key="mut.simple.bitprob">0.001</Entry>
       <Entry key="size">10</Entry>
       <!-- number of bits (mandatory) -->
   </BitString>

   <FloatingPoint>
       <Entry key="crx.arithmetic">0</Entry>
       <Entry key="crx.arithmeticsimple">0</Entry>
       <Entry key="crx.arithmeticsingle">0</Entry>
       <Entry key="crx.average">0</Entry>
       <Entry key="crx.bga">0</Entry>
       <Entry key="crx.blxalpha">0</Entry>
       <Entry key="crx.blxalphabeta">0</Entry>
       <Entry key="crx.discrete">0</Entry>
       <Entry key="crx.flat">0</Entry>
       <Entry key="crx.heuristic">0</Entry>
       <Entry key="crx.local">0</Entry>
       <Entry key="crx.onepoint">0</Entry>
       <Entry key="crx.sbx">0</Entry>
       <Entry key="dimension">1</Entry>
       <!-- number of real valued variables (mandatory) -->
       <Entry key="lbound">-10</Entry>
       <!-- lower bound for each variable (mandatory) -->
       <Entry key="mut.simple">0</Entry>
       <Entry key="ubound">10</Entry>
       <!-- upper bound for each variable (mandatory) -->
   </FloatingPoint>

   <Permutation>
       <Entry key="crx.COSA">0</Entry>
       <Entry key="crx.DPX">0</Entry>
       <Entry key="crx.OBX">0</Entry>
       <Entry key="crx.OPX">0</Entry>
       <Entry key="crx.OX">0</Entry>
       <Entry key="crx.OX2">0</Entry>
       <Entry key="crx.PBX">0</Entry>
       <Entry key="crx.PMX">0</Entry>
       <Entry key="crx.SPX">0</Entry>
       <Entry key="crx.ULX">0</Entry>
       <Entry key="crx.UPMX">0</Entry>
       <Entry key="crx.cyclic">0</Entry>
       <Entry key="crx.cyclic2">0</Entry>
       <Entry key="mut.ins">0</Entry>
       <Entry key="mut.inv">0</Entry>
       <Entry key="mut.toggle">0</Entry>
       <Entry key="size">1</Entry>
       <!-- genotype size: number of indices (mandatory) -->
   </Permutation>

   <Tree>
       <Entry key="crx.context">0</Entry>
       <Entry key="crx.onepoint">0</Entry>
       <Entry key="crx.simple">0</Entry>
       <Entry key="crx.simple.functionprob">0.9</Entry>
       <Entry key="crx.sizefair">0</Entry>
       <Entry key="crx.uniform">0</Entry>
       <Entry key="functionset"></Entry>
       <!-- set of functional tree elements (mandatory) -->
       <Entry key="initmaxdepth">5</Entry>
       <!-- maximum initial tree depth (default: 5) -->
       <Entry key="initmindepth">1</Entry>
       <!-- minimum initial tree depth (default: 1) -->
       <Entry key="maxdepth">5</Entry>
       <!-- maximum tree depth (default: 5) -->
       <Entry key="mindepth">1</Entry>
       <!-- minimum tree depth (default: 1) -->
       <Entry key="mut.gauss">0</Entry>
       <Entry key="mut.hoist">0</Entry>
       <Entry key="mut.nodecomplement">0</Entry>
       <Entry key="mut.nodereplace">0</Entry>
       <Entry key="mut.permutation">0</Entry>
       <Entry key="mut.shrink">0</Entry>
       <Entry key="mut.subtree">0</Entry>
       <Entry key="terminalset"></Entry>
       <!-- set of terminal tree elements (mandatory) -->
   </Tree>

   </Genotype>


   <Registry>
       <Entry key="batch.repeats">0</Entry>
       <!-- number of independent runs to perform (default: 1) -->
       <Entry key="batch.singlemilestone">0</Entry>
       <!-- use only one milestone file for all the batch runs (1) or one for each run (0) (default: 0) -->
       <Entry key="batch.statsfile"></Entry>
       <!-- output batch end of run stats in a single file (default: none) -->
       <Entry key="beta">0.25</Entry>
       <Entry key="crossover.genotypes">random</Entry>
       <!-- if there are multiple genotypes, which to cross? 'random': a random pair, 'all': all pairs (default: random) -->
       <Entry key="crossover.protected"></Entry>
       <!-- indexes of genotypes (separated by spaces) that are excluded (protected) from crossover (default: none) -->
       <Entry key="log.filename"></Entry>
       <!-- log filename (default: none) -->
       <Entry key="log.frequency">1</Entry>
       <!-- log only every 'frequency' generations (default: 1) -->
       <Entry key="log.level">3</Entry>
       <!-- log level; valid values are 1 (minimal) to 5 (verbose) (default: 3) -->
       <Entry key="migration.freq">0</Entry>
       <!-- individuals are exchanged each 'freq' generations (default: none) -->
       <Entry key="migration.number">1</Entry>
       <!-- number of individuals to be sent to another deme (default: 1) -->
       <Entry key="milestone.elapsedtime_">0</Entry>
       <Entry key="milestone.filename">milestone.txt</Entry>
       <!-- milestone file (if stated) stores all the population (default: none) -->
       <Entry key="milestone.generation_">0</Entry>
       <Entry key="milestone.interval">0</Entry>
       <!-- milestone saving interval in generations; 0: save only at the end (default: 0) -->
       <Entry key="mutation.genotypes">random</Entry>
       <!-- if there are multiple genotypes, which to mutate? 'random': a random one, all: mutate all (default: random) -->
       <Entry key="mutation.indprob">0.3</Entry>
       <!-- individual mutation probability (unless the algorithm overrides it) (default: 0.3) -->
       <Entry key="mutation.protected"></Entry>
       <!-- indexes of genotypes (separated by spaces) that are excluded (protected) from mutation (default: none) -->
       <Entry key="population.demes">1</Entry>
       <!-- number of demes (default: 1) -->
       <Entry key="population.size">100</Entry>
       <!-- number of individuals (default: 100) -->
       <Entry key="randomizer.seed">0</Entry>
       <!-- randomizer seed, 0 uses time(NULL) (default: 0) -->
       <Entry key="stats.file"></Entry>
       <Entry key="term.eval">0</Entry>
       <!-- max number of fitness evaluations (default: none) -->
       <Entry key="term.fitnessval">0</Entry>
       <!-- fitness value for termination (default: none) -->
       <Entry key="term.maxgen">0</Entry>
       <!-- max number of generations (default: none) -->
       <Entry key="term.maxtime">0</Entry>
       <!-- max number of seconds to run (default: none) -->
       <Entry key="term.stagnation">50</Entry>
       <!-- max number of consecutive generations without improvement (default: 5000 / pop_size) -->

       <!-- parallel ECF parameters: -->
       <Entry key="parallel.type">eval</Entry> <!-- implicit parallelization method: eval - evaluation, mut - mutation + eval -->
       <Entry key="parallel.sync">0</Entry> <!-- implicit parallelization synchronicity: 0 - async, 1 - sync -->
       <Entry key="parallel.jobsize">10</Entry> <!-- implicit parallelization jobsize (individuals per job) -->
   </Registry>
   </ECF>

