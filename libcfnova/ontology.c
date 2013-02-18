/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "constraints.h"
#include "promises.h"
#include "parser.h"
#include "files_names.h"
#include "item_lib.h"
#include "mod_files.h"
#include "expand.h"
#include "probes.h"
#include "matching.h"
#include "unix.h"
#include "cfstream.h"
#include "ontology.h"
#include "vars.h"
#include "fncall.h"

#include <assert.h>

/*****************************************************************************/

static char *NovaEscape(const char *s); /* Thread-unsafe */
static void Nova_ShowBundleDependence(Writer *writer);
static void NovaShowValues(Writer *writer, BodySyntax bs);
static void Nova_MapClassParameterAssociations(Writer *writer, const Promise *pp, char *promise_id);

/*****************************************************************************/

static Item *NOVA_BUNDLEDEPENDENCE = NULL;

/*****************************************************************************/

static const char *CF_VALUETYPES[18][3] =
{
    {"hup,int,trap,kill,pipe,cont,abrt,stop,quit,term,child,usr1,usr2,bus,segv", "system signals",
     "a unix signal name"},
    {"true,false,yes,no,on,off", "boolean", "a positive or a negative"},
    {"symlink,hardlink,relative,absolute,none", "link type", "a support link type"},
    {"0,2147483648", "a time range",
     "a value from zero to a maximum system time -- but you should use time functions to convert this"},
    {"0,99999999999", "a positive integer", "a number between zero and the maximum value"},
    {"-99999999999,9999999999", "integer", "a number between the minus and positive maximum values"},
    {"-9.99999E100,9.99999E100", "real number", "a number between the minus and positive maximum values"},
    {"^.$", "a single character", "one symbol"},
    {"[0-7augorwxst,+-]+", "posix file mode or permission", "something that you would give as an argument to chmod"},
    {"[a-zA-Z0-9_!&|.()]+", "a cfengine class expression",
     "an alphanumeric string with option underscores and logical operators"},
    {"[a-zA-Z0-9_$.]+", "a cfengine identifier", "an alphanumeric string with option underscores"},
    {"[a-zA-Z0-9_$.-]+", "a user/group id", "an alphanumeric string with option underscores and hyphens"},
    {"[cC]:\\\\.*|/.*", "a file path", "a system file path suitable for the target system"},
    {"LOG_USER,LOG_DAEMON,LOG_LOCAL0,LOG_LOCAL1,LOG_LOCAL2,LOG_LOCAL3,LOG_LOCAL4,LOG_LOCAL5,LOG_LOCAL6,LOG_LOCAL7",
     "a syslog level", "a syslog constant"},
    {"", "an arbitrary string", "unspecified characters"},
    {".*", "an arbitrary string", "unspecified characters"},
    {NULL, NULL, NULL}
};

/*********************************************************************/

void Nova_MapPromiseToTopic(const ReportContext *report_context, const Promise *pp, const char *version)
{
    Writer *writer = report_context->report_writers[REPORT_OUTPUT_TYPE_KNOWLEDGE];
    assert(writer);
    if (!writer)
    {
        return;
    }

    char promise_id[CF_BUFSIZE];
    Rlist *rp, *rp2, *depends_on = GetListConstraint("depends_on", pp);
    Rlist *class_list = RlistFromSplitRegex(pp->classes, "[.!()|&]+", 100, false);
    char *bundlename = NULL, *bodyname = NULL;

    if (LICENSES == 0)
    {
        WriterWriteF(writer, "license expired or no license");
        return;
    }

    if (strcmp(pp->bundletype,"knowledge") == 0)
    {
        return;
    }

    strcpy(promise_id, PromiseID(pp));

    WriterWriteF(writer, "\ntopics:\n\n");

    WriterWriteF(writer, "bundles::\n");
    WriterWriteF(writer, "  \"%s\";\n", pp->bundle);

    WriterWriteF(writer, "class_contexts::\n");
    WriterWriteF(writer, "  \"%s\";\n", pp->classes);

/* First the bundle container */

    WriterWriteF(writer, "promises::\n\n");
    WriterWriteF(writer, "  \"%s\"\n", promise_id);
    WriterWriteF(writer, "      association => a(\"%s\",\"bundles::%s\",\"%s\");\n", KM_PARTOF_CERT_F, pp->bundle,
            "has promise");

    WriterWriteF(writer, "promisers::\n\n");
    WriterWriteF(writer, "  \"%s\"\n", NovaEscape(pp->promiser));
    WriterWriteF(writer, "      association => a(\"%s\",\"bundles::%s\",\"%s\");\n", KM_PARTOF_CERT_F, pp->bundle,
            "has promiser");
    WriterWriteF(writer, "  \"%s\"\n", NovaEscape(pp->promiser));
    WriterWriteF(writer,
            "      association => a(\"makes promise of type\",\"promise_types::%s\",\"promises have been made by\");\n",
            pp->parent_subtype->name);
    WriterWriteF(writer, "  \"%s\"\n", NovaEscape(pp->promiser));
    WriterWriteF(writer, "      association => a(\"has alias\",\"%s\",\"is a promise handle for\");\n", promise_id);

    WriterWriteF(writer, "promise_types::\n");
    WriterWriteF(writer, "  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n", pp->agentsubtype, "is promised in", pp->bundle,
            "has promises of type");

/* Look for copies and edits that lead to influence imports */

    if (strcmp(pp->agentsubtype, "files") == 0)
    {
        Rlist *servers = GetListConstraint("servers", pp);
        FnCall *edit_bundle = (FnCall *) GetConstraintValue("edit_line", pp, RVAL_TYPE_FNCALL);
        char *source = GetConstraintValue("source", pp, RVAL_TYPE_SCALAR);

        for (rp = servers; rp != NULL; rp = rp->next)
        {
            WriterWriteF(writer, "files:: \"%s\" association => a(\"might use data from\",\"%s\",\"might provide data for\");  \n",
                    pp->promiser, (const char *) rp->item);

            WriterWriteF(writer, "class_contexts:: \"%s\" association => a(\"uses data from\",\"hosts::%s\",\"provides data for\");  \n",
                    pp->classes, (const char *) rp->item);

            WriterWriteF(writer, "class_contexts:: \"%s\" association => a(\"%s\",\"hosts::%s\",\"%s\");  \n",
                    pp->classes, KM_CONNECTS_UNCERT_F, (const char *) rp->item, KM_CONNECTS_UNCERT_F);

        }

        if (source)
        {
            WriterWriteF(writer, "files:: \"%s\" association => a(\"uses data from\",\"%s\",\"provides data for\");  \n", pp->promiser,
                    source);
        }

        if (edit_bundle)
           {
           WriterWriteF(writer, "bundles:: \"%s\" association => a(\"%s\",\"%s\",\"%s\");  \n", pp->bundle, KM_USES_CERT_F, (const char *) edit_bundle->name,KM_USES_CERT_B);
        
            if (strcmp(edit_bundle->name, "insert_file") == 0)  // stdlib
            {
                if (edit_bundle->args)  // Single arg
                {
                    WriterWriteF(writer, "files:: \"%s\" association => a(\"uses data from\",\"%s\",\"provides data for\");  \n", NovaEscape(pp->promiser), (const char *) edit_bundle->args);
                    WriterWriteF(writer, "handles:: \"%s\" association => a(\"uses data from\",\"%s\",\"provides data for\");  \n", promise_id, (const char *) edit_bundle->args);
                }
            }
        }
    }
    
/* Look for bundles used as promises through methods  -- these are service bundles */
    
    if (strcmp(pp->agentsubtype, "meta") == 0)
    {
        for (size_t i = 0; i < SeqLength(pp->conlist); i++)
        {
            Constraint *cp = SeqAt(pp->conlist, i);

            char buffer[CF_BUFSIZE];

            RvalPrint(buffer, sizeof(buffer), cp->rval);
            WriterWriteF(writer, "occurrences: \n\n");
            WriterWriteF(writer, "  \"%s\"  representation => \"literal\",\n", buffer);
            WriterWriteF(writer, "  about_topics => { \"bundles::%s\"},", pp->bundle);
            WriterWriteF(writer, "   represents => { \"%s\" }; \n", pp->promiser);
        }
    }


    if (strcmp(pp->agentsubtype, "methods") == 0)
    {
        FnCall *fnp;

        WriterWriteF(writer, "topics: \n\n");

        // Look at the unexpanded promise to see the variable refs
        for (size_t i = 0; i < SeqLength(pp->conlist); i++)
        {
            Constraint *cp = SeqAt(pp->conlist, i);

            if (strcmp(cp->lval, "usebundle") == 0)
            {
                switch (cp->rval.type)
                {
                case RVAL_TYPE_SCALAR:
                    bundlename = (char *) cp->rval.item;
                    break;
                case RVAL_TYPE_FNCALL:
                    fnp = (FnCall *) cp->rval.item;
                    bundlename = fnp->name;
                    break;

                default:
                    break;
                }
            }
        }

        for (size_t i = 0; i < SeqLength(pp->org_pp->conlist); i++)
        {
            Constraint *cp = SeqAt(pp->org_pp->conlist, i);

            if (strcmp(cp->lval, "usebundle") == 0)
            {
                Rlist *allvars = NULL;

                switch (cp->rval.type)
                {
                case RVAL_TYPE_SCALAR:

                    MapIteratorsFromRval(pp->bundle, &allvars, &allvars, (Rval) {cp->rval.item, RVAL_TYPE_SCALAR}, pp);

                    for (rp2 = allvars; rp2 != NULL; rp2 = rp2->next)
                    {
                        WriterWriteF(writer, "bundles::\n\n");
                        WriterWriteF(writer, "  \"%s\"\n", bundlename);
                        if (strchr(rp2->item, '.'))
                        {
                            WriterWriteF(writer, "      association => a(\"%s\",\"variables::%s\",\"%s\");\n",
                                    NOVA_ISIMPACTED, (const char *) rp2->item, NOVA_IMPACTS);
                        }
                        else
                        {
                            WriterWriteF(writer, "      association => a(\"%s\",\"variables::%s.%s\",\"%s\");\n",
                                    NOVA_ISIMPACTED, pp->bundle, (const char *) rp2->item, NOVA_IMPACTS);
                        }
                    }

                    RlistDestroy(allvars);
                    break;

                case RVAL_TYPE_FNCALL:
                    fnp = (FnCall *) cp->rval.item;

                    // For each argument, variables in actual params affect the bundle

                    for (rp = fnp->args; rp != NULL; rp = rp->next)
                    {
                        MapIteratorsFromRval(pp->bundle, &allvars, &allvars, (Rval) {rp->item, RVAL_TYPE_SCALAR}, pp);

                        for (rp2 = allvars; rp2 != NULL; rp2 = rp2->next)
                        {
                            WriterWriteF(writer, "bundles::\n\n");
                            WriterWriteF(writer, "  \"%s\"\n", bundlename);
                            if (strchr(rp2->item, '.'))
                            {
                                WriterWriteF(writer, "      association => a(\"%s\",\"variables::%s\",\"%s\");\n",
                                        NOVA_ISIMPACTED, (const char *) rp2->item, NOVA_IMPACTS);
                            }
                            else
                            {
                                WriterWriteF(writer, "      association => a(\"%s\",\"variables::%s.%s\",\"%s\");\n",
                                        NOVA_ISIMPACTED, pp->bundle, (const char *) rp2->item, NOVA_IMPACTS);
                            }
                        }

                        RlistDestroy(allvars);
                    }

                    break;

                default:
                    break;
                }
            }
            else                // must be something generic action/classes etc
            {
                switch (cp->rval.type)
                {
                case RVAL_TYPE_SCALAR:
                    if (cp->references_body)
                    {
                        bodyname = (char *) cp->rval.item;
                    }
                    break;

                case RVAL_TYPE_FNCALL:
                    fnp = (FnCall *) cp->rval.item;
                    bodyname = fnp->name;
                    break;

                default:
                    break;
                }

                if (bodyname)
                {
                    char bodyref[CF_MAXVARSIZE];

                    snprintf(bodyref, CF_MAXVARSIZE, "bodies::%s", bodyname);
                    WriterWriteF(writer, "promise_types::  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n", pp->agentsubtype,
                            KM_USES_POSS_F, bodyref, KM_USES_POSS_B);
                    WriterWriteF(writer, "handles::  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n", promise_id,
                            KM_USES_CERT_F, bodyref, KM_USES_CERT_B);
                    WriterWriteF(writer, "body_constraints::  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n", cp->lval,
                            KM_USES_POSS_F, bodyref, "has a body constraint of type");
                }
            }

            if (bundlename)
            {
                Bundle *bp;

                if (pp->ref)
                {
                    WriterWriteF(writer, "occurrences: \n\n");
                    WriterWriteF(writer, " bundles:: \"%s\"  representation => \"literal\",\n", pp->ref);
                    WriterWriteF(writer, " about_topics => { \"%s\"},",bundlename);
                    WriterWriteF(writer, "   represents => { \"description\" }; \ntopics:\n");
                }

                // The used bundle affects the parent, in principle

                WriterWriteF(writer, "bundles::\n\n");
                WriterWriteF(writer, "  \"%s\"\n", bundlename);
                WriterWriteF(writer, "      association => a(\"%s\",\"bundles::%s\",\"%s\");\n", NOVA_IMPACTS,
                        (const char *) pp->bundle, NOVA_ISIMPACTED);

                /* Bundlename is a conduit that is said to affect its formal
                   parameters, through the substituted values */

                if (IsCf3VarString(bundlename))
                {
                    // Method by parameter cannot be determined before runtime
                    break;
                }
                
                const Policy *policy = PolicyFromPromise(pp);
                bp = PolicyGetBundle(policy, NULL, "agent", bundlename);
                if (!bp)
                {
                    bp = PolicyGetBundle(policy, NULL, "common", bundlename);
                }

                if (bp == NULL)
                {
                    CfOut(OUTPUT_LEVEL_ERROR, "", " Non-existent bundle \"%s\" referenced in \"%s\"\n", bundlename, (char *)cp->rval.item);
                    break;
                }

                for (rp = bp->args; rp != NULL; rp = rp->next)
                {
                    WriterWriteF(writer, "bundles::\n\n");
                    WriterWriteF(writer, "  \"%s\"\n", bundlename);
                    WriterWriteF(writer, "      association => a(\"%s\",\"parameters::%s\",\"%s\");\n", NOVA_ISIMPACTED,
                            (const char *) rp->item, NOVA_IMPACTS);
                }

                break;
            }
        }
    }

    WriterWriteF(writer, "topics: \n\n");
    
    for (size_t i = 0; i < SeqLength(pp->conlist); i++)
    {
        Constraint *cp = SeqAt(pp->conlist, i);

        WriterWriteF(writer, "handles::  \"%s\" association => a(\"%s\",\"body_constraints::%s\",\"%s\");\n", promise_id,
               KM_USES_CERT_F,cp->lval, KM_USES_CERT_B);

        WriterWriteF(writer, "promisers::  \"%s\" association => a(\"%s\",\"body_constraints::%s\",\"%s\");\n", NovaEscape(pp->promiser),
               KM_AFFECTS_CERT_B,cp->lval, KM_AFFECTS_CERT_F);
    }
    
    
/* Promisees as topics too */

    switch (pp->promisee.type)
    {
    case RVAL_TYPE_SCALAR:
        WriterWriteF(writer, "promisees::\n\n");
        WriterWriteF(writer, "  \"%s\"\n", (const char *) pp->promisee.item);
        WriterWriteF(writer, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_STAKEHOLDER, NovaEscape(pp->promiser), NOVA_STAKEHOLDER_INV);
        WriterWriteF(writer, "  \"%s\"\n", (const char *) pp->promisee.item);
        WriterWriteF(writer, "      association => a(\"%s\",\"promises::%s\",\"%s\");\n", NOVA_STAKEHOLDER, promise_id, NOVA_STAKEHOLDER_INV);

        WriterWriteF(writer, "  \"%s\"\n", (const char *) pp->promisee.item);
        WriterWriteF(writer, "      association => a(\"%s\",\"promises::%s\",\"%s\");\n", KM_AFFECTS_CERT_B, promise_id,
                KM_AFFECTS_CERT_F);
        WriterWriteF(writer, "  \"%s\"\n", (const char *) pp->promisee.item);
        WriterWriteF(writer, "      association => a(\"%s\",\"%s\",\"%s\");\n", KM_AFFECTS_CERT_B, NovaEscape(pp->promiser), KM_AFFECTS_CERT_F);

        WriterWriteF(writer, "  \"%s\"\n", (const char *) pp->promisee.item);
        WriterWriteF(writer, "      association => a(\"is a promisee for\",\"%s\",\"has promisee\");\n", NovaEscape(pp->promiser));


        for (rp = GOALS; rp != NULL; rp = rp->next)
        {
            if (FullTextMatch(rp->item, pp->promisee.item))
            {
                WriterWriteF(writer, "promises::\n\n");
                WriterWriteF(writer, "  \"%s\"\n", promise_id);
                WriterWriteF(writer, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_IMPACTS,
                        (const char *) pp->promisee.item, NOVA_ISIMPACTED);

                if (bundlename)
                {
                    WriterWriteF(writer, "bundles::\n\n");
                    WriterWriteF(writer, "  \"%s\"\n", bundlename);
                    WriterWriteF(writer, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_IMPACTS,
                            (const char *) pp->promisee.item, NOVA_ISIMPACTED);
                }

                if (strstr(pp->promisee.item,"::"))
                {
                    WriterWriteF(writer, " handles:: \"%s\"  association => a(\"%s\",\"%s\",\"%s\");\n", promise_id, NOVA_GOAL,
                             (const char *) pp->promisee.item, NOVA_GOAL_INV);
                }
                else
                {
                    WriterWriteF(writer, " handles:: \"%s\"  association => a(\"%s\",\"handles::%s\",\"%s\");\n", promise_id, NOVA_GOAL,
                             (const char *) pp->promisee.item, NOVA_GOAL_INV);
                }
            }
        }
        break;

    case RVAL_TYPE_LIST:

        WriterWriteF(writer, "promisees::\n\n");
        for (rp = (Rlist *) pp->promisee.item; rp != NULL; rp = rp->next)
        {
            WriterWriteF(writer, "  \"%s\"\n", (const char *) rp->item);
            WriterWriteF(writer, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_STAKEHOLDER, NovaEscape(pp->promiser), NOVA_STAKEHOLDER_INV);
            WriterWriteF(writer, "  \"%s\"\n", (const char *) rp->item);
            WriterWriteF(writer, "      association => a(\"%s\",\"promises::%s\",\"%s\");\n", NOVA_STAKEHOLDER, promise_id, NOVA_STAKEHOLDER_INV);

            WriterWriteF(writer, "  \"%s\"\n", (const char *) rp->item);
            WriterWriteF(writer, "      association => a(\"is a promisee for\",\"%s\",\"has promisee\");\n", NovaEscape(pp->promiser));

            // Make goals knowledge explicit is promisee mentioned a goal

            WriterWriteF(writer, "topics:\n\n");
                    
            for (rp2 = GOALS; rp2 != NULL; rp2 = rp2->next)
            {
                if (FullTextMatch(rp2->item, rp->item))
                {
                    WriterWriteF(writer, "promises::\n\n");
                    WriterWriteF(writer, "  \"%s\"\n", promise_id);
                    WriterWriteF(writer, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_IMPACTS,
                            (const char *) rp2->item, NOVA_ISIMPACTED);

                    if (bundlename)
                    {
                        WriterWriteF(writer, "bundles::\n\n");
                        WriterWriteF(writer, "  \"%s\"\n", bundlename);
                        WriterWriteF(writer, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_IMPACTS,
                                (const char *) rp->item, NOVA_ISIMPACTED);
                    }


                    if (strstr(rp->item,"::"))
                       {
                       WriterWriteF(writer, " handles:: \"%s\"  association => a(\"%s\",\"%s\",\"%s\");\n", promise_id, NOVA_GOAL,
                                (const char *) rp->item, NOVA_GOAL_INV);

                       // If a goal is identified, other promisees are by implication stakeholders in those goals
                       for (Rlist *rp3 = (Rlist *) pp->promisee.item; rp3 != NULL; rp3 = rp3->next)
                       {
                          if (strcmp(rp3->item, rp->item) != 0)
                             {
                                 char t[CF_BUFSIZE], c[CF_BUFSIZE];
                                 DeClassifyTopic((char *)rp3->item, t, c);
                                 if (strcmp(t, rp->item) != 0)
                                 {
                                     WriterWriteF(writer, "%s:: \"%s\"  association => a(\"%s\",\"%s\",\"%s\");\n", c, t, NOVA_STAKEHOLDER_INV,(const char *) rp->item, NOVA_STAKEHOLDER);
                                 }
                             }
                       }
                    }
                    else
                    {
                       WriterWriteF(writer, " handles:: \"%s\"  association => a(\"%s\",\"handles::%s\",\"%s\");\n", promise_id, NOVA_GOAL, (const char *) rp->item, NOVA_GOAL_INV);
                       
                       // If a goal is identified, other promisees are by implication stakeholders in those goals
                       for (Rlist *rp3 = (Rlist *) pp->promisee.item; rp3 != NULL; rp3 = rp3->next)
                       {
                          if (strcmp(rp3->item, rp->item) != 0)
                             {
                                 char t[CF_BUFSIZE], c[CF_BUFSIZE];
                                 DeClassifyTopic((char *)rp3->item, t, c);
                                 if (strcmp(t, rp->item) != 0)
                                 {
                                     WriterWriteF(writer, "%s:: \"%s\"  association => a(\"%s\",\"handles::%s\",\"%s\");\n", c, t, NOVA_STAKEHOLDER_INV,(const char *) rp->item, NOVA_STAKEHOLDER);
                                 }
                             }
                       }
                    }
                }
            }
        }

        break;
    default:
        break;

    }

/* This promise handle's place in the cosmos */

    WriterWriteF(writer, "promises::\n\n");

    WriterWriteF(writer, "\"%s\"\n", promise_id);

    if (pp->ref)
    {
        WriterWriteF(writer, "   comment => \"%s\";\n", NovaEscape(pp->ref));
    }
    else
    {
        WriterWriteF(writer, "   comment => \"Uncommented \\\"%s\\\" promise\";\n", pp->agentsubtype);
    }

    WriterWriteF(writer, "\"%s\" association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n", promise_id, NOVA_ACTIVATED,
            pp->classes, NOVA_ACTIVATES);
    WriterWriteF(writer, "\"%s\" association => a(\"%s\",\"%s\",\"%s\");\n", promise_id, NOVA_TYPE,
                 pp->agentsubtype, NOVA_TYPE_INV);

    for (rp = depends_on; rp != NULL; rp = rp->next)
    {
        WriterWriteF(writer, "  \"%s\"\n", promise_id);
        WriterWriteF(writer, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_USES_PR, (const char *) rp->item,
                NOVA_GIVES_PR);
        WriterWriteF(writer, "  \"%s\"\n", promise_id);
        WriterWriteF(writer, "      association => a(\"%s\",\"%s\",\"%s\");\n", KM_AFFECTS_CERT_B, (const char *) rp->item,
                KM_AFFECTS_CERT_F);
        WriterWriteF(writer, "  \"%s\"\n", NovaEscape(pp->promiser));
        WriterWriteF(writer, "      association => a(\"%s\",\"%s\",\"%s\");\n", KM_AFFECTS_CERT_B, (const char *) rp->item,
                KM_AFFECTS_CERT_F);
    }

//DeleteRlist(depends_on);

    WriterWriteF(writer," class_contexts::\n");

    for (rp = class_list; rp != NULL; rp = rp->next)
    {
        WriterWriteF(writer, "  \"%s\"\n", promise_id);
        WriterWriteF(writer, "      association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n", NOVA_ACTIVATED,
                NovaEscape(rp->item), NOVA_ACTIVATES);
    }

    // Register which classes affect class expressions
    
    for (rp = class_list; rp != NULL; rp = rp->next)
    {
        WriterWriteF(writer, "  \"%s\"\n", pp->classes);
        WriterWriteF(writer, "      association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n", KM_AFFECTS_CERT_B,
                (char *)rp->item, KM_AFFECTS_CERT_F);
    }
    
    RlistDestroy(class_list);

/* Now pointers to the policy compilation */

    WriterWriteF(writer, "\n occurrences:\n");

    WriterWriteF(writer, "handles::\n");
    WriterWriteF(writer, "\"%s/%s\",\n", PROMISE_CONTROLLER_PREFIX, promise_id);
    WriterWriteF(writer, "   about_topics => { \"handles::%s\" },\n", promise_id);
    WriterWriteF(writer, "   represents => { \"definition\" };\n\n");

    WriterWriteF(writer, "bundles::\n");
    WriterWriteF(writer, "\"%s/%s\"\n", BUNDLE_CONTROLLER_PREFIX, pp->bundle);
    WriterWriteF(writer, "   about_topics => { \"bundles::%s\" },\n", pp->bundle);
    WriterWriteF(writer, "   represents => { \"parent bundle\" };\n\n");

/*  Now we should analyze the classes to look for dependents and dependencies */

    Nova_MapClassParameterAssociations(writer, pp, promise_id);

// Look for any networks mentioned

    // Nova_ScanAccessRelationships(fp, pp, promise_id);  FIXME: fp undefined
}

/*****************************************************************************/

static char *NovaEscape(const char *s)
{
    const char *sp1;
    char *sp2;
    static char buffer[CF_EXPANDSIZE];

    memset(buffer, 0, CF_EXPANDSIZE);

    if (s == NULL)
    {
        return buffer;
    }

    if (strlen(s) == 0)
    {
        return buffer;
    }

    for (sp1 = s, sp2 = buffer; *sp1 != '\0'; sp1++)
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

const char *PromiseID(const Promise *pp)
{
    static char id[CF_MAXVARSIZE];
    char vbuff[CF_MAXVARSIZE];
    const char *handle = GetConstraintValue("handle", pp, RVAL_TYPE_SCALAR);

    if (LICENSES == 0)
    {
        return "license_expired";
    }

    if (handle)
    {
        snprintf(id, CF_MAXVARSIZE, "%s", CanonifyName(handle));
    }
    else if (pp && pp->audit)
    {
        snprintf(vbuff, CF_MAXVARSIZE, "%s", ReadLastNode(pp->audit->filename));
        snprintf(id, CF_MAXVARSIZE, "promise_%s_%zu", CanonifyName(vbuff), pp->offset.line);
    }
    else
    {
        snprintf(id, CF_MAXVARSIZE, "unlabelled_promise");
    }

    return id;
}

/*********************************************************************/

char *Name2Id(char *s)
{
    static char ret[CF_BUFSIZE], detox[CF_BUFSIZE];
    char *sp;

    strncpy(ret, s, CF_BUFSIZE - 1);

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

    strncpy(detox, CanonifyName(ret), CF_BUFSIZE - 1);
    return detox;
}

/*****************************************************************************/

void RegisterBundleDependence(char *name, const Promise *pp)
{
    char assertion[CF_BUFSIZE];
    char *handle;

    /*
     * NOVA_BUNDLEDEPENDENCE is used in a single place, namely in the generation
     * of knowledge map. This list is not being emptied anywhere, so just avoid
     * populating it if generation of knowledge map is not requested, otherwise
     * it creates a memory leak for long-living processes.
     */
    if (SHOWREPORTS == false)
    {
        return;
    }

    if (pp == NULL || pp->bundle == NULL)
    {
        return;
    }

    if (strcmp(name, pp->bundle) == 0)
    {
        return;
    }

    if (strcmp(name, "const") == 0)
    {
        return;
    }

/* Store everything first in a list because we don't have access to the
   output channel here -- summarize at the end. */

    snprintf(assertion, CF_BUFSIZE - 1, "topics: \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n", name,
             NOVA_BUNDLE_DATA_INV_B, pp->bundle, NOVA_BUNDLE_DATA);

    PrependItemList(&NOVA_BUNDLEDEPENDENCE, assertion);

    if ((handle = (char *) GetConstraintValue("handle", pp, RVAL_TYPE_SCALAR)))
    {
        snprintf(assertion, CF_BUFSIZE - 1, "topics: \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n", name,
                 NOVA_BUNDLE_DATA_INV_P, handle, NOVA_BUNDLE_DATA);

        PrependItemList(&NOVA_BUNDLEDEPENDENCE, assertion);
    }
}

/*****************************************************************************/

static void Nova_ShowBundleDependence(Writer *writer)
{
    Item *ip;

    for (ip = NOVA_BUNDLEDEPENDENCE; ip != NULL; ip = ip->next)
    {
        WriterWriteF(writer, "%s", ip->name);
    }
}

/*****************************************************************************/

void ShowTopicRepresentation(const ReportContext *report_context)
{
    Writer *writer = report_context->report_writers[REPORT_OUTPUT_TYPE_KNOWLEDGE];
    assert(writer);
    if (!writer)
    {
        return;
    }

    int i, j, k, l;
    const SubTypeSyntax *ss;
    const BodySyntax *bs, *bs2;
    static char *level[] = { "high", "low", "normal", NULL };
    static char *dev[] = { "dev1", "dev2", "microanomaly", "anomaly", NULL };

    if (LICENSES == 0)
    {
        return;
    }

    WriterWriteF(writer,
            "\n# This information is jointly Copyrighted by (C) Cfengine and the Licensee may not be redistributed without the permission of both parties\ntopics:\n");

    WriterWriteF(writer,
            "any:: \"system_reports\" comment => \"Reports collected from Cfengine managed systems by a reporting hub\";\n");
    WriterWriteF(writer,
            "\"remote_scalars\" comment => \"Scalar variable values that are made accessible to remote agents through cf-serverd\";\n");

    WriterWriteF(writer, "system_reports::\n");

    for (i = 0; i < cfrep_unknown; i++)
    {
        WriterWriteF(writer, "  \"%s\" comment => \"%s\",\n", BASIC_REPORTS[i].name_old, BASIC_REPORTS[i].description);
        WriterWriteF(writer, "         association => a(\"is now called\",\"%s\",\"was previously called\");\n",
                BASIC_REPORTS[i].name);

        WriterWriteF(writer, "  \"%s\" comment => \"%s\",\n", BASIC_REPORTS[i].name, BASIC_REPORTS[i].description);
        WriterWriteF(writer, "         association => a(\"deals with\",\"%s\",\"reports\");\n", BASIC_REPORTS[i].category);
    }

    WriterWriteF(writer, "  # New assocs\n");

    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_classes].name);
    WriterWriteF(writer, "    association => a(\"%s\",\"promise_types::classes\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_lastseen].name);
    WriterWriteF(writer, "   association => a(\"is affected by\",\"body_constraints::copy_from\",\"%s\");\n", NOVA_REPORTED);
    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_lastseen].name);
    WriterWriteF(writer, "   association => a(\"is affected by\",\"promise_types::access\",\"%s\");\n", NOVA_REPORTED);

    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_promise_compliance].name);
    WriterWriteF(writer, "    association => a(\"is based on\",\"promises\",\"%s\");\n", NOVA_REPORTED);
    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_promise_compliance].name);
    WriterWriteF(writer, "    association => a(\"%s\",\"promise report\",\"%s\");\n", NOVA_SEEALSO, NOVA_SEEALSO);

    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_performance].name);
    WriterWriteF(writer, "    association => a(\"is based on\",\"promises\",\"%s\");\n", NOVA_REPORTED);
    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_setuid].name);
    WriterWriteF(writer, "    association => a(\"%s\",\"promise_types::files\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    WriterWriteF(writer, "  \"hashes report\"\n");
    WriterWriteF(writer, "    association => a(\"%s\",\"promise_types::files\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    WriterWriteF(writer, "  \"hashes report\"\n");
    WriterWriteF(writer, "    association => a(\"%s\",\"body_constraints::changes\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_change].name);
    WriterWriteF(writer, "    association => a(\"%s\",\"promise_types::files\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_change].name);
    WriterWriteF(writer, "    association => a(\"%s\",\"body_constraints::changes\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_diff].name);
    WriterWriteF(writer, "    association => a(\"%s\",\"promise_types::files\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_diff].name);
    WriterWriteF(writer, "    association => a(\"%s\",\"body_constraints::changes\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_software_installed].name);
    WriterWriteF(writer, "    association => a(\"%s\",\"promise_types::packages\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_patch_status].name);
    WriterWriteF(writer, "    association => a(\"%s\",\"promise_types::packages\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    WriterWriteF(writer, "  \"%s\"\n", BASIC_REPORTS[cfrep_patch_avail].name);
    WriterWriteF(writer, "    association => a(\"%s\",\"promise_types::packages\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);

    WriterWriteF(writer, "system_policy::\n");
    WriterWriteF(writer, "  \"bundles\" comment => \"A modular collection of promises of different types\";\n");
    WriterWriteF(writer, "  \"bodies\" comment => \"A modular collection of body constraints for re-use\";\n");
    WriterWriteF(writer, "  \"class contexts\" comment => \"Class expressions that say where or when a promise applies\";\n");
    WriterWriteF(writer, "  \"promisees\" comment => \"Recipients of a promise, i.e. promise handles, or persons\";\n");
    WriterWriteF(writer, "  \"promisers\" comment => \"The objects affected by a promise\";\n");
    WriterWriteF(writer, "  \"promises\" comment => \"Complete occurrences of promiser + promisee + promise-body\";\n");
    WriterWriteF(writer, "  \"promise types\" comment => \"The types of promise that cfengine can keep\";\n");
    WriterWriteF(writer,
            "  \"body constraints\" comment => \"The attributes that cfengine enables you to promise about the promiser\";\n");
    WriterWriteF(writer, "\"comments\"\n");
    WriterWriteF(writer, "      association => a(\"see instances of\",\"comment\",\"is one of a number of\");\n");
    WriterWriteF(writer, "\"functions\" comment => \"In-built functions that may be used to set variables or classes\";");

    WriterWriteF(writer, " \"values\"  comment => \"Formal rvalues in constraint assignments and their legal ranges\";\n");


    // Reports linkage

    WriterWriteF(writer, "occurrences:\n\n");
        
    for (i = 0; i < cfrep_unknown; i++)
    {
        WriterWriteF(writer, "\"%s/%s\" represents => { \"report generator\" }, about_topics => { \"system_reports::%s\", \"system_reports::%s\" };\n", REPORT_CONTROLLER_PREFIX, BASIC_REPORTS[i].id, BASIC_REPORTS[i].name, BASIC_REPORTS[i].name_old);
    }


    // Value types
        
    WriterWriteF(writer, "topics:\n");
    WriterWriteF(writer, "values::\n\n");

    for (i = 0; CF_VALUETYPES[i][0] != NULL; i++)
    {
        WriterWriteF(writer, "\"%s\"   comment =>\"Should match the generic pattern %s, i.e. %s\";\n",
                CF_VALUETYPES[i][1], NovaEscape(CF_VALUETYPES[i][0]), CF_VALUETYPES[i][2]);
    }

    WriterWriteF(writer, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[1][1], CF_DATATYPES[DATA_TYPE_INT]);
    WriterWriteF(writer, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[3][1], CF_DATATYPES[DATA_TYPE_INT]);
    WriterWriteF(writer, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[4][1], CF_DATATYPES[DATA_TYPE_INT]);
    WriterWriteF(writer, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[5][1], CF_DATATYPES[DATA_TYPE_INT]);
    WriterWriteF(writer, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[6][1], CF_DATATYPES[DATA_TYPE_REAL]);
    WriterWriteF(writer, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[9][1], CF_DATATYPES[DATA_TYPE_CONTEXT]);
    WriterWriteF(writer, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[10][1], CF_DATATYPES[DATA_TYPE_STRING]);
    WriterWriteF(writer, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[11][1], CF_DATATYPES[DATA_TYPE_STRING]);
    WriterWriteF(writer, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[12][1], CF_DATATYPES[DATA_TYPE_STRING]);
    WriterWriteF(writer, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[13][1], CF_DATATYPES[DATA_TYPE_STRING]);
    WriterWriteF(writer, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[14][1], CF_DATATYPES[DATA_TYPE_STRING]);
    WriterWriteF(writer, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[15][1], CF_DATATYPES[DATA_TYPE_STRING]);

    WriterWriteF(writer, "bundles::\n");
    WriterWriteF(writer, "\"sys\" comment => \"cfengine's internal bundle of system specific values\";\n");
    Nova_ShowBundleDependence(writer);

    for (i = 0; i < CF3_MODULES; i++)
    {
        if ((ss = CF_ALL_SUBTYPES[i]) == NULL)
        {
            continue;
        }

        for (j = 0; ss[j].bundle_type != NULL; j++)
        {
            WriterWriteF(writer,
                    "occurrences:  \"/docs/cf3-Reference.html#%s-in-agent-promises\" represents => { \"manual reference %s\" }, about_topics => { \"%s\"}; \n",
                    ss[j].subtype, ss[j].subtype,ss[j].subtype);

            if (ss[j].bs != NULL)       /* In a bundle */
            {
                bs = ss[j].bs;

                for (l = 0; bs[l].lval != NULL; l++)
                {
                    WriterWriteF(writer, "topics:\n promise_types::\n");
                    WriterWriteF(writer, "   \"%s\";\n", ss[j].subtype);

                    WriterWriteF(writer, "body_constraints::\n");
                    WriterWriteF(writer, "   \"%s\"\n", bs[l].lval);
                    WriterWriteF(writer, "   comment => \"%s\",\n", NovaEscape(bs[l].description));
                    WriterWriteF(writer, "   association => a(\"%s\",\"promise_types::%s\",\"%s\");\n", KM_PARTOF_POSS_F,
                            ss[j].subtype, KM_PARTOF_POSS_B);

                    if (bs[l].dtype == DATA_TYPE_BODY)
                    {
                        bs2 = (BodySyntax *) (bs[l].range);

                        if (bs2 == NULL || bs2 == (void *) CF_BUNDLE)
                        {
                            continue;
                        }

                        // Certain lvals can be parts of body templates

                        for (k = 0; bs2[k].lval != NULL; k++)
                        {
                            WriterWriteF(writer, "   \"%s\"\n", bs2[k].lval);
                            WriterWriteF(writer, "   comment => \"%s\",\n", NovaEscape(bs2[k].description));
                            WriterWriteF(writer, "   association => a(\"%s\",\"%s\",\"%s\");\n", KM_PARTOF_POSS_F, bs[l].lval,
                                    KM_PARTOF_POSS_B);

                            NovaShowValues(writer, bs2[k]);
                        }
                    }
                    else
                    {
                        NovaShowValues(writer, bs[l]);
                    }
                }
            }
        }
    }

    for (i = 0; CF_COMMON_BODIES[i].lval != NULL; i++)
    {
        WriterWriteF(writer, "   \"%s\";\n", CF_COMMON_BODIES[i].lval);
    }

    for (i = 0; CF_COMMON_EDITBODIES[i].lval != NULL; i++)
    {
        WriterWriteF(writer, "   \"%s\";\n", CF_COMMON_EDITBODIES[i].lval);
    }

    WriterWriteF(writer, "miscellaneous_concepts::\n");

    WriterWriteF(writer, "  \"data types\";\n");

    WriterWriteF(writer, "data_types::\n");

    for (i = 0; CF_VARBODY[i].lval != NULL; i++)
    {
        WriterWriteF(writer, "  \"%s\" comment => \"%s matching %s\";\n", CF_VARBODY[i].lval, (char *) CF_VARBODY[i].description,
                (const char *) CF_VARBODY[i].range);
    }

    WriterWriteF(writer, "  \"class\" comment => \"A boolean returned by certain functions in classes promises\";\n");

    WriterWriteF(writer, "functions::\n\n");

    for (i = 0; CF_FNCALL_TYPES[i].name != NULL; i++)
    {
        WriterWriteF(writer, " \"%s\" ", CF_FNCALL_TYPES[i].name);
        WriterWriteF(writer, "    comment => \"%s\",\n", CF_FNCALL_TYPES[i].description);
        WriterWriteF(writer, "    association => a(\"returns data-type\",\"%s\",\"is returned by function\");\n",
                CF_DATATYPES[CF_FNCALL_TYPES[i].dtype]);
    }

// Things for monitoring system state

    WriterWriteF(writer, "things:\n");

    WriterWriteF(writer,
            "  \"measurements\" comment => \"Classes set by cf-monitord based on the observed system state.\"; \n");

    WriterWriteF(writer, "measurements::\n");
    
    for (i = 0; i < CF_OBSERVABLES; i++)
    {
        if (strcmp(OBS[i][0], "spare") == 0)
        {
            continue;
        }

        WriterWriteF(writer, " \"%s\" comment => \"%s\",", OBS[i][0], OBS[i][1]);
        WriterWriteF(writer, "      generalizations => { \"vital signs\", \"observables\" },");
        WriterWriteF(writer, "      determines => { \"actual state\" , \"measurements\", \"application services\" };\n");
    }

// Operating system classes

    WriterWriteF(writer, "operating_systems::");

    for (i = 2; CLASSTEXT[i] != NULL; i++)
    {
        WriterWriteF(writer, " \"%s\" generalizations => { \"operating systems\" };  ", CLASSTEXT[i]);
    }

    WriterWriteF(writer, " \"debian\" generalizations => { \"linux\"};");
    WriterWriteF(writer, " \"ubuntu\" generalizations => { \"linux\", \"debian\" };");
    WriterWriteF(writer, " \"redhat\" generalizations => { \"linux\"};");
    WriterWriteF(writer, " \"sles\" generalizations => { \"suse\", \"linux\"};");
    WriterWriteF(writer, " \"suse\" generalizations => { \"linux\"};");
    WriterWriteF(writer, " \"slackware\" generalizations => { \"linux\"};");
    WriterWriteF(writer, " \"fedora\" generalizations => { \"linux\"};");
    WriterWriteF(writer, " \"mandrake\" generalizations => { \"linux\"};");
    WriterWriteF(writer, " \"mandriva\" generalizations => { \"linux\"};");
    WriterWriteF(writer, " \"gentoo\" generalizations => { \"linux\"};");
    WriterWriteF(writer, " \"unitedlinux\" generalizations => { \"linux\"};");
    WriterWriteF(writer, " \"suncobalt\" generalizations => { \"linux\"};");
    WriterWriteF(writer, " \"Win2000\" generalizations => { \"windows\"};");
    WriterWriteF(writer, " \"WinXP\" generalizations => { \"windows\"};");
    WriterWriteF(writer, " \"WinServer2003\" generalizations => { \"windows\"};");
    WriterWriteF(writer, " \"WinVista\" generalizations => { \"windows\"};");
    WriterWriteF(writer, " \"WinServer2008\" generalizations => { \"windows\"};");


// Monitoring

    WriterWriteF(writer, "ports::");

    for (i = 0; i < ATTR; i++)
    {
        WriterWriteF(writer,
                "  \"port %s\" comment => \"The standard reserved port for %s\", generalizations => { \"ports\" }, synonyms => {\"ports::%s\"}; ",
                ECGSOCKS[i].portnr, ECGSOCKS[i].name, ECGSOCKS[i].name);
    }

    WriterWriteF(writer,
            "any:: \"anomalies\" comment => \"Measurements that exceed the boundaries of normal behaviour, as learned by cf-monitord\";");

    WriterWriteF(writer, "anomalies::");

    for (i = 0; OBS[i] != NULL; i++)
    {
        if (strcmp(OBS[i][0], "spare") == 0)
        {
            break;
        }

        for (j = 0; level[j] != NULL; j++)
        {
            for (k = 0; dev[k] != NULL; k++)
            {
                WriterWriteF(writer,
                        " \"%s_%s_%s\" comment => \"%s is %s relative to the learned normal average\", generalizations => { \"vital signs\", \"performance\", \"anomalies\" };\n",
                        OBS[i][0], level[j], dev[k], OBS[i][1], level[j]);

                if (strstr(OBS[i][0], "_in"))
                {
                    if (strstr(OBS[i][0], "_low"))
                    {
                        WriterWriteF(writer, " \"%s_%s_%s\" caused_by => { \"Reduced incoming traffic\" };", OBS[i][0], level[j],
                                dev[k]);
                    }
                    else if (strstr(OBS[i][0], "_high"))
                    {
                        WriterWriteF(writer, " \"%s_%s_%s\" caused_by => { \"Increased incoming traffic\" };", OBS[i][0],
                                level[j], dev[k]);
                    }
                }
                else if (strstr(OBS[i][0], "_out"))
                {
                    if (strstr(OBS[i][0], "_low"))
                    {
                        WriterWriteF(writer, " \"%s_%s_%s\" caused_by => { \"Reduced outgoing traffic\" };", OBS[i][0], level[j],
                                dev[k]);
                    }
                    else if (strstr(OBS[i][0], "_high"))
                    {
                        WriterWriteF(writer, " \"%s_%s_%s\" caused_by => { \"Increased outgoing traffic\" };", OBS[i][0],
                                level[j], dev[k]);
                    }
                }
            }
        }
    }


    static char *others[] = { MON_IO_READS, MON_IO_WRITES, MON_IO_READDATA, MON_IO_WRITTENDATA, MON_MEM_TOTAL, MON_MEM_FREE, MON_MEM_CACHED, MON_MEM_SWAP, MON_MEM_FREE_SWAP, NULL };

    WriterWriteF(writer," \"%s\" comment => \"Disk I/O reads measured by the kernel, an indication of read activity\", generalizations => { \"vital signs\", \"performance\", \"anomalies\" };\n",  MON_IO_READS);

    WriterWriteF(writer," \"%s\" comment => \"Disk I/O writes measured by the kernel, an indication of write activity\", generalizations => { \"vital signs\", \"performance\", \"anomalies\" };\n",  MON_IO_WRITES);

    WriterWriteF(writer," \"%s\" comment => \"Disk I/O reads measured by the kernel, an indication of read activity\", generalizations => { \"vital signs\", \"performance\", \"anomalies\" };\n",  MON_IO_READDATA);

    WriterWriteF(writer," \"%s\" comment => \"Disk I/O writes measured by the kernel, an indication of write activity\", generalizations => { \"vital signs\", \"performance\", \"anomalies\" };\n",  MON_IO_WRITTENDATA);

    WriterWriteF(writer," \"%s\" comment => \"Total amount of available memory on the system\", generalizations => { \"vital signs\", \"performance\", \"anomalies\" };\n",  MON_MEM_TOTAL);

    WriterWriteF(writer," \"%s\" comment => \"Total amount of free memory on the system\", generalizations => { \"vital signs\", \"performance\", \"anomalies\" };\n",  MON_MEM_FREE);

    WriterWriteF(writer," \"%s\" comment => \"Total amount of memory cache on the system\", generalizations => { \"vital signs\", \"performance\", \"anomalies\" };\n",  MON_MEM_CACHED);

    WriterWriteF(writer," \"%s\" comment => \"Total amount of available swap memory on the system\", generalizations => { \"vital signs\", \"performance\", \"anomalies\" };\n",  MON_MEM_SWAP);

    WriterWriteF(writer," \"%s\" comment => \"Total amount of available free swap memory on the system\", generalizations => { \"vital signs\", \"performance\", \"anomalies\" };\n",  MON_MEM_FREE_SWAP);

    
    
    for (i = 0; others[i] != NULL; i++)
       {       
       for (j = 0; level[j] != NULL; j++)
          {
          for (k = 0; dev[k] != NULL; k++)
             {
             WriterWriteF(writer,
                          " \"%s_%s_%s\" comment => \"%s is %s relative to the learned normal average\", generalizations => { \"vital signs\", \"performance\", \"anomalies\" };\n",
                          others[i], level[j], dev[k], others[i], level[j]);
             
             }
          }
       }


}

/*****************************************************************************/

static void NovaShowValues(Writer *writer, BodySyntax bs)
{
    int i;
    const char *range = NULL;

    for (i = 0; CF_VALUETYPES[i][0] != NULL; i++)
    {
        if (bs.dtype == DATA_TYPE_BUNDLE || bs.dtype == DATA_TYPE_BODY)
        {
            continue;
        }

        if (strcmp(CF_VALUETYPES[i][0], bs.range) == 0)
        {
            range = CF_VALUETYPES[i][1];
            break;
        }
    }

    if (range == NULL)
    {
        range = CF_DATATYPES[bs.dtype];
    }

    WriterWriteF(writer, "body_constraints::\n");
    WriterWriteF(writer,
            "   \"%s\" association => a(\"is a body constraint of type\",\"values::%s\",\"can be used in body constraints\"),\n",
            bs.lval, range);
    WriterWriteF(writer, "          comment => \"%s\";\n", NovaEscape(bs.description));

    switch (bs.dtype)
    {
    case DATA_TYPE_STRING_LIST:
    case DATA_TYPE_INT_LIST:
    case DATA_TYPE_REAL_LIST:

        WriterWriteF(writer, "   \"%s\" association => a(\"is a list of type\",\"%s\",\"is used in\");\n", bs.lval, range);

    default:
        break;
    }

    if (CF_VALUETYPES[i][0] != NULL)
    {
        WriterWriteF(writer, "values::\n\n");
        WriterWriteF(writer, " \"%s\" comment => \"Represent type %s and should match %s\",\n", range, CF_DATATYPES[bs.dtype],
                NovaEscape(CF_VALUETYPES[i][0]));
        WriterWriteF(writer,
                "    association => a(\"are used in constraint parameters\",\"body_constraints::%s\",\"takes value\");\n",
                bs.lval);
    }
}

/*****************************************************************************/

static void Nova_MapClassParameterAssociations(Writer *writer, const Promise *pp, char *promise_id)
{
    Rlist *impacted = NULL, *dependency = NULL, *potential, *rp;
    char *value;
    int found = false;

    if (promise_id && pp->ref)
    {
        WriterWriteF(writer, "topics: handles:: \"%s\"  comment => \"%s\", ", promise_id, NovaEscape(pp->ref));
        WriterWriteF(writer, "association => a(\"is the promise_id for\",\"%s\",\"has a promise with promise_id\");\n",
                NovaEscape(pp->promiser));
    }
    else if (promise_id)
    {
        WriterWriteF(writer, "topics: handles:: \"%s\" association => a(\"%s\",\"promisers::%s\",\"%s\");\n", promise_id,
                NOVA_HANDLE, NovaEscape(pp->promiser), NOVA_HANDLE_INV);
    }

    if (promise_id)
    {
        WriterWriteF(writer, "occurrences:  \"%s/%s\", represents => { \"definition\" }, about_topics => {\"handles::%s\" }; \n",
                     PROMISE_CONTROLLER_PREFIX, promise_id, promise_id);
    }

/* For activated classes we can assume that no one will */

    potential = GetListConstraint("promise_kept", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        RlistPrependScalarIdemp(&impacted, rp->item, RVAL_TYPE_SCALAR);
    }

    potential = GetListConstraint("promise_repaired", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        RlistPrependScalarIdemp(&impacted, rp->item, RVAL_TYPE_SCALAR);
    }

    potential = GetListConstraint("repair_failed", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        RlistPrependScalarIdemp(&impacted, rp->item, RVAL_TYPE_SCALAR);
    }

    potential = GetListConstraint("repair_denied", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        RlistPrependScalarIdemp(&impacted, rp->item, RVAL_TYPE_SCALAR);
    }

    potential = GetListConstraint("promise_timeout", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        RlistPrependScalarIdemp(&impacted, rp->item, RVAL_TYPE_SCALAR);
    }

    potential = GetListConstraint("or", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        RlistPrependScalarIdemp(&dependency, rp->item, RVAL_TYPE_SCALAR);
    }

    potential = GetListConstraint("and", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        RlistPrependScalarIdemp(&dependency, rp->item, RVAL_TYPE_SCALAR);
    }

    if ((value = GetConstraintValue("expression", pp, RVAL_TYPE_SCALAR)))
    {
        RlistPrependScalarIdemp(&dependency, value, RVAL_TYPE_SCALAR);
    }

// Now look for impact

    for (rp = dependency; rp != NULL; rp = rp->next)
    {
        WriterWriteF(writer, "topics:\n");
        WriterWriteF(writer, "class_contexts::");
        WriterWriteF(writer, "  \"%s\"\n", NovaEscape(pp->promiser));
        WriterWriteF(writer, "      association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n", NOVA_ISIMPACTED,
                NovaEscape(rp->item), NOVA_IMPACTS);

        // Might need to break these up further
    }

    RlistDestroy(dependency);

    if (impacted == NULL)
    {
        return;
    }

    CfOut(OUTPUT_LEVEL_VERBOSE, "", " ?? Analysing promise dependencies for \"%s\"\n", promise_id);

/* So now we have the list of classes to scan for and we must undertake this
 expensive search */

    const Policy *policy = PolicyFromPromise(pp);

    for (size_t i = 0; i < SeqLength(policy->bundles); i++)
    {
        Bundle *bp = SeqAt(policy->bundles, i);

        for (size_t j = 0; j < SeqLength(bp->subtypes); j++)
        {
            SubType *sp = SeqAt(bp->subtypes, j);

            for (size_t pp2i = 0; i < SeqLength(sp->promises); pp2i++)
            {
                Promise *pp2 = SeqAt(sp->promises, pp2i);

                if (pp2 == pp)
                {
                    continue;
                }

                for (rp = impacted; rp != NULL; rp = rp->next)
                {
                    char *varclass = GetConstraintValue("ifvarclass", pp, RVAL_TYPE_SCALAR);

                    if (strstr(pp2->classes, rp->item) || (varclass && strstr(varclass, rp->item)))
                    {
                        //Debug("Found %s in %s+%s\n",rp->item,pp2->classes,varclassPrint); //FIXME: Segfault on Solaris (Nullpointers?)
                        // found a connection
                        WriterWriteF(writer, "topics:\n");
                        WriterWriteF(writer, "promises::");
                        WriterWriteF(writer, "  \"%s\"\n", promise_id);
                        WriterWriteF(writer, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_IMPACTS, (char *) rp->item,
                                NOVA_ISIMPACTED);
                        WriterWriteF(writer, "promisers::");
                        WriterWriteF(writer, "  \"%s\"\n", NovaEscape(pp->promiser));
                        WriterWriteF(writer, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_IMPACTS, (char *) rp->item,
                                NOVA_ISIMPACTED);
                        found = true;
                    }

                    /* Omit class "any" */

                    if (strstr(pp2->classes, rp->item) && strcmp(rp->item, "any") != 0
                        && strcmp(pp->classes, "any") != 0)
                    {
                        WriterWriteF(writer, "class_contexts::");
                        WriterWriteF(writer, "  \"%s\"\n", pp->classes);
                        WriterWriteF(writer, "      association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n", NOVA_ACTIVATES,
                                NovaEscape(rp->item), NOVA_ACTIVATES);
                    }
                }
            }
        }
    }

    if (!found && THIS_AGENT_TYPE == AGENT_TYPE_COMMON)
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "",
              "Classes activated by the outcomes of promise ref \"%s\" did not occur in any promise proposal, so they are nullpotent",
              promise_id);
        PromiseRef(OUTPUT_LEVEL_INFORM, pp);
    }

    RlistDestroy(impacted);
}

/*****************************************************************************/

void DeClassifyTopic(char *classified_topic, char *topic, char *context)
{
    char *spm, *sp;
    context[0] = '\0';
    topic[0] = '\0';

    if (classified_topic == NULL)
    {
        return;
    }

    // Unqualified names may contain ":" but we assume the first "::" means a qualified context
    // as long as the context is a valid identifier
    
    if ((spm = strstr(classified_topic, "::")))
    {
        for (sp = classified_topic; *sp != '\0'; sp++)
        {
            if (strncmp(sp, "::", 2) == 0)
            {
                sscanf(classified_topic, "%255[^:]::%255[^\n]", context, topic);
                break;
            }
            else if (!isalnum(*sp) && *sp != '_')
            {
                strncpy(topic, classified_topic, CF_MAXVARSIZE - 1);
                break;
            }
        }
    }
    else
    {
        strncpy(topic, classified_topic, CF_MAXVARSIZE - 1);
    }

    if (strlen(context) == 0)
    {
        strcpy(context,"any");
    }
}
