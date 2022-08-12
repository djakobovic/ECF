/* flowshop.c
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
#include "flowshop.h"



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
 *   Algorithm described verbally in Knuth (1997), The Art of Computer
 *   Programming, Volume 2, Seminumerical Algorithms, 3rd ed., p. 145.
 *   See also https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#The_%22inside-out%22_algorithm
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

/*
 * Evaluate schedule makespan
 * Status: FINAL
 */
static void makespan(struct solution *s) {
    /* order is jobs x machines - job order changes, machine order does not */
    int j, n = s->n;            /* jobs */
    int i, m = s->m;            /* machines */
    int k;                      /* auxiliary running index */
    int *t = s->times;          /* time array */
    int *pd = s->prob->data;    /* processing times */
    int *sd = s->data;          /* job order */
    /* first row and column of s->times initialised to 0 by allocSolution() */
    /* evaluate from first job that changed since last evaluation */
    k = (m+1)*(s->eval+1) + 1;
    for(j = s->eval; j < n; j++, k++)
        for(i = 0; i < m; i++, k++)
            t[k] = pd[m*sd[j]+i] + ((t[k-1] >= t[k-m-1]) ? t[k-1] : t[k-m-1]);
    s->objvalue = t[k-2];
    s->eval = n; /* fully evaluated */
}

/*
 * Longest Increasing Subsequence
 * Status: INTERIM
 * Notes:
 *   Based on https://en.wikipedia.org/wiki/Longest_increasing_subsequence
 *   This implementation determines the lexicographically smallest LIS.
 */
static int lis(int *P, int *M, int *X, int n) {
    int i, lo, mid, hi;
    int newL, L = 0;
    for (i = 0; i < n; i++) {
        lo = 1;
        hi = L;
        while (lo <= hi) {
            mid = (lo + hi + 1) / 2;
            if (X[M[mid-1]] < X[i])
                lo = mid + 1;
            else
                hi = mid - 1;
        }
        newL = lo;
        P[i] = (newL > 1) ? M[newL-2] : -1;
        M[newL-1] = i;
        if (newL > L)
            L = newL;
    }
    return L;
}

/*
 * Binary search
 * Status: FINAL
 * Notes:
 *   After Cormen et al. (2009), Introduction to Algorithms, 3rd ed., p. 799.
 */
static int binary_search(int *A, int *I, int x, int low, int high) {
    int mid;
    while (low < high) {
        mid = (low + high) / 2;
        if (x <= A[I[mid]])
            high = mid;
        else
            low = mid + 1;
    }
    return high;
}

/**********************************************/
/* ----- Problem-specific instantiation ----- */
/**********************************************/

/*
 * PFSP instantiation
 * Status: TENTATIVE
 * Notes:
 *   Reads files in "standard" PFSP format
 *   Needs better error checking
 */
struct problem *newProblem(const char *filename) {
    struct problem *p = NULL;
    FILE *infile;
    int j, i, n=-1, m=-1;
    infile = fopen(filename, "r");
    if (infile) {
        fscanf(infile, "%d", &n);
        fscanf(infile, "%d", &m);
        if (n > 0 && m > 0) {
            p = (problem*) malloc(sizeof (struct problem));
            p->data = (int *) malloc(n * m * sizeof (int));
            for (j = 0; j < n; j++)
                for (i = 0; i < m; i++) {
                    fscanf(infile, "%*d %d", p->data+m*j+i);
                }
            p->n = n;
            p->m = m;
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
 * Status: FINAL
 */
struct solution *allocSolution(struct problem *p) {
    int i;
    struct solution *s = (solution*) malloc(sizeof (struct solution));
    s->prob = p;
    s->data = (int*) malloc(p->n * sizeof (int));
    s->n = p->n;
    s->m = p->m;
    s->times = (int*) malloc((p->n+1) * (p->m+1) * sizeof (int));
    /* init first row and column of s->times - makespan() depends on this */
    for(i = 0; i <= p->m; i++)
        s->times[i] = 0;
    for(i = 1; i <= p->n; i++)
        s->times[(p->m+1)*i] = 0;
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
 * Status: FINAL
 */
struct pathState *allocPathState(struct problem *p) {
    struct pathState *ps = (pathState*) malloc (sizeof (struct pathState));
    ps->prob = p;
    ps->data = (int*) malloc(p->n * sizeof (int));
    ps->pred = (int*) malloc(p->n * sizeof (int));
    ps->pos = (int*) malloc(p->n * sizeof (int));
    ps->n = p->n;
    return ps;
}

/*
 * Free the memory used by a move
 * Status: FINAL
 */
void freeProblem(struct problem *p) {
    free(p->data);
    free(p);
}

/*
 * Free the memory used by a solution
 * Status: FINAL
 */
void freeSolution(struct solution *s) {
    free(s->data);
    free(s->times);
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
 * Status: FINAL
 */
void freePathState(struct pathState *ps) {
    free(ps->data);
    free(ps->pred);
    free(ps->pos);
    free(ps);
}

/* I/O  */
void printProblem(struct problem *p) {
    int i,j;
    int n = p->n;
    int m = p->m;
    printf("# Permutation Flowshop Problem (%d jobs, %d machines)\n", n, m);
    printf("%d %d\n", n, m);
    for (j = 0; j < n; j++) {
        for (i = 0; i < m; i++)
            printf(" %d",p->data[j*m+i]);
        printf("\n");
    }
}

void printSolution(struct solution *s) {
    int i;
    printf("%dx%d Flowshop solution\n  p:", s->n, s->m);
    for(i = 0; i < s->n; i++)
        printf(" %d",s->data[i]);
    printf("\n");
#if 0
    for(i = 0; i < (s->eval+1)*(s->m+1); i++) {
        printf(" %d",s->times[i]);
        if ((i+1) % (s->m+1) == 0)
            printf("\n");
    }
#endif
}

void printMove(struct move *v) {
    printf("%dx%d Flowshop move: %d, %d\n", v->prob->n, v->prob->m, v->data[0], v->data[1]);
}

void printPathState(struct pathState *ps) {
    int i;
    printf("%dx%d Flowshop path state\n", ps->n, ps->prob->m);
    printf("  Path length: %d\n  Permutation:", ps->n - ps->in_order);
    for (i = 0; i < ps->n; i++)
        printf(" %d",ps->data[i]);
    printf("\n  In order: %d elements\n  Positions:",ps->in_order);
    for (i = 0; i < ps->n; i++)
        printf(" %d",ps->pos[i]);
    printf("\n");
}


/* Solution generation */

/*
 * Generate solutions uniformly at random
 * Status: FINAL
 */
struct solution *randomSolution(struct solution *s) {
    /* solution s must have been allocated with allocSolution() */
    randperm(s->data, s->n);
    s->eval = 0;    /* Not evaluated yet */
    return s;
}

/* Solution inspection */

/*
 * Generate solutions uniformly at random
 * Status: FINAL
 * Notes:
 *   Supports incremental evaluation by makespan()
 */
double getObjectiveValue(struct solution *s) {
    if (s->eval < s->n)
        makespan(s);
    return (double)s->objvalue;
}

/* Operations on solutions*/
struct solution *copySolution(struct solution *dest, const struct solution *src) {
    dest->prob = src->prob;
    dest->n = src->n;
    dest->m = src->m;
    memcpy(dest->data, src->data, src->n * sizeof (int));
    memcpy(dest->times, src->times, (src->n+1) * (src->m+1) * sizeof (int));
    dest->objvalue = src->objvalue;
    dest->eval = src->eval;
    return dest;
}

/*
 * Apply a move to a solution
 * Status: FINAL
 * Notes:
 *   Supports incremental evaluation by makespan()
 */
struct solution *applyMove(struct solution *s, const struct move *v) {
    int i, j, el;
    /* int k; */
    i = v->data[0];
    j = v->data[1];
    if (i == j)     /* do nothing */
        return s;
    el = s->data[i];
    /* Would it be worth using memmove() here? It doesn't seem to matter much. */
    if (i < j) {
#if 0
        for (k = i; k < j; k++)
            s->data[k] = s->data[k+1];
#else
        memmove(s->data+i, s->data+i+1, (j-i) * sizeof (int)); 
#endif
        if (i < s->eval)
            s->eval = i;
    } else {
#if 0
        for (k = i; k > j; k--)
            s->data[k] = s->data[k-1];
#else
        memmove(s->data+j+1, s->data+j, (i-j) * sizeof (int)); 
#endif
        if (j < s->eval)
            s->eval = j;
    }
    s->data[j] = el;
    return s;
}

/* Move generation */

/*
 * Generate moves uniformly at random
 * Status: FINAL
 */
struct move *randomMove(struct move *v, const struct solution *s) {
    /* move v must have been allocated with allocMove() */
    int n, r, c;
    n = s->n;
    r = randint(n-2);
    c = randint(n-2);
    /* convert to actual indices */
    if(r <= c) {
        v->data[0] = r;
        v->data[1] = c+1;
    } else {
        v->data[0] = r+1;
        v->data[1] = c;
    }
    return v;
}

/*
 * Generate the next random move in a path towards a solution
 * Status: TENTATIVE, NEEDS_TESTING
 * Notes:
 *   Move distribution is not uniform
 */
struct move *nextRandomMove(struct move *v, struct pathState *ps) {
    int max = ps->in_order;
    int i, j, k, el, r, s, n = ps->n;
    int *pos = ps->pos, *data = ps->data;
    int low, high;

    if (max >= n) { /* end of path has been reached, no move is possible */
        v->data[0] = v->data[1] = 0; /* null move */
        return v;
    }

    /* Draw at random a position to move */
    r = max + randint(n-max-1);
    i = v->data[0] = pos[r];
    /* Find where it can be inserted */
#if 0
    for (s = 0; s < max; s++)
        if (data[i] <= data[pos[s]])
            break;
    printf("data[i] = %d, data[pos[s]] = %d, max = %d, s = %d\n", data[i], data[pos[s]], max, s);
    printf("s = %d, bs = %d, bss = %d\n\n", s, binary_search(data, pos, data[i], 0, max), binary_search(data, pos, data[pos[s]], 0, max));
#else
    s = binary_search(data, pos, data[i], 0, max);
#endif
    if (s == 0) {
        low = 0;
        high = pos[s];
    } else if (s == max) {
        low = pos[s-1];
        high = n-1;
    } else if (i < pos[s]) {
        low = pos[s-1];
        high = pos[s]-1;
    } else {
        low = pos[s-1]+1;
        high = pos[s];
    }
    j = v->data[1] = low + randint(high-low);

    /* Update pathState */
    ps->in_order++;
    /* replicated from applyMove() */
    el = data[i];
    if (i < j) {
        for (k = i; k < j; k++)
            data[k] = data[k+1];
#if 0
        for (k = 0; k < n; k++) /* should be split into two for loops */
            if (pos[k] > i && pos[k] <= j)
                pos[k]--;
#else
        for (k = s-1; k >= 0 && pos[k] > i; k--)
            pos[k]--;
        for (k = r+1; k < n && pos[k] <= j; k++)
            pos[k]--;
#endif
    } else {
        for (k = i; k > j; k--)
            data[k] = data[k-1];
#if 0
        for (k = 0; k < n; k++) /* should be split into two for loops */
            if (pos[k] >= j && pos[k] < i)
                pos[k]++;
#else
        for (k = s; k < max && pos[k] < i; k++)
            pos[k]++;
        for (k = r-1; k >= max && pos[k] >= j; k--)
            pos[k]++;
#endif
    }
    data[j] = el;
    /* insert the new position into pos */
#if 0
    for (k = r; k > s; k--)
        pos[k] = pos[k-1];
#else
    memmove(pos+s+1, pos+s, (r-s) * sizeof (int)); 
#endif
    pos[s] = j;

    return v;
}

/* Path generation */

/*
 * Set up a path from one solution to another
 * Status: TENTATIVE, NEEDS_TESTING
 */
struct pathState *initPathTo(struct pathState *ps, const struct solution *s1, const struct solution *s2) {
    int i, j, k, max;
    int n = ps->n;

    /* Compute reference permutation p2i[p1] using ps->pos as a buffer */
    for (i = 0; i < n; i++)
        ps->pos[s2->data[i]] = i;
    for (i = 0; i < n; i++)
        ps->data[i] = ps->pos[s1->data[i]];

    /* Compute LIS using ps->pos as a buffer */
    ps->in_order = max = lis(ps->pred, ps->pos, ps->data, n);
    k = ps->pos[max-1];

    /* Store LIS positions in ps->pos */
    for (i = j = n; i > 0;) {
        i--;
        if (i == k) {
            ps->pos[--max] = k;
            k = ps->pred[k];
        } else
            ps->pos[--j] = i;
    }
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
    return ps->n - ps->in_order;
}


