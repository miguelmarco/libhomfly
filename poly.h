/*
------------------------------------------------------------------------------
  POLY.H - Bob Jenkins, 1988, Public Domain
  Structures and procedures dealing with Laurent polynomials on M and L
  Laurent polynomials can have integer exponents; not just natural numbers.
  -(M^-1)(L^-1) - (M^-1)(L^1) is a typical polynomial of this type.
------------------------------------------------------------------------------
*/

#ifndef STANDARD
# include "standard.h"
#endif
#ifndef POLY
#define POLY

/*
------------------------------------------------------------------------------
 Something of type TERM is a single term in a polynomial.
 coef is the coefficient of the term.
 L is the power of L in the term.
 M is the power of M in the term.
------------------------------------------------------------------------------
*/
struct term
{
  sb4    coef;
  sb2    m;
  sb2    l;
};
typedef struct term   term;


/*
------------------------------------------------------------------------------
  Something of type POLY is a polynomial of M and L.
  TERM is the array of terms.
  len is the number of terms.
------------------------------------------------------------------------------
*/
struct poly
{
  term  *term;
  sb4     len;
};
typedef struct poly   poly;


/*
------------------------------------------------------------------------------
  Procedures defined in poly.c
------------------------------------------------------------------------------
*/

#define p_init( p) ((p)->len = (sb4)0)

#define p_kill( p) \
if (1) \
{ \
  if ((p)->len) free((char *)(p)->term); \
  (p)->len = (sb4)0; \
} else

word   p_check(poly *p);                 /* check if a poly is a power of -2 */
void   p_copy(poly *inp, poly *outp);                       /* copies a poly */
void   p_show(poly *l);                                 /* displays the poly */
void   p_add(poly *inp1, poly *inp2, poly *outp);          /* adds two polys */
void   p_mult(poly *inp1, poly *inp2, poly *outp);     /* multiply two polys */

/* returns a poly with term added */
void   p_term(sb4 coef, sb4 m, sb4 l, poly *inp, poly *outp);

#endif /* POLY */
