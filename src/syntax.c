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

  Nova_ShowBundleTypes();

  printf("\n\n");

  Nova_ShowControlBodies();

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
  int closeBrack = false;

printf("var edit_line_bundle_syntax = {\n");

Nova_ShowPromiseTypesFor("*");
printf(",\n");

st = CF_FILES_SUBTYPES;

for (i = 0; st[i].btype != NULL; i++)
   {
   if (strcmp("edit_line",st[i].btype) == 0)
      {
	if(closeBrack)
	  {
	    printf("},\n");
	  }


      Nova_Indent(3);
      printf("\"%s\":\n",st[i].subtype);
      Nova_Indent(6);
      printf("{\n");
      Nova_ShowBodyParts(st[i].bs,6);
      Nova_Indent(6);
      closeBrack = true;
      }
   }

printf("}\n};\n\n\n");


  printf("var bundle_syntax = {\n");

for (i = 0; CF_ALL_BODIES[i].btype != NULL; i++)
   {
   printf("  %s:\n", CF_ALL_BODIES[i].btype);
   printf("   {\n");
   Nova_ShowPromiseTypesFor(CF_ALL_BODIES[i].btype);

   if(CF_ALL_BODIES[i+1].btype != NULL)
     {
       printf("   },\n");
     }
   }

printf("   }\n};\n");

}

/*******************************************************************/

void Nova_ShowControlBodies()

{ int i;
  int closeBrack = false;

  printf("var body_control_syntax = {\n");

for (i = 0; CF_ALL_BODIES[i].btype != NULL; i++)
   {
     if(closeBrack)
       {
	 printf("   },\n");
       }

   printf("  %s:\n",CF_ALL_BODIES[i].btype);
   printf("   {\n");
   Nova_ShowBodyParts(CF_ALL_BODIES[i].bs,6);

   
   closeBrack = true;
   }

 printf("   }\n};\n\n");
}


/*******************************************************************/

void Nova_ShowPromiseTypesFor(char *s)

{ int i,j;
  struct SubTypeSyntax *st;
  int closeBrack = false;

for (i = 0; i < CF3_MODULES; i++)
   {
   st = CF_ALL_SUBTYPES[i];
   
   for (j = 0; st[j].btype != NULL; j++)
      {
      if (strcmp(s,st[j].btype) == 0 || strcmp("*",st[j].btype) == 0)
         {
         if(closeBrack)
	   {
	   printf("},\n");
	   }

	 Nova_Indent(3);
         printf("\"%s\":\n",st[j].subtype);
         Nova_Indent(6);
         printf("{\n");
         Nova_ShowBodyParts(st[j].bs,6);
         Nova_Indent(6);

	 closeBrack = true;
         }
      }
   }
 printf("}\n");
}

/*******************************************************************/

void Nova_ShowBodyParts(struct BodySyntax *bs,int indent)

{ int i;
  int closeBrack = false;

if (bs == NULL)
   {
   return;
   }

for (i = 0; bs[i].lval != NULL; i++)
   {
     
     if(closeBrack)
       {
	 printf("},\n");	 
       }
     
     closeBrack = true;

   Nova_Indent(indent);

   if (bs[i].range == (void *)CF_BUNDLE)
      {
	printf("\"%s\" : edit_line_bundle_syntax",bs[i].lval);

	if(strcmp(bs[i].lval, "usebundle")) // no comma in "methods"
	  {
	    printf(",");
	  }

	printf("\n");

	closeBrack = false;
      }
   else if (bs[i].dtype == cf_body)
      {
      printf("\"%s\" : \n",bs[i].lval);
      Nova_Indent(indent+6);
      printf("{\n");
      //Nova_Indent(indent+6);
      Nova_ShowBodyParts((struct BodySyntax *)bs[i].range,indent+3);
      Nova_Indent(indent+6);
      //      printf("},\n\n");
      printf("\n");
      Nova_Indent(indent+6);
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
      //      printf("description : \"%s\",\n",bs[i].description);
      //      Nova_Indent(indent+3);
      //printf("},\n");
      }


   }

 if(closeBrack)
   {
     printf("}\n");
   }
}

/*******************************************************************/

void Nova_ShowRange(char *s,enum cfdatatype type)

{ char *sp;
  char sBuf[CF_MAXVARSIZE];
 
if (strlen(s) == 0)
   {
   printf("\".*\"\n");
   return;
   }

 snprintf(sBuf, sizeof(sBuf), "%s", s);
 EscapeChar(sBuf, sizeof(sBuf), '"');

printf("\"%s\"\n",sBuf);
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

