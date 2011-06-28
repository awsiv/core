
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: knowledge.c                                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

struct Item *NOVA_BUNDLEDEPENDENCE = NULL;

/*****************************************************************************/

static char *CF_VALUETYPES[18][3] =
 {
 {"hup,int,trap,kill,pipe,cont,abrt,stop,quit,term,child,usr1,usr2,bus,segv","system signals","a unix signal name"},
 {"true,false,yes,no,on,off","boolean","a positive or a negative"},
 {"symlink,hardlink,relative,absolute,none","link type","a support link type"},
 {"0,2147483648","a time range","a value from zero to a maximum system time -- but you should use time functions to convert this"},
 {"0,99999999999","a positive integer","a number between zero and the maximum value"},
 {"-99999999999,9999999999","integer","a number between the minus and positive maximum values"},
 {"-9.99999E100,9.99999E100","real number","a number between the minus and positive maximum values"},
 {"^.$","a single character","one symbol"},
 {"[0-7augorwxst,+-]+","posix file mode or permission","something that you would give as an argument to chmod"},
 {"[a-zA-Z0-9_!&|.()]+","a cfengine class expression","an alphanumeric string with option underscores and logical operators"},
 {"[a-zA-Z0-9_$.]+","a cfengine identifier","an alphanumeric string with option underscores"},
 {"[a-zA-Z0-9_$.-]+","a user/group id","an alphanumeric string with option underscores and hyphens"},
 {"[cC]:\\\\.*|/.*","a file path","a system file path suitable for the target system"},
 {"LOG_USER,LOG_DAEMON,LOG_LOCAL0,LOG_LOCAL1,LOG_LOCAL2,LOG_LOCAL3,LOG_LOCAL4,LOG_LOCAL5,LOG_LOCAL6,LOG_LOCAL7","a syslog level","a syslog constant"},
 {"","an arbitrary string","unspecified characters"},
 {".*","an arbitrary string","unspecified characters"},
 {NULL,NULL,NULL}
 };

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

void Nova_StoreKMDB(struct Topic **topichash,struct Occurrence *occurrences,struct Inference *inferences)
    
{
#ifdef HAVE_LIBMONGOC

  struct Topic *tp;
  struct TopicAssociation *ta;
  struct Occurrence *op;
  struct Inference *ip;
  struct Item *itp;
  char packNumStr[CF_MAXVARSIZE];
  mongo_connection dbconn = {0};
  bson_buffer bb,bbuf,*sub,*assocs,*setObj;
  bson b;
  int slot,assoc_id = 0;

if (!CFDB_Open(&dbconn, "127.0.0.1", CFDB_PORT))
   {
   CfOut(cf_error, "", "!! Could not open connection to database to store Knowledge Map");
   return;
   }
 
// remove existing data first

mongo_remove(&dbconn,MONGO_KM_TOPICS, bson_empty(&b));

 /* Class types and topics */

for (slot = 0; slot < CF_HASHTABLESIZE; slot++)
   {
   for (tp = topichash[slot]; tp != NULL; tp=tp->next)
      {
      bson_buffer_init(&bbuf);
      bson_append_new_oid(&bbuf, "_id" );
      bson_append_string(&bbuf,cfk_topicname,tp->topic_name);
      bson_append_string(&bbuf,cfk_topiccontext,tp->topic_context);
      bson_append_string(&bbuf,cfk_topiccontext,tp->topic_context);
      bson_append_int(&bbuf,cfk_topicid,tp->id);

      Debug("Add Topic(topic_name,topic_context,pid) values ('%s','%s','%d')\n",tp->topic_name,tp->topic_context,tp->id);
      
      // Associations

      bson_buffer_init(&bb);
      assocs = bson_append_start_array(&bbuf,cfk_associations);

      for (ta = tp->associations; ta != NULL; ta=ta->next)
         {
         for (itp = ta->associates; itp != NULL; itp=itp->next)
            {
            char to_context[CF_MAXVARSIZE],to_topic[CF_MAXVARSIZE];
            char tcontext[CF_MAXVARSIZE],ttype[CF_MAXVARSIZE],ttopic[CF_MAXVARSIZE];
            int to_id = GetTopicPid(itp->name);

            DeClassifyTopic(itp->name,to_topic,to_context);

            Debug(" - Association: '%s::%s' (%d) %s '%s:%s' (%d)\n",tp->topic_context,tp->topic_name,tp->id,ta->fwd_name,to_context,to_topic,to_id);
            Debug(" - Hence  by implication : '%s::%s' (%d) %s '%s::%s' (%d)\n",to_context,to_topic,to_id,ta->bwd_name,tp->topic_context,tp->topic_name,tp->id);

            // Append variable list item to assocs

            snprintf(packNumStr,sizeof(packNumStr),"%d",assoc_id++);
            sub = bson_append_start_object(assocs,packNumStr);
            bson_append_string(sub,cfk_fwd,ta->fwd_name);
            bson_append_string(sub,cfk_bwd,ta->bwd_name);
            bson_append_string(sub,cfk_assocname,to_topic);
            bson_append_string(sub,cfk_assoccontext,to_context);
            bson_append_int(sub,cfk_associd,to_id);
            bson_append_finish_object(sub);
            }
         }
      
      bson_append_finish_object(assocs);
      
      bson_from_buffer(&b,&bbuf);
      mongo_insert(&dbconn,MONGO_KM_TOPICS,&b);
      bson_destroy(&b);
      }
   }

// Occurrences

mongo_remove(&dbconn,MONGO_KM_OCCURRENCES,bson_empty(&b));

for (op = occurrences; op != NULL; op=op->next)
   {
   struct Rlist *rp;
   
   for (rp = op->represents; rp != NULL; rp=rp->next)
      {
      Debug("Add occurrence (context,locator,locator_type,subtype) values ('%s','%s','%d','%s')\n",op->occurrence_context,op->locator,op->rep_type,rp->item);

      bson_buffer_init(&bbuf);
      bson_append_new_oid(&bbuf, "_id" );
      bson_append_string(&bbuf,cfk_occurlocator,op->locator);
      bson_append_string(&bbuf,cfk_occurcontext,op->occurrence_context);
      bson_append_int(&bbuf,cfk_occurtype,op->rep_type);
      bson_append_string(&bbuf,cfk_occurrep,rp->item);

      bson_from_buffer(&b,&bbuf);
      mongo_insert(&dbconn,MONGO_KM_OCCURRENCES,&b);
      bson_destroy(&b);
      }
   }

//

// Occurrences

mongo_remove(&dbconn,MONGO_KM_INFERENCES,bson_empty(&b));

for (ip = inferences; ip != NULL; ip=ip->next)
   {
   Debug("Add inferences (precedent,qualifier,inference) values ('%s','%s','%s')\n",ip->precedent,ip->qualifier,ip->inference);

   bson_buffer_init(&bbuf);
   bson_append_new_oid(&bbuf, "_id" );
   bson_append_string(&bbuf,cfk_precedent,ip->precedent);
   bson_append_string(&bbuf,cfk_qualifier,ip->qualifier);
   bson_append_string(&bbuf,cfk_inference,ip->inference);
   
   bson_from_buffer(&b,&bbuf);
   mongo_insert(&dbconn,MONGO_KM_INFERENCES,&b);
   bson_destroy(&b);
   }

CFDB_Close(&dbconn);
#endif
}

/*****************************************************************************/

void Nova_SyntaxCompletion(char *s)

{ int i,j,k,l,m;
  struct SubTypeSyntax *ss;
  struct BodySyntax *bs,*bs2 = NULL;

if (EnterpriseExpiry())
   {
   CfOut(cf_error,"","Cfengine - autonomous configuration engine. This enterprise license has expired.\n");
   exit(1);
   }

if (cf_strncmp(s,"function",strlen("function")) == 0)
   {
   Nova_ListFunctions();
   return;
   }

if (cf_strncmp(s,"bundle",strlen("bundle")) == 0 || cf_strncmp(s,"agent",strlen("agent")) == 0)
   {
   Nova_ListAgents();
   return;
   }

if (cf_strcmp(s,"promise") == 0)
   {
   Nova_ListPromiseTypes();
   return;
   }

/* Else search for the specific word in the syntax tree */

for  (i = 0; i < CF3_MODULES; i++)
   {
   if ((ss = CF_ALL_SUBTYPES[i]) == NULL)
      {
      continue;
      }

   /* ss[j] is an array of promise types */
   
   for (j = 0; ss[j].btype != NULL; j++)
      {
      bs = (struct BodySyntax *)ss[j].bs;

      if (s && cf_strcmp(s,ss[j].subtype) == 0)
         {
         printf("Promise type %s has possible contraints:\n\n",ss[j].subtype);
         
         for (k = 0; bs[k].lval !=  NULL; k++)
            {
            printf("   %s\n",bs[k].lval);
            }

         if (cf_strcmp(s,"classes") == 0)
            {
            /* non-unique */
            continue;
            }
         else
            {
            return;
            }
         }

      for (k = 0; bs[k].lval !=  NULL; k++)
         {
         if (s && cf_strcmp(s,bs[k].lval) == 0)
            {
            printf("constraint %s (of promise type %s) has possible values:\n\n",bs[k].lval,ss[j].subtype);
            
            switch (bs[k].dtype)
               {
               case cf_bundle:
                   printf("   %s  ~ defines a separate bundle of promises\n\n",bs[k].lval);
                   break;
               case cf_body:
                   printf("   %s  ~ defined in a separate body, with elements\n\n",bs[k].lval);
                   bs2 = (struct BodySyntax *)bs[k].range;
                   
                   for (l = 0; bs2[l].lval !=  NULL; l++)
                      {
                      printf("     %s",bs2[l].lval);
                      for (m = 0; m + strlen(bs2[l].lval) < 22; m++)
                         {
                         putchar(' ');
                         }
                      
                      printf("~ (%s)\n",bs2[l].range);
                      }
                   
                   printf("\nDescription: %s\n",bs2[l].description);
                   break;
                   
               default:
                   printf("   %s  ~ (%s)\n",bs[k].lval,bs[k].range);
                   printf("\nDescription: %s\n",bs[k].description);
                   break;
               }

            return;
            }

         /* Could be in a sub-body */
         
         if (bs[k].dtype == cf_body)
            {
            bs2 = (struct BodySyntax *)bs[k].range;
            
            for (l = 0; bs2[l].lval !=  NULL; l++)
               {
               if (cf_strcmp(s,bs2[l].lval) == 0)
                  {
                  printf("body constraint %s is part of %s (in promise type %s) and has possible values:\n\n",bs2[l].lval,bs[k].lval,ss[j].subtype);
                  
                  printf("     %s",bs2[l].lval);
                  for (m = 0; m + strlen(bs2[l].lval) < 22; m++)
                     {
                     putchar(' ');
                     }
                  
                  printf("~ (%s)\n",bs2[l].range);
                  printf("\nDescription: %s\n",bs2[l].description);
                  return;
                  }
               }
            }      
         }
      }
   }

/* Check functions */

for (i = 0; CF_FNCALL_TYPES[i].name != NULL; i++)
    {
    if (cf_strcmp(s,CF_FNCALL_TYPES[i].name) == 0)
       {
       Nova_ListFunction(CF_FNCALL_TYPES[i],true);
       return;
       }
    }

/* If nothing found */

printf("Try one of the following:\n\n");
printf("   function\n");
printf("   promise type\n");
printf("   bundle type\n");
printf("   <syntax element>\n");
}

/*****************************************************************************/

void Nova_MapPromiseToTopic(FILE *fp,struct Promise *pp,const char *version)

{ char promise_id[CF_BUFSIZE];
  struct Rlist *rp,*depends_on = GetListConstraint("depends_on",pp), *rp2;
  struct Rlist *class_list = SplitRegexAsRList(pp->classes,"[.!()|&]+",100,false);
  char *bundlename = NULL, *bodyname = NULL;

if (LICENSES == 0)
   {
   fprintf(fp,"license expired or no license");
   return;
   }

strcpy(promise_id,Nova_PromiseID(pp));

fprintf(fp,"\ntopics:\n\n");

fprintf(fp,"bundles::\n");
fprintf(fp,"  \"%s\";\n",pp->bundle);

fprintf(fp,"class_contexts::\n");
fprintf(fp,"  \"%s\";\n",pp->classes);

/* First the bundle container */

fprintf(fp,"promisers::\n\n");
fprintf(fp,"  \"%s\"\n",NovaEscape(pp->promiser));
fprintf(fp,"      association => a(\"%s\",\"bundles::%s\",\"%s\");\n",KM_PARTOF_CERT_F,pp->bundle,"has promiser");
fprintf(fp,"  \"%s\"\n",NovaEscape(pp->promiser));
fprintf(fp,"      association => a(\"makes promise of type\",\"promise_types::%s\",\"promises have been made by\");\n",pp->agentsubtype);
fprintf(fp,"  \"%s\"\n",NovaEscape(pp->promiser));
fprintf(fp,"      association => a(\"has alias\",\"%s\",\"is a promise handle for\");\n",promise_id);


fprintf(fp,"promise_types::\n");
fprintf(fp,"  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",pp->agentsubtype,"is promised in",pp->bundle,"has promises of type");

/* Look for bundles used as promises through methods  -- these are service bundles */

if (strcmp(pp->agentsubtype,"methods") == 0)
   {
   struct Constraint *cp;
   struct FnCall *fnp;
   
   for (cp = pp->conlist; cp != NULL; cp = cp->next)
      {
      if (strcmp(cp->lval,"usebundle") == 0)
         {
         switch (cp->type)
            {
            case CF_SCALAR:
                bundlename = (char *)cp->rval;
                break;
            case CF_FNCALL:
                fnp = (struct FnCall *)cp->rval;
                bundlename = fnp->name;
                break;
            default:
                break;
            }
         }
      else
         {
         switch(cp->type)
            {
            case CF_SCALAR:
                if (cp->isbody)
                   {
                   bodyname = (char *)cp->rval;
                   }
                break;   
            case CF_FNCALL:
                fnp = (struct FnCall *)cp->rval;
                bodyname = fnp->name;                
                break;
            }

         if (bodyname)
            {
            char bodyref[CF_MAXVARSIZE];

            snprintf(bodyref,CF_MAXVARSIZE,"bodies::%s",bodyname);
            fprintf(fp,"promise_types::  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",pp->agentsubtype,KM_USES_POSS_F,bodyref,KM_USES_POSS_B);
            fprintf(fp,"handles::  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",promise_id,KM_USES_CERT_F,bodyref,KM_USES_CERT_B);
            fprintf(fp,"body_constraints::  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",cp->lval,KM_USES_POSS_F,bodyref,"has a body constraint of type");
            }
         }

      if (bundlename)
         {
         if (pp->ref)
            {
            fprintf(fp,"occurrences: \n\n");
            fprintf(fp," %s:: \"%s\"  representation => \"literal\",\n",bundlename,pp->ref);
            fprintf(fp,"   represents => { \"description\" }; \ntopics:\n");
            }
         break;
         }
      }
   }

/* Promisees as topics too */

switch (pp->petype)
   {
   case CF_SCALAR:
       fprintf(fp,"promisees::\n\n");
       fprintf(fp,"  \"%s\"\n",pp->promisee);
       fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,NovaEscape(pp->promiser),NOVA_GIVES);
       fprintf(fp,"  \"%s\"\n",pp->promisee);
       fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,promise_id,NOVA_GIVES);          
       fprintf(fp,"  \"%s\"\n",pp->promisee);
       fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",KM_AFFECTS_CERT_B,promise_id,KM_AFFECTS_CERT_F);          
       fprintf(fp,"  \"%s\"\n",pp->promisee);
       fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",KM_AFFECTS_CERT_B,NovaEscape(pp->promiser),KM_AFFECTS_CERT_F);          

       
       for (rp = GOALS; rp != NULL; rp = rp->next)
          {
          if (FullTextMatch(rp->item,pp->promisee))
             {
             fprintf(fp,"promises::\n\n");
             fprintf(fp,"  \"%s\"\n",promise_id);
             fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_IMPACTS,pp->promisee,NOVA_ISIMPACTED);
             
             if (bundlename)
                {
                fprintf(fp,"bundles::\n\n");
                fprintf(fp,"  \"%s\"\n",bundlename);
                fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_IMPACTS,pp->promisee,NOVA_ISIMPACTED);
                fprintf(fp,"  \"%s\"  association => a(\"%s\",\"goals::%s\",\"%s\");",bundlename,NOVA_GOAL,pp->promisee,NOVA_GOAL_INV);
                }
             }
          }
       break;

   case CF_LIST:

       fprintf(fp,"promisees::\n\n");
       for (rp = (struct Rlist *)pp->promisee; rp != NULL; rp=rp->next)
          {
          fprintf(fp,"  \"%s\"\n",rp->item);
          fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,NovaEscape(pp->promiser),NOVA_GIVES);          
          fprintf(fp,"  \"%s\"\n",rp->item);
          fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,promise_id,NOVA_GIVES);          

          for (rp2 = GOALS; rp2 != NULL; rp2 = rp2->next)
             {
             if (FullTextMatch(rp2->item,pp->promisee))
                {
                fprintf(fp,"promises::\n\n");
                fprintf(fp,"  \"%s\"\n",promise_id);
                fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_IMPACTS,rp2->item,NOVA_ISIMPACTED);
                
                if (bundlename)
                   {
                   fprintf(fp,"bundles::\n\n");
                   fprintf(fp,"  \"%s\"\n",bundlename);
                   fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_IMPACTS,rp2->item,NOVA_ISIMPACTED);
                   fprintf(fp,"  \"%s\"  association => a(\"%s\",\"goals::%s\",\"%s\");",bundlename,NOVA_GOAL,rp2->item,NOVA_GOAL_INV);
                   }
                }
             }
	  }

       break;
   default:
       break;
       
   }


/* This promise handle's place in the cosmos */

fprintf(fp,"promises::\n\n");

fprintf(fp,"\"%s\"\n",promise_id);

if (pp->ref)
   {
   fprintf(fp,"   comment => \"%s\";\n",NovaEscape(pp->ref));
   }
else
   {
   fprintf(fp,"   comment => \"(Uncommented <i>%s</i> promise by: %.25s..)\";\n",pp->agentsubtype,NovaEscape(pp->promiser));
   }

fprintf(fp,"\"%s\" association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n",promise_id,NOVA_ACTIVATED,pp->classes,NOVA_ACTIVATES);
fprintf(fp,"\"%s\" association => a(\"is a promise of type\",\"%s\",\"has current exemplars\");\n",promise_id,pp->agentsubtype);

for (rp = depends_on; rp != NULL; rp=rp->next)
   {
   fprintf(fp,"  \"%s\"\n",promise_id);
   fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,rp->item,NOVA_GIVES);
   fprintf(fp,"  \"%s\"\n",promise_id);
   fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",KM_AFFECTS_CERT_B,rp->item,KM_AFFECTS_CERT_F);
   fprintf(fp,"  \"%s\"\n",NovaEscape(pp->promiser));
   fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",KM_AFFECTS_CERT_B,rp->item,KM_AFFECTS_CERT_F);
   }

//DeleteRlist(depends_on);

for (rp = class_list; rp != NULL; rp=rp->next)
   {
   fprintf(fp,"  \"%s\"\n",promise_id);
   fprintf(fp,"      association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n",NOVA_USES,NovaEscape(rp->item),NOVA_GIVES);
   }

DeleteRlist(class_list);

/* Now pointers to the policy compilation */

fprintf(fp,"\n occurrences:\n");

fprintf(fp,"%s::\n",promise_id);

fprintf(fp,"\"/promise/details/%s\",\n",promise_id);
fprintf(fp,"   represents => { \"definition\" };\n\n");

fprintf(fp,"system_policy.bundles.%s::\n",pp->bundle);
fprintf(fp,"\"/bundle/details/bundle/%s\"\n",pp->bundle);
fprintf(fp,"   represents => { \"parent bundle\" };\n\n");

/*  Now we should analyze the classes to look for dependents and dependencies */

Nova_MapClassParameterAssociations(fp,pp,promise_id);

// Look for any networks mentioned

#ifdef HAVE_CONSTELLATION
Constellation_ScanAccessRelationships(fp,pp,promise_id);
#endif
}

/*****************************************************************************/

void Nova_ShowTopicRepresentation(FILE *fp)
    
{ int i,j,k,l;
  struct SubTypeSyntax *ss;
  struct BodySyntax *bs,*bs2;
  void *retval;
  char rettype;
  static char *level[] = { "high", "low", "normal", NULL };
  static char *dev[] = { "dev1", "dev2", "microanomaly", "anomaly", NULL };

if (LICENSES == 0)
   {
   return;
   }
  
fprintf(fp,"\n# This information is jointly Copyrighted by (C) Cfengine and the Licensee may not be redistributed without the permission of both parties\ntopics:\n");

fprintf(fp,"any:: \"system_reports\" comment => \"Reports collected from Cfengine managed systems by a reporting hub\";\n");
fprintf(fp,"\"remote_scalars\" comment => \"Scalar variable values that are made accessible to remote agents through cf-serverd\";\n");
fprintf(fp,"system_reports::\n");


for (i = 0; i < cfrep_unknown; i++)
   {
   fprintf(fp,"  \"%s\" comment => \"%s\";\n",BASIC_REPORTS[i][0],BASIC_REPORTS[i][1]);
   }

fprintf(fp,"  # New assocs\n");

fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_classes][0]);
fprintf(fp,"    association => a(\"%s\",\"promise_types::classes\",\"%s\");\n",NOVA_GEN,NOVA_REPORTED);
fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_lastseen][0]);
fprintf(fp,"   association => a(\"is affected by\",\"body_constraints::copy_from\",\"%s\");\n",NOVA_REPORTED);
fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_lastseen][0]);
fprintf(fp,"   association => a(\"is affected by\",\"promise_types::access\",\"%s\");\n",NOVA_REPORTED);

fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_promise_compliance][0]);
fprintf(fp,"    association => a(\"is based on\",\"promises\",\"%s\");\n",NOVA_REPORTED);
fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_promise_compliance][0]);
fprintf(fp,"    association => a(\"%s\",\"promise report\",\"%s\");\n",NOVA_SEEALSO,NOVA_SEEALSO);

fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_performance][0]);
fprintf(fp,"    association => a(\"is based on\",\"promises\",\"%s\");\n",NOVA_REPORTED);
fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_setuid][0]);
fprintf(fp,"    association => a(\"%s\",\"promise_types::files\",\"%s\");\n",NOVA_GEN,NOVA_REPORTED);
fprintf(fp,"  \"hashes report\"\n");
fprintf(fp,"    association => a(\"%s\",\"promise_types::files\",\"%s\");\n",NOVA_GEN,NOVA_REPORTED);
fprintf(fp,"  \"hashes report\"\n");
fprintf(fp,"    association => a(\"%s\",\"body_constraints::changes\",\"%s\");\n",NOVA_GEN,NOVA_REPORTED);
fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_change][0]);
fprintf(fp,"    association => a(\"%s\",\"promise_types::files\",\"%s\");\n",NOVA_GEN,NOVA_REPORTED);
fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_change][0]);
fprintf(fp,"    association => a(\"%s\",\"body_constraints::changes\",\"%s\");\n",NOVA_GEN,NOVA_REPORTED);
fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_diff][0]);
fprintf(fp,"    association => a(\"%s\",\"promise_types::files\",\"%s\");\n",NOVA_GEN,NOVA_REPORTED);
fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_diff][0]);
fprintf(fp,"    association => a(\"%s\",\"body_constraints::changes\",\"%s\");\n",NOVA_GEN,NOVA_REPORTED);
fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_software_installed][0]);
fprintf(fp,"    association => a(\"%s\",\"promise_types::packages\",\"%s\");\n",NOVA_GEN,NOVA_REPORTED);
fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_patch_status][0]);
fprintf(fp,"    association => a(\"%s\",\"promise_types::packages\",\"%s\");\n",NOVA_GEN,NOVA_REPORTED);
fprintf(fp,"  \"%s\"\n",BASIC_REPORTS[cfrep_patch_avail][0]);
fprintf(fp,"    association => a(\"%s\",\"promise_types::packages\",\"%s\");\n",NOVA_GEN,NOVA_REPORTED);

        
fprintf(fp,"system_policy::\n");
fprintf(fp,"  \"bundles\" comment => \"A modular collection of promises of different types\";\n");
fprintf(fp,"  \"bodies\" comment => \"A modular collection of body constraints for re-use\";\n");
fprintf(fp,"  \"class contexts\" comment => \"Class expressions that say where or when a promise applies\";\n");
fprintf(fp,"  \"promisees\" comment => \"Recipients of a promise, i.e. promise handles, or persons\";\n");
fprintf(fp,"  \"promisers\" comment => \"The objects affected by a promise\";\n");
fprintf(fp,"  \"promises\" comment => \"Complete occurrences of promiser + promisee + promise-body\";\n");
fprintf(fp,"  \"promise types\" comment => \"The types of promise that cfengine can keep\";\n");
fprintf(fp,"  \"body constraints\" comment => \"The attributes that cfengine enables you to promise about the promiser\";\n");
fprintf(fp,"\"comments\"\n");
fprintf(fp,"      association => a(\"see instances of\",\"comment\",\"is one of a number of\");\n");
fprintf(fp,"\"functions\" comment => \"In-built functions that may be used to set variables or classes\";");

fprintf(fp," \"values\"  comment => \"Formal rvalues in constraint assignments and their legal ranges\";\n");

fprintf(fp,"values::\n\n");

for (i = 0; CF_VALUETYPES[i][0] != NULL; i++)
   {
   fprintf(fp,"\"%s\"   comment =>\"Should match the generic pattern %s, i.e. %s\";\n",
           CF_VALUETYPES[i][1],
           NovaEscape(CF_VALUETYPES[i][0]),
           CF_VALUETYPES[i][2]);
   }

fprintf(fp,"\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[1][1],CF_DATATYPES[cf_int]);
fprintf(fp,"\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[3][1],CF_DATATYPES[cf_int]);
fprintf(fp,"\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[4][1],CF_DATATYPES[cf_int]);
fprintf(fp,"\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[5][1],CF_DATATYPES[cf_int]);
fprintf(fp,"\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[6][1],CF_DATATYPES[cf_real]);
fprintf(fp,"\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[9][1],CF_DATATYPES[cf_class]);
fprintf(fp,"\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[10][1],CF_DATATYPES[cf_str]);
fprintf(fp,"\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[11][1],CF_DATATYPES[cf_str]);
fprintf(fp,"\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[12][1],CF_DATATYPES[cf_str]);
fprintf(fp,"\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[13][1],CF_DATATYPES[cf_str]);
fprintf(fp,"\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[14][1],CF_DATATYPES[cf_str]);
fprintf(fp,"\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[15][1],CF_DATATYPES[cf_str]);


fprintf(fp,"bundles::\n");
fprintf(fp,"\"sys\" comment => \"cfengine's internal bundle of system specific values\";\n");
Nova_ShowBundleDependence(fp);

for (i = 0; i < CF3_MODULES; i++)
   {
   if ((ss = CF_ALL_SUBTYPES[i]) == NULL)
      {
      continue;
      }

   for (j = 0; ss[j].btype != NULL; j++)
      {
      fprintf(fp,"occurrences: %s:: \"/docs/cf3-reference.html#%s-in-agent-promises\" represents => { \"manual reference %s\" }; \n",CanonifyName(ss[j].subtype),ss[j].subtype,ss[j].subtype);
      
      if (ss[j].bs != NULL) /* In a bundle */
         {
         bs = ss[j].bs;

         for (l = 0; bs[l].lval != NULL; l++)
            {
            fprintf(fp,"topics:\n promise_types::\n");
            fprintf(fp,"   \"%s\";\n",ss[j].subtype);
            
            fprintf(fp,"body_constraints::\n");
            fprintf(fp,"   \"%s\"\n",bs[l].lval);
            fprintf(fp,"   comment => \"%s\",\n",NovaEscape(bs[l].description));
            fprintf(fp,"   association => a(\"%s\",\"promise_types::%s\",\"%s\");\n",KM_PARTOF_POSS_F,ss[j].subtype,KM_PARTOF_POSS_B);
            
            if (bs[l].dtype == cf_body)
               {
               bs2 = (struct BodySyntax *)(bs[l].range);
               
               if (bs2 == NULL || bs2 == (void *)CF_BUNDLE)
                  {
                  continue;
                  }


               // Certain lvals can be parts of body templates
               
               for (k = 0; bs2[k].lval != NULL; k++)
                  {
                  fprintf(fp,"   \"%s\"\n",bs2[k].lval);
                  fprintf(fp,"   comment => \"%s\",\n",NovaEscape(bs2[k].description));
                  fprintf(fp,"   association => a(\"%s\",\"%s\",\"%s\");\n",KM_PARTOF_POSS_F,bs[l].lval,KM_PARTOF_POSS_F);
                  
                  NovaShowValues(fp,bs2[k]);
                  }
               }
            else
               {
               NovaShowValues(fp,bs[l]);
               }
            }
         }
      }
   }

for (i = 0; CF_COMMON_BODIES[i].lval != NULL; i++)
   {
   fprintf(fp,"   \"%s\";\n",CF_COMMON_BODIES[i].lval);
   }


for (i = 0; CF_COMMON_EDITBODIES[i].lval != NULL; i++)
   {
   fprintf(fp,"   \"%s\";\n",CF_COMMON_EDITBODIES[i].lval);
   }

fprintf(fp,"miscellaneous_concepts::\n");

fprintf(fp,"  \"data types\";\n");

fprintf(fp,"data_types::\n");


for (i = 0; CF_VARBODY[i].lval != NULL; i++)
   {
   fprintf(fp,"  \"%s\" comment => \"%s matching %s\";\n",CF_VARBODY[i].lval,CF_VARBODY[i].description,CF_VARBODY[i].range);
   }

fprintf(fp,"  \"class\" comment => \"A boolean returned by certain functions in classes promises\";\n");

fprintf(fp,"functions::\n\n");

for (i = 0; CF_FNCALL_TYPES[i].name != NULL; i++)
   {
   fprintf(fp," \"%s\" ",CF_FNCALL_TYPES[i].name);
   fprintf(fp,"    comment => \"%s\",\n",CF_FNCALL_TYPES[i].description);
   fprintf(fp,"    association => a(\"returns data-type\",\"%s\",\"is returned by function\");\n",CF_DATATYPES[CF_FNCALL_TYPES[i].dtype]);
   }

// Things for monitoring system state

fprintf(fp,"things:\n");

fprintf(fp,"  \"monitoring classes\" comment => \"Classes set by cf-monitord based on the observed system state.\"; \n");

for (i = 0; i < CF_OBSERVABLES; i++)
   {
   if (strcmp(OBS[i][0],"spare") == 0)
      {
      continue;
      }
   
   fprintf(fp," \"%s\" comment => \"%s\",",OBS[i][0],OBS[i][1]);   
   fprintf(fp,"      generalizations => { \"vital signs\", \"observables\" },",OBS[i][0],OBS[i][1]);
   fprintf(fp,"      determines => { \"actual state\" , \"monitoring classes\" };\n");
   }

// Operating system classes

fprintf(fp,"operating_systems::");

for (i = 2; CLASSTEXT[i] != NULL; i++)
   {
   fprintf(fp," \"%s\" generalizations => { \"operating systems\" };  ", CLASSTEXT[i]);
   }


fprintf(fp," \"debian\" generalizations => { \"linux\"};");
fprintf(fp," \"ubuntu\" generalizations => { \"linux\", \"debian\" };");
fprintf(fp," \"redhat\" generalizations => { \"linux\"};");
fprintf(fp," \"sles\" generalizations => { \"suse\", \"linux\"};");
fprintf(fp," \"suse\" generalizations => { \"linux\"};");
fprintf(fp," \"slackware\" generalizations => { \"linux\"};");
fprintf(fp," \"fedora\" generalizations => { \"linux\"};");
fprintf(fp," \"mandrake\" generalizations => { \"linux\"};");
fprintf(fp," \"mandriva\" generalizations => { \"linux\"};");
fprintf(fp," \"gentoo\" generalizations => { \"linux\"};");
fprintf(fp," \"unitedlinux\" generalizations => { \"linux\"};");
fprintf(fp," \"suncobalt\" generalizations => { \"linux\"};");
fprintf(fp," \"Win2000\" generalizations => { \"windows\"};");
fprintf(fp," \"WinXP\" generalizations => { \"windows\"};");
fprintf(fp," \"WinServer2003\" generalizations => { \"windows\"};");
fprintf(fp," \"WinVista\" generalizations => { \"windows\"};");
fprintf(fp," \"WinServer2008\" generalizations => { \"windows\"};");

// Check for location descriptors - we need this categorization for hierarchy view / spanning tree

if (GetVariable("control_common",CFG_CONTROLBODY[cfg_site_classes].lval,&retval,&rettype) != cf_notype)
   {
   struct Rlist *rp;
   
   fprintf(fp,"locations::");

   for (rp = retval; rp != NULL; rp=rp->next)
      {
      fprintf(fp,"  \"%s\" comment => { \"A hosting location for computers.\" }, generalizations => { \"locations\" }; ");
      }
   }

// Monitoring

fprintf(fp,"ports::");

for (i = 0; i < ATTR; i++)
   {
   fprintf(fp,"  \"port %s\" comment => \"The standard reserved port for %s\", generalizations => { \"ports\" }, synonyms => {\"%s\"}; ",ECGSOCKS[i].portnr,ECGSOCKS[i].name,ECGSOCKS[i].name);
   }

fprintf(fp,"measurements:: \"anomalies\" comment => \"Measurements that exceed the boundaries of normal behaviour, as learned by cf-monitord\";");

fprintf(fp,"anomalies::");

for (i = 0; OBS[i] != NULL; i++)
   {
   if (strcmp(OBS[i][0],"spare") == 0)
      {
      break;
      }
   
   for (j = 0; level[j] != NULL; j++)
      {
      for (k = 0; dev[k] != NULL; k++)
         {
         fprintf(fp," \"%s_%s_%s\" comment => \"%s is %s relative to the learned normal average\", generalizations => { \"vital signs\", \"performance\", \"anomalies\" };\n",OBS[i][0],level[j],dev[k],OBS[i][1],level[j]);

         if (strstr(OBS[i][0],"_in"))
            {
            if (strstr(OBS[i][0],"_low"))
               {
               fprintf(fp," \"%s_%s_%s\" caused_by => { \"Reduced incoming traffic\" };",OBS[i][0],level[j],dev[k]);
               }
            else if (strstr(OBS[i][0],"_high"))
               {
               fprintf(fp," \"%s_%s_%s\" caused_by => { \"Increased incoming traffic\" };",OBS[i][0],level[j],dev[k]);
               }
            }
         else if (strstr(OBS[i][0],"_out"))
            {
            if (strstr(OBS[i][0],"_low"))
               {
               fprintf(fp," \"%s_%s_%s\" caused_by => { \"Reduced outgoing traffic\" };",OBS[i][0],level[j],dev[k]);
               }
            else if (strstr(OBS[i][0],"_high"))
               {
               fprintf(fp," \"%s_%s_%s\" caused_by => { \"Increased outgoing traffic\" };",OBS[i][0],level[j],dev[k]);
               }            
            }
         }
      }
   }
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

void Nova_ListAgents()

{ int i;

printf("Agent types are used as bundle types and control-body types:\n\n");
 
for (i = 0; CF_ALL_BODIES[i].btype != NULL; i++)
    {
    printf("   %s\n",CF_ALL_BODIES[i].btype);
    }
}

/*****************************************************************************/

void Nova_ListFunctions()

{ int i;

printf("In-built functions:\n\n");
 
for (i = 0; CF_FNCALL_TYPES[i].name != NULL; i++)
    {
    Nova_ListFunction(CF_FNCALL_TYPES[i],false);
    }
}

/*****************************************************************************/

void Nova_ListFunction(struct FnCallType f,int full)

{ int j;
  struct FnCallArg *args = f.args;
 
switch (f.dtype)
   {
   case cf_str:
       printf("   (string)  ");
       break;
   case cf_int:
       printf("   (integer) ");
       break;
   case cf_class:
       printf("   (class)   ");
       break;
   case cf_slist:
       printf("   (slist)   ");
       break;
   case cf_ilist:
       printf("   (ilist)   ");
       break;
   case cf_rlist:
       printf("   (rlist)   ");
       break;
   case cf_rrange:
   case cf_irange:
       printf("   (range)   ");
       break;
       
   }

printf("   %s",f.name);

for (j = 0; strlen(f.name)+j < 18; j++)
   {
   putchar(' ');
   }

printf(" %s\n",f.description);

if (full)
   {
   printf("\n");
   for (j = 0; args[j].pattern != NULL; j++)
      {
      printf("       arg %d : %s, in the range %s\n",j,args[j].description,args[j].pattern);
      }
   }
}

/*****************************************************************************/

void Nova_ListPromiseTypes()

{ int i;

printf("promise types:\n\n");
 
for (i = 0; CF_COMMON_SUBTYPES[i].subtype != NULL; i++)
    {
    printf("   %s  (%s)\n",CF_COMMON_SUBTYPES[i].subtype,CF_COMMON_SUBTYPES[i].btype);
    }
}

/*****************************************************************************/

char *Nova_PromiseID(struct Promise *pp)

{ char static id[CF_MAXVARSIZE];
  char vbuff[CF_MAXVARSIZE];
  char *handle = GetConstraint("handle",pp,CF_SCALAR);

if (LICENSES == 0)
   {
   return "license_expired";
   }

if (handle)
   {
   snprintf(id,CF_MAXVARSIZE,"%s",CanonifyName(handle));
   }
else if (pp && pp->audit)
   {
   snprintf(vbuff,CF_MAXVARSIZE,"%s",ReadLastNode(pp->audit->filename));
   snprintf(id,CF_MAXVARSIZE,"promise_%s_%d",CanonifyName(vbuff),pp->lineno);
   }
else
   {
   snprintf(id,CF_MAXVARSIZE,"unlabelled_promise");
   }

return id;
}

/*****************************************************************************/

void Nova_MapClassParameterAssociations(FILE *fp, struct Promise *pp,char *promise_id)

{ struct Rlist *impacted = NULL, *dependency = NULL, *potential,*rp;
  struct Bundle *bp;
  struct SubType *sp;
  struct Promise *pp2;
  char *value;
  int found = false;

if (promise_id && pp->ref)
   {
   fprintf(fp,"topics: handless:: \"%s\"  comment => \"%s\", ",promise_id,NovaEscape(pp->ref));
   fprintf(fp,"association => a(\"is the promise_id for\",\"%s\",\"has a promise with promise_id\");\n",NovaEscape(pp->promiser));
   }
else if (promise_id)
   {
   fprintf(fp,"topics: handles:: \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",promise_id,NOVA_HANDLE,NovaEscape(pp->promiser),NOVA_HANDLE_INV);
   }

if (promise_id)
   {
   fprintf(fp,"occurrences: %s::  \"/promise/details/%s\", represents => { \"declaration\" }; \n",CanonifyName(promise_id),promise_id);
   }

/* For activated classes we can assume that no one will */

potential = GetListConstraint("promise_kept",pp);

for (rp = potential; rp !=  NULL; rp=rp->next)
   {
   IdempPrependRScalar(&impacted,rp->item,CF_SCALAR);
   }

potential = GetListConstraint("promise_repaired",pp);

for (rp = potential; rp !=  NULL; rp=rp->next)
   {
   IdempPrependRScalar(&impacted,rp->item,CF_SCALAR);
   }

potential = GetListConstraint("repair_failed",pp);

for (rp = potential; rp !=  NULL; rp=rp->next)
   {
   IdempPrependRScalar(&impacted,rp->item,CF_SCALAR);
   }

potential = GetListConstraint("repair_denied",pp);

for (rp = potential; rp !=  NULL; rp=rp->next)
   {
   IdempPrependRScalar(&impacted,rp->item,CF_SCALAR);
   }

potential = GetListConstraint("promise_timeout",pp);

for (rp = potential; rp !=  NULL; rp=rp->next)
   {
   IdempPrependRScalar(&impacted,rp->item,CF_SCALAR);
   }

potential = GetListConstraint("or",pp);

for (rp = potential; rp !=  NULL; rp=rp->next)
   {
   IdempPrependRScalar(&dependency,rp->item,CF_SCALAR);
   }

potential = GetListConstraint("and",pp);

for (rp = potential; rp !=  NULL; rp=rp->next)
   {
   IdempPrependRScalar(&dependency,rp->item,CF_SCALAR);
   }

if ((value = GetConstraint("expression",pp,CF_SCALAR)))
   {
   IdempPrependRScalar(&dependency,value,CF_SCALAR);
   }

// Now look for impact

for (rp = dependency; rp != NULL; rp=rp->next)
   {
   fprintf(fp,"topics:\n");
   fprintf(fp,"class_contexts::");
   fprintf(fp,"  \"%s\"\n",NovaEscape(pp->promiser));
   fprintf(fp,"      association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n",NOVA_ISIMPACTED,NovaEscape(rp->item),NOVA_IMPACTS);

   // Might need to break these up further
   }

DeleteRlist(dependency);

if (impacted == NULL)
   {
   return;
   }

CfOut(cf_verbose,""," ?? Analysing promise dependencies for \"%s\"\n",promise_id);
  
/* So now we have the list of classes to scan for and we must undertake this
 expensive search */

for (bp = BUNDLES; bp != NULL; bp = bp->next)
   {
   for (sp = bp->subtypes; sp != NULL; sp = sp->next)
      {
      for (pp2 = sp->promiselist; pp2 != NULL; pp2=pp2->next)
         {
         if (pp2 == pp)
            {
            continue;
            }
         
         for (rp = impacted; rp != NULL; rp=rp->next)
            {
            char *varclass = GetConstraint("ifvarclass",pp,CF_SCALAR);

            if (strstr(pp2->classes,rp->item) || (varclass && strstr(varclass,rp->item)))
               {
	       //Debug("Found %s in %s+%s\n",rp->item,pp2->classes,varclassPrint); //FIXME: Segfault on Solaris (Nullpointers?)
               // found a connection
               fprintf(fp,"topics:\n");
               fprintf(fp,"promises::");
               fprintf(fp,"  \"%s\"\n",promise_id);
               fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_IMPACTS,rp->item,NOVA_ISIMPACTED);
               fprintf(fp,"promisers::");
               fprintf(fp,"  \"%s\"\n",NovaEscape(pp->promiser));
               fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_IMPACTS,rp->item,NOVA_ISIMPACTED);
               found = true;
               }

            /* Omit class "any" */
            
            if (strstr(pp2->classes,rp->item) && cf_strcmp(rp->item,"any") != 0 && cf_strcmp(pp->classes,"any") != 0)
               {                           
               fprintf(fp,"class_contexts::");
               fprintf(fp,"  \"%s\"\n",pp->classes);
               fprintf(fp,"      association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n",NOVA_ACTIVATED,NovaEscape(rp->item),NOVA_ACTIVATES);
               }
            }
         }
      }
   }

if (!found && THIS_AGENT_TYPE == cf_common)
   {
   CfOut(cf_verbose,"","Classes activated by the outcomes of promise ref \"%s\" did not occur in any promise proposal, so they are nullpotent",promise_id);
   PromiseRef(cf_inform,pp);
   }

DeleteRlist(impacted);
}

/*****************************************************************************/

void Nova_RegisterBundleDepedence(char *name,struct Promise *pp)

{ char assertion[CF_BUFSIZE];
  char *handle;

if (pp == NULL || pp->bundle == NULL)
   {
   return;
   }

if (cf_strcmp(name,pp->bundle) == 0)
   {
   return;
   }

if (cf_strcmp(name,"const") == 0)
   {
   return;
   }
  
/* Store everything first in a list because we don't have access to the
   output channel here -- summarize at the end. */

snprintf(assertion,CF_BUFSIZE-1,"topics: \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",name,NOVA_BUNDLE_DATA_INV_B,pp->bundle,NOVA_BUNDLE_DATA);

PrependItemList(&NOVA_BUNDLEDEPENDENCE,assertion);

if ((handle = (char *)GetConstraint("handle",pp,CF_SCALAR)))
   {
   snprintf(assertion,CF_BUFSIZE-1,"topics: \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",name,NOVA_BUNDLE_DATA_INV_P,handle,NOVA_BUNDLE_DATA);

   PrependItemList(&NOVA_BUNDLEDEPENDENCE,assertion);
   }
}

/*****************************************************************************/

void Nova_ShowBundleDependence(FILE *fp)

{ struct Item *ip;

for (ip = NOVA_BUNDLEDEPENDENCE; ip != NULL; ip =ip->next)
   {
   fprintf(fp,"%s",ip->name);
   }
}

/*****************************************************************************/

char *NovaEscape(char *s)
    
{ char *sp1,*sp2;
  static char buffer[CF_EXPANDSIZE];

memset(buffer,0,CF_EXPANDSIZE);

if (s == NULL)
   {
   return buffer;
   }
  
if (strlen(s) == 0)
   {
   return s;
   }

for (sp1 = s,sp2 = buffer; *sp1 != '\0'; sp1++)
   {
   if (*sp1 == '\"')
      {
      *sp2++ = '\\';
      *sp2++ = *sp1;
      }
   else
      {
      *sp2++ = *sp1;
      }
   }

return buffer;
}

/*****************************************************************************/

void NovaShowValues(FILE *fp,struct BodySyntax bs)

{ int i;
  char *range =  NULL;

for (i = 0; CF_VALUETYPES[i][0] != NULL; i++)
   {
   if (bs.dtype == cf_bundle || bs.dtype == cf_body)
      {
      continue;
      }
   
   if (cf_strcmp(CF_VALUETYPES[i][0],bs.range) == 0)
      {
      range = CF_VALUETYPES[i][1];
      break;
      }
   }

if (range == NULL)
   {
   range = CF_DATATYPES[bs.dtype];
   }

fprintf(fp,"body_constraints::\n");
fprintf(fp,"   \"%s\" association => a(\"is a body constraint of type\",\"values::%s\",\"can be used in body constraints\"),\n",bs.lval,range);
fprintf(fp,"          comment => \"%s\";\n",NovaEscape(bs.description));

switch(bs.dtype)
   {
   case cf_slist:
   case cf_ilist:
   case cf_rlist:

       fprintf(fp,"   \"%s\" association => a(\"is a list of type\",\"%s\",\"is used in\");\n",bs.lval,range);

   }

if (CF_VALUETYPES[i][0] != NULL)
   {
   fprintf(fp,"values::\n\n");
   fprintf(fp," \"%s\" comment => \"Represent type %s and should match %s\",\n",range,CF_DATATYPES[bs.dtype],NovaEscape(CF_VALUETYPES[i][0]));
   fprintf(fp,"    association => a(\"are used in constraint parameters\",\"body_constraints::%s\",\"takes value\");\n",bs.lval);
   }
}

/*********************************************************************/

char *Name2Id(char *s)

{ static char ret[CF_BUFSIZE],detox[CF_BUFSIZE];
  char *sp;

strncpy(ret,s,CF_BUFSIZE-1);

for (sp = ret; *sp != '\0'; sp++)
   {
   if (!isalnum(*sp) && *sp != '_')
      {
      if (isalnum(*sp & 127))
         {
         *sp &= 127;
         }
      }
   }
  
strncpy(detox,CanonifyName(ret),CF_BUFSIZE-1);
return detox;
}


