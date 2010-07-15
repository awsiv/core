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

#define cf_codebook_size 23

char *CF_CODEBOOK[cf_codebook_size] =
   {
   CFR_PERF,
   CFR_CLASS,
   CFR_SETUID,
   CFR_FCHANGE,
   CFR_FDIFF,
   CFR_MONITOR_WEEK,
   CFR_MONITOR_MAG,
   CFR_MONITOR_HIST,
   CFR_MONITOR_YEAR,
   CFR_PCOMPLIANCE,
   CFR_TCOMPLIANCE,
   CFR_SOFTWARE,
   CFR_AVAILPATCH,
   CFR_PATCHSTATUS,
   CFR_PROMISEOUT,
   CFR_VALUE,
   CFR_VARS,
   CFR_LASTSEEN,
   CFR_REPAIRLOG,
   CFR_NOTKEPTLOG,
   CFR_METER,
   CFR_BUNDLES,
   NULL
   };

void *CF_CODEBOOK_HANDLER[cf_codebook_size] =
   {
   Nova_UnPackPerformance,  // DBOK
   Nova_UnPackClasses,      // DBOK (nopurge)
   Nova_UnPackSetuid,       // DBOK (nopurge)
   Nova_UnPackFileChanges,  // bug  LOG
   Nova_UnPackDiffs,        // bug  LOG 
   Nova_UnPackMonitorWeek,  // DBOK
   Nova_UnPackMonitorMag,   // DBOK
   Nova_UnPackMonitorHist,  // DBOK
   Nova_UnPackMonitorYear,  // nodata
   Nova_UnPackCompliance,   //        LOG
   Nova_UnPackTotalCompliance, // DBOK (append) LOG BUG?
   Nova_UnPackSoftware,     // DBOK + QUERYOK
   Nova_UnPackAvailPatches, // nodata
   Nova_UnPackPatchStatus,  // nodata
   Nova_UnPack_promise_output_common,  // nodata
   Nova_UnPackValueReport,  // nodata
   Nova_UnPackVariables,    // DBOK
   Nova_UnPackLastSeen,     // DBOK
   Nova_UnPackRepairLog,    // DBOK (append - nonconvergent+reverse) LOG
   Nova_UnPackNotKeptLog,   // DBOK (append - nonconvergent+reverse) LOG
   Nova_UnPackMeter,        // DBOK
   Nova_UnPackBundles
   };

/*********************************************************************/

int Nova_QueryForKnowledgeMap(struct cfagent_connection *conn,char *menu,time_t since)

{ int done = false,tosend,cipherlen=0,value;
  char in[CF_BUFSIZE],out[CF_BUFSIZE],workbuf[CF_BUFSIZE],cfchangedstr[265];
  unsigned char iv[32] = {1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8};
  long n_read_total = 0,length = 0;
  EVP_CIPHER_CTX ctx;
  int plainlen,more = true,header = true,current_report = -1;
  time_t now = time(NULL),time2 = 0,delta1 = 0,delta2 = 0;
  struct Item *reports[cf_codebook_size] = {0};

NewReportBook(reports);
  
snprintf(cfchangedstr,255,"%s%s",CF_CHANGEDSTR1,CF_CHANGEDSTR2);

workbuf[0] = '\0';
EVP_CIPHER_CTX_init(&ctx);  

snprintf(in,CF_BUFSIZE-CF_PROTO_OFFSET,"QUERY %s %ld %ld",menu,(long)since,now);
cipherlen = EncryptString(conn->encryption_type,in,out,conn->session_key,strlen(in)+1);
snprintf(workbuf,CF_BUFSIZE,"SQUERY %4d",cipherlen);
memcpy(workbuf+CF_PROTO_OFFSET,out,cipherlen);
tosend=cipherlen+CF_PROTO_OFFSET;   

/* Send proposition C0 - query */

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
      CfOut(cf_error,""," !! Abort transmission %s",in+4);
      break;;
      }   

   plainlen = DecryptString(conn->encryption_type,in,out,conn->session_key,cipherlen);

    // Check the header for timing of response - we can eventually use this to
    // measure the network performance
   
   if (header)
      {
      char validate[5];
      
      header = false; // First one is special
      sscanf(out,"%4s %ld %ld %ld",validate,&delta1,&time2,&length);

      if (strcmp(validate,"CFR:") != 0)
         {
         CfOut(cf_error,""," !! Invalid report format");
         return false;
         }
      
      now = time(NULL);
      delta2 = time2 - time(NULL);
      
      CfOut(cf_verbose,""," -> Received reply of %d bytes at %s",length,ctime(&now));

      if (delta2 > 0)
         {
         CfOut(cf_verbose,""," -> Data rate of %lf",(double)length/(double)delta2);
         }
      else
         {
         CfOut(cf_verbose,""," -> Data rate was unmeasurable (instantaneous)");
         }
      }
   else
      {
      // Put data into a separate linked list for each code
      current_report = Nova_StoreIncomingReports(out,reports,current_report);
      }
   }

EVP_CIPHER_CTX_cleanup(&ctx);

if (reports == NULL)
   {
   return false;
   }

UnpackReportBook(HashPrint(cf_md5,conn->digest),reports);
DeleteReportBook(reports);
return true;
}

/*********************************************************************/

int Nova_StoreIncomingReports(char *reply,struct Item **reports,int current_report)

{ struct Item *ip;
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

void UnpackReportBook(char *id, struct Item **reports)

{ int i;
  mongo_connection dbconn = {0};
  mongo_connection *dbconnp = &dbconn;

#ifdef HAVE_LIBMONGOC
if (!Nova_DBOpen(&dbconn, "127.0.0.1", 27017))
   {
   CfOut(cf_error, "", "!! Could not open connection to report database");
   dbconnp = NULL;
   }
#endif

for (i = 0; CF_CODEBOOK[i] != NULL; i++)
   {
   if (reports[i] != NULL)
      {
      void (*fnptr)() = CF_CODEBOOK_HANDLER[i];
      (*fnptr)(dbconnp,id,reports[i]);
      }
   }

#ifdef HAVE_LIBMONGOC
if (dbconnp && !Nova_DBClose(&dbconn))
   {
   CfOut(cf_error, "", "!! Could not close connection to report database");
   }
#endif

}
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
