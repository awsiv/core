
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

#ifdef HAVE_LIBMONGOC

void Nova_StoreExpandedPromise(struct Promise *pp)

{ struct Constraint *cp;
  struct Body *bp;
  struct FnCall *fp;
  struct Rlist *rp;
  char rval_buffer[CF_BUFSIZE];
  static int firstCall = true;
  char *v,rettype,vbuff[CF_BUFSIZE],*sp,jStr[32];
  char con[CF_MAXVARSIZE];
  void *retval;
  time_t lastseen,last;
  double val,av,var;
  mongo_connection dbconn = {0};
  bson_buffer bbuf,*cstr;
  bson b;
  int j;

if (!SHOWREPORTS)
   {
   return;
   }
 
// NOTE: Inefficient to Open/Close DB for each promise,
// but call is coming from community which may not have DB
if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_error, "", "!! Could not open connection to promise database to store expanded promises");
   return;
   }

 if(firstCall)
   {
     // clear existing data first
     mongo_remove(&dbconn, MONGO_PROMISES_EXP, bson_empty(&b));
     firstCall = false;
   }

 bson_buffer_init(&bbuf);
 bson_append_new_oid(&bbuf, "_id" );

Debug("PROMISE: \n");
//MapPromiseToTopic(FKNOW,pp,v);

Debug("\nPromise type is %s, ",pp->agentsubtype);
Debug("class context %s \n",pp->classes);

 bson_append_string(&bbuf,cfp_promisetype,pp->agentsubtype);
 bson_append_string(&bbuf, cfp_classcontext, pp->classes);

Debug("promiser is %s\n",pp->promiser);

 bson_append_string(&bbuf,cfp_promiser_exp,pp->promiser);


Debug("Bundle %s\n",pp->bundle);
 Debug("Bundle of type %s\n",pp->bundletype);

 bson_append_string(&bbuf,cfp_bundlename,pp->bundle);
 bson_append_string(&bbuf,cfp_bundletype,pp->bundletype);

if (pp->audit)
   {
   Debug("In file %s near line %d\n",pp->audit->filename,pp->lineno);
   bson_append_string(&bbuf,cfp_file,pp->audit->filename);
   bson_append_int(&bbuf, cfp_lineno, pp->lineno);
   }


// The promise body

if (pp->promisee)
   {
   memset(rval_buffer, 0, sizeof(rval_buffer));
   PrintRval(rval_buffer,CF_BUFSIZE,pp->promisee,pp->petype);

   Debug(" -> %s\n",rval_buffer);

   bson_append_string(&bbuf,cfp_promisee_exp,rval_buffer);
   }

if (pp->ref)
   {
   Debug("Comment: %s\n",pp->ref);
   bson_append_string(&bbuf,cfp_comment_exp,pp->ref);
   }
 
 if ((sp = GetConstraint("handle",pp,CF_SCALAR)) || (sp = PromiseID(pp)))
   {
     bson_append_string(&bbuf, cfp_handle_exp, sp);
   }

 cstr = bson_append_start_array(&bbuf, cfp_constraints_exp);
for (cp = pp->conlist, j = 0; cp != NULL; cp = cp->next)
   {
   // comments and handles have their own fields
   if (strcmp(cp->lval, "comment") == 0 || strcmp(cp->lval, "handle") == 0)
     { 
     continue;
     }


   memset(rval_buffer, 0, sizeof(rval_buffer));
   PrintRval(rval_buffer,CF_BUFSIZE,cp->rval,cp->type);
   Debug("  %s => %s\n",cp->lval,rval_buffer);
   
   snprintf(con, sizeof(con), "%s => %s", cp->lval, rval_buffer);
	    
   snprintf(jStr, sizeof(jStr), "%d", j);
   bson_append_string(cstr, jStr, con);
   j++;
   }
 bson_append_finish_object(cstr);

	 
 bson_from_buffer(&b, &bbuf);
 mongo_insert(&dbconn, MONGO_PROMISES_EXP, &b);
 bson_destroy(&b);


 CFDB_Close(&dbconn);

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
  bson_buffer *sub, *args, *cstr;
  bson b;
  int i,j;
  char iStr[32],jStr[32];
  char rval_buffer[CF_BUFSIZE];
  char con[CF_MAXVARSIZE];
  char *sp;

if (!SHOWREPORTS)
   {
   return;
   }

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_error, "", "!! Could not open connection to promise database to store unexpanded promises");
   return;
   }
 
// remove existing data first
 mongo_remove(&dbconn, MONGO_PROMISES_UNEXP, bson_empty(&b));

for (bp = bundles; bp != NULL; bp=bp->next)
   {   
   for (st = bp->subtypes; st != NULL; st = st->next)
      {
      Debug("PROMISE-TYPE: %s\n",st->name);
      
      for (pp = st->promiselist, i = 0; pp != NULL; pp = pp->next, i++)
         {	 
         
         bson_buffer_init(&bbuf);
	 bson_append_new_oid(&bbuf, "_id" );
         
         Debug("\nBundle %s of type %s\n\n",bp->name,bp->type);
         Debug("BUNDLE ARGS:\n");
         
	 bson_append_string(&bbuf,cfp_bundlename,bp->name);
	 bson_append_string(&bbuf,cfp_bundletype,bp->type);         

         args = bson_append_start_array(&bbuf, cfp_bundleargs);
         for (rp = bp->args, i = 0; rp != NULL; rp=rp->next, i++)
            {   
            Debug("   scalar arg %s\n",(char *)rp->item);
            
            snprintf(iStr, sizeof(iStr), "%d", i);
            bson_append_string(args, iStr, (char *)rp->item);
            }
         bson_append_finish_object(args);
         
         snprintf(iStr, sizeof(iStr), "%d", i);
         
         bson_append_string(&bbuf, cfp_promiser, pp->promiser);

	 bson_append_string(&bbuf,cfp_promisetype,pp->agentsubtype);
	 bson_append_string(&bbuf, cfp_classcontext, pp->classes);

         
         if (pp->promisee)
            {
            memset(rval_buffer, 0, sizeof(rval_buffer));
            PrintRval(rval_buffer,CF_BUFSIZE,pp->promisee,pp->petype);
            
            bson_append_string(&bbuf, cfp_promisee, rval_buffer);
            }
	 
         // NOTE: We also use audit info to update right expanded promise;
	 // can't use handle if there is a variable in it
         if (pp->audit)
            {
	    bson_append_string(&bbuf,cfp_file,pp->audit->filename);
	    bson_append_int(&bbuf, cfp_lineno, pp->lineno);
            }
         
         if ((sp = GetConstraint("handle",pp,CF_SCALAR)) || (sp = PromiseID(pp)))
            {
            bson_append_string(&bbuf, cfp_handle, sp);
            }
	 
         if (sp = GetConstraint("comment",pp,CF_SCALAR))
            {
            bson_append_string(&bbuf, cfp_comment, sp);
            }
	          
         cstr = bson_append_start_array(&bbuf, cfp_constraints);

         for (cp = pp->conlist, j = 0; cp != NULL; cp = cp->next)
            {
            // comments and handles have their own fields
            if (strcmp(cp->lval, "comment") == 0 || strcmp(cp->lval, "handle") == 0)
               { 
               continue;
               }
            
            memset(rval_buffer, 0, sizeof(rval_buffer));
            PrintRval(rval_buffer,CF_BUFSIZE,cp->rval,cp->type);
            Debug("  %s => %s\n",cp->lval,rval_buffer);
            
            snprintf(con, sizeof(con), "%s => %s", cp->lval, rval_buffer);
	    
            snprintf(jStr, sizeof(jStr), "%d", j);
            bson_append_string(cstr, jStr, con);
            j++;
            }
	 bson_append_finish_object(cstr);
	 
         bson_from_buffer(&b, &bbuf);
         
         mongo_insert(&dbconn, MONGO_PROMISES_UNEXP, &b);
	 bson_destroy(&b);
		     
         	 
         }      
      }   
   }

 /* Now summarize all bodies */


 // clear existing bodies first
mongo_remove(&dbconn, MONGO_BODIES, bson_empty(&b));

for (bdp = bodies; bdp != NULL; bdp=bdp->next)
   {
   Nova_StoreBody(&dbconn, bdp);
   }


 CFDB_Close(&dbconn);

}

/*****************************************************************************/

void Nova_StoreBody(mongo_connection *dbconn, struct Body *body)
    
{ struct Rlist *rp;
  struct Constraint *cp;
  bson_buffer bbuf,bb, *args, *setObj;
  char classContext[CF_MAXVARSIZE],varName[CF_MAXVARSIZE];
  char rval_buffer[CF_BUFSIZE];
  char iStr[32];
  int i;
  bson b,query;

  bson_buffer_init(&bbuf);
  bson_append_new_oid(&bbuf, "_id" );

Debug("body %s type %s\n",body->name,body->type);
 
 bson_append_string(&bbuf, cfb_bodyname, body->name);
 bson_append_string(&bbuf, cfb_bodytype, body->type);


 args = bson_append_start_array(&bbuf, cfb_bodyargs);
 for (rp = body->args, i = 0; rp != NULL; rp=rp->next,i++)
   {   
     Debug("%s\n",(char *)rp->item);
     snprintf(iStr, sizeof(iStr), "%d", i);
     bson_append_string(args, iStr, (char *)rp->item);
   }
 bson_append_finish_object(args);
 
 bson_from_buffer(&b, &bbuf);
 
 mongo_insert(dbconn, MONGO_BODIES, &b);

 bson_destroy(&b);


 // do update with the lval - rval attribs
 bson_buffer_init(&bb);
 bson_append_string(&bb,cfb_bodyname,body->name);
 bson_append_string(&bb,cfb_bodytype,body->type);
 bson_from_buffer(&query,&bb);
 
 
 bson_buffer_init(&bbuf);

 setObj = bson_append_start_object(&bbuf, "$set");
   for (cp = body->conlist; cp != NULL; cp=cp->next)
     {
       if (cp->classes != NULL)
	 {
	   // replace illegal key char '.' with  '&'
	   ReplaceChar(cp->classes,classContext,sizeof(classContext),'.','&');
	   Debug(" if class context %s\n",cp->classes);
	 }
       else
       {
	 snprintf(classContext,sizeof(classContext),"any");
       }

     memset(rval_buffer,0,sizeof(rval_buffer));
     
     PrintRval(rval_buffer,sizeof(rval_buffer),cp->rval,cp->type);
     
     Debug("  %s => %s\n",cp->lval,rval_buffer);

     snprintf(varName,sizeof(varName),"%s.%s.%s",cfb_classcontext,classContext,cp->lval);
     
     bson_append_string(&bbuf,varName,rval_buffer);

     }
   bson_append_finish_object(setObj);
   
   bson_from_buffer(&b, &bbuf);
   mongo_update(dbconn,MONGO_BODIES,&query,&b,MONGO_UPDATE_UPSERT);

   bson_destroy(&query);
   bson_destroy(&b);

}

#endif
