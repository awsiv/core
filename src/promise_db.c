
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: promise_db.c                                                        */
/*                                                                           */
/* Created: Tue Aug 10 10:34:56 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

void Nova_StoreExpandedPromise(struct Promise *pp)

{ struct Constraint *cp;
  struct Body *bp;
  struct FnCall *fp;
  struct Rlist *rp;
  char *v,rettype,vbuff[CF_BUFSIZE];
  void *retval;
  time_t lastseen,last;
  double val,av,var;

if (!SHOWREPORTS)
   {
   return;
   }

printf("PROMISE: \n");
//MapPromiseToTopic(FKNOW,pp,v);

printf("\nPromise type is %s, ",pp->agentsubtype);
printf("class context %s \n",pp->classes);

printf("promiser is %s\n",pp->promiser);

// Cache these?

printf("Bundle %s\n",pp->bundle);
printf("Bundle of type",pp->bundletype);
if (pp->audit)
   {
   printf("In file %s near line %d\n",pp->audit->filename,pp->lineno);
   }

// The promise body

if (pp->promisee)
   {
   char rval_buffer[CF_BUFSIZE];

   PrintRval(rval_buffer,CF_BUFSIZE,pp->promisee,pp->petype);

   printf(" -> %s\n",rval_buffer);
   }

if (pp->ref)
   {
   printf("Comment: %s\n",pp->ref);
   }

for (cp = pp->conlist; cp != NULL; cp = cp->next)
   {
   char rval_buffer[CF_BUFSIZE];
   PrintRval(rval_buffer,CF_BUFSIZE,cp->rval,cp->type);
   printf("  %s => %s\n",cp->lval,rval_buffer);
   }
}

/*****************************************************************************/

void Nova_StoreUnExpandedPromises(struct Bundle *bundles,struct Body *bodies)

{ struct Body *bdp;
  struct Bundle *bp;
  struct Rlist *rp;
  struct SubType *sp;
  struct Promise *pp;

if (!SHOWREPORTS)
   {
   return;
   }

printf("BUNDLES........--------------------------------------------------------\n");

for (bp = bundles; bp != NULL; bp=bp->next)
   {
   printf("\nBundle %s of type %s\n\n",bp->name,bp->type);
   printf("BUNDLE ARGS:\n");

   for (rp = bp->args; rp != NULL; rp=rp->next)
      {   
      printf("   scalar arg %s\n",(char *)rp->item);
      }

   for (sp = bp->subtypes; sp != NULL; sp = sp->next)
      {
      printf("PROMISE-TYPE: %s\n",sp->name);
      
      for (pp = sp->promiselist; pp != NULL; pp = pp->next)
         {
         Nova_StoreExpandedPromise(pp);
         }
      }
   }

/* Now summarize all bodies */

for (bdp = bodies; bdp != NULL; bdp=bdp->next)
   {
   Nova_StoreBody(bdp);
   }
}

/*****************************************************************************/

void Nova_StoreBody(struct Body *body)
    
{ struct Rlist *rp;
  struct Constraint *cp;

printf("body %s type %s\n",body->name,body->type);

for (rp = body->args; rp != NULL; rp=rp->next)
   {
   if (rp->type != CF_SCALAR)
      {
      FatalError("ShowBody - non-scalar paramater container");
      }

   printf("%s\n",rp->item);
   }


for (cp = body->conlist; cp != NULL; cp=cp->next)
   {
   char rval_buffer[CF_BUFSIZE];

   PrintRval(rval_buffer,CF_BUFSIZE,cp->rval,cp->type);

   printf("  %s => %s\n",cp->lval,rval_buffer);

   if (cp->classes != NULL)
      {
      printf(" if class context %s\n",cp->classes);
      }
   }
}
