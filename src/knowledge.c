
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

struct Item *NOVA_BUNDLEDEPENDENCE = NULL;

static char *CF_VALUETYPES[18][3] =
 {
 "hup,int,trap,kill,pipe,cont,abrt,stop,quit,term,child,usr1,usr2,bus,segv","system signals","a unix signal name",
 "true,false,yes,no,on,off","boolean","a positive or a negative",
 "symlink,hardlink,relative,absolute,none","link type","a support link type",
 "0,2147483648","a time range","a value from zero to a maximum system time -- but you should use time functions to convert this",
 "0,99999999999","a positive integer","a number between zero and the maximum value",
 "-99999999999,9999999999","integer","a number between the minus and positive maximum values",
 "-9.99999E100,9.99999E100","real number","a number between the minus and positive maximum values",
 "^.$","a single character","one symbol",
 "[0-7augorwxst,+-]+","posix file mode or permission","something that you would give as an argument to chmod",
 "[a-zA-Z0-9_!&|.()]+","a cfengine class expression","an alphanumeric string with option underscores and logical operators",
 "[a-zA-Z0-9_$.]+","a cfengine identifier","an alphanumeric string with option underscores",
 "[a-zA-Z0-9_$.-]+","a user/group id","an alphanumeric string with option underscores and hyphens",
 "[cC]:\\\\.*|/.*","a file path","a system file path suitable for the target system",
 "LOG_USER,LOG_DAEMON,LOG_LOCAL0,LOG_LOCAL1,LOG_LOCAL2,LOG_LOCAL3,LOG_LOCAL4,LOG_LOCAL5,LOG_LOCAL6,LOG_LOCAL7","a syslog level","a syslog constant",
 "","An arbitrary string","unspecified characters",
 ".*","An arbitrary string","unspecified characters",
 NULL,NULL,NULL
 };

    
/*****************************************************************************/

void Nova_SyntaxCompletion(char *s)

{ int i,j,k,l,m;
  struct SubTypeSyntax *ss;
  struct BodySyntax *bs,*bs2 = NULL;
  char output[CF_BUFSIZE];

if (strncmp(s,"function",strlen("function")) == 0)
   {
   Nova_ListFunctions();
   return;
   }

if (strncmp(s,"bundle",strlen("bundle")) == 0 || strncmp(s,"agent",strlen("agent")) == 0)
   {
   Nova_ListAgents();
   return;
   }

if (strcmp(s,"promise") == 0)
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

      if (s && strcmp(s,ss[j].subtype) == 0)
         {
         printf("Promise type %s has possible contraints:\n\n",ss[j].subtype);
         
         for (k = 0; bs[k].lval !=  NULL; k++)
            {
            printf("   %s\n",bs[k].lval);
            }

         if (strcmp(s,"classes") == 0)
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
         if (s && strcmp(s,bs[k].lval) == 0)
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
               if (strcmp(s,bs2[l].lval) == 0)
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
    if (strcmp(s,CF_FNCALL_TYPES[i].name) == 0)
       {
       Nova_ListFunction(CF_FNCALL_TYPES[i]);
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

void Nova_MapPromiseToTopic(FILE *fp,struct Promise *pp,char *version)

{ struct Constraint *cp;
  char promise_id[CF_BUFSIZE];
  struct Rlist *rp,*depends_on = GetListConstraint("depends_on",pp->conlist);
  struct DefineClasses c = GetClassDefinitionConstraints(pp);

strcpy(promise_id,Nova_PromiseID(pp));

fprintf(fp,"\ntopics:\n\n");

fprintf(fp,"bundles::\n");
fprintf(fp,"  \"%s\";\n",pp->bundle);

fprintf(fp,"contexts::\n");
fprintf(fp,"  \"%s\";\n",pp->classes);

/* First the bundle container */

fprintf(fp,"promisers::\n\n");
fprintf(fp,"  \"%s\"\n",NovaEscape(pp->promiser));
fprintf(fp,"      association => a(\"occurs in bundle\",\"bundles::%s\",\"bundle contains promiser\");\n",pp->bundle);
fprintf(fp,"  \"%s\"\n",NovaEscape(pp->promiser));
fprintf(fp,"      association => a(\"makes promise of type\",\"promise_types::%s\",\"promises have been made by\");\n",pp->agentsubtype);
fprintf(fp,"  \"%s\"\n",NovaEscape(pp->promiser));
fprintf(fp,"      association => a(\"makes promises\",\"%s\",\"is a promise made by\");\n",promise_id);



fprintf(fp,"promise_types::\n");
fprintf(fp,"  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",pp->agentsubtype,"is employed in bundle",pp->bundle,"employs promises of type");

/* Promisees as topics too */

switch (pp->petype)
   {
   case CF_SCALAR:
       fprintf(fp,"promisees::\n\n");
       fprintf(fp,"  \"%s\"\n",pp->promisee);
       fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,NovaEscape(pp->promiser),NOVA_GIVES);
       fprintf(fp,"  \"%s\"\n",pp->promisee);
       fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,promise_id,NOVA_GIVES);          
       break;

   case CF_LIST:

       fprintf(fp,"promisees::\n\n");
       for (rp = (struct Rlist *)pp->promisee; rp != NULL; rp=rp->next)
          {
          fprintf(fp,"  \"%s\"\n",rp->item);
          fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,NovaEscape(pp->promiser),NOVA_GIVES);          
          fprintf(fp,"  \"%s\"\n",rp->item);
          fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,promise_id,NOVA_GIVES);          
          }
       break;
   default:
       break;
       
   }

/* Now the constraint list */

fprintf(fp,"promises::\n\n");

fprintf(fp,"\"%s\"\n",promise_id);

if (pp->ref)
   {
   fprintf(fp,"   comment => \"%s\";\n",NovaEscape(pp->ref));
   }
else
   {
   fprintf(fp,"   comment => \"(Uncommented promise of type <i>%s</i> made by: %.25s..)\";\n",pp->agentsubtype,NovaEscape(pp->promiser));
   }

fprintf(fp,"\"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",promise_id,NOVA_ACTIVATED,pp->classes,NOVA_ACTIVATES);
fprintf(fp,"\"%s\" association => a(\"is a promise of type\",\"%s\",\"has current exemplars\");\n",promise_id,pp->agentsubtype);

for (rp = depends_on; rp != NULL; rp=rp->next)
   {
   fprintf(fp,"  \"%s\"\n",promise_id);
   fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,rp->item,NOVA_GIVES);
   }


/* Now pointers to the policy compilation */

fprintf(fp,"\n occurrences:\n");

fprintf(fp,"%s::\n",promise_id);

PromiseNode(fp,pp,1);

if (pp->ref)
   {
   fprintf(fp,"   comment => \"%s\",\n",NovaEscape(pp->ref));
   }
else
   {
   fprintf(fp,"   comment => \"A promise of type %s made by: %s\",\n",pp->agentsubtype,NovaEscape(pp->promiser));
   }

fprintf(fp,"   represents => { \"%s\", \"%s\" };\n\n",pp->classes,pp->agentsubtype);

fprintf(fp,"%s::\n",CanonifyName(pp->classes));
PromiseNode(fp,pp,1);
fprintf(fp,"   represents => { \"promise\", \"%s\", \"%s\" };\n\n",NovaEscape(pp->promiser),pp->agentsubtype);

fprintf(fp,"%s::\n",pp->bundle);
fprintf(fp,"\"promises.cf.html#bundle_%s\"\n",pp->bundle);
fprintf(fp,"   represents => { \"bundle reference\" };\n\n");

for (cp = pp->conlist; cp != NULL; cp=cp->next)
   {
   fprintf(fp,"%s::\n",cp->lval);
   PromiseNode(fp,pp,1);
   fprintf(fp,"   represents => { \"used in promise\", \"%s\" };\n\n",promise_id);

   if (strcmp(cp->lval,"comment") == 0)
      {
      fprintf(fp,"\"%s\"\n",cp->rval);
      fprintf(fp,"   representation => \"literal\",\n\n");
      fprintf(fp,"   represents => { \"%s\" };\n",promise_id);
      fprintf(fp,"%s::\n",promise_id);
      fprintf(fp,"\"%s\"\n",cp->rval);
      fprintf(fp,"   representation => \"literal\",\n\n");
      fprintf(fp,"   represents => { \"comment\" };\n");
      }
   }

/*
  Now we should analyze the classes to look for dependents and dependencies */

Nova_MapClassParameterAssociations(fp,pp,promise_id);
}

/*****************************************************************************/

void Nova_ShowTopicRepresentation(FILE *fp)
    
{ int i,j,k,l,m;
  struct SubTypeSyntax *ss;
  struct BodySyntax *bs,*bs2;

fprintf(fp,"\ntopics:\n");

fprintf(fp,"references::\n");
fprintf(fp,"  \"bundle reference\";\n");
fprintf(fp,"  \"used in promise\";\n");
fprintf(fp,"  \"has current exemplars\";\n");
fprintf(fp,"  \"is a promise of type\";\n");
fprintf(fp,"  \"occurs in bundle\";\n");
fprintf(fp,"  \"bundle contains promiser\";\n");
fprintf(fp,"  \"makes promise of type\";\n");
fprintf(fp,"  \"promises have been made by\";\n");
fprintf(fp,"  \"makes promises\";\n");
fprintf(fp,"  \"is a promise made by\";\n");

fprintf(fp,"system_policy::\n");
fprintf(fp,"  \"bundles\";\n");
fprintf(fp,"  \"bodies\";\n");
fprintf(fp,"  \"contexts\";\n");
fprintf(fp,"  \"promisees\";\n");
fprintf(fp,"  \"promisers\";\n");
fprintf(fp,"  \"promises\" comment => \"occurrences of promise topics or suggestions\";\n");
fprintf(fp,"  \"promise types\";\n");
fprintf(fp,"  \"body constraints\";\n");
fprintf(fp,"\"comments\"\n");
fprintf(fp,"      association => a(\"see instances of\",\"comment\",\"is one of a number of\");\n");
fprintf(fp,"\"functions\" comment => \"In built functions that may be used to set variables or classes\";");

fprintf(fp," \"values\"  comment => \"Formal rvalues in constraint assignments and their legal ranges\";\n");
fprintf(fp,"values::\n\n");

for (i = 0; CF_VALUETYPES[i][0] != NULL; i++)
   {
   fprintf(fp,"\"%s\"   comment =>\"Should match the generic pattern %s, i.e. %s\";\n",CF_VALUETYPES[i][1],NovaEscape(CF_VALUETYPES[i][0]),CF_VALUETYPES[i][2]);
   }

fprintf(fp,"\"%s\"   association => a(\"is a special case of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[1][1],CF_DATATYPES[cf_int]);
fprintf(fp,"\"%s\"   association => a(\"is a special case of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[3][1],CF_DATATYPES[cf_int]);
fprintf(fp,"\"%s\"   association => a(\"is a special case of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[4][1],CF_DATATYPES[cf_int]);
fprintf(fp,"\"%s\"   association => a(\"is a special case of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[5][1],CF_DATATYPES[cf_int]);
fprintf(fp,"\"%s\"   association => a(\"is a special case of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[6][1],CF_DATATYPES[cf_real]);
fprintf(fp,"\"%s\"   association => a(\"is a special case of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[9][1],CF_DATATYPES[cf_class]);
fprintf(fp,"\"%s\"   association => a(\"is a special case of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[10][1],CF_DATATYPES[cf_str]);
fprintf(fp,"\"%s\"   association => a(\"is a special case of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[11][1],CF_DATATYPES[cf_str]);
fprintf(fp,"\"%s\"   association => a(\"is a special case of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[12][1],CF_DATATYPES[cf_str]);
fprintf(fp,"\"%s\"   association => a(\"is a special case of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[13][1],CF_DATATYPES[cf_str]);
fprintf(fp,"\"%s\"   association => a(\"is a special case of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[14][1],CF_DATATYPES[cf_str]);
fprintf(fp,"\"%s\"   association => a(\"is a special case of\",\"%s\",\"is the generic type for\");\n",CF_VALUETYPES[15][1],CF_DATATYPES[cf_str]);


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
      if (ss[j].bs != NULL) /* In a bundle */
         {
         bs = ss[j].bs;

         for (l = 0; bs[l].lval != NULL; l++)
            {
            fprintf(fp,"promise_types::\n");
            fprintf(fp,"   \"%s\";\n",ss[j].subtype);
            
            fprintf(fp,"body_constraints::\n");
            fprintf(fp,"   \"%s\"\n",bs[l].lval);
            fprintf(fp,"   comment => \"%s\",\n",NovaEscape(bs[l].description));
            fprintf(fp,"   association => a(\"is a possible body constraint for\",\"promise_types::%s\",\"can have body constraints\");\n",ss[j].subtype);
            
            if (bs[l].dtype == cf_body)
               {
               bs2 = (struct BodySyntax *)(bs[l].range);
               
               if (bs2 == NULL || bs2 == (void *)CF_BUNDLE)
                  {
                  continue;
                  }
               
               for (k = 0; bs2[k].lval != NULL; k++)
                  {
                  fprintf(fp,"   \"%s\"\n",bs2[k].lval);
                  fprintf(fp,"   comment => \"%s\",\n",NovaEscape(bs2[k].description));
                  fprintf(fp,"   association => a(\"is a possible sub-body constraint for\",\"%s\",\"may have sub-body constraints\");\n",bs[l].lval);
                  
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

fprintf(fp,"functions::\n\n");

for (i = 0; CF_FNCALL_TYPES[i].name != NULL; i++)
   {
   char *type;
   
   fprintf(fp," \"%s\" ",CF_FNCALL_TYPES[i].name);
   fprintf(fp,"    comment => \"%s\",\n",CF_FNCALL_TYPES[i].description);
   fprintf(fp,"    association => a(\"returns data-type\",\"%s\",\"is returned by function\");\n",CF_DATATYPES[CF_FNCALL_TYPES[i].dtype]);
   }
}

/*****************************************************************************/

void Nova_BundleNode(FILE *fp,char *bundle)

{
fprintf(fp,"<a name=\"bundle_%s\"></a>",bundle);
}

/*****************************************************************************/

void Nova_BodyNode(FILE *fp,char *body,int calltype)

{
 switch (calltype)
    {
    case 0:
        fprintf(fp,"<a name=\"body_%s\"></a>",body);
        break;
    case 1:
        fprintf(fp,"<a href=\"#body_%s\">",body);
        break;
    case 2:
        fprintf(fp,"</a>");
        break;
    }

}

/*****************************************************************************/

void Nova_TypeNode(FILE *fp,char *type)

{
fprintf(fp,"<a name=\"type_%s\"></a>\n",CanonifyName(type));
}

/*****************************************************************************/

void Nova_PromiseNode(FILE *fp,struct Promise *pp,int calltype)

{ char id[CF_BUFSIZE];

strcpy(id,Nova_PromiseID(pp));
 
if (calltype)
   {
   fprintf(fp,"\"promises.cf.html#%s\"\n",id);
   }
else 
   {
   fprintf(fp,"<a name=\"%s\"></a>\n",id);
   fprintf(fp,"<div id=\"nav\">\n");
   fprintf(fp,"<a href=\"promises.cf.html#bundle_%s\">[BUNDLE]</a>&nbsp;\n",pp->bundle);
   fprintf(fp,"<a href=\"promises.cf.html#%s\">[TEMPLATE]</a>\n",id);
   fprintf(fp,"<a href=\"promise_output_common.html#%s\">[RESULT]</a>\n",id);
   fprintf(fp,"</div>\n");
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

{ int i,j;

printf("In-built functions:\n\n");
 
for (i = 0; CF_FNCALL_TYPES[i].name != NULL; i++)
    {
    Nova_ListFunction(CF_FNCALL_TYPES[i]);
    }
}

/*****************************************************************************/

void Nova_ListFunction(struct FnCallType f)

{ int j;
 
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
  char *handle = GetConstraint("handle",pp->conlist,CF_SCALAR);

if (handle)
   {
   snprintf(id,CF_MAXVARSIZE,"%s",CanonifyName(handle));
   }
else if (pp->audit)
   {
   snprintf(vbuff,CF_MAXVARSIZE,"%s",ReadLastNode(pp->audit->filename));
   snprintf(id,CF_MAXVARSIZE,"promise_%s_%d",CanonifyName(vbuff),pp->lineno);
   }
else
   {
   snprintf(id,CF_MAXVARSIZE,"Anonymous_promise");
   }

return id;
}

/*****************************************************************************/

void Nova_MapClassParameterAssociations(FILE *fp, struct Promise *pp,char *promise_id)

{ struct Rlist *impacted = NULL, *potential, *rp;
  struct Bundle *bp;
  struct SubType *sp;
  struct Promise *pp2;
  int found = false;
  
/* For activated classes we can assume that no one will */

potential = GetListConstraint("promise_kept",pp->conlist);

for (rp = potential; rp !=  NULL; rp=rp->next)
   {
   IdempPrependRScalar(&impacted,rp->item,CF_SCALAR);
   }

potential = GetListConstraint("promise_repaired",pp->conlist);

for (rp = potential; rp !=  NULL; rp=rp->next)
   {
   IdempPrependRScalar(&impacted,rp->item,CF_SCALAR);
   }

potential = GetListConstraint("repair_failed",pp->conlist);

for (rp = potential; rp !=  NULL; rp=rp->next)
   {
   IdempPrependRScalar(&impacted,rp->item,CF_SCALAR);
   }

potential = GetListConstraint("repair_denied",pp->conlist);

for (rp = potential; rp !=  NULL; rp=rp->next)
   {
   IdempPrependRScalar(&impacted,rp->item,CF_SCALAR);
   }

potential = GetListConstraint("promise_timeout",pp->conlist);

for (rp = potential; rp !=  NULL; rp=rp->next)
   {
   IdempPrependRScalar(&impacted,rp->item,CF_SCALAR);
   }

if (impacted == NULL)
   {
   return;
   }

CfOut(cf_verbose,"","Looking for control dependencies for promise \"%s\"\n",promise_id);
  
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
            char *varclass = GetConstraint("ifvarclass",pp->conlist,CF_SCALAR);

            if (strstr(pp2->classes,rp->item) || (varclass && strstr(varclass,rp->item)))
               {
               Debug("Found %s in %s+%s\n",rp->item,pp2->classes,varclass);
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
            
            if (strstr(pp2->classes,rp->item) && strcmp(rp->item,"any") != 0 && strcmp(pp->classes,"any") != 0)
               {                           
               fprintf(fp,"contexts::");
               fprintf(fp,"  \"%s\"\n",NovaEscape(pp->classes));
               fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_ACTIVATED,rp->item,NOVA_ACTIVATES);
               }
            }
         }
      }
   }

if (!found)
   {
   CfOut(cf_inform,"","Classes activated by the outcomes of promise ref \"%s\" did not occur in any promise proposal, so they are nullpotent",promise_id);
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

if (strcmp(name,pp->bundle) == 0)
   {
   return;
   }

if (strcmp(name,"const") == 0)
   {
   return;
   }
  
handle = (char *)GetConstraint("handle",pp->conlist,CF_SCALAR);

/* Store everything first in a list because we don't have access to the
   output channel here -- summarize at the end. */

snprintf(assertion,CF_BUFSIZE-1,"topics: \"%s\" association => a(\"%s\",\"%s\",\"%s\")\n",name,NOVA_BUNDLE_DATA_INV_B,pp->bundle,NOVA_BUNDLE_DATA);

PrependItemList(&NOVA_BUNDLEDEPENDENCE,assertion);

snprintf(assertion,CF_BUFSIZE-1,"topics: \"%s\" association => a(\"%s\",\"%s\",\"%s\")\n",name,NOVA_BUNDLE_DATA_INV_P,handle,NOVA_BUNDLE_DATA);

PrependItemList(&NOVA_BUNDLEDEPENDENCE,assertion);
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
  int count = 0;

memset(buffer,0,CF_EXPANDSIZE);

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
   if (strcmp(CF_VALUETYPES[i][0],bs.lval) == 0)
      {
      range = CF_VALUETYPES[i][1];
      break;
      }
   }

if (CF_VALUETYPES[i][0] == NULL)
   {
   range = CF_DATATYPES[bs.dtype];
   }

fprintf(fp,"body_constraints::\n");
fprintf(fp,"   \"%s\" association => a(\"is a body constraint of type\",\"%s\",\"has possible body constraints\"),\n",bs.lval,range);
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
   fprintf(fp,"    association => a(\"are used in constraint parameters\",\"%s\",\"takes value\");\n",bs.lval);
   }
}
