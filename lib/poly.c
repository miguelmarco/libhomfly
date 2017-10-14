/*  Yet another program by Bob Jenkins.  3/16/88, for 15-451 */
/*  Public Domain */

#include <stdlib.h>
#include <gc.h>

#include "standard.h"
#include "poly.h"

/* These macros are machine dependent */
#define p_sign(m) ((sb4)(m))
#define p_cast(m) ((sb2)(m))
#define p_add_pow(m1, m2) ((m1)+(m2))

/* check that the coefficients add up to a power of -2 */
boolean p_check(Poly *p)
{
  ub4 i;
  sb4 sum;
  sb4 compare;

  if (!p || !p->len) return FALSE;
  for (sum=0, i=0; i<p->len; ++i)
  {
    sum += p->term[i].coef;
  }
  for (compare=1; compare && compare != sum; compare *= (sb4)(-2))
  {
    ;
  }
  if (!compare) return FALSE;
  return TRUE;
}

void p_copy(Poly *inp, Poly *outp)
/*  This returns a MALLOCed copy of inp.
    It eliminates all terms with coefficients of 0. */
{
  Term  *interm;
  Term  *outerm;
  Term  *endterm;

  if (inp->len)
  {
    outp->term = (Term *)GC_MALLOC((size_t)(inp->len*sizeof(Term)));
    for (interm = inp->term, outerm = outp->term,
         endterm = interm + inp->len; interm != endterm; ++interm)
    {
      if (interm->coef)
        *(outerm++) = *interm;
    }
    outp->len = outerm-outp->term;
  }
  else
  {
    outp->term = (Term *)0;
    outp->len  = 0;
  }
}

/* This displays the polynomial p */
char *p_show(Poly *p)
{
  Term  *pt;
  Term  *pend;
  sb4    first;
  sb4    m;
  sb4    l;
  char *bp;
  size_t size;
  int pos, offs, maxlen;
#define bp_chunksize 10000
#define bp_chunkreserve  500
#define bp_sprintf(...) {\
        sprintf(bp+offs, __VA_ARGS__, &pos);\
        offs += pos;\
        if (offs > maxlen - bp_chunkreserve) {\
	   maxlen += bp_chunksize + bp_chunkreserve;\
           bp = (char *)GC_REALLOC(bp, sizeof(char) * maxlen);\
        }\
      }
  maxlen =  bp_chunksize + bp_chunkreserve;
  offs = 0;
  bp = (char *)GC_MALLOC(sizeof(char) * maxlen);
  if (!p->len)
  {
    bp_sprintf("0%n")
    return bp;
  }

  for (first = 1, pt = p->term, pend = pt+p->len; pt != pend; ++pt)
  {
    if (!pt->coef) continue;
    m = p_sign(pt->m);
    l = p_sign(pt->l);
    if (pt->coef < ((sb4)0)) bp_sprintf(" - %n")
    else if (!first) bp_sprintf(" + %n")
    if (pt->coef > ((sb4)1) ) bp_sprintf("%ld%n", pt->coef)
    else if (pt->coef < (sb4)(-1)) bp_sprintf("%ld%n", -pt->coef)
    else if ((!l) && (!m)) bp_sprintf("%d%n", 1)
    if (pt->m)
    {
      bp_sprintf("M%n")
      if (m != 1) bp_sprintf("^%ld%n", m)
    }
    if (pt->l)
    {
      bp_sprintf("L%n")
      if (l != 1) bp_sprintf("^%ld%n", l)
    }
    if (first) first = 0;
  }
  return bp;
}




/* outp = inp + {t.c=c; t.l=l; t.m=m;} */
void p_term(sb4 coef, sb4 m, sb4 l, Poly *inp, Poly *outp)
{
  Poly   p[1];
  Term   t[1];

  t->coef = coef;
  t->m    = p_cast(m);
  t->l    = p_cast(l);
  p->len  = 1;
  p->term = t;
  p_add(p, inp, outp);
}


/* This is like a merge sort */
void p_add(Poly *inp1, Poly *inp2, Poly *outp)
{
  Term  *t1;
  Term  *t2;
  Term  *tout;
  Term  *end1;
  Term  *end2;
  sb4    eq1;

  outp->term = (Term *)GC_MALLOC((size_t)((inp1->len+inp2->len)*sizeof(Term)));
  t1         = inp1->term;
  end1       = t1+inp1->len;
  t2         = inp2->term;
  end2       = t2+inp2->len;
  tout       = outp->term;
  if (inp1->len && inp2->len)
  {
    while(TRUE)
    {
      eq1 = (t1->m == t2->m);
      if ((t1->m < t2->m) || (eq1 && (t1->l < t2->l)))
      {
        *(tout++) = *(t1++);
        if (t1 == end1) break;
      }
      else if ((!eq1) || (eq1 && (t1->l > t2->l)))
      {
        *(tout++) = *(t2++);
        if (t2 == end2) break;
      }
      else /* the powers of m and l are equal */
      {
        *(tout)    = *(t1);
        tout->coef = (t1++)->coef + (t2++)->coef;
        if (!(tout++)->coef) --tout; /* I don't know if this does anything - TCS */
        if (t1 == end1) break;
        if (t2 == end2) break;
      }
    }
  }
  if (t2 == end2)
  {
    while (t1 != end1)
      *(tout++) = *(t1++);
  }
  else /* t1 == end1 */
  {
    while (t2 != end2)
      *(tout++) = *(t2++);
  }
  outp->len = tout-outp->term;
}


/* *outp = (*inp1)*(*inp2) */
void p_mult(Poly *inp1, Poly *inp2, Poly *outp)
/*  It is assumed that inp1, inp2, and outp point to existing Poly structs */
{
  Poly            xinp2[1];
  Poly            temp[1];
  Poly  *blank;
  Term  *t1;
  Term  *t2;
  Term  *end1;
  Term  *end2;
  Term  *tx;

  if (inp1->len > inp2->len)
  {
    blank = inp1;
    inp1  = inp2;
    inp2  = blank;
  }
  p_init(outp);
  xinp2->term = (Term *)GC_MALLOC((size_t)(inp2->len*sizeof(Term)));
  xinp2->len  = inp2->len;
  for (t1 = inp1->term, end1 = t1 + inp1->len,
       end2 = inp2->term+inp2->len; t1 < end1; ++t1)
  {
    for (t2 = inp2->term, tx = xinp2->term; t2 < end2;)
    {
      tx->m    = p_add_pow(t1->m, t2->m);
      tx->l    = p_add_pow(t1->l, t2->l);
      (tx++)->coef = t1->coef * (t2++)->coef;
    }
    p_add(outp, xinp2, temp);
    p_kill(outp);
    *outp = *temp;
  }
  p_kill(xinp2);
}

