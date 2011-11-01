/*****************************************************************************/
/*                                                                           */
/* File: client_code.c<2>                                                    */
/*                                                                           */
/* Created: Wed Jun 16 18:22:22 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef HAVE_LIBMONGOC 
static void Nova_RecordNetwork(mongo_connection *dbconnp, time_t now, double datarate,struct cfagent_connection *conn);
#endif 

char *CF_CODEBOOK[CF_CODEBOOK_SIZE] =
   {
   CFR_PERF,
   CFR_CLASS,
   CFR_SETUID,
   CFR_FCHANGE,
   CFR_FDIFF,
   CFR_MONITOR_MAG,
   CFR_MONITOR_WEEK,
   CFR_MONITOR_YEAR,
   CFR_MONITOR_HIST,
   CFR_MONITOR_MG,
   CFR_MONITOR_WK,
   CFR_MONITOR_YR,
   CFR_MONITOR_HG,
   CFR_PCOMPLIANCE,
   CFR_TCOMPLIANCE,
   CFR_SOFTWARE,
   CFR_AVAILPATCH,
   CFR_PATCHSTATUS,
   CFR_PROMISEOUT,
   CFR_VALUE,
   CFR_VARD,
   CFR_VARS,
   CFR_LASTSEEN,
   CFR_REPAIRLOG,
   CFR_NOTKEPTLOG,
   CFR_METER,
   CFR_BUNDLES,
   CFR_SWDATES,
   NULL
   };

void *CF_CODEBOOK_HANDLER[CF_CODEBOOK_SIZE] =
   {
   Nova_UnPackPerformance,  // DBOK
   Nova_UnPackClasses,      // DBOK (nopurge)
   Nova_UnPackSetuid,       // DBOK (nopurge)
   Nova_UnPackFileChanges,  // DBOK
   Nova_UnPackDiffs,        // DBOK
   Nova_UnPackMonitorMag,   // DBOK   - DEPRECATED
   Nova_UnPackMonitorWeek,  // DBOK   - DEPRECATED
   Nova_UnPackMonitorYear,  // nodata - DEPRECATED
   Nova_UnPackMonitorHist,  // DEPRECATED
   Nova_UnPackMonitorMg,
   Nova_UnPackMonitorWk,
   Nova_UnPackMonitorYr,
   Nova_UnPackMonitorHg,  // DBOK
   Nova_UnPackCompliance,   // DBOK
   Nova_UnPackTotalCompliance, // DBOK
   Nova_UnPackSoftware,     // DBOK
   Nova_UnPackAvailPatches, // nodata
   Nova_UnPackPatchStatus,  // nodata
   Nova_UnPack_promise_output_common,  // nodata
   Nova_UnPackValueReport,  //  (append) LOG
   Nova_UnPackVariables2,    // DBOK
   Nova_UnPackVariables,    // DBOK  - DEPRECATED
   Nova_UnPackLastSeen,     // DBOK
   Nova_UnPackRepairLog,    // DBOK (nopurge)
   Nova_UnPackNotKeptLog,   // DBOK (nopurge)
   Nova_UnPackMeter,        // DBOK
   Nova_UnPackBundles,
   Nova_UnPackSoftwareDates
   };

/*********************************************************************/

#ifdef HAVE_LIBMONGOC

int Nova_QueryForKnowledgeMap(mongo_connection *dbconn, struct cfagent_connection *conn,char *menu,time_t since)

{ int tosend,cipherlen=0;
 char in[CF_BUFSIZE],out[CF_BUFSIZE],workbuf[CF_BUFSIZE],cfchangedstr[265];
  long n_read_total = 0,length = 0;
  int plainlen,more = true,header = true,current_report = -1;
  time_t now,then,time2 = 0,delta1 = 0,delta2 = 0;
  struct Item *reports[CF_CODEBOOK_SIZE] = {0};
  char keyHash[EVP_MAX_MD_SIZE*4];
  double datarate;

NewReportBook(reports);

snprintf(cfchangedstr,255,"%s%s",CF_CHANGEDSTR1,CF_CHANGEDSTR2);

workbuf[0] = '\0';

now = time(NULL);

snprintf(in,CF_BUFSIZE-CF_PROTO_OFFSET,"QUERY %s %ld %ld",menu,(long)since,now);
cipherlen = EncryptString(conn->encryption_type,in,out,conn->session_key,strlen(in)+1);
snprintf(workbuf,CF_BUFSIZE,"SQUERY %4d",cipherlen);
memcpy(workbuf+CF_PROTO_OFFSET,out,cipherlen);
tosend=cipherlen+CF_PROTO_OFFSET;

/* Send proposition C0 - query */

 CfOut(cf_verbose,""," -> Sending query at %s",cf_ctime(&now));

if (SendTransaction(conn->sd,workbuf,tosend,CF_DONE) == -1)
   {
   CfOut(cf_error,"send","Couldn't send data");
   return false;
   }

n_read_total = 0;

while (more)
   {
   out[0] = '\0';

   if ((cipherlen = ReceiveTransaction(conn->sd,in,&more)) == -1)
      {
      CfOut(cf_error,"recv"," !! Failed to collect data");
      return false;
      }

   if (strncmp(in,"BAD:",4) == 0)
      {
      CfOut(cf_error,""," !! Abort transmission: got \"%s\" from %s",in+4,conn->remoteip);
      break;;
      }

   plainlen = DecryptString(conn->encryption_type,in,out,conn->session_key,cipherlen);


    // Check the header for timing of response - we can eventually use this to
    // measure the network performance

   if (header)
      {
      char validate[5];
      char timebuffer[26];

      header = false; // First one is special
      sscanf(out,"%4s %ld %ld %ld",validate,&delta1,&time2,&length);

      if (strcmp(validate,"CFR:") != 0)
         {
         CfOut(cf_error,""," !! Invalid report format");
         return false;
         }

      then = now;
      now = time(NULL);
      delta2 = now - time2;

      CfOut(cf_verbose,""," -> Received reply of %d bytes at %s -> Xfer time %d seconds (processing time %d seconds)",
            length, cf_strtimestamp_local(now,timebuffer), delta2, now-then);

      if (delta2 > 0)
         {
         datarate = (double)length/(double)delta2;
         CfOut(cf_verbose,""," -> Data rate of %lf",datarate);
         }
      else
         {
         delta2 = (now-then)/2;
         CfOut(cf_verbose,""," -> Data rate was unmeasurable (instantaneous) - computing outer boundary");
         if (delta2 != 0)
            {
            datarate = (double)length/(double)delta2;
            }
         else
            {
            datarate = 0;
            }
         }

      // Promise to record data rate per host-digest and per IP

      Nova_RecordNetwork(dbconn,now,datarate,conn);
      }
   else
      {
      // Put data into a separate linked list for each code
      current_report = Nova_StoreIncomingReports(out,reports,current_report);
      }
   }

if (reports == NULL)
   {
   return false;
   }

UnpackReportBook(dbconn,HashPrintSafe(CF_DEFAULT_DIGEST,conn->digest,keyHash),reports);
DeleteReportBook(reports);
return true;
}

#endif  /* HAVE_LIBMONGOC */


/*********************************************************************/

int Nova_StoreIncomingReports(char *reply,struct Item **reports,int current_report)

{
int report;

for (report = 0; CF_CODEBOOK[report] != NULL; report++)
   {
   if (strcmp(reply,CF_CODEBOOK[report]) == 0)
      {
      CfOut(cf_verbose,""," -> New chapter ... %s",reply);
      current_report = report;
      return report;
      }
   }

if (current_report < 0)
   {
   CfOut(cf_error,""," !! Report format is corrupted. Got \"%s\" before codebook",reply);
   return -1;
   }

AppendItem(&(reports[current_report]),reply,NULL);
return current_report;
}

/*********************************************************************/

void NewReportBook(struct Item **reports)

{ int i;

for (i = 0; CF_CODEBOOK[i] != NULL; i++)
   {
   reports[i] = NULL;
   }
}

/*********************************************************************/

#ifdef HAVE_LIBMONGOC

void UnpackReportBook(mongo_connection *dbconn, char *id,struct Item **reports)

{ int i;

for (i = 0; CF_CODEBOOK[i] != NULL; i++)
   {
   if (reports[i] != NULL)
      {
      void (*fnptr)() = CF_CODEBOOK_HANDLER[i];
      (*fnptr)(dbconn,id,reports[i]);
      }
   }

CFDB_SaveLastUpdate(dbconn,MONGO_DATABASE,cfr_keyhash,id);

}

#endif

/*********************************************************************/

void DeleteReportBook(struct Item **reports)

{ int i;

for (i = 0; CF_CODEBOOK[i] != NULL; i++)
   {
   if (reports[i] != NULL)
      {
      DeleteItemList(reports[i]);
      }
   }
}

/*********************************************************************/

#ifdef HAVE_LIBMONGOC

static void Nova_RecordNetwork(mongo_connection *dbconnp, time_t now, double datarate,struct cfagent_connection *conn)
// NOTE: NOT Thread-safe (use of HashPrint())
{
  mongo_cursor *cursor;
  bson_buffer bb, *setObj;
  bson query,field,update;
  struct Event e = {0},newe = {0};
  double delta;

  newe.t = now;
  newe.Q.q = 0;
  newe.Q.expect = 0;
  newe.Q.var = 0;

// query

bson_buffer_init(&bb);
bson_append_string(&bb,cfr_keyhash,HashPrint(CF_DEFAULT_DIGEST,conn->digest));
bson_from_buffer(&query,&bb);

// returned value

bson_buffer_init(&bb);
bson_append_int(&bb,cfr_netmeasure,1);
bson_from_buffer(&field,&bb);

cursor = mongo_find(dbconnp,MONGO_DATABASE,&query,&field,0,0,CF_MONGO_SLAVE_OK);

if (mongo_cursor_next(cursor))  // not more than one record
   {
   bson_iterator it;
   bson_iterator_init(&it, cursor->current.data);

   while(bson_iterator_next(&it))
      {
      if (strcmp(bson_iterator_key(&it),cfr_netmeasure) == 0)
	 {
         if (bson_iterator_bin_len(&it) == sizeof(e))
            {
            memcpy(&e,bson_iterator_bin_data(&it),sizeof(e));
            newe.t = now;
            newe.Q.q = datarate;
            newe.Q.expect = GAverage(datarate,e.Q.expect,0.5);
            delta = (datarate - e.Q.expect)*(datarate - e.Q.expect);
            newe.Q.var = GAverage(delta,e.Q.var,0.5);
            }
         else
            {
            CfOut(cf_verbose, "", "!! Existing network measurement incorrect - overwriting");
            }
	 }
      }

   }

mongo_cursor_destroy(cursor);


// update DB with new measurement
bson_buffer_init(&bb);

setObj = bson_append_start_object(&bb, "$set");
bson_append_binary(setObj,cfr_netmeasure,' ',(char *)&newe,sizeof(newe));  // unused type
bson_append_finish_object(setObj);

bson_from_buffer(&update,&bb);

mongo_update(dbconnp,MONGO_DATABASE,&query,&update,MONGO_UPDATE_UPSERT);

bson_destroy(&query);
bson_destroy(&field);
bson_destroy(&update);
}

#endif  /* HAVE_LIBMONGOC */
