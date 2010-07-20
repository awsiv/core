
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

void Nova_CfQueryCFDB(char *query)
{
#ifdef HAVE_LIBMONGOC
 mongo_connection dbconn;
 char query_lval[CF_MAXVARSIZE];
 bson b;

if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   }

// CFDB_ReadAllSoftware(&dbconn, bson_empty(&b));
// CFDB_ListEverything(&dbconn);
// CFDB_ListAllHosts(&dbconn);

CFDB_ListAllHostsWithArrayElement(&dbconn,cfr_software,cfr_name,"samba-client");

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_error, "", "!! Could not close connection to report database");
   } 
#endif
}
