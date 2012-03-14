/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: pscalar.c                                                           */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

void Nova_SetPersistentScalar(char *lval, char *rval)
{
    CF_DB *dbp;
    PersistentScalar new;

    strncpy(new.rval, rval, CF_MAXVARSIZE);
    new.time = time(NULL);

    if (!OpenDB(&dbp, dbid_scalars))
    {
        return;
    }

    CfOut(cf_verbose, "", " -> Setting persistent hub knowledge: %s =>\"%s\"", lval, rval);
    WriteDB(dbp, lval, &new, sizeof(PersistentScalar));
    CloseDB(dbp);
}

/*****************************************************************************/

int Nova_GetPersistentScalar(char *lval, char *rval, int size, time_t timeout)
{
    CF_DB *dbp;
    PersistentScalar var;
    time_t now = time(NULL);
    *rval = '\0';

    if (!OpenDB(&dbp, dbid_scalars))
    {
        CfOut(cf_verbose, "", " -> Unable to open db while looking for persistent scalar");
        return false;
    }

    if (ReadDB(dbp, lval, &var, sizeof(PersistentScalar)))
    {
        if (now > var.time + timeout)
        {
            DeleteDB(dbp, lval);
            CfOut(cf_verbose, "", " -> Persistent scalar timed out (%jd too late), so looking for default",
                  (intmax_t) now - var.time);
            CloseDB(dbp);
            return false;
        }
        else
        {
            CloseDB(dbp);
            strncpy(rval, var.rval, size - 1);
            return true;
        }
    }
    else
    {
        CloseDB(dbp);
        CfOut(cf_verbose, "", " -> Persistent scalar was not found, so looking for default");
        return false;
    }
}
