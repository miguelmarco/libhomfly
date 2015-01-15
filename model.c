/*
------------------------------------------------------------------------------
  MODEL.C - Bob Jenkins, 1989.  Public Domain.

  When weaves do not fit in to one of the special cases handled in BOUND.C, it
  becomes necessary to actually model the weave (that is, build strings that
  cross over each other) and manipulate the model.  The model is copied and 
  manipulated until the original weave has been replaced by a set of simple
  weaves.  c_handle() is called directly whenever a simple weave is produced.
  A single weave may need a set of as many as 2^oldin simple weaves to replace
  it.  Refer to my masters thesis for a description and justification of the
  algorithm used to replace weaves with simple weaves.
------------------------------------------------------------------------------
*/

/*  A program by Bob Jenkins, Spring 1989 */
#ifndef STANDARD
#include "standard.h"
#endif
#ifndef POLY
#include "poly.h"
#endif
#ifndef ORDER
#include "order.h"
#endif
#ifndef BOUND
#include "bound.h"
#endif
#ifndef CONTROL
#include "control.h"
#endif
#ifndef MODEL
#include "model.h"
#endif

/* Since models are used only underneath the routines that create them,
   the memory they use actually comes from local variables.  This cuts 
   down the time and space for memory allocation/deallocation incredibly. */

/* Create a copy of the weave modeled in model1 in model2. */
static void   m_copy(node **model1,
		     node **model2,
		     char  *mem)
{
  word   i;
  word   offset = 0;
  node  *nodemem = (node *)mem;
  node  *this,
        *t;
  node  *tab[BIGWEAVE][BIGWEAVE];

  for (i = 0; i < newcross; i++)
  {
    for (this = model1[i]; this != 0; this = this->z)
    {
      tab[this->o1][this->o2] = 0;
    }
  }
  for (i = 0; i < newcross; i++)
  {
    if (model1[i] != 0)
    {
      model2[i] = (node *)(&nodemem[offset++]);
      this      = model1[i];
      t         = model2[i];
      while (this != 0)
      {
        *t = *this;
        if (tab[t->o1][t->o2] != 0)
        {
          t->m    = tab[t->o1][t->o2];
          t->m->m = t;
        }
        else tab[t->o1][t->o2] = t;
        if (this->z != 0)
        {
          t->z = (node *)(&nodemem[offset++]);
        }
        else t->z = 0;
        this = this->z;
        t    = t->z;
      }
    }
    else model2[i] = 0;
  }
}



/* Display the structure of the model *model* */
void   m_show(node **model)
{
  word   i;
  node  *t;
  for (i = 0; i < newcross; i++)
    if (model[i] != 0)
    {
      printf("SHOWING STRING %d\n", i);
      for (t = model[i]; t != 0; t = t->z)
      {
        printf("%d %d %d  %d %d %d %d  %d\n", t->correct, t->over,
               t->right, t->self, t->m->self, t, t->m, t->z);
      }
    }
}



/* Often it can be proven that no bad crossings are or ever will be in a
   string.  In this case, eliminate all crossings the string is involved
   in, eliminate the memory associated with the string, and eliminate the
   string.  This reduces the complexity of the model.  */
static void   m_string_kill(node **model, word where)
{
  word   i,
         done;
  node  *t;

  for (i = newcross; --i >= 0;)
  {
    if ((model[i]) && (i != where))
    {
      for (done = 0; !done;)
      {
        done = !model[i]->z;
        if (model[i]->m->self == where)
        {
          model[i] = model[i]->z;
        }
        else done = 1;
      }
      if (model[i]) 
      {
        for (t = model[i]; t->z;)
        {
          if (t->z->m->self == where)
          {
	    t->z = t->z->z;
          }
	  else t = t->z;
	}
      }
    }
  }
  model[where] = 0;
}


/* Should string *a* cross string *x* before string *b*? */
/* Positions of crossings are determined by the lowest numbered boundary
   crossing of a string.  Overpass/underpass is determined by the crossing
   of the string which is the input.  m_before() is concerned with the former
   order, while the rest of this project cares mostly about the latter order.
   Thus all the (list[a] < a), *backwards*, and so on in this routine.   */
static word  m_before(word x, word a, word b, word *list)
{
  word  backwards;

  if (backwards = (list[x] < x)) x = list[x];
  if (list[a] < a) a = list[a];
  if (list[b] < b) b = list[b];
  if (a == b) return 0;
  if (b_cross(a, b, list[a], list[b])) return((a < b) != backwards);
  a = ((x < a) && (a < list[x])) ? a : list[a];
  b = ((x < b) && (b < list[x])) ? b : list[b];
  return((a < b) != backwards);
}



/* Put the crossings in the simple weave being modeled in the correct places.
   This is called "a simple weave in standard form" in my thesis.  */
static void   m_sort(word i, node **model, word *list)
/*  This assumes that the weave is simple */
{
  word   done = 0;
  node **t,
        *temp,
        *temp2;

  while (!done)
  {
    for (done = 1, t = (&(model[i])); (*t)->z; t = (&((*t)->z)))
    {
      if (m_before(i, (*t)->z->m->self, (*t)->m->self, list))
      {
        temp       = (*t)->z->z;
        (*t)->z->z = (*t);
        temp2      = (*t)->z;
        (*t)->z    = temp;
        (*t)       = temp2;
        done       = 0;
      }
    }
  }
}



/* Are the correct strings overpasses in all the crossings of this string? */
static void   m_correct(node **model, word string)
{
  node  *t;

  if (model[string] && model[string]->self != string)
  {
      printf("how can that be bob\n");
  }

  for (t = model[string]; t != 0; t = t->z)
  {
    word  self = t->self;
    word  other = t->m->self;
    t->correct = (t->over == (self < other));
    if (self == other) t->correct = 1;
    t->m->correct = t->correct;
  }
}



/* Break, as in the HOMFLY recursion formula */
static void   m_k_break(node **at, node **bt)
{
  word   a,
         b;
  node  *c;

  a   = (*at)->self;
  b   = (*bt)->self;
  c   = (*at);
  *at = (*bt)->z;
  *bt = c->z;
  for (c = (*at); c != 0; c = c->z) c->self = a;
  for (c = (*bt); c != 0; c = c->z) c->self = b;
}



/* Switch, as in the HOMFLY recursion formula */
static void   m_k_switch(node *at, node *bt)
{
  at->right   = !at->right;
  at->over    = !at->over;
  at->correct = !at->correct;
  bt->right   = !bt->right;
  bt->over    = !bt->over;
  bt->correct = !bt->correct;
}



/* Given a model *model* with boundary crossings *list*, either confirm that
   *model* models a simple weave, or find a crossing to apply the HOMFLY
   recursion formula to.

   A simple weave is a circle with strings going in and out of it.  Its 
   internal structure is determined by the order of the strings going in
   and out of it.  Arrange its border in a right triangle, with the diagonal
   going from bottom left to upper right, and number the crossings 0..n-1
   along that diagonal.  Have all the strings form right angles, going
   straight up then to the right.  I suppose it's really straight left then
   down if the input comes after the output.  That layout determines where
   the crossings should be.  If two strings cross, the string with the lower
   input should be the overpass.
*/
static void    m_recurse(node  **model,
			 word   *list,
			 weave  *oldweave,
			 weave *newweaves)
{
  word    i,
          j,
          wasright,
          mfirst,
          msecond,
          mtop,
          mbot,
          list2[BIGWEAVE];
  char    mem2[BIGMODEL];
  node   *t,
        **t2,
         *model2[BIGWEAVE];
  weave   other[1];
  poly    temp[1];

  /* remove strings lying below everything interesting */
  for (mbot = newcross; --mbot >= 0;)
  {
    for (t = model[mbot]; t && t->correct; t = t->z) ;
    if (t) break;
    if (model[mbot]) m_string_kill(model, mbot);
  }

  /* remove strings lying above everything interesting */
  for (mtop = 0; mtop < newcross; mtop++)
  {
    for (t = model[mtop]; t && t->correct; t = t->z) ;
    if (t) break;
    if (model[mtop]) m_string_kill(model, mtop);
  }

  if (mtop < mbot)
  {
    for (t = model[mbot], i = 0; t; t = t->z) i += !t->correct;
    for (t = model[mtop], j = 0; t; t = t->z) j += !t->correct;
    for (t = model[i>j ? mbot : mtop]; t->correct; t = t->z);
    wasright  = t->right;
    mfirst  = t->self;
    msecond = t->m->self;

    for (j = 0; j < newcross; j++) list2[j] = list[j];
    m_copy(model, model2, mem2);
    *other = *oldweave;
    p_copy(&oldweave->tag, &other->tag);
    m_k_switch(t, t->m);

    b_switch(list2, mfirst, msecond, j);
    for (t2 = (&(model2[mfirst])); (*t2)->correct; t2 = (&((*t2)->z))) ;
    if (model2[msecond]->m != (*t2))
    {
      for (t = model2[msecond]; t->z->m != (*t2); t = t->z) ;
      m_k_break(&(t->z), t2);
    }
    else m_k_break(&model2[msecond], t2);
    m_correct(model2, mfirst);
    m_correct(model2, msecond);

    other->boundary[0] = oldweave->boundary[0];
    other->boundary[1] = oldweave->boundary[1];
    if (wasright)
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
    m_recurse(model, list, oldweave, newweaves);
    m_recurse(model2, list2, other, newweaves);
    return;
  }
  else
  {
    c_handle(list, oldweave, newweaves);
  }
}




static void   m_add(node **model,   /* original weave */
		    node *t,        /* crossing to add to the weave */
		    word  *list,    /* mapping of inputs to outputs */
		    word where)     /* where to add the crossing */
{
  node  *temp;

  if (going_in[where])
  {
    /* add this node to the start of the string */
    t->z         = model[where];
    model[where] = t;
  }
  else
  {
    /* add this node to the end of the string */
    t->z = 0;
    if (model[list[where]])
    {
      for (temp = model[list[where]]; temp->z; temp = temp->z) ;
      temp->z = t;
    }
    else model[list[where]] = t;
  }
}




static void m_shrink(word *list,
		     node  **model,
		     weave  *oldweave)
{
  poly   blank;
  word   this,
         that,
         i,
         j;
  node  *t;

  this = (old_going_in[first]) ? list[second] : list[first];
  that = (old_going_in[first]) ? first : second;
  for (t = model[that]; t != 0; t = t->z) t->self = this;
  if (model[this] != 0)
  {
    for (t = model[this]; t->z != 0; t = t->z) ;
    t->z = model[that];
  }
  else model[this] = model[that];
  model[that] = 0;

  list[this]       = list[that];
  list[list[this]] = this;
  for (i = 0; i < oldcross; i++)
  {
    if ((i != first) && (i != second))
    {
      j = map[i];
      if (j > i)
      {
        printf("you have a wrong assumption\n");
      }
      model[j] = model[i];
      for (t = model[j]; t != 0; t = t->z) t->self = j;
      list[j] = list[i]-((list[i] > first)+(list[i] > second));
    }
  }

  if (this == first || this == second)
  {
    /* A loop is being eliminated, multiply oldweave->tag by mll */
    p_mult(&mll, &oldweave->tag, &blank);
    p_kill(&oldweave->tag);
    oldweave->tag = blank;
  }
  else
  {
    /* some of the crossings might now be wrong */
    this = map[this];
    m_correct(model, this);
  }

}






static void   m_switch(word *list,
		       node **model,
		       char  *mem)
{
  word   i,
         temp[BIGWEAVE],
         mfirst,
         msecond;
  node  *t,   /* first string's half of new crossing */
        *t2,  /* second string's half of new crossing */
        *temp2[BIGWEAVE];

  mfirst  = first;
  msecond = second;

  if (mfirst != msecond + 1)
  {
    /*
     * Augh, we need some massive rearrangement here.
     * I think this also causes lots of bad crossings, due to the highest
     * string suddenly becoming the lowest string or vice versa, when the
     * one swapping from high to low or low to high is an input.
     * Could we finagle the plan so that this never happens?
     */
    for (i = 0; i < oldcross; i++) temp[map[i]] = list[i];
    for (i = 0; i < oldcross; i++) list[i] = map[temp[i]];
    for (i = 0; i < oldcross; i++) temp2[map[i]] = model[i];
    for (i = 0; i < oldcross; i++) model[i] = temp2[i];
    for (i = 0; i < oldcross; i++) 
      for (t = model[i]; t != 0; t = t->z) t->self = i;
    mfirst  = map[mfirst];
    msecond = map[msecond];
  }
  else
  {
    /* swap the two neighboring strings */
    b_switch(list, mfirst, msecond, i);
    t2            = model[mfirst];
    model[mfirst]  = model[msecond];
    model[msecond] = t2;

    /* note, model[mfirst] is empty if mfirst happens to be an output */
    for (t2 = model[mfirst]; t2 != 0; t2 = t2->z) t2->self = mfirst;
    for (t2 = model[msecond]; t2 != 0; t2 = t2->z) t2->self = msecond;
  }

  if (mfirst > msecond)
  {
    i      = mfirst;
    mfirst  = msecond;
    msecond = i;
  }

  t        = (node *)mem;
  t->right  = plan.right;
  if (t->right == (plan.over ^ old_going_in[plan.which] ^ plan.prev))
  {
    printf("handedness is messed up in models\n");
  }
  t->self  = (going_in[mfirst]) ? mfirst : list[mfirst];
  t->over  = (t->right ^ going_in[mfirst] ^ going_in[msecond]);
  t->o1    = 0;
  t->o2    = 0;
  t2        = (node *)(&mem[sizeof(node)]);
  t2->self  = (going_in[msecond]) ? msecond : list[msecond];
  t2->right  = t->right;
  t2->over  = !t->over;
  t2->o1    = 0;
  t2->o2    = 0;
  t->m  = t2;
  t2->m = t;
  m_add(model, t, list, mfirst);
  m_add(model, t2, list, msecond);
  m_correct(model, mfirst);
  m_correct(model, msecond);
}




/*
------------------------------------------------------------------------------
Given the boundary description of a simple weave, construct the weave.
------------------------------------------------------------------------------
*/
void   m_make(word  *list,
	      node **model,
	      char  *mem)
{
  word   i,
         j,
         temp;
  word   offset = 0;
  node  *a,
        *b;
  node  *nodemem = (node *)mem;

  for (i = 0; i < oldcross; ++i) model[i] = 0;/* null terminate every string */
  for (i = 1; i < oldcross; ++i)
  {
    if (old_going_in[i])
    {
      for (j = 0; j < i; ++j)
      {
        if ((old_going_in[j]) && (b_cross(i, j, list[i], list[j])))
        {
          a       = (node *)(&nodemem[offset++]);                /* overpass */
          b       = (node *)(&nodemem[offset++]);               /* underpass */
          a->self = j;                                   /* a is in string j */
          b_right(list, old_going_in, j, i, temp);
	                                    /* Is this right handed? *temp*. */
          a->right  = temp;                                  /* righthanded? */
          a->over   = 1;                                 /* a is an overpass */
          a->correct = 1;
                        /* yes, this crossing should be the hand that it is. */

          a->m      = b;         /* the other node in this internal crossing */
          a->z      = model[j];
                           /* the internal crossing one closer to the output */
          a->o1     = j;                 /* a was originally in crossing i,j */
          a->o2     = i;                 /* a was originally in crossing i,j */
          *b        = *a;                /* copy everything that is the same */
          b->self   = i;
          b->over   = 0;
          b->m      = a;
          b->z      = model[i];
          model[j]  = a;              /* tack a onto the front of the string */
          model[i]  = b;              /* tack b onto the front of the string */
        }
      }
    }
  }
  for (i = 0; i < oldcross; ++i) 
  {
    if (model[i])
    {
      m_sort(i, model, list);
    }
  }
}



/*
------------------------------------------------------------------------------
The change to the weave is too severe to be handled by special cases.
Construct a model of the weave and systematically apply the recursion formula
to it.  Whenever you get new simple weaves, c_handle them.
------------------------------------------------------------------------------
*/
void    m_model_weave(word   *list,
		      weave  *oldweave,
		      weave  *newweaves)
{
  node  *model[BIGWEAVE];
  char   mem[BIGMODEL];
  char   extra[2*sizeof(node)];

  m_make(list, model, mem);
  if (plan.which == (-1))
  {
    m_shrink(list, model, oldweave);
  }
  else
  {
    m_switch(list, model, extra);
  }
  m_recurse(model, list, oldweave, newweaves);
}

