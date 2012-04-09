
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

bool Nova_ReadMagTimeSeries2(mongo_connection *conn, DataView *cfv, char *hostkey, char *vitalId)
{
    double ry, rq, rs;
    double q[CF_MAGDATA], e[CF_MAGDATA], d[CF_MAGDATA];
    int i, have_data = false;

    CFDB_QueryMagView2(conn, hostkey, vitalId, time(NULL) - 4 * 3600, q, e, d);

    cfv->max = 0;
    cfv->min = 99999;
    cfv->error_scale = 0;

    for (i = 0; i < CF_MAGDATA; i++)
    {
        cfv->data_E[i] = 0;
        cfv->data_q[i] = 0;
        cfv->bars[i] = 0;
    }

    for (i = 0; i < CF_MAGDATA; i++)
    {
        ry = Num(e[i]);
        rq = Num(q[i]);
        rs = Num(d[i]);

        // Num() resets to zero negative numbers
        if (q[i] >= 0)
        {
            have_data++;
        }

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

    if (have_data > 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}
