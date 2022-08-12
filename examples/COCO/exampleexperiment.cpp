/* runs an entire experiment for benchmarking an arbitrary ECF algorithm,
* on the noise-free testbed
* or the noisy testbed (change the ifun loop in this case as given below).
*/

/**************************************************
 *          BBOB Mandatory initialization         *
 *************************************************/
/* 
BBOB parameters defined in this file are:
	- function id
	- function instance
(given in for loops, see below)

All the other BBOB parameters are given in the ECF configuration file:
	- algorithm name
	- algorithm description
	- dimensionality
	- termination criteria (number of evaluations)
as well as the actual ECF optimization algorithm.
*/


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "./bbob/bbobStructures.h" /* Include all declarations for BBOB calls */
#include <ECF/ECF.h>
#include "FunctionMinEvalOp.h"

extern int enableOutput;


int main(int argc, char **argv)
{
    unsigned int instances[15] = {1, 2, 3, 4, 5, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
    unsigned int ifun, idx_instances;
    int independent_restarts;
    double maxfunevals, minfunevals;

    clock_t t0 = clock();
    time_t Tval;

    /* To make the noise deterministic. */
    /* fgeneric_noiseseed(30); printf("seed for the noise set to: 30\n"); */

    /* Function indices are from 1 to 24 (noiseless) or from 101 to 130 (noisy) */
    /* for the noisy functions exchange the for loop with */
    /* for (ifun = 101; ifun <= 130; ifun++) */
    for (ifun = 1; ifun <= 24; ifun++)
    {

        for (idx_instances = 0; idx_instances < 15; idx_instances++)
        {
			// set up ECF
			StateP state (new State);
			// set the evaluation operator
			FunctionMinEvalOp* evalOp = new FunctionMinEvalOp;
			state->setEvalOp(evalOp);

			// initialize with current parameters, but skip evaluateOp
			evalOp->experimentMode_ = true;
			state->initialize(argc, argv);
			evalOp->experimentMode_ = false;

			// force BBOB output (overrides ECF configuration)
			enableOutput = 1;
			// set function ID (overrides ECF configuration)
			state->getRegistry()->modifyEntry("coco.function", (voidP) (new uint(ifun)));
			state->getRegistry()->modifyEntry("coco.enableoutput", (voidP) (new uint(1)));

			// set function instance
			state->getRegistry()->modifyEntry("coco.instance", (voidP) (new uint(instances[idx_instances])));

			// initialize evaluator with current instance
			state->getEvalOp()->initialize(state);


			maxfunevals = 10; /* PUT APPROPRIATE MAX. NUMBER OF FEVALS */
                                             /* 5. * dim should be fine to just check everything */
            minfunevals = 10;  /* PUT MINIMAL USEFUL NUMBER OF FEVALS */
            independent_restarts = -1;
            while (fgeneric_evaluations() + minfunevals <= maxfunevals)
            {
                if (++independent_restarts > 0) 
                    fgeneric_restart("independent restart");  /* additional info */

				// ECF optimization
				state->run();

				if (fgeneric_best() < fgeneric_ftarget())
                    break;
            }

            printf("  f%d in %d-D, instance %d: FEs=%.0f with %d restarts,", ifun, 10,
                   instances[idx_instances], fgeneric_evaluations(), independent_restarts);
            printf(" fbest-ftarget=%.4e, elapsed time [h]: %.2f\n", 
                   fgeneric_best() - fgeneric_ftarget(), (double)(clock()-t0)/CLOCKS_PER_SEC/60./60.);

			fgeneric_finalize();
        }
        Tval = time(NULL);
        printf("    date and time: %s", ctime(&Tval));
    }

	return 0;
}
