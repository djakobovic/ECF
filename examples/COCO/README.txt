
Function minimization example with BBoB function minimization (from http://coco.gforge.inria.fr).


Running the example (more info in help/install.html)
	Windows: compile the provided VisualStudio solution in 'VS' directory (having previously compiled the ECF_lib solution)
	Linux: compile with the provided 'Makefile' (having previously compiled the ECF library)

Description
	Tries to find the (global) minimum of a continuous multidimensional function. The example uses either FloatingPoint or Binary genotype. Functions are implemented in Black-Box Optimization Benchmarking package available at http://coco.gforge.inria.fr. 
	The location of the optimum is drawn uniformly from [-4, 4] in all dimensions for all functions. The value of the optimum of all functions is set to zero in this example (in original BBoB package the value is in [-100, 100]). 

The example produces two executables (separate VS projects on Windows):
	- funcmin: runs the selected algorithm on a selected function
	- ecf_experiment: runs the selected algorithm on a range of functions in a BBOB-defined experimentation scheme (see exampleexperiment.cpp)

To run the ecf_experiment executable:
	- define functions and function instances in exampleexperiment.cpp
	- define all the other parameters (the algorithm, output folder, dimension, termination criteria) in the configuration file (see parameters.txt)

Parameters (in the config file)
	- "coco.function": defines the minimization function to be used (default: 1), 1-24 noiseless functions, 101-130 noisy
	- "coco.enableoutput": should COCO framework output results for post-processing (disabled by default)
	- "coco.folder": destination folder for COCO output; must be created beforehand (see /bbob/README.txt)
	- with FloatingPoint genotype, a number of dimensions and the interval [lbound, ubound] must be set
	- with Binary genotype, a number of dimensions, the interval [lbound, ubound] and precision (number of digits after the decimal point) must be set


References

	http://coco.gforge.inria.fr
	http://coco.lri.fr/downloads/download11.06/bbobdocfunctions.pdf

