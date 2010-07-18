
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: request.c                                                           */
/*                                                                           */
/* Created: Sun Jul 18 16:45:02 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

void Nova_ReportDB()

{ mongo_connection dbconn;
  bson b;

if (!Nova_DBOpen(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   }

Nova_DBReadAllSoftware(&dbconn, bson_empty(&b));

if (!Nova_DBClose(&dbconn))
   {
   CfOut(cf_error, "", "!! Could not close connection to report database");
   } 
}
