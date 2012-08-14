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

#include "mod_files.h"

/* FIXME: only for cf-report */

static void Nova_ShowBundleTypes(void);
static void Nova_ShowControlBodies(void);
static void Nova_ShowPromiseTypesFor(const char *s, const SubTypeSyntax *commonMerge);
static void Nova_ShowBodyParts(const BodySyntax *bs, int i);
static void Nova_ShowRange(char *s, enum cfdatatype type);
static void Nova_ShowBuiltinFunctions(void);
static void Nova_Indent(int i);


void SyntaxExport(void)
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

static void Nova_ShowBundleTypes(void)
{
    int i;
    const SubTypeSyntax *st;
    const SubTypeSyntax *commonEditLine = NULL;
    int closeBrack = false;

    printf("var edit_line_bundle_syntax = {\n");

    st = CF_FILES_SUBTYPES;

    for (i = 0; st[i].bundle_type != NULL; i++)
    {
        if (strcmp("edit_line", st[i].bundle_type) == 0)
        {
            if (closeBrack)
            {
                printf("},\n");
            }

            // merge the "*" subtype with the common one
            if (strcmp(st[i].subtype, "*") == 0)
            {
                commonEditLine = &st[i];
                continue;
            }

            Nova_Indent(3);
            printf("\"%s\":\n", st[i].subtype);
            Nova_Indent(6);
            printf("{\n");

            //if(strcmp(st[i].subtype, "*") == 0)
            //        {
            //          Nova_ShowBodyParts(CF_COMMON_BODIES,6);
            //          printf(",");
            //        }

            Nova_ShowBodyParts(st[i].bs, 6);
            Nova_Indent(6);
            closeBrack = true;
        }
    }

    printf("},\n");
    Nova_ShowPromiseTypesFor("*", commonEditLine);
    printf("};\n\n\n");

    printf("var bundle_syntax = {\n");

// first make a reference to the edit_line bundle syntax (seperate var)

    printf("  edit_line: edit_line_bundle_syntax,\n");

    for (i = 0; CF_ALL_BODIES[i].bundle_type != NULL; i++)
    {
        printf("  %s:\n", CF_ALL_BODIES[i].bundle_type);
        printf("   {\n");
        Nova_ShowPromiseTypesFor(CF_ALL_BODIES[i].bundle_type, NULL);

        if (CF_ALL_BODIES[i + 1].bundle_type != NULL)
        {
            printf("   },\n");
        }
    }

    printf("   }\n};\n");

}

/*******************************************************************/

static void Nova_ShowControlBodies(void)
{
    int i;
    int closeBrack = false;

    printf("var body_control_syntax = {\n");

    for (i = 0; CF_ALL_BODIES[i].bundle_type != NULL; i++)
    {
        if (closeBrack)
        {
            printf("   },\n");
        }

        printf("  %s:\n", CF_ALL_BODIES[i].bundle_type);
        printf("   {\n");
        Nova_ShowBodyParts(CF_ALL_BODIES[i].bs, 6);

        closeBrack = true;
    }

    printf("   }\n};\n\n");
}

/*******************************************************************/

static void Nova_ShowPromiseTypesFor(const char *s, const SubTypeSyntax *commonMerge)
{
    int i, j;
    const SubTypeSyntax *st;
    int closeBrack = false;

    for (i = 0; i < CF3_MODULES; i++)
    {
        st = CF_ALL_SUBTYPES[i];

        for (j = 0; st[j].bundle_type != NULL; j++)
        {
            if (strcmp(s, st[j].bundle_type) == 0 || strcmp("*", st[j].bundle_type) == 0)
            {
                if (closeBrack)
                {
                    printf("},\n");
                }

                Nova_Indent(3);
                printf("\"%s\":\n", st[j].subtype);
                Nova_Indent(6);
                printf("{\n");
                Nova_ShowBodyParts(st[j].bs, 6);
                Nova_Indent(6);

                if ((commonMerge != NULL) && strcmp("*", st[j].subtype) == 0)
                {
                    printf(",\n");
                    Nova_ShowBodyParts(commonMerge->bs, 6);
                    Nova_Indent(6);
                }

                closeBrack = true;
            }
        }
    }
    printf("}\n");
}

/*******************************************************************/

static void Nova_ShowBodyParts(const BodySyntax *bs, int indent)
{
    int i;
    int closeBrack = false;

    if (bs == NULL)
    {
        return;
    }

    for (i = 0; bs[i].lval != NULL; i++)
    {

        if (closeBrack)
        {
            printf("},\n");
        }

        closeBrack = true;

        Nova_Indent(indent);

        if (bs[i].range == (void *) CF_BUNDLE)
        {
            printf("\"%s\" : edit_line_bundle_syntax", bs[i].lval);

            if (strcmp(bs[i].lval, "usebundle"))        // no comma in "methods"
            {
                printf(",");
            }

            printf("\n");

            closeBrack = false;
        }
        else if (bs[i].dtype == cf_body)
        {
            printf("\"%s\" : \n", bs[i].lval);
            Nova_Indent(indent + 6);
            printf("{\n");
            //Nova_Indent(indent+6);
            Nova_ShowBodyParts((BodySyntax *) bs[i].range, indent + 3);
            Nova_Indent(indent + 6);
            //      printf("},\n\n");
            printf("\n");
            Nova_Indent(indent + 6);
        }
        else
        {
            printf("\"%s\" : \n", bs[i].lval);
            Nova_Indent(indent + 3);
            printf("{\n");
            Nova_Indent(indent + 3);
            printf("datatype: \"%s\",\n", CF_DATATYPES[bs[i].dtype]);
            Nova_Indent(indent + 3);
            printf("pcre_range: ");
            Nova_ShowRange((char *) bs[i].range, bs[i].dtype);
            Nova_Indent(indent + 3);
            //      printf("description : \"%s\",\n",bs[i].description);
            //      Nova_Indent(indent+3);
            //printf("},\n");
        }

    }

    if (closeBrack)
    {
        printf("}\n");
    }
}

/*******************************************************************/

static void Nova_ShowRange(char *s, enum cfdatatype type)
{
    char *sp;

    if (strlen(s) == 0)
    {
        printf("\".*\"\n");
        return;
    }

    if (type == cf_opts || type == cf_olist)
    {
        printf("\"");
        for (sp = s; *sp != '\0'; sp++)
        {
            if (*sp == ',')
            {
                printf("|");
            }
            else
            {
                printf("%c", *sp);
            }
        }

        printf("\"\n");
    }
    else
    {
        printf("\"");
        for (sp = s; *sp != '\0'; sp++)
        {
            if (*sp == '\"')
            {
                printf("\\\"");
            }
            else if (*sp == '\'')
            {
                printf("\\\'");
            }
            else if (*sp == '\\')
            {
                printf("\\\\");
            }
            else
            {
                printf("%c", *sp);
            }
        }

        printf("\"\n");
    }
}

/*******************************************************************/

static void Nova_Indent(int i)
{
    int j;

    for (j = 0; j < i; j++)
    {
        putchar(' ');
    }
}
