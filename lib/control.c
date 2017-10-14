/*
------------------------------------------------------------------------------
By Bob Jenkins, revamped August 1990, in relation to my masters thesis
Public Domain
------------------------------------------------------------------------------
*/
#include <stdlib.h>
#include <gc.h>

#include "standard.h"
#include "poly.h"
#include "knot.h"
#include "order.h"
#include "bound.h"
#include "model.h"
#include "control.h"

Instruct   plan;
Poly   llplus;
Poly   lplusm;
Poly   lminusm;
Poly   llminus;
Poly   mll;
ub4    total_weaves_handled;

/* these variables are global, and may be because nobody ever changes them */
void c_init()
{
  Poly          blank[1];

  p_init(blank);
  p_term((-1), 0, 2, blank, &llplus);
  p_term((-1), 1, 1, blank, &lplusm);
  p_term((-1), 1, (-1), blank, &lminusm);
  p_term((-1), 0, (-2), blank, &llminus);
  p_init(&mll);
  p_term((-1), (-1), 1, &mll, blank);
  p_kill(&mll);
  p_term((-1), (-1), (-1), blank, &mll);
  p_kill(blank);
  total_weaves_handled = 0;
}


void c_handle(word *list,   /* matching of inputs/outputs in thisweave */
              weave *thisweave,             /* the weave to be handled */
              weave *newweaves)                  /* all the new weaves */
/*  Given a simple weave WEAVE, this figures out what BODY should be,
    and adds the weave to the list NEWWEAVES */
/*  Rather than keeping a heap or tree of new weaves, I have assumed that every
    permutation will probably be used.  I use a total ordering on the
    set of permutations as an index to my array of all possible new weaves. */
/*  This is one of the most used procedures in this program */
{
  Poly   temp[1];
  word   i, j, k;
  word   sum;
  word   prod;
  word   count;
  ub4    boundary[2];
  word   inputs[BIGWEAVE];

  ++total_weaves_handled;
  boundary[0] = 0;
  boundary[1] = 0;
  /*  Make a list of which inputs go to which outputs; compute *boundary* */
  for (i = newin, j = newcross; i--;)
  {
    while (!going_in[--j]);
    inputs[i] = list[j];
    k = (i < 6) ? 0 : 1;
    boundary[k] = inputs[i] + (boundary[k] << 5);
  }

  /*  Determine this weave's index in the array of newweaves */
  sum  = 0;
  prod = 1;
  for (i = 1; i < newin; ++i)
  {
    prod *= i;
    count = 0;
    for (j = i, k = inputs[i]; --j >= 0;) if (k > inputs[j]) ++count;
    sum += count * prod;
  }

  newweaves[sum].boundary[0] = boundary[0];
  newweaves[sum].boundary[1] = boundary[1];
  p_add(&newweaves[sum].tag, &thisweave->tag, temp);
  p_kill(&thisweave->tag);
  p_kill(&newweaves[sum].tag);
  newweaves[sum].tag = *temp;
}


/*  This does all the weave manipulation associated with adding a single
    crossing to a single weave.                                          */
static void c_do_one_weave(
   weave *oldweave,         /* the single weave to apply the instructions to */
   weave *newweaves)                          /* the array of all new weaves */
{
  word    one = 0;
  word    two = 0;
  word    i, j, k;
  ub4     boundary[2];
  weave   other[1];
  Poly    temp[1];

  /*-------------------------------------------------- undecode the boundary */
  boundary[0] = oldweave->boundary[0];
  boundary[1] = oldweave->boundary[1];
  for (j = 0, i = 0; i < oldin; ++i, ++j)
  {
    while (!old_going_in[j]) j++;                     /* find the next input */
    k = (i < 6) ? 0 : 1;
    list[j]       = boundary[k] & 0x1f;                /* set boundary input */
    boundary[k] >>= 5;
    list[list[j]] = j;                                /* set boundary output */
  }


  /*----------------------------------------------- Try to handle it quickly */
  if (plan.reductions == 0)
    b_no_pairs(list, list2, &one, &two);
  else if (plan.which >= 0)
    b_one_pair(list, list2, &one, &two);

  /*----------------------- Is the old weave replaced by only one new weave? */
  if (one)
  {
    c_handle(list, oldweave, newweaves);
    return;
  }

  /*--------------------------- Is the old weave replaced by two new weaves? */
  if (two)
  {
    if (right)
    {
      p_mult(&lminusm, &oldweave->tag, &other->tag);
      p_mult(&llminus, &oldweave->tag, temp);
      p_kill(&oldweave->tag);
      oldweave->tag = *temp;
    }
    else
    {
      p_mult(&lplusm, &oldweave->tag, &other->tag);
      p_mult(&llplus, &oldweave->tag, temp);
      p_kill(&oldweave->tag);
      oldweave->tag = *temp;
    }
    c_handle(list, oldweave, newweaves);
    c_handle(list2, other, newweaves);
    return;
  }

  /*------------------- The old weave is replace by up to 2^(n-1) new weaves */
  m_model_weave(list, oldweave, newweaves);
}



/*
------------------------------------------------------------------------------
  NAME:
    C_FOLLOW()
  FUNCTION:
    Given a list of *crossings* instructions *l*, follow them and produce the
    polynomial *answer* which is the HOMFLY polynomial for the original link.
  METHOD:
    For each instruction,
      Break the instruction into (add crossing/remove pair), (remove pair), ...
      (Each operation changes the size of the weave boundary from n inputs
       to either n+2 (add), n (add/remove), or n-2 (remove) crossings)
      For each of the (oldn/2)! old weaves, apply the instruction and merge
       the result into the list of (newn/2)! new weaves.
    At the end (n/2)=1, so there are (n/2)! = 1 final weaves, and the
     tag associated with that weave is *answer* .
  NOTES:
    The tags for the weaves are being freed immediately after they are
    last used by whichever procedure last uses them.  They are not being
    freed in the procedures which MALLOC them.  Before you change where
    things are freed, think.
------------------------------------------------------------------------------
*/

Poly *c_follow(Instruct *l, int num_crossings)
{
  word       i, j, k;
  ub4        oldfact;
  ub4        newfact;
  weave     *oldweaves, *newweaves;
  extern Instruct plan;

  /*---------------------------------------------- Set up the starting weave */
  oldweaves             = (weave *)GC_MALLOC(sizeof(weave));
  oldweaves[0].boundary[0] = 1;
  oldweaves[0].boundary[1] = 0;
  oldweaves->tag.len = 0;
  p_term(1, 0, 0, &oldweaves->tag,
         &oldweaves->tag);                      /* tag of original link is 1 */

  /*-------------- For each crossing, follow the instructions on every weave */
  for (i = 0; i < num_crossings; ++i)
  {
    /*----- Handle adding the crossing, plus the first boundary pair removal */
    plan            = l[i];
    plan.reductions = (plan.reductions > 0);
    plan.newn       = plan.oldn + 2 - 2*plan.reductions;
    b_manip(oldweaves);
    for (j = 2, newfact = 1; j <= newin; newfact *= (j++)) ;
    newweaves = (weave *)GC_MALLOC(sizeof(weave) * newfact);
    for (j = 0; j < newfact;) newweaves[j++].tag.len = 0;
    for (j = 2, oldfact = 1; j <= oldin; oldfact *= (j++)) ;
    for (j = 0; j < oldfact; ++j)
    {
      if (oldweaves[j].tag.len)
      {
        c_do_one_weave(oldweaves + j, newweaves);
      }
    }
    //free((char *)oldweaves);
    oldweaves = newweaves;
    // printf("firstbatch\n");
    /*for (j = 0; j < newfact; ++j)
    {
      if (newweaves[j].tag.len != 0)
      {
        p_show(&newweaves[j].tag);
      }
    }*/

    /*----------- Remove any other pairs of boundary crossings one at a time */
    for (k = 1; k < l[i].reductions; ++k)
    {
      oldfact    = newfact;
      plan.which = -1;
      plan.r0[0] = l[i].r0[k];
      plan.r1[0] = l[i].r1[k];
      plan.oldn  = plan.newn;
      plan.newn -= 2;
      b_manip(oldweaves);
      newfact   = oldfact / oldin;
      newweaves = (weave *)GC_MALLOC(sizeof(weave) * newfact);
      for (j = 0; j < newfact;) newweaves[j++].tag.len = 0;
      for (j = 0; j < oldfact; j++)
      {
        if (oldweaves[j].tag.len)
        {
          c_do_one_weave((oldweaves+j), newweaves);
        }
      }
      //free((char *)oldweaves);
      oldweaves = newweaves;
      // printf("reduction %d\n", k);
    }

    /*------------------------ Gloat about how little work needed to be done */

  }


  /*-------------------------------------------- Return the final polynomial */
  return &oldweaves->tag;
}
