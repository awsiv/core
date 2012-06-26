
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "db_query.h"

/*****************************************************************************/
/*                                                                           */
/* File: magnify.c                                                           */
/*                                                                           */
/*****************************************************************************/

/**********************************************************************/
/* Magdata                                                            */
/**********************************************************************/

bool Nova_ReadMagTimeSeries2(EnterpriseDB *conn, DataView *cfv, char *hostkey, char *vitalId)
{
 double ry, rq, rs, rg;
    double q[CF_MAGDATA], e[CF_MAGDATA], d[CF_MAGDATA], g[CF_MAGDATA];
    int i;
    bool hasData = false;

    CFDB_QueryMagView2(conn, hostkey, vitalId, time(NULL) - 4 * 3600, q, e, d, g);

    cfv->max = 0;
    cfv->min = 99999;
    cfv->error_scale = 0;

    for (i = 0; i < CF_MAGDATA; i++)
    {
        if (q[i] >= 0)
        {
            hasData |= true;
        }

        // Num() resets negative numbers to zero
        ry = cfv->data_E[i] = Num(e[i]);
        rq = cfv->data_q[i] = Num(q[i]);
        rs = cfv->bars[i] = Num(d[i]);
        rg = cfv->bars[i] = Num(g[i]);

        if (rq > cfv->max)
        {
            cfv->max = rq;
        }

        if (rq < cfv->min)
        {
            cfv->min = rq;
        }

        cfv->error_scale = (cfv->error_scale + rs) / 2;
    }

   return hasData;
}
