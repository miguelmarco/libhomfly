/*
------------------------------------------------------------------------------
  POLY.H - Bob Jenkins, 1988, Public Domain
  Structures and procedures dealing with Laurent polynomials on M and L
  Laurent polynomials can have integer exponents; not just natural numbers.
  -(M^-1)(L^-1) - (M^-1)(L^1) is a typical polynomial of this type.
------------------------------------------------------------------------------
*/

#ifndef POLY
#define POLY

#include "standard.h"

/*
------------------------------------------------------------------------------
 Something of type TERM is a single term in a polynomial.
 coef is the coefficient of the term.
 L is the power of L in the term.
 M is the power of M in the term.
------------------------------------------------------------------------------
*/
struct Term
{
  sb4    coef;
  sb2    m;
  sb2    l;
};
typedef struct Term Term;


/*
------------------------------------------------------------------------------
  Something of type POLY is a polynomial of M and L.
  TERM is the array of terms.
  len is the number of terms.
------------------------------------------------------------------------------
*/
struct Poly
{
  Term  *term;
  sb4     len;
};
typedef struct Poly Poly;


/*
------------------------------------------------------------------------------
  Procedures defined in poly.c
------------------------------------------------------------------------------
*/

#define p_init( p) ((p)->len = (sb4)0)

#define p_kill( p) \
if (1) \
{ \
  (p)->len = (sb4)0; \
} else
    //if ((p)->len) free((char *)(p)->term); \


boolean p_check(Poly *p);                 /* check if a poly is a power of -2 */
void    p_copy(Poly *inp, Poly *outp);                       /* copies a Poly */
char    *p_show(Poly *l);                                 /* displays the Poly */
void    p_add(Poly *inp1, Poly *inp2, Poly *outp);          /* adds two Polys */
void    p_mult(Poly *inp1, Poly *inp2, Poly *outp);     /* multiply two Polys */

/* returns a poly with term added */
void   p_term(sb4 coef, sb4 m, sb4 l, Poly *inp, Poly *outp);

#endif /* POLY */
