/*
------------------------------------------------------------------------------
  By Bob Jenkins, 1989, in relation to my Master's Thesis.  Public Domain.
  This module handles entire knots.
------------------------------------------------------------------------------
*/
#include <stdlib.h>
#include <gc.h>
#include <string.h>

#include "standard.h"
#include "dllink.h"
#include "knot.h"

/*
------------------------------------------------------------------------------
  Assumes the dllinks all form cycles
  This displays each string once by listing its crossings in order.
  After that, it displays the HANDedness of each crossing.
------------------------------------------------------------------------------
*/
void k_show(Link *link)
{
  word   i, j;
  word   tab[MAXCROSS];
  dllink  *count, *start;

  for (i=0; i<link->num_crossings; ++i) tab[i] = 0;
  for (j=0; j<2; ++j)
  {
    for (i=0; i<link->num_crossings; ++i)
    {
      if (link->data[i].hand && ((tab[i] == 0) || (tab[i] == 1) || (tab[i] == 10)))
      {
        if (tab[i] != 10)
          count = link->data[i].o;
        else
          count = link->data[i].u;
        start = count;

        if (link->data[count->c].o == count)
        {
          tab[count->c] += 10;
        }
        else
        {
          tab[count->c] += 1;
        }
        if (count != 0)
          count = count->z;
        while (count != start)
        {
          if (link->data[count->c].o == count)
            tab[count->c] += 10;
          else
            tab[count->c] += 1;

          count = count->z;
        }
      }

    }
  }

}


/*
------------------------------------------------------------------------------
  Assumes the file given by the user exists and contains a legal knot.
------------------------------------------------------------------------------
*/
boolean k_read(Link **link, char *f)
{
  char       name[20];
  word       links,
             startwhere,
             where,
             startover,
             over,
             i,
             j;
  int        num_crossings, pos;
  crossing  *kk;
  crossing   k[MAXCROSS];

  for (i = 0; i < MAXCROSS; i++) k[i].hand = 0;

  if (f == 0)
  {
    return FALSE;
  }
  sscanf(f, "%d%n", &links, &pos);
  f += pos;
  for (i=0; i<links; ++i)                    /* how many pieces of string */
  {
    sscanf(f, "%d%n", &num_crossings, &pos);
    f += pos;
    sscanf(f, "%d %d%n", &startwhere, &startover, &pos);
    f += pos;
    if (startover == 1)
      l_add((dllink *)0, startwhere, &k[startwhere].o);
    else
      l_add((dllink *)0, startwhere, &k[startwhere].u);
    for (j=1; j<num_crossings; ++j)
    {
      sscanf(f, "%d %d%n", &where, &over, &pos);
      f += pos;

      /* check that OVER is legal */
      if ((over != 1) && (over != -1))
      {
        //printf("string %d, %dth crossing (%d %d), %d is neither 1 nor -1\n",
        //       i,j,where,over,over);
        return FALSE;
      }
      if (over == 1)
      {
        if (startover == 1)
          l_add(k[startwhere].o, where, &k[where].o);
        else
          l_add(k[startwhere].u, where, &k[where].o);
      }
      else
      {
        if (startover == 1)
          l_add(k[startwhere].o, where, &k[where].u);
        else
          l_add(k[startwhere].u, where, &k[where].u);
      }
    }
  }
  num_crossings = 0;
  while (sscanf(f, "%d %d%n", &where, &over, &pos) == 2)
  {
    f += pos;
    k[where].hand = over;
    if (where > num_crossings) num_crossings = where;
  }
  ++num_crossings;

  kk = (crossing *)GC_MALLOC(sizeof(crossing) * num_crossings);
  for (i=0; i < num_crossings; ++i) kk[i] = k[i];

  /* check that every crossing has an overpass and underpass */
  for (i=0; i<num_crossings; ++i)
  {
    if (!k[i].o || !k[i].u || !(k[i].hand == 1 || k[i].hand == -1))
      return FALSE;
  }

  *link = (Link*)GC_MALLOC(sizeof(Link));
  (*link)->num_crossings = num_crossings;
  (*link)->data = kk;
  return TRUE;
}

