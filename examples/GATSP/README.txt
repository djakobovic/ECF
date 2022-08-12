
TSP example - illustration for Permutation genotype (see more on this example in help/tutorial.html).


Running the example (more info in help/install.html)
	Windows: compile the provided VisualStudio solution in 'VS' directory (having previously compiled the ECF_lib solution)
	Linux: compile with the provided 'Makefile' (having previously compiled the ECF library)

Description
    The TSP is a classic combinatorial problem which represents a large number of problem instances. The objective is to find a route connecting all the 'cities' that minimizes the total traveled distance.
	The test examples are copied from the TSPLIB repository at http://www.iwr.uni-heidelberg.de/groups/comopt/software/TSPLIB95/. (pick additional examples as needed!)

Parameters (in the config file)
	- path to a TSP file containing distances between cities (e.g. 'bays29.tsp') must be given
	- for Permutation genotype, you only need to define its size (number of cities in this case) (update: in current revision the size is auto-adjusted to the number of cities in the TSP file!)