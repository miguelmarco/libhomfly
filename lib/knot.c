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
boolean k_read(Link **link, char *filename)
{
  char       name[20];
  word       links,
             startwhere,
             where,
             startover,
             over,
             i,
             j;
  int        num_crossings;
  crossing  *kk;
  crossing   k[MAXCROSS];
  FILE      *f;

  for (i = 0; i < MAXCROSS; i++) k[i].hand = 0;

  f = fmemopen(filename, strlen(filename), "r");
  if (f == 0)
  {
    fclose(f);
    return FALSE;
  }
  fscanf(f, "%d ", &links);
  for (i=0; i<links; ++i)                    /* how many pieces of string */
  {
    fscanf(f, "%d ", &num_crossings);
    fscanf(f, "%d %d ", &startwhere, &startover);
    if (startover == 1)
      l_add((dllink *)0, startwhere, &k[startwhere].o);
    else
      l_add((dllink *)0, startwhere, &k[startwhere].u);
    for (j=1; j<num_crossings; ++j)
    {
      fscanf(f, "%d %d ", &where, &over);

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
  while (fscanf(f, "%d %d ", &where, &over) == 2)
  {
    k[where].hand = over;
    if (where > num_crossings) num_crossings = where;
  }
  ++num_crossings;
  i = fclose(f);

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

