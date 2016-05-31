/*
------------------------------------------------------------------------------
  By Bob Jenkins, Spring 1989, Public Domain
  These are the data structures used in my knot programs
------------------------------------------------------------------------------
*/

#ifndef CONTROL
#define CONTROL

#include "poly.h"
#include "order.h"

/* Global variables */

Instruct   plan;       /* plan of what to do to each old weave for this step */
Poly       llplus;                                                   /* -L^2 */
Poly       lplusm;                                                    /* -LM */
Poly       lminusm;                                              /* -(L^-1)M */
Poly       llminus;                                                 /* -L^-2 */
Poly       mll;                                       /* (M^-1)(-(L^-1) - L) */
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
Poly *c_follow(Instruct *l, word num_crossings);

#endif /* ifndef CONTROL */
