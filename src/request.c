
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

void Nova_CfQueryCFDB(char *querystr)
{
#ifdef HAVE_LIBMONGOC
 mongo_connection dbconn;
 bson query,b;
 bson_buffer bb;
 struct Rlist *rp,*list;
 struct HubSoftware *hs;
 
if (!CFDB_Open(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   }

// bson_buffer_init(&bb);
// bson_append_string(&bb,cfr_keyhash,"MD5=b6755ae1cd085528535daed2f27a531c");
// bson_from_buffer(&query,&bb);
// CFDB_ReadAllSoftware(&dbconn,&query);

list = CFDB_QuerySoftware(&dbconn,"zypper",NULL,NULL,false);

for (rp = list; rp != NULL; rp=rp->next)
   {
   hs = (struct HubSoftware *)rp->item;
   printf("N: %s\n",hs->name);
   printf("V: %s\n",hs->version);
   printf("A: %s\n",Nova_LongArch(hs->arch));
   }

DeleteRlist(list);

//CFDB_ListEverything(&dbconn);

if (!CFDB_Close(&dbconn))
   {
   CfOut(cf_error, "", "!! Could not close connection to report database");
   } 

#endif
}

