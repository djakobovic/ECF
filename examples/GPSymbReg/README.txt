
Symbolic regression example - illustration for Tree genotype (see more on this example in help/tutorial.html).


Running the example (more info in help/install.html)
	Windows: compile the provided VisualStudio solution in 'VS' directory (having previously compiled the ECF_lib solution)
	Linux: compile with the provided 'Makefile' (having previously compiled the ECF library)

Description
    The objective is to find a symbolic form of an unknown function (i.e. find a mapping between the given set of input and output values). The fitness is defined with total error for the given set of output values.

Tree genotype parameters (in the config file)

        set of functions (inner tree nodes)
        set of terminals (variables and constants, tree leaves)
        min and max tree depth
