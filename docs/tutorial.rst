
2. Tutorial examples – by genotype
==================================

These examples are a short introduction to the use of ECF when solving a
specific problem with a specific genotype (individual data structure).

The examples also contain the commented use of std::shared_ptr 
pointers which are used in other ECF components; for instance, a
std::dynamic_pointer_cast construct may be used to check the Genotype
type in the Individual.


#. :ref:`BitString genotype – OneMax example <bitstring>`
#. :ref:`FloatingPoint genotype – Function minimization example <flpoint>`
#. :ref:`Binary genotype - alternative to FloatingPoint encoding <binary>`
#. :ref:`Tree genotype – Symbolic regression example <gp_tree>`
#. :ref:`Permutation genotype – Traveling Salesperson Problem <permutation>`

--------------

.. _bitstring:

1. Genotype BitString – OneMax example
--------------------------------------

Suppose you want to maximize the number of ones in a vector of bits –
which is a simple \ \ \ `OneMax
problem <http://tracer.lcc.uma.es/problems/onemax/onemax.html>`__\ \ \ .
You would then use the BitString genotype, which offers no
coding/decoding, just a storage of n bits (and associated genetic
operators). To implement the evaluation operator, you need to inherit
from the EvaluateOp class:

.. code-block:: cpp

   class OneMaxEvalOp : public EvaluateOp
   {
   public:
           FitnessP evaluate(IndividualP individual);
   };

and then implement the *evaluate* function:

.. code-block:: cpp

	// evaluate() receives a smart pointer to the individual to evaluate
	FitnessP OneMaxEvalOp::evaluate(IndividualP individual)
	{
		// evaluation creates a new fitness object (using a smart pointer)
		// in our case, we try to maximize the number of ones, so we use FitnessMax (for maximization problems)
		FitnessP fitness (new FitnessMax);

		// Each individual is a vector of genotypes (defined in the configuration file).
		// We'll use BitString, and retrieve it as the first and only genotype
		BitString::BitString* bitstr = (BitString::BitString*) individual->getGenotype(0).get(); // don't need zero for the first one
		// (you can also use smart pointers:)
		//BitStringP bitstr = std::static_pointer_cast<BitString::BitString> (individual->getGenotype(0));

		// count the ones; where are they?
		// BitString genotype contains a std::vector of bool's named 'bits'
		uint ones = 0;
		for (uint i = 0; i < bitstr->bits.size(); i++) {
			if (bitstr->bits[i] == true)
			ones++ ;
		}
		fitness->setValue(ones);

		// return the new fitness
		return fitness;
	}

Then, we reference the evaluation op. in main:

.. code-block:: cpp

	int main(int argc, char **argv)
	{
		StateP state (new State);
		state->setEvalOp(new OneMaxEvalOp);

		state->initialize(argc, argv);
		state->run();
		return 0;
	}

and finally, define the BitString genotype – with its one parameter, the
number of bits – in a minimal configuration file:

.. code-block:: xml

   <ECF>
           <Genotype>
                   <BitString>
                           <Entry key="size">20</Entry>
                   </BitString>
           </Genotype>
   </ECF>

The program needs to be run with the name of the above configuration
file as its command line argument, i.e. "./**program parameters.txt**".

That's it. No other parameters needed (see section `Using the
parameters <parameters.html>`__ to see the default parameter settings).
And with a problem this simple, this will usually work fine. `See this
example <http://ecf.zemris.fer.hr/html/group__onemax.html>`__ in the manual.

 

--------------

.. _flpoint:

2. FloatingPoint genotype – Function minimization example
---------------------------------------------------------

Suppose you want to find the minimum of a multidimensional function,
which is a common task of `function
minimization <http://en.wikipedia.org/wiki/Optimization_(mathematics)>`__.
Say the function looks like this:

f(x) = (x[1]-1)\ :sup:`2` + (x[2]-2)\ :sup:`2` + ... +
(x[n]-n)\ :sup:`2` , 

where n is a given parameter (function
dimensionality) and x[i] the *i*-th component of vector x.

(**NOTE**: you can find additional common test functions provided with
this example in the source code)

You would then most often use the *FloatingPoint* genotype which is
simply a vector of floating-point numbers (variables of type *double*;
the alternative representation is binary in :ref:`the next
example <binary>`). *FloatingPoint* genotype offers these mandatory
parameters:

-  *lbound* – lower bound of real valued variable (e.g. -100),

-  *ubound* – upper bound (e.g. 100),

-  *dimension* – number of distinct variables (function dimension) (e.g.
   5).

All of these must be defined for a valid genotype. If you need different
boundaries for different variables, you can simply use more than one
genotype, as shown in section :doc:`Using the
parameters <parameters>`. To implement the
evaluation operator, we will inherit from the *EvaluateOp* class:

.. code-block:: cpp

   class FunctionMinEvalOp : public EvaluateOp
   {
   public:
           FitnessP evaluate(IndividualP individual);
   };

and then implement the *evaluate* function:

.. code-block:: cpp

   FitnessP evaluate(IndividualP individual)
   {
           // evaluation creates a new fitness object using a smart pointer
           // in our case, we try to minimize the function value, so we use FitnessMin fitness (for minimization problems)
           FitnessP fitness (new FitnessMin);

           // we define FloatingPoint as the only genotype (in the configuration file)
           FloatingPoint::FloatingPoint* gen = (FloatingPoint::FloatingPoint*) individual->getGenotype().get();
           // (you can also use smart pointers:)
           //FloatingPointP gen = std::dynamic_pointer_cast<FloatingPoint::FloatingPoint> (individual->getGenotype());

           double realTemp, value = 0;
           // we implement the fitness function 'as is', without any translation
           // the number of variables we read from the genotype itself (size of 'realValue' vactor)
           for (uint i = 0; i < gen->realValue.size(); i++){
                   realTemp = pow((gen->realValue[i] - (i + 1)), 2.);
                   value += realTemp;
           }

           fitness->setValue(value);
           return fitness;
   }

Then, we reference the evaluation operator in main:

.. code-block:: cpp

   int main(int argc, char **argv)
   {
           StateP state (new State);
           state->setEvalOp(new FunctionMinEvalOp);

           state->initialize(argc, argv);
           state->run();
           return 0;
   }

and finally, define the *FloatingPoint* genotype and its parameters in a
minimal configuration file:

.. code-block:: xml

   <ECF>
           <Genotype>
                   <FloatingPoint>
                           <Entry key="lbound">-50</Entry>
                           <Entry key="ubound">50</Entry>
                           <Entry key="dimension">3</Entry>
                   </FloatingPoint>
           </Genotype>
   </ECF>

The program can be run with the name of the above configuration file as
its command line argument, i.e. "./**program parameters.txt**".

This function has an arbitrary number of variables. Where this is not
the case, then the *dimension* parameter should be the same (or at least
not less than) as implemented in the evaluation operator. `See this
example <http://ecf.zemris.fer.hr/html/group__fmin.html>`__ in the manual.

This example can be solved in ECF with many algorithms (all variants of
GA, Differential evolution, PSO, GAn...) ; the configuration files with
appropriate parameters are provided in the example folder.

 

--------------

.. _binary:

3. Binary genotype – alternative to FloatingPoint encoding
----------------------------------------------------------

The previous example of function minimization/maximization
can also be solved using *Binary* genotype, which encodes real-valued
numbers as a sequence of bits using binary encoding. Depending on the
problem, this representation may exhibit better (or worse) convergence
than floating point encoding, but the floating point is *much faster* in
almost any configuration. Binary genotype uses equivalent parameters
with addition of *precision*:

-  *lbound* – lower bound of real valued variable (e.g. -100),

-  *ubound* – upper bound (e.g. 100),

-  *dimension* – number of distinct variables (function dimension) (e.g.
   5),

-  *precision* – number of decimal places that are taken into account
   (e.g. 3).

All of these must be defined for a valid genotype. The only difference
in the previous source code is that the following line in
FunctionMinEvalOp::evaluate

.. code-block:: cpp

           FloatingPoint::FloatingPoint* gen = (FloatingPoint::FloatingPoint*) individual->getGenotype().get();

should be replaced with

.. code-block:: cpp

           Binary::Binary* gen = (Binary::Binary*) individual->getGenotype().get();

Minimal configuration file for Binary genotype would look like this (see
"**parameters_binary.txt**" in this example's directory):

.. code-block:: xml

   <ECF>
           <Genotype>
                   <Binary>
                           <Entry key="lbound">-10</Entry>
                           <Entry key="ubound">10</Entry>
                           <Entry key="precision">3</Entry>
                           <Entry key="dimension">3</Entry>
                   </Binary>
           </Genotype>
   </ECF>

 

--------------

.. _gp_tree:

4. Genotype Tree – Symbolic regression example
----------------------------------------------

Suppose you want to discover the analytic form of an unknown function
that matches some given data – this is a typical `symbolic
regression <http://alphard.ethz.ch/gerber/approx/default.html>`__
problem. This problem is commonly solved with genetic programming, using
functional primitives and variables in a tree-like syntactic structure.
In ECF you would use the genotype *Tree*, which can contain predefined
or user defined functions and variables.

Say we want to discover the function that will best represent the
following data in range [-10, 10]:

+----------+--------+--------+--------+--------+--------+-------+-------+-------+-------+-------+
| X values | -10    | -8     | -6     | -4     | -2     | 0     | 2     | 4     | 6     | 8     |
+----------+--------+--------+--------+--------+--------+-------+-------+-------+-------+-------+
| Y values | -9.456 | -8.989 | -5.721 | -3.243 | -2.909 | 0.000 | 2.909 | 3.243 | 5.721 | 8.989 |
+----------+--------+--------+--------+--------+--------+-------+-------+-------+-------+-------+

This data is actually generated with the function y = x + sin(x), and
this is the target function that GP will try to evolve. How? Genotype
Tree offers these parameters:

-  *functionset* – set of functions that can appear in a tree,

-  *terminalset* – set of (independent) variables,

-  *mindepth* – minimal tree depth,

-  *maxdepth* – maximum tree depth,

-  *initmindepth* - initial minimal tree depth (optional: if not stated,
   *mindepth* is used),

-  *initmaxdepth* – initial maximum tree depth (optional: if not stated,
   *maxdepth* is used).

When applied to this problem, we may the define the following elements:
the variable is only one, and we may name it anyway we like (say X). The
function set should be *sufficient* to describe the given data – for
instance, we may include functions sin, cos, +, -, \* and /. The
*mindepth* and *maxdepth* values may be 1 and 5, so we get the following
configuration file:

.. code-block:: xml

   <ECF>
           <Genotype>
                   <Tree>
                           <Entry key="maxdepth">5</Entry>
                           <Entry key="mindepth">1</Entry>
                           <Entry key="functionset">sin cos + - / *</Entry>
                           <Entry key="terminalset">X</Entry>
                   </Tree>
           </Genotype>
   </ECF>

The evaluation operator should measure the difference between the given
data (y value) and the output of the actual evolved tree (function) for
each given x value. To do that, the evaluation operator should have the
pairs of x and y values predefined or calculated beforehand (no need to
calculate y values each time an individual is evaluated). This can be
done in the *initialization* phase, before the evolution starts. So the
evaluation operator is defined as:

.. code-block:: cpp

   class SymbRegEvalOp : public EvaluateOp
   {
   public:
           FitnessP evaluate(IndividualP individual);
           bool initialize(StateP); // initialization of training data
           std::vector<double> domain;
           std::vector<double> codomain;
           uint nSamples;
   };

(check section `5.8 Using the parameters <parameters.html>`__ to see how
data points can also be loaded from the configuration file). The
evaluator implementation may look like this:

.. code-block:: cpp

   // called only once, before the evolution – generates training data
   bool SymbRegEvalOp::initialize(StateP state)
   {
           nSamples = 10;
           double x = -10;
           for(uint i = 0; i < nSamples; i++) {
                   domain.push_back(x);
                   codomain.push_back(x + sin(x));
                   x += 2;
           }
           return true;
   }

   FitnessP SymbRegEvalOp::evaluate(IndividualP individual)
   {
           // we try to minimize the function value, so we use FitnessMin fitness (for minimization problems)
           FitnessP fitness (new FitnessMin);

           // get the genotype we defined in the configuration file
           Tree::Tree* tree = (Tree::Tree*) individual->getGenotype().get();
           // (you can also use smart pointers:)
           //TreeP tree = std::static_pointer_cast<Tree::Tree> (individual->getGenotype());

           double value = 0;
           for(uint i = 0; i < nSamples; i++) {
                   // for each test data instance, the x value (domain) must be set
                   tree->setTerminalValue("X", &domain[i]);
                   // get the y value of the current tree
                   double result;
                   tree->execute(&result);
                   // add the difference
                   value += abs(codomain[i] - result);
           }
           fitness->setValue(value);
           
           return fitness;
   }

Then, we reference the evaluation op. in main:

.. code-block:: cpp

   int main(int argc, char **argv)
   {
       StateP state (new State);
       state->setEvalOp(new SymbRegEvalOp);

       state->initialize(argc, argv);
       state->run();
       return 0;
   }

And the example may be run with the described configuration file, i.e.
"./**program parameters.txt**". `See this
example <http://ecf.zemris.fer.hr/html/group__symbreg.html>`__ in the manual.

**NOTE**: For additional customization of Tree genotype (e.g. defining
**ephemereal random constants**, adding **new functions**, adding
**custom function/terminal types** etc.) see section `Customizing the
Tree genotype <gp.html>`__.

 

--------------

.. _permutation:

5. Genotype Permutation – Traveling Salesperson Problem
-------------------------------------------------------

Suppose you want to solve what is commonly known as a `Traveling
Salesman
Problem <http://en.wikipedia.org/wiki/Travelling_salesman_problem>`__
(TSP), i.e. finding the shortest route to visit all 'cities' in a set
where each 'city' must be visited once. The most common representation
uses the `permutation solution
encoding <http://www.obitko.com/tutorials/genetic-algorithms/tsp-example.php>`__
where each potential solution is a sequence of 'city' indexes. In ECF,
this is what the *Permutation* genotype is used for.

A single most important genotype parameter is its size, usually
corresponding to the number of 'cities', which we can define in a
minimal configuration file (in the default ECF TSP example there are 29
cities). We also define a separate problem description file which
contains the distances between each two cities:

.. code-block:: xml

    <ECF>
		<Genotype>
			<Permutation>
				<Entry key="size">29</Entry>
			</Permutation>
		</Genotype>

		<Registry>
			<Entry key="tsp.infile">./bays29.tsp</Entry>
		</Registry>
    </ECF>

A permutation genotype of size *n* will contain a permutation of indexes
0,1, ... , *n -* 1 which can be decoded into a corresponding sequence of
'cities'. The evaluation operator may be declared as follows, where the
matrix weights will represent all the distances between any two cities:

.. code-block:: cpp

	class TSPEvalOp : public EvaluateOp 
	{
	private:
		int dimension;
		std::vector< std::vector<int> > weights;
		std::vector< std::vector<double> > coordinates;
	public:
		void registerParameters(StateP);
		bool initialize(StateP);
		FitnessP evaluate(IndividualP individual);
	};

The evaluation operator should either define the distances between
cities or load them from a separate problem description file:

.. code-block:: cpp

	void TSPEvalOp::registerParameters(StateP state)
	{
		state->getRegistry()->registerEntry("tsp.infile", (voidP) (new std::string), ECF::STRING);
	}

	bool TSPEvalOp::initialize(StateP state)
	{
		if(!state->getRegistry()->isModified("tsp.infile")) {
			state->getLogger()->log(1, "Error: no input file defined for TSP! (parameter 'tsp.infile'");
			return false;
		}

		voidP sptr = state->getRegistry()->getEntry("tsp.infile"); // get parameter value
		std::string filePath = *((std::string*) sptr.get()); // convert from voidP to user defined type

		std::ifstream iFile(filePath.c_str());
		std::string line;
		if(!iFile.is_open()) {
			state->getLogger()->log(1, "Error: can't open input file " + filePath);
			return false;
		}

		// ...
		// not shown here: code that initializes weights from tsp.infile
		// ...

		return true;
	}

	FitnessP TSPEvalOp::evaluate(IndividualP individual)
	{
		// minimize travel distance, so use FitnessMin
		FitnessP fitness (new FitnessMin);

		// get Permutation genotype from the individual
		Permutation::Permutation* perm = (Permutation::Permutation*) individual->getGenotype().get();
		// (you can also use smart pointers:)
		//PermutationP perm = std::static_pointer_cast<Permutation::Permutation> (individual->getGenotype());

		int fitnessV = 0;
		// genotype Permutation keeps a vector of indexes named 'variables'
		uint size = (uint) perm->variables.size();
		for(uint i = 0; i < size - 1; i++){
			// the length of each route is the sum of distances (weights) between each city in a route
			fitnessV += weights[perm->variables[i]][perm->variables[i + 1]];
		}
		fitnessV += weights[perm->variables[0]][perm->variables[dimension - 1]];
		
		fitness->setValue(fitnessV);
		return fitness;
	}

Then, we reference the evaluation op. in main:

.. code-block:: cpp

	int main(int argc, char **argv)
	{
		StateP state (new State);

		TSPEvalOp* tsp = new TSPEvalOp();
		state->setEvalOp(tsp);

		state->initialize(argc, argv);
		state->run();

		return 0;
	}

And the example may be run with the described configuration file, i.e.
"./**program parameters.txt**". `See this
example <http://ecf.zemris.fer.hr/html/group__tsp.html>`__ in the manual.

