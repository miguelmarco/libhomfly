/**
------------------------------------------------------------------------------
  HOMFLY.C
  Main functions for the library
------------------------------------------------------------------------------
*/

#include <stdlib.h>
#include <gc.h>

#include "standard.h"
#include "poly.h"
#include "knot.h"
#include "homfly.h"
#include "order.h"
#include "control.h"

char *homfly_str(char *argv)
{
  Poly      *answer;              /* HOMFLY polynomial for the original link */
  char      *out;

  answer = homfly(argv);
  out = p_show(answer);                               /* display the answer */
  //free((char *)*knot);
  //free((char *)*plan);
  //p_kill(answer);
  /* If you want to be thorough, free the polynomials defined in c_init */

  //free (llplus.term);
  //free (lplusm.term);
  //free (lminusm.term);
  //free (llminus.term);
  //free (mll.term);

  return out;
}

Poly *homfly(char *argv)
{
  crossing  *link;          /* link for which to calculate HOMFLY polynomial */
  word       num_crossings;                   /* number of crossings in link */

  k_read(&num_crossings, &link, argv);                          /* read link */
  return c_homfly(link, num_crossings);
}

/**
 * Compute the homfly polynomial and return the result as the polynomial answer.
 */
Poly *c_homfly(crossing *link, word num_crossings)
{
  Instruct  *plan;                                   /* list of instructions */

  c_init();                                          /* initialize variables */

  o_make(link, num_crossings, &plan);    /* make plan for attacking the link */
  return c_follow(plan, num_crossings);                   /* follow the plan */
}
