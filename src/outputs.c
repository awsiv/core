
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: outputs.c                                                           */
/*                                                                           */
/* Created: Thu May 13 08:33:13 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "constraints.h"

Item *NOVA_HANDLE_OUTPUTS = NULL;
Item *NOVA_BUNDLE_OUTPUTS = NULL;

int SAVE_INFORM = 0;
int SAVE_VERBOSE = 0;

/*****************************************************************************/

void VerifyOutputsPromise(Promise *pp)
{
    Attributes a = GetOutputsAttributes(pp);
    Item *ip;
    static char *defaultval = "verbose";

    if (a.output.level == NULL)
    {
        a.output.level = defaultval;
    }

    if (a.output.promiser_type && (strcmp(a.output.promiser_type, "bundle") == 0))
    {
        if ((ip = ReturnItemIn(NOVA_BUNDLE_OUTPUTS, pp->promiser)))
        {
            if (strcmp(ip->classes, a.output.level) != 0)
            {
                cfPS(cf_error, CF_FAIL, "", pp, a,
                     " !! Promise for bundle \"%s\" conflicts with an existing outputs promise", pp->promiser);
            }
            else
            {
                CfOut(cf_verbose, "", " -> Idempotent duplicate for bundle \"%s\"", pp->promiser);
            }
        }
        else
        {
            cfPS(cf_inform, CF_CHG, "", pp, a, " -> Setting output level for bundle \"%s\"", pp->promiser);
            PrependItem(&NOVA_BUNDLE_OUTPUTS, pp->promiser, a.output.level);
        }
    }
    else                        // promises
    {
        if ((ip = ReturnItemIn(NOVA_HANDLE_OUTPUTS, pp->promiser)))
        {
            if (strcmp(ip->classes, a.output.level) != 0)
            {
                cfPS(cf_error, CF_FAIL, "", pp, a,
                     " !! Promise for handle \"%s\" conflicts with an existing outputs promise", pp->promiser);
            }
            else
            {
                CfOut(cf_verbose, "", " -> Idempotent duplicate for promise with handle \"%s\"",
                      a.output.promiser_type);
            }
        }
        else
        {
            cfPS(cf_inform, CF_CHG, "", pp, a, " -> Setting output level for promise handle \"%s\"", pp->promiser);
            PrependItem(&NOVA_HANDLE_OUTPUTS, pp->promiser, a.output.level);
        }
    }
}

/*****************************************************************************/

void SetPromiseOutputs(Promise *pp)
{
    char *handle = GetConstraintValue("handle", pp, CF_SCALAR);
    char *setting = GetConstraintValue("report_level", pp, CF_SCALAR);
    enum cfreport report_level = String2ReportLevel(setting);
    int verbose = false, inform = false;
    Item *ip;

    if (handle)
    {
        ip = ReturnItemIn(NOVA_HANDLE_OUTPUTS, handle);

        if (ip == NULL)
        {
            return;
        }

        if (ip && strcmp(ip->classes, "verbose") == 0)
        {
            verbose = true;
        }

        if (ip && strcmp(ip->classes, "inform") == 0)
        {
            verbose = inform;
        }

        if (setting)
        {
            switch (report_level)
            {
            case cf_verbose:
                break;

            case cf_inform:
                if (verbose)
                {
                    EditScalarConstraint(pp->conlist, "report_level", "verbose");
                }

                break;

            default:
                if (inform || verbose)
                {
                    EditScalarConstraint(pp->conlist, "report_level", ip->classes);
                }
                break;
            }
        }
        else
        {
            if (ip && ip->classes)
            {
                ConstraintAppendToPromise(pp, "report_level", (Rval) {xstrdup(ip->classes), CF_SCALAR}, "any", false);
            }
            else
            {
                ConstraintAppendToPromise(pp, "report_level", (Rval) {xstrdup("verbose"), CF_SCALAR}, "any", false);
            }
        }
    }
}

/*****************************************************************************/

void SetBundleOutputs(char *name)
{
    Item *ip;

    for (ip = NOVA_BUNDLE_OUTPUTS; ip != NULL; ip = ip->next)
    {
        if (strcmp(ip->name, name) == 0)
        {
            SAVE_VERBOSE = VERBOSE;
            SAVE_INFORM = INFORM;

            if (strcmp(ip->classes, "inform") == 0)
            {
                INFORM = true;
            }
            else
            {
                VERBOSE = true;
            }

            CfOut(cf_inform, "", "%s Begin trace output on bundle %s\n", VPREFIX, name);
        }
    }
}

/*****************************************************************************/

void ResetBundleOutputs(char *name)
{
    Item *ip;

    for (ip = NOVA_BUNDLE_OUTPUTS; ip != NULL; ip = ip->next)
    {
        if (strcmp(ip->name, name) == 0)
        {
            VERBOSE = SAVE_VERBOSE;
            INFORM = SAVE_INFORM;

            if (VERBOSE > SAVE_VERBOSE)
            {
                printf("%s End trace output on bundle %s\n", VPREFIX, name);
            }
        }
    }
}
