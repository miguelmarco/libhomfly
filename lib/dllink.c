/*
------------------------------------------------------------------------------
 dllink.c - routines for dealing with doubly linked, circular lists
 Bob Jenkins, 1990.  Public Domain.
------------------------------------------------------------------------------
*/
#include <stdlib.h>
#include <gc.h>

#include "standard.h"
#include "dllink.h"

/*
------------------------------------------------------------------------------
  This displays all the nodes in a single string
------------------------------------------------------------------------------
*/
void l_show(dllink *l)
{
  dllink  *count;

  if ((count = l) != 0)
  {
    //printf("%d ", count->c);
    count = count->z;
    while (count != l)
    {
      //printf("%d ", count->c);
      count = count->z;
    }
  }
  //printf("\n");
}


/*
------------------------------------------------------------------------------
    This takes a dllink inp, adds a dllink before it, and places a
    pointer to the added dllink in outp
------------------------------------------------------------------------------
*/
void l_add(dllink *inp,     /* the next crossing in this string */
	         word crossings,  /* the new crossing number */
	         dllink **outp)   /* out: pointer to the new crossing */
{
  dllink  *newlink;

  newlink    = (dllink *)GC_MALLOC(sizeof(dllink));
  newlink->c = crossings;
  if (inp == 0)
  {
    newlink->a = newlink;
    newlink->z = newlink;
    inp        = newlink;
  }
  else
  {
    newlink->a = inp->a;
    newlink->z = inp;
    inp->a->z  = newlink;
    inp->a     = newlink;
  }
  *outp = inp->a;
}




/*
------------------------------------------------------------------------------
  This deletes a given dllink from a loop
------------------------------------------------------------------------------
*/
void   l_del(l)
dllink **l;
{
  dllink  *ll = *l;

  if (!ll) return;
  if (ll->a != ll)
  {
    ll->z->a = ll->a;
    ll->a->z = ll->z;
  }
  //free((char *)ll);
  *l = (dllink *)0;
}
