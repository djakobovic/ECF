
**********************************************************
Evolutionary Computation Framework - documentation (v 1.5)
**********************************************************

Introduction
============

For a headfirst introduction, see the examples in :doc:`Tutorial examples – by genotype <tutorial>`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


0. In short - an overview (below)
#. :doc:`Installation <install>` (or “\ *How do I make it run*\ ”)
#. :doc:`Tutorial examples – by genotype <tutorial>` (or “\ *I just
   want to solve my problem, show me how*\ ”)
#. :doc:`Evolutionary algorithms in ECF <algorithms>` (or “\ *What
   exactly does it do??*\ ”)
#. :doc:`Using the parameters <parameters>` (or “\ *Hm, it doesn't
   work that well, can I make it any better*\ ”)
#. :doc:`Customizing the Tree genotype <gp>` (or “\ *Where's GP? I
   want genetic programming*\ ”)
#. :doc:`Adding components to ECF <components>` (or “\ *I know EC, I
   just want to use my special genotype/algorithm/whatever, how do I do
   it*\ ”)

Project webpage: `http://ecf.zemris.fer.hr/ <http://ecf.zemris.fer.hr/>`_

--------------

In short...
~~~~~~~~~~~

To use ECF, you can combine/parametrize the following components:

-  **Genotype**. The basic component of ECF is *Genotype*. Each
   individual in the population has one or more genotypes – any
   genotypes of any number. The simplest way of defining genotypes is
   listing them (and their parameters) in the configuration file.
   Genotype is *the only* component that *has* to be defined in a
   configuration file – all the others may be used by default.
   Each genotype in ECF has its data structure and associated genetic
   operators – of crossover and mutation type (so there's no
   possibility of using wrong operator on wrong genotype). Knowing the
   genotype's data structure is a requirement for defining the
   *fitness function* that will evaluate the individual containing the
   given genotype.

-  **Algorithm**. The ECF runs an algorithm. The desired algorithm (and
   its parameters) can be stated in the configuration file. But you
   don't even have to, there's a :doc:`default one <algorithms>`.

-  **Evolutionary system – the state**. Algorithm and genotypes exist in
   a context that takes care of everything else: the size of the
   population, termination condition, logging etc.

The above components are fully configurable without compilation via
configuration file. The only element you *need* to program and define
programmatically is the **evaluation operator** which embodies the
fitness function. Once you write the evaluation operator, you only need
to reference it in *main* and then run the ECF. So a typical *main*
function might look like this:

.. code-block:: cpp

	int main(int argc, char **argv)
	{
		StateP state (new State);
		state->setEvalOp(new MyEvalOperator);

		state->initialize(argc, argv);
		state->run();
		return 0;
	}

while the **configuration file** might look like this:

.. code-block:: xml

	<ECF>
		<Algorithm>
			<SteadyStateTournament>
				<Entry key="tsize">3</Entry>
			</SteadyStateTournament>
		</Algorithm>

		<Genotype>
			<BitString>
				<Entry key="size">20</Entry>
			</BitString>
			<Tree>
				<Entry key="functionset">sin cos + - / *</Entry>
				<Entry key="terminalset">X Y</Entry>
			</Tree>
		</Genotype>

		<Registry>
			<Entry key="population.size">30</Entry>
			<Entry key="term.maxgen">100</Entry> 
		</Registry>
	</ECF>

| **Implementation note**: each *Class* in ECF has an associated type
  *ClassP* which is a *boost* smart pointer to an object of that class.
| See `more information on using
  boost <http://www.codeproject.com/KB/stl/boostsmartptr.aspx>`__.



.. toctree::
	:maxdepth: 1

	install
	tutorial
	algorithms
	parameters
	gp
	components
