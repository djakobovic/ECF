
3. ECF Algorithms
=================

ECF contains a number of algorithms, which may be used by default of
further parametrized. Current algorithms are listed below. For
instructions of how to add an algorithm, see the section :doc:`Adding
components to ECF <components>`.

The algorithm to be used is stated in the <Algorithm> block in the
:doc:`configuration file <parameters>`. The default
algorithm, if not stated in the configuration, is
*SteadyStateTournament* (see below).



1. SteadyStateTournament
------------------------

This algorithm is Genotype independent, which means it can be used with
any Genotype class. The algorithm performs a steady-state algorithm with
tournament elimination selection:

.. highlight:: none
::

	single generation {
	 repeat(deme size times) {
	  randomly add <nTournament_> individuals to the tournament;
	  select the worst one in the tournament;
	  randomly select two parents from remaining ones in the tournament;
	  replace the worst with crossover child;
	  perform mutation on child;
	 }
	}

This is the default algorithm, if the user does not state one.
Parameters and usage in configuration file:

.. code-block:: xml

	<Algorithm>
		<SteadyStateTournament>
			<Entry key="tsize">3</Entry>              <!-- tournament size -->
		</SteadyStateTournament>
	</Algorithm>

2. RouletteWheel
----------------

This algorithm is Genotype independent, which means it can be used with
any Genotype class. The algorithm implements a classical simple genetic
algorithm (SGA) with roulette-wheel (fitness proportional) generational
selection operator:

.. highlight:: none
::

	single generation {
	 select individuals to form the new generation (fitness proportional 
	   selection operator);
	 create new generation (make copies);
	 noCrx = (deme size) \* <crxRate_> / 2;
	 repeat(<noCrx> times) {
	  randomly select two parents;
	  perform crossover, \_replace\_ parents with their children;
	 }
	 perform mutation on new generation;
	}

Parameters and usage in configuration file:

.. code-block:: xml

	<Algorithm>
		<RouletteWheel>
			<Entry key="crxprob">0.5</Entry>          <!-- crossover rate -->
			<Entry key="selpressure">10</Entry>       <!-- selection pressure: how much the best individual is 'better' than the worst -->
		</RouletteWheel>
	</Algorithm>

3. Clonal Selection Algorithm (CLONALG)
---------------------------------------

This algorithm is Genotype *dependent* and can currently be used only
with a single FloatingPoint genotype (of any dimension). For algorithm
description, see e.g.
http://en.wikipedia.org/wiki/Clonal_Selection_Algorithm.

Parameters and usage in configuration file:

.. code-block:: xml

	<Algorithm>
		<Clonalg>
			<Entry key="beta">1</Entry>                         <!-- number of clones (percentage) for every antibody (default: 1.0) -->
			<Entry key="c">0.2</Entry>                          <!-- mutation rate (default: 0.2) -->
			<Entry key="cloningVersion">proportional</Entry>    <!-- cloning version, static or proportional (default: proportional) -->
			<Entry key="d">0</Entry>                            <!-- fraction of population regenerated every generation (default: 0) -->
			<Entry key="n">100</Entry>                          <!-- number of antibodies cloned every generation (default: 100) -->
			<Entry key="selectionScheme">CLONALG1</Entry>       <!-- which selection scheme to use, CLONALG1 or CLONALG2 (default: CLONALG1) -->
		</Clonalg>
	</Algorithm>

4. Immune Algorithm (optIA)
---------------------------

This algorithm is Genotype *dependent* and can currently be used only
with a single FloatingPoint genotype (of any dimension). For algorithm
description, see e.g.
http://www.artificial-immune-systems.org/algorithms.shtml.

Parameters and usage in configuration file:

.. code-block:: xml

	<Algorithm>
		<OptIA>
			<Entry key="c">0.2</Entry>                          <!-- mutation rate (default: 0.2) -->
			<Entry key="dup">5</Entry>                          <!-- number of clones for each individual in clone population (default: 5) -->
			<Entry key="elitism">0</Entry>                      <!-- use elitism (default: 0) -->
			<Entry key="tauB">100</Entry>                       <!-- maximum number of generations to keep an individual without improvement (default: 100) -->
		</OptIA>
	</Algorithm>

5. Evolution Strategy
---------------------

This algorithm is Genotype independent, which means it can be used with
any Genotype class. The algorithm implements a (mu/rho +/, lambda) -
Evolution Strategy (ES). The algorithm parameters are:

-  *mu*: the size of the parent population
-  *lambda*: number of offspring created in each iteration
-  *rho*: number of parents used to create an offspring; 1: mutation is
   used, 2: crossover
-  *selection*: may be "plus", which uses both parents and offspring or
   "comma", which uses just the offspring pool

Since the ECF defines "population.size" independently of the algorithm,
this implementation actually runs multiple ES parent populations; the
*population.size* must therefore be a multiple of parent pool size
(*mu*). For instance, if *mu* = 5 and *population.size* = 30, then 6 ES
subpopulations will be executed (no migration).

The algorithm flow:

.. highlight:: none
::

	single generation {
	 repeat(for every subpopulation) {
	    add mu individuals to the parent pool;
	    create lambda offspring using random rho parents for each;
	    if comma selection
	        create new parent pool with mu best from offspring pool;
	    else
	        create new parent pool with mu best from offspring and 
	        parents pool;
	 }
	}

Parameters and usage in configuration file:

.. code-block:: xml

	<Algorithm>
		<EvolutionStrategy>
			<Entry key="lambda">4</Entry>                        <!-- number of offspring created in each iteration (default: 4) -->
			<Entry key="mu">1</Entry>                            <!-- the size of parent population (default: 1) -->
			<Entry key="rho">1</Entry>                           <!-- number of parents used to create an offspring; may be 1 or 2 (default: 1) -->
			<Entry key="selection">plus</Entry>                  <!-- selection scheme: "plus", uses both parents and offspring) or "comma", uses just offspring (default: plus) -->
		</EvolutionStrategy>
	</Algorithm>

6. ParticleSwarmOptimization
----------------------------

This algorithm is Genotype *dependent* and can currently be used only
with a single FloatingPoint genotype (of any dimension). For algorithm
description, see e.g.
http://en.wikipedia.org/wiki/Particle_swarm_optimization.

Parameters and usage in configuration file:

.. code-block:: xml

	<Algorithm>
		<ParticleSwarmOptimization>
			<Entry key="weightType">0</Entry>         <!-- weight type update: 0 - constant, 1 - time dependant (based on max generations) -->
			<Entry key="weight">0.8</Entry>           <!-- initial inertia weight (either constant or time dependant) -->
			<Entry key="maxVelocity">100</Entry>      <!-- max particle velocity -->
		</ParticleSwarmOptimization>
	</Algorithm>

7. DifferentialEvolution
------------------------

This algorithm is Genotype *dependent* and can currently be used only
with a single FloatingPoint genotype (of any dimension). For algorithm
description, see e.g.
http://en.wikipedia.org/wiki/Differential_evolution.

Parameters and usage in configuration file:

.. code-block:: xml

	<Algorithm>
		<DifferentialEvolution>
			<Entry key="F">1</Entry>                  <!-- scaling constant -->
			<Entry key="CR">0.9</Entry>               <!-- crossover rate -->
		</DifferentialEvolution>
	</Algorithm>

8. Elimination
--------------

This algorithm is Genotype independent, which means it can be used with
any Genotype class. The algorithm implements an elimination algorithm
with inverse fitness proportional elimination operator:

.. highlight:: none
::

	single generation {
	 eliminate (genGap \* deme_size) individuals from current generation
  	  (inverse fitness proportional selection operator);
	 create new individuals with crossover (random selection);
	 perform mutation on new generation;
	}

Parameters and usage in configuration file:

.. code-block:: xml

	<Algorithm>
		<Elimination>
			<Entry key="gengap">0.6</Entry>           <!-- generation gap (percentage of population to be eliminated) -->
			<Entry key="selpressure">10</Entry>       <!-- selection pressure: how much is the worst individual 'worse' than the best -->
		</Elimination>
	</Algorithm>

9. GeneticAnnealing
-------------------

This algorithm is Genotype independent, which means it can be used with
any Genotype class. However, it is currently implemented **only for
minimization problems**, as the original (see
http://drdobbs.com/architecture-and-design/184409333?pgno=10).

.. highlight:: none
::

	single generation {
	 energy_bank = 0;
	 for each individual {
	  mutant = mutate(individual);
	  if fitness(mutant) < fitness(individual) + threshold(individual) {
	   diff = fitness(individual) + threshold(individual) - fitness(mutant);
	   energy_bank += diff;
	   replace individual with mutant;
	  }
	 energy_diff = energy_bank \* C / N;
	 for each individual {
	  threshold(individual) += energy_diff;
	 }
	}

Parameters and usage in configuration file:

.. code-block:: xml

	<Algorithm>
		<GeneticAnnealing>
			<Entry key="energybank">100</Entry>        <!-- total starting energy bank (fitness dependant) -->
			<Entry key="coolingfactor">0.7</Entry>     <!-- simulated annealing cooling factor -->
			<Entry key="elitism">0</Entry>             <!-- is the best individual preserved -->
		</GeneticAnnealing>
	</Algorithm>

10. ArtificialBeeColony
-----------------------

This algorithm is Genotype *dependent* and can currently be used only
with a single FloatingPoint genotype (of any dimension). For algorithm
description, see e.g.
http://www.scholarpedia.org/article/Artificial_bee_colony_algorithm.

Parameters and usage in configuration file:

.. code-block:: xml

	<Algorithm>
		<ArtificialBeeColony>
			<Entry key="limit">300</Entry>             <!-- Maximum number of cycles for each individual (default: 300) -->
			<Entry key="elitism">1</Entry>             <!-- Elitism: the current best food source is preserved (default: 1) -->
		</ArtificialBeeColony>
	</Algorithm>

11. Cuckoo Search
-----------------

This algorithm is Genotype *dependent* and can currently be used only
with a single FloatingPoint genotype (of any dimension). For algorithm
description, see e.g. http://en.wikipedia.org/wiki/Cuckoo_search.

.. highlight:: none
::

	single generation {
	 generate new solutions via Levy flights (adding constants 
	   multiplied with normal distribution random numbers and 
	   difference between current solution and best solution);
	 'pa' amount of new generated solutions are replaced with new 
	   solutions, related to the difference in solutions;
	}

Parameters and usage in configuration file:

.. code-block:: xml

	<Algorithm>
		<CuckooSearch>
			<Entry key="pa">0.75</Entry>             <!-- Maximum number of cycles for each individual (default: 300) -->
		</CuckooSearch>
	</Algorithm>

12. RandomSearch
----------------

This is an example of a random search algorithm, which is genotype
independent (can be used with any Genotype class).

.. highlight:: none
::

	single generation {
	 reinitialize all individuals;
	 evaluate all individuals;
	}

This algorithm uses no parameters.
