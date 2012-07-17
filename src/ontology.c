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

/*****************************************************************************/

void Nova_RegisterImg(Item **list, char *dir, char *pic)
{
    char work[CF_MAXVARSIZE], *sp;

    strcpy(work, pic);

    for (sp = work; *sp != '\0'; sp++)
    {
        if (*sp == '.')
        {
            *sp = '\0';
            break;
        }
    }

    PrependItem(list, pic, work);
}

/*********************************************************************/

void Nova_RegisterDoc(Item **list, char *dir, char *doc)
{
    char title[CF_BUFSIZE] = { 0 }, path[CF_BUFSIZE], line[CF_BUFSIZE];
    int i;
    FILE *fp;

    snprintf(path, CF_BUFSIZE, "%s/%s", dir, doc);

    if ((fp = fopen(path, "r")) == NULL)
    {
        CfOut(cf_error, "fopen", "Couldn't open %s for reading", path);
        return;
    }

/* The title should be on line 3, but just in case something changes
   read only a few lines of these long docs
*/

    for (i = 0; i < 10; i++)
    {
        fgets(line, CF_BUFSIZE, fp);

        if (strncmp(line, "<title>", strlen("<title>")) == 0)
        {
            sscanf(line + strlen("<title>"), "%[^<]", title);
            break;
        }
    }

    fclose(fp);

    if (strlen(title) > 0)
    {
        PrependItem(list, doc, title);
    }
}

/*********************************************************************/

void Nova_MapPromiseToTopic(FILE *fp, Promise *pp, const char *version)
{
    char promise_id[CF_BUFSIZE];
    Rlist *rp, *rp2, *depends_on = GetListConstraint("depends_on", pp);
    Rlist *class_list = SplitRegexAsRList(pp->classes, "[.!()|&]+", 100, false);
    char *bundlename = NULL, *bodyname = NULL;
    Constraint *cp;

    if (LICENSES == 0)
    {
        fprintf(fp, "license expired or no license");
        return;
    }

    strcpy(promise_id, PromiseID(pp));

    fprintf(fp, "\ntopics:\n\n");

    fprintf(fp, "bundles::\n");
    fprintf(fp, "  \"%s\";\n", pp->bundle);

    fprintf(fp, "class_contexts::\n");
    fprintf(fp, "  \"%s\";\n", pp->classes);

/* First the bundle container */

    fprintf(fp, "promisers::\n\n");
    fprintf(fp, "  \"%s\"\n", NovaEscape(pp->promiser));
    fprintf(fp, "      association => a(\"%s\",\"bundles::%s\",\"%s\");\n", KM_PARTOF_CERT_F, pp->bundle,
            "has promiser");
    fprintf(fp, "  \"%s\"\n", NovaEscape(pp->promiser));
    fprintf(fp,
            "      association => a(\"makes promise of type\",\"promise_types::%s\",\"promises have been made by\");\n",
            pp->agentsubtype);
    fprintf(fp, "  \"%s\"\n", NovaEscape(pp->promiser));
    fprintf(fp, "      association => a(\"has alias\",\"%s\",\"is a promise handle for\");\n", promise_id);

    fprintf(fp, "promise_types::\n");
    fprintf(fp, "  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n", pp->agentsubtype, "is promised in", pp->bundle,
            "has promises of type");

/* Look for copies and edits that lead to influence imports */

    if (strcmp(pp->agentsubtype, "files") == 0)
    {
        Rlist *servers = GetListConstraint("servers", pp);
        FnCall *edit_bundle = (FnCall *) GetConstraintValue("edit_line", pp, CF_FNCALL);
        char *source = GetConstraintValue("source", pp, CF_SCALAR);

        for (rp = servers; rp != NULL; rp = rp->next)
        {
            fprintf(fp, "files:: \"%s\" association => a(\"might use data from\",\"%s\",\"might provide data for\");  \n",
                    pp->promiser, (const char *) rp->item);

            fprintf(fp, "class_contexts:: \"%s\" association => a(\"uses data from\",\"hosts::%s\",\"provides data for\");  \n",
                    pp->classes, (const char *) rp->item);

            fprintf(fp, "class_contexts:: \"%s\" association => a(\"%s\",\"hosts::%s\",\"%s\");  \n",
                    pp->classes, KM_CONNECTS_UNCERT_F, (const char *) rp->item, KM_CONNECTS_UNCERT_F);

        }

        if (source)
        {
            fprintf(fp, "files:: \"%s\" association => a(\"uses data from\",\"%s\",\"provides data for\");  \n", pp->promiser,
                    source);
        }

        if (edit_bundle)
           {
           fprintf(fp, "bundles:: \"%s\" association => a(\"%s\",\"%s\",\"%s\");  \n", pp->bundle, KM_USES_CERT_F, (const char *) edit_bundle->name,KM_USES_CERT_B);
        
            if (strcmp(edit_bundle->name, "insert_file") == 0)  // stdlib
            {
                if (edit_bundle->args)  // Single arg
                {
                    fprintf(fp, "files:: \"%s\" association => a(\"uses data from\",\"%s\",\"provides data for\");  \n", NovaEscape(pp->promiser), (const char *) edit_bundle->args);
                    fprintf(fp, "handles:: \"%s\" association => a(\"uses data from\",\"%s\",\"provides data for\");  \n", promise_id, (const char *) edit_bundle->args);                    
                }
            }
        }
    }

/* Look for bundles used as promises through methods  -- these are service bundles */

    if (strcmp(pp->agentsubtype, "methods") == 0)
    {
        FnCall *fnp;

        // Look at the unexpanded promise to see the variable refs

        for (cp = pp->conlist; cp != NULL; cp = cp->next)
        {
            if (strcmp(cp->lval, "usebundle") == 0)
            {
                switch (cp->rval.rtype)
                {
                case CF_SCALAR:
                    bundlename = (char *) cp->rval.item;
                    break;
                case CF_FNCALL:
                    fnp = (FnCall *) cp->rval.item;
                    bundlename = fnp->name;
                    break;
                }
            }
        }

        for (cp = pp->org_pp->conlist; cp != NULL; cp = cp->next)
        {
            if (strcmp(cp->lval, "usebundle") == 0)
            {
                Rlist *allvars = NULL;

                switch (cp->rval.rtype)
                {
                case CF_SCALAR:

                    MapIteratorsFromRval(pp->bundle, &allvars, &allvars, (Rval) {cp->rval.item, CF_SCALAR}, pp);

                    for (rp2 = allvars; rp2 != NULL; rp2 = rp2->next)
                    {
                        fprintf(fp, "bundles::\n\n");
                        fprintf(fp, "  \"%s\"\n", bundlename);
                        if (strchr(rp2->item, '.'))
                        {
                            fprintf(fp, "      association => a(\"%s\",\"variables::%s\",\"%s\");\n",
                                    NOVA_ISIMPACTED, (const char *) rp2->item, NOVA_IMPACTS);
                        }
                        else
                        {
                            fprintf(fp, "      association => a(\"%s\",\"variables::%s.%s\",\"%s\");\n",
                                    NOVA_ISIMPACTED, pp->bundle, (const char *) rp2->item, NOVA_IMPACTS);
                        }
                    }

                    DeleteRlist(allvars);
                    break;

                case CF_FNCALL:
                    fnp = (FnCall *) cp->rval.item;

                    // For each argument, variables in actual params affect the bundle

                    for (rp = fnp->args; rp != NULL; rp = rp->next)
                    {
                        MapIteratorsFromRval(pp->bundle, &allvars, &allvars, (Rval) {rp->item, CF_SCALAR}, pp);

                        for (rp2 = allvars; rp2 != NULL; rp2 = rp2->next)
                        {
                            fprintf(fp, "bundles::\n\n");
                            fprintf(fp, "  \"%s\"\n", bundlename);
                            if (strchr(rp2->item, '.'))
                            {
                                fprintf(fp, "      association => a(\"%s\",\"variables::%s\",\"%s\");\n",
                                        NOVA_ISIMPACTED, (const char *) rp2->item, NOVA_IMPACTS);
                            }
                            else
                            {
                                fprintf(fp, "      association => a(\"%s\",\"variables::%s.%s\",\"%s\");\n",
                                        NOVA_ISIMPACTED, pp->bundle, (const char *) rp2->item, NOVA_IMPACTS);
                            }
                        }

                        DeleteRlist(allvars);
                    }

                    break;

                default:
                    break;
                }
            }
            else                // must be something generic action/classes etc
            {
                switch (cp->rval.rtype)
                {
                case CF_SCALAR:
                    if (cp->references_body)
                    {
                        bodyname = (char *) cp->rval.item;
                    }
                    break;

                case CF_FNCALL:
                    fnp = (FnCall *) cp->rval.item;
                    bodyname = fnp->name;
                    break;
                }

                if (bodyname)
                {
                    char bodyref[CF_MAXVARSIZE];

                    snprintf(bodyref, CF_MAXVARSIZE, "bodies::%s", bodyname);
                    fprintf(fp, "promise_types::  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n", pp->agentsubtype,
                            KM_USES_POSS_F, bodyref, KM_USES_POSS_B);
                    fprintf(fp, "handles::  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n", promise_id,
                            KM_USES_CERT_F, bodyref, KM_USES_CERT_B);
                    fprintf(fp, "body_constraints::  \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n", cp->lval,
                            KM_USES_POSS_F, bodyref, "has a body constraint of type");
                }
            }

            if (bundlename)
            {
                Bundle *bp;

                if (pp->ref)
                {
                    fprintf(fp, "occurrences: \n\n");
                    fprintf(fp, " bundles:: \"%s\"  representation => \"literal\",\n", pp->ref);
                    fprintf(fp, " about_topics => { \"%s\"},",bundlename);
                    fprintf(fp, "   represents => { \"description\" }; \ntopics:\n");
                }

                // The used bundle affects the parent, in principle

                fprintf(fp, "bundles::\n\n");
                fprintf(fp, "  \"%s\"\n", bundlename);
                fprintf(fp, "      association => a(\"%s\",\"bundles::%s\",\"%s\");\n", NOVA_IMPACTS,
                        (const char *) pp->bundle, NOVA_ISIMPACTED);

                /* Bundlename is a conduit that is said to affect its formal
                   parameters, through the substituted values */

                const Policy *policy = PolicyFromPromise(pp);
                bp = GetBundle(policy, bundlename, "agent");

                if(bp == NULL)
                {
                    CfOut(cf_error, "", " Non-existent bundle \"%s\" referenced in \"%s\"\n", bundlename, (char *)cp->rval.item);
                    break;
                }

                for (rp = bp->args; rp != NULL; rp = rp->next)
                {
                    fprintf(fp, "bundles::\n\n");
                    fprintf(fp, "  \"%s\"\n", bundlename);
                    fprintf(fp, "      association => a(\"%s\",\"parameters::%s\",\"%s\");\n", NOVA_ISIMPACTED,
                            (const char *) rp->item, NOVA_IMPACTS);
                }

                break;
            }
        }
    }


    for (cp = pp->conlist; cp != NULL; cp = cp->next)
    {
       
       fprintf(fp, "handles::  \"%s\" association => a(\"%s\",\"body_constraints::%s\",\"%s\");\n", promise_id,
               KM_USES_CERT_F,cp->lval, KM_USES_CERT_B);
       
       fprintf(fp, "promisers::  \"%s\" association => a(\"%s\",\"body_constraints::%s\",\"%s\");\n", NovaEscape(pp->promiser),
               KM_AFFECTS_CERT_B,cp->lval, KM_AFFECTS_CERT_F);
    }
    
    
/* Promisees as topics too */

    switch (pp->promisee.rtype)
    {
    case CF_SCALAR:
        fprintf(fp, "promisees::\n\n");
        fprintf(fp, "  \"%s\"\n", (const char *) pp->promisee.item);
        fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_USES_PR, NovaEscape(pp->promiser),
                NOVA_GIVES_PR);
        fprintf(fp, "  \"%s\"\n", (const char *) pp->promisee.item);
        fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_USES_PR, promise_id, NOVA_GIVES_PR);
        fprintf(fp, "  \"%s\"\n", (const char *) pp->promisee.item);
        fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", KM_AFFECTS_CERT_B, promise_id,
                KM_AFFECTS_CERT_F);
        fprintf(fp, "  \"%s\"\n", (const char *) pp->promisee.item);
        fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", KM_AFFECTS_CERT_B, NovaEscape(pp->promiser),
                KM_AFFECTS_CERT_F);

        fprintf(fp, "  \"%s\"\n", (const char *) pp->promisee.item);
        fprintf(fp, "      association => a(\"is a promisee for\",\"%s\",\"has promisee\");\n", NovaEscape(pp->promiser));

        for (rp = GOALS; rp != NULL; rp = rp->next)
        {
            if (FullTextMatch(rp->item, pp->promisee.item))
            {
                fprintf(fp, "promises::\n\n");
                fprintf(fp, "  \"%s\"\n", promise_id);
                fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_IMPACTS,
                        (const char *) pp->promisee.item, NOVA_ISIMPACTED);

                if (bundlename)
                {
                    fprintf(fp, "bundles::\n\n");
                    fprintf(fp, "  \"%s\"\n", bundlename);
                    fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_IMPACTS,
                            (const char *) pp->promisee.item, NOVA_ISIMPACTED);
                    fprintf(fp, "  \"%s\"  association => a(\"%s\",\"goals::%s\",\"%s\");", bundlename, NOVA_GOAL,
                            (const char *) pp->promisee.item, NOVA_GOAL_INV);
                }
            }
        }
        break;

    case CF_LIST:

        fprintf(fp, "promisees::\n\n");
        for (rp = (Rlist *) pp->promisee.item; rp != NULL; rp = rp->next)
        {
            fprintf(fp, "  \"%s\"\n", (const char *) rp->item);
            fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_USES_PR, NovaEscape(pp->promiser),
                    NOVA_GIVES_PR);
            fprintf(fp, "  \"%s\"\n", (const char *) rp->item);
            fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_USES_PR, promise_id, NOVA_GIVES_PR);

            fprintf(fp, "  \"%s\"\n", (const char *) rp->item);
            fprintf(fp, "      association => a(\"is a promisee for\",\"%s\",\"has promisee\");\n", NovaEscape(pp->promiser));

            for (rp2 = GOALS; rp2 != NULL; rp2 = rp2->next)
            {
                if (FullTextMatch(rp2->item, rp->item))
                {
                    fprintf(fp, "promises::\n\n");
                    fprintf(fp, "  \"%s\"\n", promise_id);
                    fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_IMPACTS,
                            (const char *) rp2->item, NOVA_ISIMPACTED);

                    fprintf(fp, "  \"%s\"  association => a(\"%s\",\"goals::%s\",\"%s\");", promise_id, NOVA_GOAL,
                                (const char *) rp->item, NOVA_GOAL_INV);
                    
                    if (bundlename)
                    {
                        fprintf(fp, "bundles::\n\n");
                        fprintf(fp, "  \"%s\"\n", bundlename);
                        fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_IMPACTS,
                                (const char *) rp->item, NOVA_ISIMPACTED);
                        fprintf(fp, "  \"%s\"  association => a(\"%s\",\"goals::%s\",\"%s\");", bundlename, NOVA_GOAL,
                                (const char *) rp->item, NOVA_GOAL_INV);
                    }
                }
            }
        }

        break;
    default:
        break;

    }

/* This promise handle's place in the cosmos */

    fprintf(fp, "promises::\n\n");

    fprintf(fp, "\"%s\"\n", promise_id);

    if (pp->ref)
    {
        fprintf(fp, "   comment => \"%s\";\n", NovaEscape(pp->ref));
    }
    else
    {
        fprintf(fp, "   comment => \"(Uncommented \\\"%s\\\" promise by: %.25s..)\";\n", pp->agentsubtype,
                NovaEscape(pp->promiser));
    }

    fprintf(fp, "\"%s\" association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n", promise_id, NOVA_ACTIVATED,
            pp->classes, NOVA_ACTIVATES);
    fprintf(fp, "\"%s\" association => a(\"is a promise of type\",\"%s\",\"has current exemplars\");\n", promise_id,
            pp->agentsubtype);

    for (rp = depends_on; rp != NULL; rp = rp->next)
    {
        fprintf(fp, "  \"%s\"\n", promise_id);
        fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_USES_PR, (const char *) rp->item,
                NOVA_GIVES_PR);
        fprintf(fp, "  \"%s\"\n", promise_id);
        fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", KM_AFFECTS_CERT_B, (const char *) rp->item,
                KM_AFFECTS_CERT_F);
        fprintf(fp, "  \"%s\"\n", NovaEscape(pp->promiser));
        fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", KM_AFFECTS_CERT_B, (const char *) rp->item,
                KM_AFFECTS_CERT_F);
    }

//DeleteRlist(depends_on);

    for (rp = class_list; rp != NULL; rp = rp->next)
    {
        fprintf(fp, "  \"%s\"\n", promise_id);
        fprintf(fp, "      association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n", NOVA_USES_PR,
                NovaEscape(rp->item), NOVA_GIVES_PR);
    }

    fprintf(fp," class_contexts::\n");

    // Register which classes affect class expressions
    
    for (rp = class_list; rp != NULL; rp = rp->next)
    {
        fprintf(fp, "  \"%s\"\n", pp->classes);
        fprintf(fp, "      association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n", KM_AFFECTS_CERT_B,
                (char *)rp->item, KM_AFFECTS_CERT_F);
    }
    
    DeleteRlist(class_list);

/* Now pointers to the policy compilation */

    fprintf(fp, "\n occurrences:\n");

    fprintf(fp, "handles::\n");
    fprintf(fp, "\"/promise/details/%s\",\n", promise_id);
    fprintf(fp, "   about_topics => { \"handles::%s\" },\n", promise_id);
    fprintf(fp, "   represents => { \"definition\" };\n\n");

    fprintf(fp, "bundles::\n");
    fprintf(fp, "\"/bundle/details/bundle/%s\"\n", pp->bundle);
    fprintf(fp, "   about_topics => { \"bundles::%s\" },\n", pp->bundle);
    fprintf(fp, "   represents => { \"parent bundle\" };\n\n");

/*  Now we should analyze the classes to look for dependents and dependencies */

    Nova_MapClassParameterAssociations(fp, pp, promise_id);

// Look for any networks mentioned

#ifdef HAVE_CONSTELLATION
    Constellation_ScanAccessRelationships(fp, pp, promise_id);
#endif
}

/*****************************************************************************/

char *NovaEscape(const char *s)
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
    char static id[CF_MAXVARSIZE];
    char vbuff[CF_MAXVARSIZE];
    const char *handle = GetConstraintValue("handle", pp, CF_SCALAR);

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

    if ((handle = (char *) GetConstraintValue("handle", pp, CF_SCALAR)))
    {
        snprintf(assertion, CF_BUFSIZE - 1, "topics: \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n", name,
                 NOVA_BUNDLE_DATA_INV_P, handle, NOVA_BUNDLE_DATA);

        PrependItemList(&NOVA_BUNDLEDEPENDENCE, assertion);
    }
}

/*****************************************************************************/

void Nova_ShowBundleDependence(FILE *fp)
{
    Item *ip;

    for (ip = NOVA_BUNDLEDEPENDENCE; ip != NULL; ip = ip->next)
    {
        fprintf(fp, "%s", ip->name);
    }
}

/*****************************************************************************/

void ShowTopicRepresentation(FILE *fp)
{
    int i, j, k, l;
    const SubTypeSyntax *ss;
    const BodySyntax *bs, *bs2;
    static char *level[] = { "high", "low", "normal", NULL };
    static char *dev[] = { "dev1", "dev2", "microanomaly", "anomaly", NULL };

    if (LICENSES == 0)
    {
        return;
    }

    fprintf(fp,
            "\n# This information is jointly Copyrighted by (C) Cfengine and the Licensee may not be redistributed without the permission of both parties\ntopics:\n");

    fprintf(fp,
            "any:: \"system_reports\" comment => \"Reports collected from Cfengine managed systems by a reporting hub\";\n");
    fprintf(fp,
            "\"remote_scalars\" comment => \"Scalar variable values that are made accessible to remote agents through cf-serverd\";\n");

    fprintf(fp, "system_reports::\n");

    for (i = 0; i < cfrep_unknown; i++)
    {
        fprintf(fp, "  \"%s\" comment => \"%s\",\n", BASIC_REPORTS[i].name_old, BASIC_REPORTS[i].description);
        fprintf(fp, "         association => a(\"is now called\",\"%s\",\"was previously called\");\n",
                BASIC_REPORTS[i].name);

        fprintf(fp, "  \"%s\" comment => \"%s\",\n", BASIC_REPORTS[i].name, BASIC_REPORTS[i].description);
        fprintf(fp, "         association => a(\"deals with\",\"%s\",\"reports\");\n", BASIC_REPORTS[i].category);
    }

    fprintf(fp, "  # New assocs\n");

    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_classes].name);
    fprintf(fp, "    association => a(\"%s\",\"promise_types::classes\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_lastseen].name);
    fprintf(fp, "   association => a(\"is affected by\",\"body_constraints::copy_from\",\"%s\");\n", NOVA_REPORTED);
    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_lastseen].name);
    fprintf(fp, "   association => a(\"is affected by\",\"promise_types::access\",\"%s\");\n", NOVA_REPORTED);

    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_promise_compliance].name);
    fprintf(fp, "    association => a(\"is based on\",\"promises\",\"%s\");\n", NOVA_REPORTED);
    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_promise_compliance].name);
    fprintf(fp, "    association => a(\"%s\",\"promise report\",\"%s\");\n", NOVA_SEEALSO, NOVA_SEEALSO);

    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_performance].name);
    fprintf(fp, "    association => a(\"is based on\",\"promises\",\"%s\");\n", NOVA_REPORTED);
    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_setuid].name);
    fprintf(fp, "    association => a(\"%s\",\"promise_types::files\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    fprintf(fp, "  \"hashes report\"\n");
    fprintf(fp, "    association => a(\"%s\",\"promise_types::files\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    fprintf(fp, "  \"hashes report\"\n");
    fprintf(fp, "    association => a(\"%s\",\"body_constraints::changes\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_change].name);
    fprintf(fp, "    association => a(\"%s\",\"promise_types::files\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_change].name);
    fprintf(fp, "    association => a(\"%s\",\"body_constraints::changes\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_diff].name);
    fprintf(fp, "    association => a(\"%s\",\"promise_types::files\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_diff].name);
    fprintf(fp, "    association => a(\"%s\",\"body_constraints::changes\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_software_installed].name);
    fprintf(fp, "    association => a(\"%s\",\"promise_types::packages\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_patch_status].name);
    fprintf(fp, "    association => a(\"%s\",\"promise_types::packages\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);
    fprintf(fp, "  \"%s\"\n", BASIC_REPORTS[cfrep_patch_avail].name);
    fprintf(fp, "    association => a(\"%s\",\"promise_types::packages\",\"%s\");\n", NOVA_GEN, NOVA_REPORTED);

    fprintf(fp, "system_policy::\n");
    fprintf(fp, "  \"bundles\" comment => \"A modular collection of promises of different types\";\n");
    fprintf(fp, "  \"bodies\" comment => \"A modular collection of body constraints for re-use\";\n");
    fprintf(fp, "  \"class contexts\" comment => \"Class expressions that say where or when a promise applies\";\n");
    fprintf(fp, "  \"promisees\" comment => \"Recipients of a promise, i.e. promise handles, or persons\";\n");
    fprintf(fp, "  \"promisers\" comment => \"The objects affected by a promise\";\n");
    fprintf(fp, "  \"promises\" comment => \"Complete occurrences of promiser + promisee + promise-body\";\n");
    fprintf(fp, "  \"promise types\" comment => \"The types of promise that cfengine can keep\";\n");
    fprintf(fp,
            "  \"body constraints\" comment => \"The attributes that cfengine enables you to promise about the promiser\";\n");
    fprintf(fp, "\"comments\"\n");
    fprintf(fp, "      association => a(\"see instances of\",\"comment\",\"is one of a number of\");\n");
    fprintf(fp, "\"functions\" comment => \"In-built functions that may be used to set variables or classes\";");

    fprintf(fp, " \"values\"  comment => \"Formal rvalues in constraint assignments and their legal ranges\";\n");

    fprintf(fp, "values::\n\n");

    for (i = 0; CF_VALUETYPES[i][0] != NULL; i++)
    {
        fprintf(fp, "\"%s\"   comment =>\"Should match the generic pattern %s, i.e. %s\";\n",
                CF_VALUETYPES[i][1], NovaEscape(CF_VALUETYPES[i][0]), CF_VALUETYPES[i][2]);
    }

    fprintf(fp, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[1][1], CF_DATATYPES[cf_int]);
    fprintf(fp, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[3][1], CF_DATATYPES[cf_int]);
    fprintf(fp, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[4][1], CF_DATATYPES[cf_int]);
    fprintf(fp, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[5][1], CF_DATATYPES[cf_int]);
    fprintf(fp, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[6][1], CF_DATATYPES[cf_real]);
    fprintf(fp, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[9][1], CF_DATATYPES[cf_class]);
    fprintf(fp, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[10][1], CF_DATATYPES[cf_str]);
    fprintf(fp, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[11][1], CF_DATATYPES[cf_str]);
    fprintf(fp, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[12][1], CF_DATATYPES[cf_str]);
    fprintf(fp, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[13][1], CF_DATATYPES[cf_str]);
    fprintf(fp, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[14][1], CF_DATATYPES[cf_str]);
    fprintf(fp, "\"%s\"   association => a(\"is an instance of\",\"%s\",\"is the generic type for\");\n",
            CF_VALUETYPES[15][1], CF_DATATYPES[cf_str]);

    fprintf(fp, "bundles::\n");
    fprintf(fp, "\"sys\" comment => \"cfengine's internal bundle of system specific values\";\n");
    Nova_ShowBundleDependence(fp);

    for (i = 0; i < CF3_MODULES; i++)
    {
        if ((ss = CF_ALL_SUBTYPES[i]) == NULL)
        {
            continue;
        }

        for (j = 0; ss[j].bundle_type != NULL; j++)
        {
            fprintf(fp,
                    "occurrences:  \"/docs/cf3-Reference.html#%s-in-agent-promises\" represents => { \"manual reference %s\" }, about_topics => { \"%s\"}; \n",
                    ss[j].subtype, ss[j].subtype,ss[j].subtype);

            if (ss[j].bs != NULL)       /* In a bundle */
            {
                bs = ss[j].bs;

                for (l = 0; bs[l].lval != NULL; l++)
                {
                    fprintf(fp, "topics:\n promise_types::\n");
                    fprintf(fp, "   \"%s\";\n", ss[j].subtype);

                    fprintf(fp, "body_constraints::\n");
                    fprintf(fp, "   \"%s\"\n", bs[l].lval);
                    fprintf(fp, "   comment => \"%s\",\n", NovaEscape(bs[l].description));
                    fprintf(fp, "   association => a(\"%s\",\"promise_types::%s\",\"%s\");\n", KM_PARTOF_POSS_F,
                            ss[j].subtype, KM_PARTOF_POSS_B);

                    if (bs[l].dtype == cf_body)
                    {
                        bs2 = (BodySyntax *) (bs[l].range);

                        if (bs2 == NULL || bs2 == (void *) CF_BUNDLE)
                        {
                            continue;
                        }

                        // Certain lvals can be parts of body templates

                        for (k = 0; bs2[k].lval != NULL; k++)
                        {
                            fprintf(fp, "   \"%s\"\n", bs2[k].lval);
                            fprintf(fp, "   comment => \"%s\",\n", NovaEscape(bs2[k].description));
                            fprintf(fp, "   association => a(\"%s\",\"%s\",\"%s\");\n", KM_PARTOF_POSS_F, bs[l].lval,
                                    KM_PARTOF_POSS_B);

                            NovaShowValues(fp, bs2[k]);
                        }
                    }
                    else
                    {
                        NovaShowValues(fp, bs[l]);
                    }
                }
            }
        }
    }

    for (i = 0; CF_COMMON_BODIES[i].lval != NULL; i++)
    {
        fprintf(fp, "   \"%s\";\n", CF_COMMON_BODIES[i].lval);
    }

    for (i = 0; CF_COMMON_EDITBODIES[i].lval != NULL; i++)
    {
        fprintf(fp, "   \"%s\";\n", CF_COMMON_EDITBODIES[i].lval);
    }

    fprintf(fp, "miscellaneous_concepts::\n");

    fprintf(fp, "  \"data types\";\n");

    fprintf(fp, "data_types::\n");

    for (i = 0; CF_VARBODY[i].lval != NULL; i++)
    {
        fprintf(fp, "  \"%s\" comment => \"%s matching %s\";\n", CF_VARBODY[i].lval, (char *) CF_VARBODY[i].description,
                (const char *) CF_VARBODY[i].range);
    }

    fprintf(fp, "  \"class\" comment => \"A boolean returned by certain functions in classes promises\";\n");

    fprintf(fp, "functions::\n\n");

    for (i = 0; CF_FNCALL_TYPES[i].name != NULL; i++)
    {
        fprintf(fp, " \"%s\" ", CF_FNCALL_TYPES[i].name);
        fprintf(fp, "    comment => \"%s\",\n", CF_FNCALL_TYPES[i].description);
        fprintf(fp, "    association => a(\"returns data-type\",\"%s\",\"is returned by function\");\n",
                CF_DATATYPES[CF_FNCALL_TYPES[i].dtype]);
    }

// Things for monitoring system state

    fprintf(fp, "things:\n");

    fprintf(fp,
            "  \"monitoring classes\" comment => \"Classes set by cf-monitord based on the observed system state.\"; \n");

    for (i = 0; i < CF_OBSERVABLES; i++)
    {
        if (strcmp(OBS[i][0], "spare") == 0)
        {
            continue;
        }

        fprintf(fp, " \"%s\" comment => \"%s\",", OBS[i][0], OBS[i][1]);
        fprintf(fp, "      generalizations => { \"vital signs\", \"observables\" },");
        fprintf(fp, "      determines => { \"actual state\" , \"monitoring classes\" };\n");
    }

// Operating system classes

    fprintf(fp, "operating_systems::");

    for (i = 2; CLASSTEXT[i] != NULL; i++)
    {
        fprintf(fp, " \"%s\" generalizations => { \"operating systems\" };  ", CLASSTEXT[i]);
    }

    fprintf(fp, " \"debian\" generalizations => { \"linux\"};");
    fprintf(fp, " \"ubuntu\" generalizations => { \"linux\", \"debian\" };");
    fprintf(fp, " \"redhat\" generalizations => { \"linux\"};");
    fprintf(fp, " \"sles\" generalizations => { \"suse\", \"linux\"};");
    fprintf(fp, " \"suse\" generalizations => { \"linux\"};");
    fprintf(fp, " \"slackware\" generalizations => { \"linux\"};");
    fprintf(fp, " \"fedora\" generalizations => { \"linux\"};");
    fprintf(fp, " \"mandrake\" generalizations => { \"linux\"};");
    fprintf(fp, " \"mandriva\" generalizations => { \"linux\"};");
    fprintf(fp, " \"gentoo\" generalizations => { \"linux\"};");
    fprintf(fp, " \"unitedlinux\" generalizations => { \"linux\"};");
    fprintf(fp, " \"suncobalt\" generalizations => { \"linux\"};");
    fprintf(fp, " \"Win2000\" generalizations => { \"windows\"};");
    fprintf(fp, " \"WinXP\" generalizations => { \"windows\"};");
    fprintf(fp, " \"WinServer2003\" generalizations => { \"windows\"};");
    fprintf(fp, " \"WinVista\" generalizations => { \"windows\"};");
    fprintf(fp, " \"WinServer2008\" generalizations => { \"windows\"};");


// Monitoring

    fprintf(fp, "ports::");

    for (i = 0; i < ATTR; i++)
    {
        fprintf(fp,
                "  \"port %s\" comment => \"The standard reserved port for %s\", generalizations => { \"ports\" }, synonyms => {\"ports::%s\"}; ",
                ECGSOCKS[i].portnr, ECGSOCKS[i].name, ECGSOCKS[i].name);
    }

    fprintf(fp,
            "measurements:: \"anomalies\" comment => \"Measurements that exceed the boundaries of normal behaviour, as learned by cf-monitord\";");

    fprintf(fp, "anomalies::");

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
                fprintf(fp,
                        " \"%s_%s_%s\" comment => \"%s is %s relative to the learned normal average\", generalizations => { \"vital signs\", \"performance\", \"anomalies\" };\n",
                        OBS[i][0], level[j], dev[k], OBS[i][1], level[j]);

                if (strstr(OBS[i][0], "_in"))
                {
                    if (strstr(OBS[i][0], "_low"))
                    {
                        fprintf(fp, " \"%s_%s_%s\" caused_by => { \"Reduced incoming traffic\" };", OBS[i][0], level[j],
                                dev[k]);
                    }
                    else if (strstr(OBS[i][0], "_high"))
                    {
                        fprintf(fp, " \"%s_%s_%s\" caused_by => { \"Increased incoming traffic\" };", OBS[i][0],
                                level[j], dev[k]);
                    }
                }
                else if (strstr(OBS[i][0], "_out"))
                {
                    if (strstr(OBS[i][0], "_low"))
                    {
                        fprintf(fp, " \"%s_%s_%s\" caused_by => { \"Reduced outgoing traffic\" };", OBS[i][0], level[j],
                                dev[k]);
                    }
                    else if (strstr(OBS[i][0], "_high"))
                    {
                        fprintf(fp, " \"%s_%s_%s\" caused_by => { \"Increased outgoing traffic\" };", OBS[i][0],
                                level[j], dev[k]);
                    }
                }
            }
        }
    }
}

/*****************************************************************************/

void NovaShowValues(FILE *fp, BodySyntax bs)
{
    int i;
    const char *range = NULL;

    for (i = 0; CF_VALUETYPES[i][0] != NULL; i++)
    {
        if (bs.dtype == cf_bundle || bs.dtype == cf_body)
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

    fprintf(fp, "body_constraints::\n");
    fprintf(fp,
            "   \"%s\" association => a(\"is a body constraint of type\",\"values::%s\",\"can be used in body constraints\"),\n",
            bs.lval, range);
    fprintf(fp, "          comment => \"%s\";\n", NovaEscape(bs.description));

    switch (bs.dtype)
    {
    case cf_slist:
    case cf_ilist:
    case cf_rlist:

        fprintf(fp, "   \"%s\" association => a(\"is a list of type\",\"%s\",\"is used in\");\n", bs.lval, range);

    default:
        break;
    }

    if (CF_VALUETYPES[i][0] != NULL)
    {
        fprintf(fp, "values::\n\n");
        fprintf(fp, " \"%s\" comment => \"Represent type %s and should match %s\",\n", range, CF_DATATYPES[bs.dtype],
                NovaEscape(CF_VALUETYPES[i][0]));
        fprintf(fp,
                "    association => a(\"are used in constraint parameters\",\"body_constraints::%s\",\"takes value\");\n",
                bs.lval);
    }
}

/*****************************************************************************/

void Nova_MapClassParameterAssociations(FILE *fp, Promise *pp, char *promise_id)
{
    Rlist *impacted = NULL, *dependency = NULL, *potential, *rp;
    Bundle *bp;
    SubType *sp;
    Promise *pp2;
    char *value;
    int found = false;

    if (promise_id && pp->ref)
    {
        fprintf(fp, "topics: handles:: \"%s\"  comment => \"%s\", ", promise_id, NovaEscape(pp->ref));
        fprintf(fp, "association => a(\"is the promise_id for\",\"%s\",\"has a promise with promise_id\");\n",
                NovaEscape(pp->promiser));
    }
    else if (promise_id)
    {
        fprintf(fp, "topics: handles:: \"%s\" association => a(\"%s\",\"promisers::%s\",\"%s\");\n", promise_id,
                NOVA_HANDLE, NovaEscape(pp->promiser), NOVA_HANDLE_INV);
    }

    if (promise_id)
    {
        fprintf(fp, "occurrences:  \"/promise/details/%s\", represents => { \"definition\" }, about_topics => {\"handles::%s\" }; \n",
                promise_id, promise_id);
    }

/* For activated classes we can assume that no one will */

    potential = GetListConstraint("promise_kept", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        IdempPrependRScalar(&impacted, rp->item, CF_SCALAR);
    }

    potential = GetListConstraint("promise_repaired", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        IdempPrependRScalar(&impacted, rp->item, CF_SCALAR);
    }

    potential = GetListConstraint("repair_failed", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        IdempPrependRScalar(&impacted, rp->item, CF_SCALAR);
    }

    potential = GetListConstraint("repair_denied", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        IdempPrependRScalar(&impacted, rp->item, CF_SCALAR);
    }

    potential = GetListConstraint("promise_timeout", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        IdempPrependRScalar(&impacted, rp->item, CF_SCALAR);
    }

    potential = GetListConstraint("or", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        IdempPrependRScalar(&dependency, rp->item, CF_SCALAR);
    }

    potential = GetListConstraint("and", pp);

    for (rp = potential; rp != NULL; rp = rp->next)
    {
        IdempPrependRScalar(&dependency, rp->item, CF_SCALAR);
    }

    if ((value = GetConstraintValue("expression", pp, CF_SCALAR)))
    {
        IdempPrependRScalar(&dependency, value, CF_SCALAR);
    }

// Now look for impact

    for (rp = dependency; rp != NULL; rp = rp->next)
    {
        fprintf(fp, "topics:\n");
        fprintf(fp, "class_contexts::");
        fprintf(fp, "  \"%s\"\n", NovaEscape(pp->promiser));
        fprintf(fp, "      association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n", NOVA_ISIMPACTED,
                NovaEscape(rp->item), NOVA_IMPACTS);

        // Might need to break these up further
    }

    DeleteRlist(dependency);

    if (impacted == NULL)
    {
        return;
    }

    CfOut(cf_verbose, "", " ?? Analysing promise dependencies for \"%s\"\n", promise_id);

/* So now we have the list of classes to scan for and we must undertake this
 expensive search */

    const Policy *policy = PolicyFromPromise(pp);
    for (bp = policy->bundles; bp != NULL; bp = bp->next)
    {
        for (sp = bp->subtypes; sp != NULL; sp = sp->next)
        {
            for (pp2 = sp->promiselist; pp2 != NULL; pp2 = pp2->next)
            {
                if (pp2 == pp)
                {
                    continue;
                }

                for (rp = impacted; rp != NULL; rp = rp->next)
                {
                    char *varclass = GetConstraintValue("ifvarclass", pp, CF_SCALAR);

                    if (strstr(pp2->classes, rp->item) || (varclass && strstr(varclass, rp->item)))
                    {
                        //Debug("Found %s in %s+%s\n",rp->item,pp2->classes,varclassPrint); //FIXME: Segfault on Solaris (Nullpointers?)
                        // found a connection
                        fprintf(fp, "topics:\n");
                        fprintf(fp, "promises::");
                        fprintf(fp, "  \"%s\"\n", promise_id);
                        fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_IMPACTS, (char *) rp->item,
                                NOVA_ISIMPACTED);
                        fprintf(fp, "promisers::");
                        fprintf(fp, "  \"%s\"\n", NovaEscape(pp->promiser));
                        fprintf(fp, "      association => a(\"%s\",\"%s\",\"%s\");\n", NOVA_IMPACTS, (char *) rp->item,
                                NOVA_ISIMPACTED);
                        found = true;
                    }

                    /* Omit class "any" */

                    if (strstr(pp2->classes, rp->item) && strcmp(rp->item, "any") != 0
                        && strcmp(pp->classes, "any") != 0)
                    {
                        fprintf(fp, "class_contexts::");
                        fprintf(fp, "  \"%s\"\n", pp->classes);
                        fprintf(fp, "      association => a(\"%s\",\"class_contexts::%s\",\"%s\");\n", NOVA_ACTIVATES,
                                NovaEscape(rp->item), NOVA_ACTIVATES);
                    }
                }
            }
        }
    }

    if (!found && THIS_AGENT_TYPE == cf_common)
    {
        CfOut(cf_verbose, "",
              "Classes activated by the outcomes of promise ref \"%s\" did not occur in any promise proposal, so they are nullpotent",
              promise_id);
        PromiseRef(cf_inform, pp);
    }

    DeleteRlist(impacted);
}
