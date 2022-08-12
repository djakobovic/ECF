/* knapsack.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "knapsack.h"


/**********************************/
/* ----- Utility functions ----- */
/**********************************/

#if 1
/*
 * Random integer x such that 0 <= x <= n_max
 * Status: FINAL
 */
static int randint(int n_max) {
    int x;
	div_t y;
    if (n_max < 1)
        return 0;
    y = div(-RAND_MAX-1, -n_max-1);
    do
        x = rand();
    while (x > RAND_MAX + y.rem);
    return x / y.quot;
}
#else
#include <gsl/gsl_rng.h>
extern gsl_rng *rng;    /* The single rng instance used by the whole code */

static int randint(int n_max) {
    return gsl_rng_uniform_int(rng, n_max+1);
}
#endif

/**********************************************/
/* ----- Problem-specific instantiation ----- */
/**********************************************/

/*
 * Knapsack instantiation
 * Status: TENTATIVE
 * Notes:
 *   Needs better error checking
 */
struct problem *newProblem(const char *filename) {
    struct problem *p = NULL;
    FILE *infile;
    int i, n=-1, capacity=-1;
    infile = fopen(filename, "r");
    if (infile) {
        fscanf(infile, "%d", &n);
        fscanf(infile, "%d", &capacity);
        if (n > 0 && capacity > 0) {
            p = (problem*) malloc(sizeof (struct problem));
            p->weight = (int *) malloc(n * sizeof (int));
            p->profit = (int *) malloc(n * sizeof (int));
            for (i = 0; i < n; i++)
                fscanf(infile, "%d %d", p->profit+i, p->weight+i);
            p->n = n;
            p->capacity = capacity;
        } else
            fprintf(stderr, "Invalid knapsack instance %s\n", filename);
        fclose(infile);
    } else
        fprintf(stderr, "Cannot open file %s\n", filename);
    return p;
}

/*****************************/
/* ----- API functions ----- */
/*****************************/

/* Memory management */

/*
 * Allocate memory for a solution
 * Status: TENTATIVE
 */
struct solution *allocSolution(struct problem *p) {
    struct solution *s = (solution*) malloc(sizeof (struct solution));
    s->prob = p;
    s->data = (char*) malloc(p->n * sizeof (char));
    s->n = p->n;
    return s;
}

struct move *allocMove(struct problem *p) {
    struct move *v = (struct move*) malloc(sizeof (struct move));
    v->prob = p;
    return v;
}

struct pathState *allocPathState(struct problem *p) {
    struct pathState *ps = (pathState*) malloc(sizeof (struct pathState));
    ps->pos = (int*) malloc(p->n * sizeof (int));
    ps->prob = p;
    ps->n = p->n;
    return ps;
}

void freeProblem(struct problem *p) {
    free(p->weight);
    free(p->profit);
    free(p);
}

void freeSolution(struct solution *s) {
    free(s->data);
    free(s);
}

void freeMove(struct move *v) {
    free(v);
}

void freePathState(struct pathState *ps) {
    free(ps->pos);
    free(ps);
}

/* I/O  */

/*
 * Print problem instance data
 * Status: INTERIM
 * Notes:
 *   There should also be a way of printing the problem in the format read
 *   by newProblem().
 */
void printProblem(struct problem *p) {
    int i;
    printf("Knapsack problem instance\n");
    printf("  Number of items: %d\n", p->n);
    printf("  Capacity: %d\n", p->capacity);
    printf("  Weights:");
    for (i = 0; i < p->n; i++)
        printf(" %d", p->weight[i]);
    printf("\n");
    printf("  Profits:");
    for (i = 0; i < p->n; i++)
        printf(" %d", p->profit[i]);
    printf("\n");
}

void printSolution(struct solution *s) {
    int i;
    int n = s->n;
    printf("Knapsack solution\n  x:");
    for (i = 0; i < n; i++)
        printf(" %c", s->data[i]+'0');
    printf("\n");
}

void printMove(struct move *v) {
    printf("Knapsack move\n  Flip x[%d]\n", v->data);
}

void printPathState(struct pathState *ps) {
    int i;
    printf("Path state\n");
    printf("  Path length: %d\n  Positions:", ps->distance);
    for (i = 0; i < ps->distance; i++)
        printf(" %d", ps->pos[i]);
    printf("\n");
}

/* Solution generation */

/*
 * Generate solutions uniformly at random
 * Status: TENTATIVE, NEEDS_TESTING
 * Note:
 *   Evaluation of the random solution is done in this function
 */
struct solution *randomSolution(struct solution *s) {
    /* solution s must have been allocated with allocSolution() */
    struct problem *p = s->prob;
    int i, n = s->n;
    /* Generate */
    for (i = 0; i < n; i++)
        s->data[i] = randint(1);
    /* Evaluate */
    s->profit = 0, s->weight = 0;
    for (i = 0; i < n; i++)
        if (s->data[i]) {
            s->profit += p->profit[i];
            s->weight += p->weight[i];
        }
    s->objvalue = s->weight - p->capacity;
    if (s->objvalue <= 0)
        s->objvalue = -s->profit;
    return s;
}

/* Solution inspection */

double getObjectiveValue(struct solution *s) {
    return (double)s->objvalue;
}

/* Move generation */

/*
 * Generate moves uniformly at random
 * Status: FINAL
 */
struct move *randomMove(struct move *v, const struct solution *s) {
    v->data = randint(s->n-1);
    return v;
}

/* Operations on solutions*/

struct solution *copySolution(struct solution *dest, const struct solution *src) {
    dest->prob = src->prob;
    dest->n = src->n;
    dest->profit = src->profit;
    dest->weight = src->weight;
    memcpy(dest->data, src->data, src->n * sizeof (char));
    dest->objvalue = src->objvalue;
    return dest;
}

/*
 * Apply a move to a solution
 * Status: TENTATIVE, NEEDS_TESTING
 */
struct solution *applyMove(struct solution *s, const struct move *v) {
    struct problem *p = s->prob;
    int i = v->data;
    if (i < 0 || i >= s->n)     /* null move, do nothing */
        return s;
    if (s->data[i] == 0) {
        s->data[i] = 1;
        s->weight += p->weight[i];
        s->profit += p->profit[i];
    } else {
        s->data[i] = 0;
        s->weight -= p->weight[i];
        s->profit -= p->profit[i];
    }
    s->objvalue = s->weight - p->capacity;
    if (s->objvalue <= 0)
        s->objvalue = -s->profit;
    return s;
}

/* Path generation */

/*
 * Set up a path from one solution to another
 * Status: TENTATIVE, NEEDS_TESTING
 */
struct pathState *initPathTo(struct pathState *ps, const struct solution *s1, const struct solution *s2) {
    int i, k, n = s1->n;
    /* Indices of the bits in which the solutions differ */
    for (i = 0, k = 0; i < n; i++)
        if (s1->data[i] != s2->data[i])
            ps->pos[k++] = i;
    ps->distance = k;
    return ps; 
}

/*
 * Set up a path away from a solution
 * Status: TENTATIVE, NEEDS_TESTING
 * Notes:
 */
struct pathState *initPathAwayFrom(struct pathState *ps, const struct solution *s) {
    int i, n = ps->n;
    for (i = 0; i < n; i++)
        ps->pos[i] = i;
    ps->distance = n;
    return ps;
}

/*
 * Generate the next random move in a path towards a solution
 * Status: TENTATIVE, NEEDS_TESTING
 * Notes:
 *   A null move does nothing when applied to a solution. This is handled
 *   by applyMove().
 */
struct move *nextRandomMove(struct move *v, struct pathState *ps) {
    int r;
    if (ps->distance == 0) { /* end of path has been reached, no move is possible */
        v->data = -1;        /* null move */
        return v;
    }
    /* Draw an index at random */
    r = randint(--ps->distance);
    v->data = ps->pos[r]; 
    /* Update pathState - remember which moves are still available (and only those) */
    ps->pos[r] = ps->pos[ps->distance];
    return v;
}

/* Path inspection */

/*
 * Current length of path
 * Status: FINAL
 */
int getPathLength(const struct pathState *ps) {
    return ps->distance;
}
