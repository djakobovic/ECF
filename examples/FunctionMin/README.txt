
Function minimization example - using FloatingPoint or Binary genotype (see more on this example in help/tutorial.html).


Running the example (more info in help/install.html)
	Windows: compile the provided VisualStudio solution in 'VS' directory (having previously compiled the ECF_lib solution)
	Linux: compile with the provided 'Makefile' (having previously compiled the ECF library)

Description
    Tries to find the (global) minimum of a continuous multidimensional function. The example uses either FloatingPoint or Binary genotype.

Parameters (in the config file)

        "function": defines the minimization function to be used (default: 1)
        with FloatingPoint genotype, a number of dimensions and the interval [lbound, ubound] must be set
        with Binary genotype, a number of dimensions, the interval [lbound, ubound] and precision (number of digits after the decimal point) must be set

References

        http://www.geatbx.com/docu/fcnindex-01.html
        http://www.gamsworld.org/performance/selconglobal/
        http://coco.lri.fr/downloads/download11.06/bbobdocfunctions.pdf

