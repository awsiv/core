
/*

 This file is (C) Cfengine AS. See LICENSE for details.

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


/*****************************************************************************/

void Nova_MapPromiseToTopic(FILE *fp,struct Promise *pp,char *version)

{ struct Constraint *cp;
  char promise_id[CF_BUFSIZE];
  struct Rlist *rp,*depends_on = GetListConstraint("builds_on",pp->conlist);
  struct DefineClasses c = GetClassDefinitionConstraints(pp);

strcpy(promise_id,Nova_PromiseID(pp));

fprintf(fp,"\ntopics:\n\n");

fprintf(fp,"Bundles::\n");
fprintf(fp,"  \"%s\";\n",pp->bundle);

fprintf(fp,"Contexts::\n");
fprintf(fp,"  \"%s\";\n",pp->classes);

/* First the bundle container */

fprintf(fp,"Promisers::\n\n");
fprintf(fp,"  \"%s\"\n",NovaEscape(pp->promiser));
fprintf(fp,"      association => a(\"occurs in bundle\",\"Bundles::%s\",\"bundle contains promiser\");\n",pp->bundle);
fprintf(fp,"  \"%s\"\n",NovaEscape(pp->promiser));
fprintf(fp,"      association => a(\"makes promise of type\",\"Promise_types::%s\",\"promises have been made by promisers\");\n",pp->agentsubtype);
fprintf(fp,"  \"%s\"\n",NovaEscape(pp->promiser));
fprintf(fp,"      association => a(\"makes promises\",\"%s\",\"is a promise made by\");\n",promise_id);



fprintf(fp,"Promise_types::\n");
fprintf(fp,"  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",pp->agentsubtype,"is employed in bundle",pp->bundle,"employs promises of type");

/* Promisees as topics too */

switch (pp->petype)
   {
   case CF_SCALAR:
       fprintf(fp,"Promisees::\n\n");
       fprintf(fp,"  \"%s\"\n",pp->promisee);
       fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,pp->promiser,NOVA_GIVES);
       fprintf(fp,"  \"%s\"\n",pp->promisee);
       fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,promise_id,NOVA_GIVES);          
       break;

   case CF_LIST:

       fprintf(fp,"Promisees::\n\n");
       for (rp = (struct Rlist *)pp->promiser; rp != NULL; rp=rp->next)
          {
          fprintf(fp,"  \"%s\"\n",rp->item);
          fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,pp->promiser,NOVA_GIVES);          
          fprintf(fp,"  \"%s\"\n",rp->item);
          fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_USES,promise_id,NOVA_GIVES);          
          }
       break;
   default:
       break;
       
   }

/* Now the constraint list */

fprintf(fp,"Promises::\n\n");

fprintf(fp,"\"%s\"\n",promise_id);

if (pp->ref)
   {
   fprintf(fp,"   comment => \"%s\";\n",pp->ref);
   }
else
   {
   fprintf(fp,"   comment => \"(Uncommented promise of type <i>%s</i> made by: %.25s..)\";\n",pp->agentsubtype,pp->promiser);
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
   fprintf(fp,"   comment => \"%s\",\n",pp->ref);
   }
else
   {
   fprintf(fp,"   comment => \"A promise of type %s made by: %s\",\n",pp->agentsubtype,pp->promiser);
   }

fprintf(fp,"   represents => { \"%s\", \"%s\" };\n\n",pp->classes,pp->agentsubtype);

fprintf(fp,"%s::\n",CanonifyName(pp->classes));
PromiseNode(fp,pp,1);
fprintf(fp,"   represents => { \"promise\", \"%s\", \"%s\" };\n\n",NovaEscape(pp->promiser),pp->agentsubtype);

fprintf(fp,"%s::\n",pp->bundle);
fprintf(fp,"\"promises.cf.html#bundle_%s\"\n",pp->bundle);
fprintf(fp,"   represents => { \"Bundle Reference\" };\n\n");

for (cp = pp->conlist; cp != NULL; cp=cp->next)
   {
   fprintf(fp,"%s::\n",cp->lval);
   PromiseNode(fp,pp,1);
   fprintf(fp,"   represents => { \"Used in promise\", \"%s\" };\n\n",promise_id);

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
    
{
fprintf(fp,"\ntopics:\n");

fprintf(fp,"References::\n");
fprintf(fp,"  \"Bundle Reference\";\n");
fprintf(fp,"  \"Used in promise\";\n");
fprintf(fp,"  \"has current exemplars\";\n");
fprintf(fp,"  \"is a promise of type\";\n");
fprintf(fp,"  \"occurs in bundle\";\n");
fprintf(fp,"  \"bundle contains promiser\";\n");
fprintf(fp,"  \"makes promise of type\";\n");
fprintf(fp,"  \"promises have been made by promisers\";\n");
fprintf(fp,"  \"makes promises\";\n");
fprintf(fp,"  \"is a promise made by\";\n");

fprintf(fp,"Policy::\n");
fprintf(fp,"  \"Bundles\";\n");
fprintf(fp,"  \"Bodies\";\n");
fprintf(fp,"  \"Contexts\";\n");
fprintf(fp,"  \"Promisees\";\n");
fprintf(fp,"  \"Promisers\";\n");
fprintf(fp,"  \"Promises\" comment => \"Occurrences of promise topics or suggestions\";\n");
fprintf(fp,"  \"Promise types\";\n");
fprintf(fp,"  \"Body-lval types\";\n");
fprintf(fp,"\"Comments\"\n");
fprintf(fp,"      association => a(\"see instances of\",\"comment\",\"is one of a number of\");\n");

fprintf(fp,"Bundles::\n");
fprintf(fp,"\"sys\" comment => \"Cfengine's internal bundle of system specific values\";\n");
Nova_ShowBundleDependence(fp);
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
               fprintf(fp,"Promises::");
               fprintf(fp,"  \"%s\"\n",promise_id);
               fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_IMPACTS,rp->item,NOVA_ISIMPACTED);
               fprintf(fp,"Promisers::");
               fprintf(fp,"  \"%s\"\n",NovaEscape(pp->promiser));
               fprintf(fp,"      association => a(\"%s\",\"%s\",\"%s\");\n",NOVA_IMPACTS,rp->item,NOVA_ISIMPACTED);
               found = true;
               }

            /* Omit class "any" */
            
            if (strstr(pp2->classes,rp->item) && strcmp(rp->item,"any") != 0 && strcmp(pp->classes,"any") != 0)
               {                           
               fprintf(fp,"Contexts::");
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
