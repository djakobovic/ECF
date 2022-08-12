
Artificial ant example.


Running the example (more info in help/install.html)
	Windows: compile the provided VisualStudio solution in 'VS' directory (having previously compiled the ECF_lib solution)
	Linux: compile with the provided 'Makefile' (having previously compiled the ECF library)

Description
	Evolves a control program for the artificial ant. The objective is to traverse a given environment and collect as many food items as possible. The ant 'sees' only a single location in front, and can only turn left, right or move forward. The food is eaten if the ant moves in the same location.
 
	The environments (food trails) are organized in two files (specified in the config file):
	- learning_trails.txt: trails used for learning (ant evolution)
	- test_trails.txt: trails used for testing the best evolved ant
	Each trail defines its dimensions, the total number of food items and the maximum allowed number of moves. You can add or change the existing trails in those files. 

	Additional files:
	- koza_solution.txt: contains the best solution from Koza 1992 (can be loaded and simulated, see main.cpp)

Function set

        IFA: If there is food ahead, execute the first subtree, else execute the second subtree.
        P2: execute the first subtree and then the second subtree.
        P3: execute all three subtrees in sequence.

Terminal set

        L: Turn 90 degrees left
        R: Turn 90 degrees right
        M: Move one square ahead

Fitness
    Number of collected food items, within the maximum number of moves.

Reference
    John R. Koza, "Genetic Programming I: On the Programming of Computers by Means of Natural Selection", MIT Press, 1992, pages 147-161. 
