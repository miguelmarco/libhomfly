/*
---------------------------------------------------------------------------
  MODEL.H
  By Bob Jenkins, August 1990, in association with my Masters Thesis
  Structures and procedures used for modelling complicated weaves
  Public Domain
---------------------------------------------------------------------------
*/

#ifndef STANDARD
# include "standard.h"
#endif
#ifndef BOUND
# include "bound.h"
#endif

#ifndef MODEL
#define MODEL

/*  When weaves are complicated enough to need modeling, a model of them is 
    made using singly linked lists of type node.  Something of type node
    is a single node in a string in a weave. */

struct node
{
    word          self;                     /* number of string m belongs to */
    word          right;      /* the crossing this node is in is righthanded */
    word          over;                          /* this node is an overpass */
    word          correct;             /* is this node where it ought to be? */
    struct node  *m;                          /* other node in this crossing */
    struct node  *z;                             /* next node in this string */
    word          o1;                         /* number of original string 1 */
    word          o2;                         /* number of original string 2 */
};
typedef struct node node;

#define BIGMODEL (((MAXSTRING)*(MAXSTRING-1)+2)*sizeof(node))

/* Public procedures defined in model.c */

/* Handle weaves that need to be modeled */
void m_model_weave(word *list, weave *oldweave, weave *newweaves);

#endif /* MODEL */

