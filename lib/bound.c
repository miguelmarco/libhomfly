/*
------------------------------------------------------------------------------
  BOUND.C
  By Bob Jenkins, in association with his Masters Thesis
  Routines dealing with boundary manipulations of weaves
  Public Domain
------------------------------------------------------------------------------
*/
#include <stdlib.h>
#include <gc.h>

#include "standard.h"
#include "order.h"
#include "control.h"
#include "bound.h"

/*
------------------------------------------------------------------------------
B_MANIP computes values of the global boundary variables.
A step is either adding a crossing, adding a crossing and removing a pair of
boundary crossings, or just removing a pair of boundary crossings.
------------------------------------------------------------------------------
*/
word list[BIGWEAVE];              /* description of first new weave */
word list2[BIGWEAVE];           /* description of second, if needed */
word old_going_in[BIGWEAVE];/* Was *i* an input? *old_going_in[i]*. */
word going_in[BIGWEAVE];    /* Will *i* be an input? *going_in[i]*. */
word map[BIGWEAVE];   /* i of old weave becomes map[i] of new weave */
word first;                    /* first boundary crossing to remove */
word second;                  /* second boundary crossing to remove */
word right;          /* Is the crossing being added be righthanded? */
word oldcross;     /* number of boundary crossings in the old weave */
word newcross;    /* number of boundary crossings in each new weave */
word oldin;                    /* number of inputs to the old weave */
word newin;                    /* number of inputs to the new weave */

void b_manip(weave *oldweaves)
{
  word       i, j, k;
  ub4        boundary[2];

  oldcross = plan.oldn;             /* number of original boundary crossings */
  oldin = oldcross/2;                           /* number of original inputs */
  newcross = plan.newn;  /* number of boundary crossings in resulting weaves */
  newin = newcross/2;                                          /* " inputs " */

  /*--------------------------------------------------- Compute old_going_in */
  for (i = 0; !oldweaves[i].tag.len; i++) ;          /* find a defined weave */
  boundary[0] = oldweaves[i].boundary[0];
  boundary[1] = oldweaves[i].boundary[1];
  for (i = 0; i < oldcross; i++)
    old_going_in[i] = 1;                 /* pretend all crossings are inputs */
  for (i = 0; i < oldin; i++)
  {
    k = (i < 6) ? 0 : 1;
    if (old_going_in[(boundary[k] & 0x1f)] == 0) {
      printf("cannot have both ends of a string be an output\n");
    }
    old_going_in[(boundary[k] & 0x1f)] = 0;            /* unmark the outputs */
    boundary[k] >>= 5;
  }
  right = plan.right;
  if (plan.which != -1)
  {
    for (i = 0; i < oldcross; ++i)
    {
      if (old_going_in[i] != plan.going_in[i]) {
	printf("going_in is messed up\n");
	break;
      }
    }
  }

  /*--------------------------- Set first and second, if they need to be set */
  first  = plan.r0[0];
  second = plan.r1[0];
  if (plan.reductions && plan.which != -1)   /* crossing added, pair removed */
  {
    if (first > plan.which+1) --first;
    if (second > plan.which+1) --second;
    if (first > plan.which) --first;
    if (second > plan.which) --second;
  }
  else ;                                  /* crossing added, no pair removed */

  /*---------------------------------------------------------------- Set map */
  if (plan.reductions == 0)
  {
    for (i = 0, j = 0; i < oldcross+2; ++i)
      if ((i != plan.which) && (i != plan.which+2)) map[j++] = i;
  }
  else if (plan.which == (-1))
  {
    for (i = 0, j = 0; j < oldcross; ++j)
      if ((j != plan.r0[0]) && (j != plan.r1[0])) map[j] = i++;
  }
  else if (plan.r0[0] > plan.r1[0])
  {
    for (i = 0; i < oldcross; i++) map[i] = i;
    map[first]  = (second);
    map[second] = (first);
  }
  else if (plan.which == 0)
  {
    for (i = 0; i < oldcross; i++) map[i] = i+1;
    map[0]   = 0;
    map[oldcross-1] = 1;
  }
  else
  {
    for (i = 0; i < oldcross; i++) map[i] = i-1;
    map[0]          = oldcross-2;
    map[oldcross-1] = oldcross-1;
  }

  /*----------------------------------------------------------- Set going_in */
  if (plan.which != (-1))
  {
    for (i = 0; i < oldcross; i++)
    {
      going_in[map[i]] = old_going_in[i];
    }
  }
  else
  {
    for (i = 0; i < oldcross; i++)
    {
      if ((i != first) && (i != second))
      {
        going_in[map[i]] = old_going_in[i];
      }
    }
  }

  if (plan.reductions == 0)
  {
    going_in[plan.which]   = plan.prev;
    going_in[plan.which+2] = !plan.prev;
  }
}




/*
------------------------------------------------------------------------------
B_NO_PAIRS adds a single crossing to a single weave.
Either the crossing is correct, in which case *one* will be set, or it is
wrong.  In this case no more than two weaves are needed, so *two* will be set.
------------------------------------------------------------------------------
*/
void  b_no_pairs(word  *list,  /* list of original inputs/outputs, modified by this routine */
                 word  *list2,                    /* inputs/outputs of the second new weave */
                 word  *one,                                 /* will one new weave suffice? */
                 word  *two)                                /* will two new weaves suffice? */
{
  word  i,
        a,
        b,
        c;


  /*-------------------------------------------------- Make the new boundary */
  for (i = oldcross; --i >= 0;) list[map[i]] = map[list[i]];
  a         = plan.which+1;
  list[a-1] = a+1;
  list[a+1] = a-1;

  /*------------------------- Decide if the crossing needs to be operated on */
  c = a-1;
  b_right(list, going_in, c, a, i);
  *one = (i == right);
  *two = !*one;
  if (*two)
  {
    for (i = newcross; --i >= 0;) list2[i] = list[i];
    b               = (plan.prev == going_in[a]) ? a-1 : a+1;
    c               = (plan.prev == going_in[a]) ? a+1 : a-1;
    list2[list2[a]] = b;
    list2[b]        = list2[a];
    list2[a]        = c;
    list2[c]        = a;
  }
}



/*
------------------------------------------------------------------------------
B_ONE_PAIR handles adding one crossing and removing one pair of boundary
crossings.  This may require replacing the original weave with one, two, or
more new weaves.  If more than two are needed, do not set *one* or *two*.
------------------------------------------------------------------------------
*/
void b_one_pair(list, list2, one, two)
word  *list;    /* list of original inputs/outputs, modified by this routine */
word  *list2;                      /* inputs/outputs of the second new weave */
word  *one;                                   /* will one new weave suffice? */
word  *two;                                  /* will two new weaves suffice? */
{
  word   a;
  word   i;
  word   crossed;
  word   shouldBeRight;
  word   temp[BIGWEAVE];

  /*---------------------------------- Check for a couple easy to spot cases */
  a = plan.which;
  if ((plan.r0[0] == a+1) || (plan.r1[0] == a+1))
  {                                            /* A Type I Reidemeister move */
    *one = 1;
    return;
  }

  /*---------------- Handle the cases where the reduction is the 0..n-1 jump */
  if (plan.r0[0] < plan.r1[0])
  {
    if (old_going_in[0] && old_going_in[oldcross-1]) return;
    crossed = b_cross(first, second, list[first], list[second]);
    b_right(list, old_going_in, first, second, shouldBeRight);
                                                              /* werecrossed */
    if (list[first] == second)
    {
        /* A Type I Reidemeister move */
      for (i = oldcross; --i >= 0; ) temp[map[i]] = list[i];
      for (i = oldcross; --i >= 0; ) list[i] = map[temp[i]];
      *one = 1;
      return;
    }

    if (old_going_in[first] != old_going_in[second])
    {
        /* this may be complicated */
        return;
    }

    for (i = oldcross; --i >= 0; ) temp[map[i]] = list[i];
    for (i = oldcross; --i >= 0; ) list[i] = map[temp[i]];
    if (!old_going_in[first] && !old_going_in[second] &&
        (crossed ^ shouldBeRight ^ right))
    {
      *two = 1;
      for (i = 0; i < newcross; i++) list2[i] = list[i];
      if (plan.which == 0) b_switch(list2, 1, 0, i);
      else b_switch(list2, newcross-1, newcross-2, i);
    }
    else *one = 1;
    return;
  }

  /*-------- Handle the cases where the numbers on the boundary are adjacent */
  if (list[first] == second)                   /* A Type I Reidemeister move */
  {
    *one = 1;
    return;
  }
  crossed = b_cross(first, second, list[first], list[second]);/* werecrossed */
  b_right(list, old_going_in, first, second, a); /* old crossing righthanded */
  b_switch(list, first, second, i);         /* switch the boundary crossings */
  b_right(list, going_in, first, second, i);    /* new should be righthanded */
  *one = (((crossed) && (a != right)) || ((!crossed) && (i == right)));
  if (*two = !*one)
  {
    if (old_going_in[first] != old_going_in[second])
    {
      *two = 0;
      b_switch(list, first, second, i);
    }
    else
    {
      for (i = newcross; --i >= 0; ) list2[i] = list[i];
      b_switch(list2, first, second, i);
    }
  }
}

