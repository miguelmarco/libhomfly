/*
------------------------------------------------------------------------------
  By Bob Jenkins, Spring 1989, Public Domain
  These are the data structures used in my knot programs
------------------------------------------------------------------------------
*/

#ifndef POLY
# include "poly.h"
#endif
#ifndef ORDER
# include "order.h"
#endif

#ifndef CONTROL
#define CONTROL

/* Global variables */

instruct   plan;       /* plan of what to do to each old weave for this step */
poly       llplus;                                                   /* -L^2 */
poly       lplusm;                                                    /* -LM */
poly       lminusm;                                              /* -(L^-1)M */
poly       llminus;                                                 /* -L^-2 */
poly       mll;                                       /* (M^-1)(-(L^-1) - L) */
ub4        total_weaves_handled;             /* how much work has been done? */

/*
------------------------------------------------------------------------------
  Procedures defined in control.c
------------------------------------------------------------------------------
*/

void   c_init(void);                        /* Initialize global polynomials */

/* c_handle: Merge the tag of a new weave with the list of all new weaves */
void   c_handle(word *list, struct weave *thisweave, struct weave *newweaves);

/* c_follow: Follow all instructions, return the poly for the original link */
void   c_follow(struct instruct *l, word crossings, struct poly **answer);

#endif /* ifndef CONTROL */

/*
------------------------------------------------------------------------------
  Here's a sketch of the algorithm:
  The Homfly polynomial is calculated using a recursive formula that replaces
a given link with two intermediate links, and associates a polynomial with
each intermediate link.  The two new links differ at the single crossing
at which the recursive formula was applied.
  The base case is always links with no crossings.
  The obvious will produce a binary tree with about 2^n nodes, where each
node is an intermediate link and all the leaf links have no crossings.
  This approach applies the recursion formula to the crossings in all
intermediate links in the same order, which encourages duplicates among the
intermediate links.  Duplicate links are collected, their polynomials are
summed, then only one copy of each type of intermediate link needs to be
dealt with.
  The number of intermediate links at any point is n! or less, where n is the
ceiling(sqrt(#crossings)) or less.  Usually n never needs to exceed
sqrt(#crossings)/2, where #crossing is the number of crossings in the original
link.
------------------------------------------------------------------------------
*/
