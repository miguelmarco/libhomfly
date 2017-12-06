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
  Link *link;
  GC_INIT();
  k_read(&link, argv);                          /* read link */
  return c_homfly(link);
}

/**
 * Compute the homfly polynomial and return the result as the polynomial answer.
 */
Poly *c_homfly(Link *link)
{
  Instruct  *plan;                                   /* list of instructions */

  c_init();                                          /* initialize variables */

  o_make(link, &plan);                   /* make plan for attacking the link */
  return c_follow(plan, link->num_crossings);             /* follow the plan */
}
