/* flowshop.h
 *
 * (C) 2018 Eva Tuba <etuba@ieee.org> and Carlos M. Fonseca <cmfonsec@dei.uc.pt>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/* This header file contains all problem dependent definitions */

/* Memory management */
struct solution *allocSolution(struct problem *p);
struct move *allocMove(struct problem *p);
struct pathState *allocPathState(struct problem *p);

void freeProblem(struct problem *p);
void freeSolution(struct solution *s);
void freeMove(struct move *v);
void freePathState(struct pathState *ps);

/* I/O */
void printProblem(struct problem *p);
void printSolution(struct solution *s);
void printMove(struct move *v);
void printPathState(struct pathState *ps);

/* Solution generation */
struct solution *randomSolution(struct solution *s);

/* Solution inspection */
double getObjectiveValue(struct solution *s);

/* Move generation */
struct move *randomMove(struct move *v, const struct solution *s);

/* Operations on solutions*/
struct solution *copySolution(struct solution *dest, const struct solution *src);
struct solution *applyMove(struct solution *s, const struct move *v);

/* Path generation */
struct pathState *initPathTo(struct pathState *ps, const struct solution *s1, const struct solution *s2);
struct pathState *initPathAwayFrom(struct pathState *ps, const struct solution *s);
struct move *nextRandomMove(struct move *v, struct pathState *ps);

/* Path inspection */
int getPathLength(const struct pathState *ps);


/* Problem-specific instantiation */
struct problem *newProblem(const char *filename);

struct problem {
    int *data;  /* array if size n*m, time needed by job j on machine i */
    int n, m;   /* number of jobs, number of machines */
};

struct solution {
    struct problem *prob;
    int *data;
    int *times;
    int n, m;   /* number of jobs, number of machines */
    int objvalue;
    int eval;   /* evaluation needed from this position onwards */
    int stale;  /* deprecated */
};

struct move {
    struct problem *prob;
    int data[2];
};

struct pathState {
    struct problem *prob;
    int *data;      /* normalised permutation computed as p2i[p1] */
    int *pos;       /* indices of the LIS followed by the remaining indices
                       in ascending order */
    int *pred;      /* temporary storage for LIS recovery */
    int n;
    int in_order;
};
