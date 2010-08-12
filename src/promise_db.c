
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
printf("Bundle of type %s\n",pp->bundletype);
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
   memset(rval_buffer, 0, sizeof(rval_buffer));
   PrintRval(rval_buffer,CF_BUFSIZE,cp->rval,cp->type);
   printf("  %s => %s\n",cp->lval,rval_buffer);
   }
}

/*****************************************************************************/

void Nova_StoreUnExpandedPromises(struct Bundle *bundles,struct Body *bodies)

{ struct Body *bdp;
  struct Bundle *bp;
  struct Rlist *rp;
  struct SubType *st;
  struct Promise *pp;
  struct Constraint *cp;

  mongo_connection dbconn = {0};
  bson_buffer bbuf;
  bson_buffer *sub, *args, *prObj, *prType, *pr, *cstr;
  bson b;
  int i, j;
  char iStr[32], jStr[32];
  char rval_buffer[CF_BUFSIZE];
  char con[CF_MAXVARSIZE];
  char *sp;
  

if (!SHOWREPORTS)
   {
   return;
   }

printf("BUNDLES........--------------------------------------------------------\n");

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_verbose, "", "!! Could not open connection to promise database");
   return;
   }

// clear existing data first
 mongo_remove(&dbconn, MONGO_PROMISES, bson_empty(&b));


for (bp = bundles; bp != NULL; bp=bp->next)
   {
   bson_buffer_init(&bbuf);
   bson_append_new_oid(&bbuf, "_id" );


   printf("\nBundle %s of type %s\n\n",bp->name,bp->type);
   printf("BUNDLE ARGS:\n");

   bson_append_string(&bbuf, cfp_bundlename, bp->name);
   bson_append_string(&bbuf, cfp_bundletype, bp->type);


   args = bson_append_start_array(&bbuf, cfp_bundleargs);
   for (rp = bp->args, i = 0; rp != NULL; rp=rp->next, i++)
      {   
      printf("   scalar arg %s\n",(char *)rp->item);

      snprintf(iStr, sizeof(iStr), "%d", i);
      bson_append_string(args, iStr, (char *)rp->item);
      }
   bson_append_finish_object(args);

   
   prObj = bson_append_start_object(&bbuf, cfp_promisetype);

   for (st = bp->subtypes; st != NULL; st = st->next)
      {
      printf("PROMISE-TYPE: %s\n",st->name);

      prType = bson_append_start_array(prObj, st->name);


      
      for (pp = st->promiselist, i = 0; pp != NULL; pp = pp->next, i++)
         {
	 snprintf(iStr, sizeof(iStr), "%d", i);

	 pr = bson_append_start_object(prType, iStr);
	 
	 bson_append_string(pr, cfp_promiser, pp->promiser);
	 bson_append_string(pr, cfp_classcontext, pp->classes);

	 if (pp->promisee)
	   {
	     memset(rval_buffer, 0, sizeof(rval_buffer));
	     PrintRval(rval_buffer,CF_BUFSIZE,pp->promisee,pp->petype);

	     bson_append_string(pr, cfp_promisee, rval_buffer);
	   }
	 

	 if (pp->audit)
	   {
	     bson_append_string(pr, cfp_file, pp->audit->filename);
	     bson_append_int(pr, cfp_lineno, pp->lineno);
	   }

	 if ((sp = GetConstraint("handle",pp,CF_SCALAR)) || (sp = PromiseID(pp)))
	   {
	     bson_append_string(pr, cfp_handle, sp);
	   }
	 
	 if (sp = GetConstraint("comment",pp,CF_SCALAR))
	   {
	     bson_append_string(pr, cfp_comment, sp);
	   }
	 

	 cstr = bson_append_start_array(pr, cfp_constraints);
	 for (cp = pp->conlist, j = 0; cp != NULL; cp = cp->next)
	   {
	     // comments and handles have their own fields
	     if(strcmp(cp->lval, "comment") == 0 ||
		strcmp(cp->lval, "handle") == 0)
	       { 
		 continue;
	       }
	     
	     memset(rval_buffer, 0, sizeof(rval_buffer));
	     PrintRval(rval_buffer,CF_BUFSIZE,cp->rval,cp->type);
	     printf("  %s => %s\n",cp->lval,rval_buffer);

	     snprintf(con, sizeof(con), "%s => %s", cp->lval, rval_buffer);
	     
	     snprintf(jStr, sizeof(jStr), "%d", j);
	     bson_append_string(cstr, jStr, con);
	     j++;
	   }
	 bson_append_finish_object(cstr);
	 
	 
	 bson_append_finish_object(pr);

	 Nova_StoreExpandedPromise(pp);
         }

      bson_append_finish_object(prType);
      }

   bson_append_finish_object(prObj);

   
   bson_from_buffer(&b, &bbuf);
   
   mongo_insert(&dbconn, MONGO_PROMISES, &b);
   bson_destroy(&b);
   }

/* Now summarize all bodies */

for (bdp = bodies; bdp != NULL; bdp=bdp->next)
   {
   Nova_StoreBody(bdp);
   }



if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_verbose,"", "!! Could not close connection to promise database");
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
