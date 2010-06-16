/*****************************************************************************/
/*                                                                           */
/* File: syntax.c                                                            */
/*                                                                           */
/* Created: Tue Jun 15 08:42:48 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

void Nova_SyntaxTree2JavaScript()
{
//Nova_ShowControlBodies();

Nova_ShowBundleTypes();

//printf("var cf3_functions = \n");
//printf("   {\n");
//Nova_ShowBuiltinFunctions();
//printf("   };\n\n");
}

/*******************************************************************/
/* Level 2                                                         */
/*******************************************************************/

void Nova_ShowBundleTypes()

{ int i;
  struct SubTypeSyntax *st;

for (i = 0; CF_ALL_BODIES[i].btype != NULL; i++)
   {
   printf("var %s_bundle_syntax = \n", CF_ALL_BODIES[i].btype);
   printf("   {\n");
   Nova_ShowPromiseTypesFor(CF_ALL_BODIES[i].btype);
   printf("   };\n\n");
   }

printf("var edit_line_bundle_syntax = \n");
printf("   {\n");

Nova_ShowPromiseTypesFor("*");

st = CF_FILES_SUBTYPES;

for (i = 0; st[i].btype != NULL; i++)
   {
   if (strcmp("edit_line",st[i].btype) == 0)
      {
      Nova_Indent(3);
      printf("\"%s\":\n",st[i].subtype);
      Nova_Indent(6);
      printf("{\n");
      Nova_ShowBodyParts(st[i].bs,6);
      Nova_Indent(6);
      printf("},\n");
      }
   }

printf("   };\n");
}

/*******************************************************************/

void Nova_ShowControlBodies()

{ int i;

for (i = 0; CF_ALL_BODIES[i].btype != NULL; i++)
   {
   printf("var %s_control_syntax = \n",CF_ALL_BODIES[i].btype);
   printf("   {\n");
   Nova_ShowBodyParts(CF_ALL_BODIES[i].bs,6);
   printf("   };\n\n");
   }
}


/*******************************************************************/

void Nova_ShowPromiseTypesFor(char *s)

{ int i,j;
  struct SubTypeSyntax *st;

for (i = 0; i < CF3_MODULES; i++)
   {
   st = CF_ALL_SUBTYPES[i];

   for (j = 0; st[j].btype != NULL; j++)
      {
      if (strcmp(s,st[j].btype) == 0 || strcmp("*",st[j].btype) == 0)
         {
         Nova_Indent(3);
         printf("\"%s\":\n",st[j].subtype);
         Nova_Indent(6);
         printf("{\n");
         Nova_ShowBodyParts(st[j].bs,6);
         Nova_Indent(6);
         printf("},\n");
         }
      }
   }
}

/*******************************************************************/

void Nova_ShowBodyParts(struct BodySyntax *bs,int indent)

{ int i;

if (bs == NULL)
   {
   return;
   }

Nova_Indent(indent);
printf("attributes: \"");

for (i = 0; bs[i].lval != NULL; i++)
   {
   printf("%s,",bs[i].lval);
   }

printf("\";\n\n");

for (i = 0; bs[i].lval != NULL; i++)
   {
   Nova_Indent(indent);

   if (bs[i].range == (void *)CF_BUNDLE)
      {
      printf("\"%s\" : \"fn-like bundle-reference, defined in edit_line_bundle_syntax\",\n",bs[i].lval);
      }
   else if (bs[i].dtype == cf_body)
      {
      printf("\"%s\" : \n",bs[i].lval);
      Nova_Indent(indent+3);
      printf("{\n");
      Nova_Indent(indent+3);
      Nova_ShowBodyParts((struct BodySyntax *)bs[i].range,indent+3);
      Nova_Indent(indent+3);
      printf("},\n\n");
      }
   else
      {
      printf("\"%s\" : \n",bs[i].lval);
      Nova_Indent(indent+3);
      printf("{\n");
      Nova_Indent(indent+3);
      printf("datatype: \"%s\",\n",CF_DATATYPES[bs[i].dtype]);
      Nova_Indent(indent+3);
      printf("pcre_range: ");
      Nova_ShowRange((char *)bs[i].range,bs[i].dtype);
      Nova_Indent(indent+3);
      printf("description : \"%s\",\n",bs[i].description);
      Nova_Indent(indent+3);
      printf("},\n");
      }
   }
}

/*******************************************************************/

void Nova_ShowRange(char *s,enum cfdatatype type)

{ char *sp;
 
if (strlen(s) == 0)
   {
   printf("\".*\",\n");
   return;
   }

printf("\"%s\",\n",s);
}

/*******************************************************************/

void Nova_ShowBuiltinFunctions()

{ int i;

Nova_Indent(3);
printf("names: \"");
 
for (i = 0; CF_FNCALL_TYPES[i].name != NULL; i++)
   {
   printf("%s,",CF_FNCALL_TYPES[i].name);
   }

printf("\";\n");


for (i = 0; CF_FNCALL_TYPES[i].name != NULL; i++)
   {
   Nova_Indent(3);
   printf("%s: \n",CF_FNCALL_TYPES[i].name);
   Nova_Indent(6);
   printf("{\n");
   Nova_Indent(6);
   printf("args: %d,\n",CF_FNCALL_TYPES[i].numargs);
   Nova_Indent(6);
   printf("description: \"%s\"\n",CF_FNCALL_TYPES[i].description);
   Nova_Indent(6);
   printf("},\n");
   }
}

/*******************************************************************/

void Nova_Indent(int i)

{ int j;

for (j = 0; j < i; j++)
   {
   putchar(' ');
   }
}

