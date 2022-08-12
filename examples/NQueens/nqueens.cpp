/* nqueens.c
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
#include "nqueens.h"



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
    if (n_max < 1)
        return 0;
    div_t y = div(-RAND_MAX-1, -n_max-1);
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

/*
 * Random permutation of size n
 * Status: FINAL
 * Notes:
 *   Based on https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#The_%22inside-out%22_algorithm
 */
static int *randperm(int *p, int n) {
    /* Inside-out algorithm */
    int i, j;
    if (n > 0) {
        p[0] = 0;
        for (i = 1; i < n; i++) {
            j = randint(i);
            p[i] = p[j];
            p[j] = i; /* = source[i] */
        }
    }
    return p;
}

static void swap(int *data, int i, int j) {
    if (i == j)
        return;
    int el = data[i];
    data[i] = data[j];
    data[j] = el;
}

/**********************************************/
/* ----- Problem-specific instantiation ----- */
/**********************************************/

/*
 * N-Queens instantiation
 * Status: TENTATIVE
 * Notes:
 *   Should just take an integer as an argument.
 *   Needs error checking
 */
#if 0
struct problem *newProblem(const char *filename) {
    struct problem *p = NULL;
    FILE *infile;
    int n=-1;
    infile = fopen(filename, "r");
    if (infile) {
        fscanf(infile, "%d", &n);
        fclose(infile);
        if (n > 0) {
            p = (struct problem*) malloc(sizeof (struct problem));
            p->n = n;
        } else
            fprintf(stderr, "Invalid n-Queens instance %s\n", filename);
    } else
        fprintf(stderr, "Cannot open file %s\n", filename);
    return p;
}
#else
struct problem *newProblem(int n) {
    struct problem *p = NULL;
    if (n > 0) {
        p = (struct problem*) malloc(sizeof (struct problem));
        p->n = n;
    } else
        fprintf(stderr, "Invalid board size: %d\n", n);
    return p;
}
#endif

/*****************************/
/* ----- API functions ----- */
/*****************************/

/* Memory management */

/*
 * Allocate memory for a solution
 * Status: CHECK
 */
struct solution *allocSolution(struct problem *p) {
    struct solution *s = (solution*) malloc(sizeof (struct solution));
    s->prob = p;
    s->data = (int*) malloc(p->n * sizeof (int));
    s->olddata = (int*) malloc(p->n * sizeof (int));
    s->mod = (int*) malloc(p->n * sizeof (int));
    s->modi = (int*) malloc(p->n * sizeof (int));
    s->n = p->n;
    return s;
}

/*
 * Allocate memory for a move
 * Status: FINAL
 */
struct move *allocMove(struct problem *p) {
    struct move *v = (move*) malloc(sizeof (struct move));
    v->prob = p;
    return v;
}

/*
 * Allocate memory for a path
 * Status: TENTATIVE
 */
struct pathState *allocPathState(struct problem *p) {
    struct pathState *ps = (pathState*) malloc (sizeof (struct pathState));
    ps->prob = p;
    ps->data = (int*) malloc(p->n * sizeof (int));
    ps->datai = (int*) malloc(p->n * sizeof (int));
    ps->cycle = (int*) malloc(p->n * sizeof (int));
    ps->pos = (int*) malloc(p->n * sizeof (int));
    ps->n = p->n;
    return ps;
}

/*
 * Free the memory used by a problem
 * Status: FINAL
 */
void freeProblem(struct problem *p) {
    free(p);
}

/*
 * Free the memory used by a solution
 * Status: FINAL
 */
void freeSolution(struct solution *s) {
    free(s->data);
    free(s->olddata);
    free(s->mod);
    free(s->modi);
    free(s);
}

/*
 * Free the memory used by a move
 * Status: FINAL
 */
void freeMove(struct move *v) {
    free(v);
}

/*
 * Free the memory used by a path
 * Status: IN_PROGRESS
 * Notes:
 *   update along with pathState
 */
void freePathState(struct pathState *ps) {
    free(ps->data);
    free(ps->datai);
    free(ps->cycle);
    free(ps->pos);
    free(ps);
}

/* I/O  */
void printProblem(struct problem *p) {
    printf("%d-Queens problem\n",p->n);
}

void printSolution(struct solution *s) {
    int i;
    printf("%d-Queens solution\n  p:", s->n);
    for(i = 0; i < s->n; i++)
        printf(" %d", s->data[i]);
#if 0
    printf("\n  mod :");
    for(i = 0; i < s->n; i++)
        printf(" %d", s->mod[i]);
    printf("\n  modi:");
    for(i = 0; i < s->n; i++)
        printf(" %d", s->modi[i]);
#endif
    printf("\n");
}

void printMove(struct move *v) {
    printf("%d-Queens move: %d, %d\n", v->prob->n, v->data[0], v->data[1]);
}

void printPathState(struct pathState *ps) {
    int i;
    printf("%d-Queens path state\n", ps->n);
    printf("  Path length: %d\n  Permutation:", ps->n - ps->n_cycles);
    for (i = 0; i < ps->n; i++)
        printf(" %d",ps->data[i]);
    printf("\n  Inverse permutation:");
    for (i = 0; i < ps->n; i++)
        printf(" %d", ps->datai[i]);
    printf("\n  Cycles:");
    for (i = 0; i < ps->n; i++)
        printf(" %d", ps->cycle[i]);
    printf("\n  Number of different positions: %d\n  Positions: ", ps->n_diff);
    for (i = 0; i < ps->n_diff; i++)
        printf("%d ",ps->pos[i]);
    printf("\n");
}

/* Solution generation */

/*
 * Generate solutions uniformly at random
 * Status: CHECK
 */
struct solution *randomSolution(struct solution *s) {
    /* solution s must have been allocated with allocSolution() */
    int i;
    randperm(s->data, s->n);
    /* Solution not evaluated yet */
    for (i = 0; i < s->n; i++)
        s->mod[i] = s->modi[i] = i;
    s->nmod = s->n;
    return s;
}

/* Solution inspection */

/*
 * Generate solutions uniformly at random
 * Status: INTERIM
 * Notes:
 *   Implements incremental evaluation for multiple moves
 */
double getObjectiveValue(struct solution *s) {
    int i, j, n = s->n, att, *mod, nmod = s->nmod;
    if (nmod > .28*n) { /* full evaluation threshold to be fine-tuned experimentally */
        att = 0;
        for (i = 0; i < n-1; i++)
            for (j = i+1; j < n; j++)
                att += (j - i == abs(s->data[i] - s->data[j]));
        s->objvalue = att;
        memcpy(s->olddata, s->data, n * sizeof (int));
        s->nmod = 0;
    } else if (nmod >= 1) { /* incremental evaluation */
        att = s->objvalue;
        mod = s->mod;
        for (i = 0; i < nmod-1; i++)
            for (j = i+1; j < nmod; j++)
                att += (abs(mod[j] - mod[i]) == abs(s->data[mod[i]] - s->data[mod[j]])) -
                    (abs(mod[j] - mod[i]) == abs(s->olddata[mod[i]] - s->olddata[mod[j]]));
        for (i = 0; i < nmod; i++)
            for (j = nmod; j < n; j++)
                att += (abs(mod[j] - mod[i]) == abs(s->data[mod[i]] - s->data[mod[j]])) -
                    (abs(mod[j] - mod[i]) == abs(s->olddata[mod[i]] - s->olddata[mod[j]]));
        s->objvalue = att;
        memcpy(s->olddata, s->data, n * sizeof (int));
        s->nmod = 0;
#if 0
        /* for testing */
        att = 0;
        for (i = 0; i < n-1; i++)
            for (j = i+1; j < n; j++)
                if (j - i == abs(s->data[i] - s->data[j]))
                    att++;
        printf("incremental = %d, full = %d, diff = %d\n", s->objvalue, att, s->objvalue-att);

#endif
    }
    return (double)s->objvalue;
}

/* Operations on solutions*/
struct solution *copySolution(struct solution *dest, const struct solution *src) {
    dest->prob = src->prob;
    dest->n = src->n;
    memcpy(dest->data, src->data, src->n * sizeof (int));
    memcpy(dest->olddata, src->olddata, src->n * sizeof (int));
    memcpy(dest->mod, src->mod, src->n * sizeof (int));
    memcpy(dest->modi, src->modi, src->n * sizeof (int));
    dest->nmod = src->nmod;
    dest->objvalue = src->objvalue;
    return dest;
}

/*
 * Apply a move to a solution
 * Status: FINAL
 */
struct solution *applyMove(struct solution *s, const struct move *v) {
    int i, j;
    i = v->data[0];
    j = v->data[1];
    if (i == j)     /* do nothing */
        return s;
    swap(s->data, i, j);
    if (s->modi[i] >= s->nmod) {
        swap(s->mod, s->modi[i], s->nmod);
        swap(s->modi, i, s->mod[s->modi[i]]);
        s->nmod++;
    }
    if (s->modi[j] >= s->nmod) {
        swap(s->mod, s->modi[j], s->nmod);
        swap(s->modi, j, s->mod[s->modi[j]]);
        s->nmod++;
    }
    return s;
}

/* Move generation */

/*
 * Generate moves uniformly at random
 * Status: TENTATIVE, NEEDS_TESTING
 * Notes:
 *   Move (i,j) such that i != j. Order is irrelevant and not enforced.
 */
struct move *randomMove(struct move *v, const struct solution *s) {
    /* move v must have been allocated with allocMove() */
    int n;
    n = s->n;
    v->data[0] = randint(n-1);
    v->data[1] = (v->data[0] + 1 + randint(n-2)) % n;
    return v;
}

/*
 * Generate the next random move in a path towards a solution
 * Status: CHECK, NEEDS_WORK
 * Notes:
 *  should compute moves by using cycles
 */
struct move *nextRandomMove(struct move *v, struct pathState *ps) {
    int i, j, r, n = ps->n;
    int *pos = ps->pos, *data = ps->data, *datai = ps->datai;

    if (ps->n_cycles >= n) { /* end of path has been reached, no move is possible */
        v->data[0] = v->data[1] = 0; /* null move */
        return v;
    }

    /* Note: a swap move may correct two positions at once, but we only check
       one at a time. Therefore, we may need to skip null moves here, and try
       again. */
    do {
        /* Draw at random a position to correct */
        r = randint(--ps->n_diff);
        i = v->data[0] = pos[r];  /* choose an element that is not correct */ 
        /* Find where correct one is */
        j = datai[i];
        pos[r] = pos[ps->n_diff];
    } while (i == j);
    v->data[1] = j;

    /* Update pathState */
    ps->n_cycles++;
    swap(ps->datai, i, data[i]);
    swap(ps->data, i, j);
    return v;
}

/* Path generation */

/*
 * Set up a path from one solution to another
 * Status: CHECK
 * Notes:
 *   find and save cycles of permutation p2i[p1]
 */
struct pathState *initPathTo(struct pathState *ps, const struct solution *s1, const struct solution *s2) {
    int i, j, n = ps->n;
    
    /* Compute reference permutation p2i[p1] using ps->pos as a buffer */
    for (i = 0; i < n; i++)
        ps->pos[s2->data[i]] = i;
    for (i = 0; i < n; i++)
        ps->data[i] = ps->pos[s1->data[i]];
    /* Inverse of reference permutation */
    for (i = 0; i < n; i++)
        ps->datai[ps->data[i]] = i;

    /* Compute the cycles in reference permutation p2i[p1] */
    ps->n_cycles = 0;
    for (i = 0; i < n; i++)
        ps->cycle[i] = 0;    /* all elements are unchecked */
    for (i = 0; i < n; i++) {
        if (ps->cycle[i] == 0) {
            ps->n_cycles++;
            j = i;
            do {
                ps->cycle[j] = ps->n_cycles;
                j = ps->data[j];
            } while (j != i);
        }
    }

    /* Find elements of data that differ from their position */
    for (i = 0, j = 0; i < n; i++)
        if (ps->data[i] != i)
            ps->pos[j++] = i;
    ps->n_diff = j;
    return ps;    
}

/*
 * Set up a path away from a solution
 * Status: NOT IMPLEMENTED
 * Notes: 
 */
struct pathState *initPathAwayFrom(struct pathState *ps, const struct solution *s) {
    return NULL;
}

/* Path inspection */

/*
 * Current length of path
 * Status: FINAL
 */
int getPathLength(const struct pathState *ps) {
    return ps->n - ps->n_cycles;
}
