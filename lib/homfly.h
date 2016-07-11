/**
 * Main functions to be called from the library
 *
 * ----------------------------------------------------------------------------
 *  Here's a sketch of the algorithm:
 *    The Homfly polynomial is calculated using a recursive formula that
 *  replaces a given link with two intermediate links, and associates a
 *  polynomial with each intermediate link.  The two new links differ at
 *  the single crossing at which the recursive formula was applied.
 *    The base case is always links with no crossings.
 *    The obvious will produce a binary tree with about 2^n nodes, where each
 *  node is an intermediate link and all the leaf links have no crossings.
 *    This approach applies the recursion formula to the crossings in all
 *  intermediate links in the same order, which encourages duplicates among the
 *  intermediate links.  Duplicate links are collected, their polynomials are
 *  summed, then only one copy of each type of intermediate link needs to be
 *  dealt with.
 *    The number of intermediate links at any point is n! or less, where n is
 *  the ceiling(sqrt(#crossings)) or less.  Usually n never needs to exceed
 *  sqrt(#crossings)/2, where #crossing is the number of crossings in the
 *  original link.
 * ----------------------------------------------------------------------------
 */

#ifndef HOMFLY_H
#define HOMFLY_H

#ifndef UB4MAXVAL
typedef int  word; 
typedef    signed long  int  sb4;
typedef  unsigned short int  ub2;
typedef    signed short int  sb2;
#endif

#ifndef KNOT
struct crossing
{
    struct dllink *o;                                              /* overpass */
    struct dllink *u;                                             /* underpass */
    int      hand; /* 1 if right handed, -1 if left, 0 if no longer a crossing */
};

typedef struct crossing crossing;

struct Link
{
    crossing *data;
    int num_crossings;
};
typedef struct Link Link;

#endif

#ifndef POLY
struct Term
{
    sb4    coef;
    sb2    m;
    sb2    l;
};
typedef struct Term Term;


struct Poly
{
    Term  *term;
    sb4     len;
};
typedef struct Poly Poly;
#endif



char *homfly_str(char *argv);

Poly *homfly(char *argv);

/* c_homfly: Compute the homfly polynomial for the link */
Poly *c_homfly(Link *link);

#endif // HOMFLY_H
