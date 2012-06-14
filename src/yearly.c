
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: yearly.c                                                           */
/*                                                                           */
/*****************************************************************************/

#include "db_query.h"

int Nova_ReadYearTimeSeries(EnterpriseDB *conn, DataView *cfv, char *keyhash, char *monId)
{
    double ry, rq, rs;
    int i, have_data = false;
    double q[CF_YEAR_SLOTS] = { 0 }, e[CF_YEAR_SLOTS] = { 0 }, d[CF_YEAR_SLOTS] = { 0 };

    CFDB_QueryMonView(conn, keyhash, monId, mon_rep_yr, q, e, d);

    cfv->max = 0;
    cfv->min = 99999;
    cfv->error_scale = 0;

    for (i = 0; i < CF_YEAR_SLOTS; i++)
    {
        ry = e[i];
        rq = q[i];
        rs = d[i];

        if (rq > 0)
        {
            have_data = true;
        }

        if (rq > cfv->max)
        {
            cfv->max = rq;
        }

        cfv->error_scale = (cfv->error_scale + rs) / 2;

        if (rq < cfv->min)
        {
            cfv->min = rq;
        }
        
        cfv->data_E[i] = ry;
        cfv->data_q[i] = rq;
        cfv->bars[i] = rs;
    }

    return have_data;
}
