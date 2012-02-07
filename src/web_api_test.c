#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "cf.nova.web_api.h"

#define WORD_LEN  10
/***************************************************************************************************************/
/* 
 * Using Join/strcat is too slow
 * Buffer limit for destination must be handled in the calling function to avoid segfault
*/

char* strcatUnsafe( char* dest,char* src)
{
  while (*dest) 
    {
    dest++; 
    }

  while (*src)
    {
    *dest++ = *src++;
    }

  return --dest;
}

/***************************************************************************************************************/
char *RandomizeString(int len, char *buffer, int buflen)

{ char *letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_2134lkahsdlkfhjsad982374lkjdflands8234987adslf567890asdf";
  int max = strlen(letters);

buffer[0]='\0';

for(int i = 0; i < len; i++)
  {
  int index = rand() % max;
  buffer[i] = letters[index];
  buffer[i+1] = '\0';
  }

return buffer;
}

/***************************************************************************************************************
 * cfpr_report_classes (sssbssbll) 
 **************************************************************************************************************/
int Nova2PHP_classes_report_test(char *hostkey,char *name,int regex,HostClassFilter *hostClassFilter,PageInfo *page, char *returnval,int bufsize)

{ char work[CF_BUFSIZE]={0};
  char header[CF_BUFSIZE]={0};
  char context[CF_MAXVARSIZE];
  char *p = returnval;
  
  int truncated = false;
  int totalLen = 0;
  int recordLen = 0;  
  int startIndex = page->resultsPerPage*(page->pageNum - 1);
  int endIndex = page->resultsPerPage*page->pageNum;
  
  const char *total_env = getenv("CFENGINE_TEST_OVERRIDE_TOTAL_RECORDS");

  int total = ( total_env == NULL ) ? endIndex : atoi(total_env);

snprintf(header,sizeof(header),
	 "\"meta\":{\"count\" : %d,"
	 "\"header\": {\"Host\":0,\"Class Context\":1,\"Occurs with Probability\":2,\"Uncertainty\":3,\"Last seen\":4"
	 "}", total);
 
StartJoin(returnval, "{\"data\":[", bufsize);
totalLen = strlen(header) + strlen(CF_NOTICE_TRUNCATED) + strlen(returnval) + 1000; 

for (int i = startIndex; (i < endIndex) || (i < total); i++)
  {
  if(!recordLen)
    {
    recordLen = strlen(work);
    }
  totalLen += recordLen;
  
  if(totalLen >= bufsize-1)
    {
    truncated = true;
    break;
    }
    
  RandomizeString(WORD_LEN,context,sizeof(context));

  snprintf(work, sizeof(work), "[\"%s\",\"%s\",%lf,%lf,%ld],",
	   "myhost",
	   context,
	   1.8,
	   1.5,
	   time(NULL));
  
  p = strcatUnsafe(p, work);	  
  }

ReplaceTrailingChar(returnval,',','\0');
EndJoin(returnval,"]",bufsize);

Nova_AddReportHeader(header, truncated, work, sizeof(work)-1);

Join(returnval, work, bufsize);
EndJoin(returnval,"}}\n",bufsize);

return true;
}

/**************************************************************************************************************/
/*
 * cfpr_promise_list_by_handle_rx(ss)
*/
int Nova2PHP_promise_list_test(PromiseFilter *promiseFilter ,char *returnval, int bufsize)

{ const char *total_env = getenv("CFENGINE_TEST_OVERRIDE_TOTAL_RECORDS");
  int total = ( total_env == NULL || (atoi(total_env) > 80000) ) ? 80000 : atoi(total_env);

StartJoin(returnval, "[", bufsize);

char *p = returnval;

for ( int i = 0; i < total; i++)
  {
  char work[CF_MAXVARSIZE] = {0};
  char handle[CF_MAXVARSIZE] = {0};
  char promiseType[CF_MAXVARSIZE] = {0};
  char bundleName[CF_MAXVARSIZE] = {0};
  char bundleType[CF_MAXVARSIZE] = {0};
  char promiser[CF_MAXVARSIZE] = {0};
  
  RandomizeString(WORD_LEN, promiseType, sizeof(promiseType));
  RandomizeString(WORD_LEN, bundleType, sizeof(bundleType));
  RandomizeString(WORD_LEN, promiser, sizeof(promiser));
  RandomizeString(WORD_LEN * 2, handle, sizeof(handle));
  RandomizeString(WORD_LEN * 2, bundleName, sizeof(bundleName));

  snprintf(work,sizeof(work),"[\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"],",
	   handle,
	   promiseType,
	   bundleName,
	   bundleType,
	   promiser);
  
  p = strcatUnsafe(p, work);	  
  }
 
ReplaceTrailingChar(returnval, ',', '\0');
EndJoin(returnval, "]", bufsize);

return true;
}
/*****************************************************************************/
/*
 * cfpr_class_cloud(s)
*/
int Nova2PHP_classes_summary_test(char **classes, char *returnval, int bufsize)

{ const char *total_env = getenv("CFENGINE_TEST_OVERRIDE_TOTAL_RECORDS"); 
  int total = (total_env == NULL || (atoi(total_env) > 9000) ) ? 9000 : atoi(total_env);
  char *p = returnval;

StartJoin(returnval, "{\"hosts\":[", bufsize);

for(int i = 0; i < (total/5) ; i++)
  {
  char work[CF_MAXVARSIZE] = {0};
  char hostName[CF_MAXVARSIZE] = {0};
  char keyHash[CF_MAXVARSIZE] = {0};
    
  RandomizeString(WORD_LEN*2,hostName,sizeof(hostName));
  RandomizeString(WORD_LEN*7,keyHash,sizeof(keyHash));

  snprintf(work, sizeof(work), "[\"%s\",\"%s\"]\n,", hostName, keyHash);

  p = strcatUnsafe(p, work);	  
  }
 
ReplaceTrailingChar(returnval, ',', '\0');
EndJoin(returnval,"]",bufsize);
  
Join(returnval, ",\n\"classes\":[", bufsize - 10);
      
for(int i = 0; i < (total*4)/5; i++)
  {
  char work[CF_MAXVARSIZE] = {0};
  char context[CF_MAXVARSIZE] = {0};

  RandomizeString(WORD_LEN*3, context, sizeof(context));

  snprintf(work, sizeof(work), "[\"%s\",%d]\n,", context, i);

  p = strcatUnsafe(p,work);
  }
 
ReplaceTrailingChar(returnval, ',', '\0');
EndJoin(returnval,"]}",bufsize);

return true;
}
/*****************************************************************************/
/*
 * cfpr_show_hosts_name(ssll)
*/
int Nova2PHP_show_hosts_test(char *hostNameRegex,char *ipRegex,char *classRegex,PageInfo *page,char *returnval,int bufsize)

{ char work[CF_MAXVARSIZE] = {0};
  int startIndex = page->resultsPerPage*(page->pageNum - 1);
  int endIndex = page->resultsPerPage*page->pageNum;
  const char *total_env = getenv("CFENGINE_TEST_OVERRIDE_TOTAL_RECORDS");

  int total = ( total_env == NULL ) ? endIndex : atoi(total_env);

// Max limit to avoid segfault
if(total > 60000)
  {
  total = 60000;
  }

char *p = returnval;

snprintf(work, sizeof(work),
	 "{\"meta\":{\"count\" : %d,"
	 "\"header\": {\"Key Hash\":0,\"Host name\":1,\"IP address\":2"
	 "}},\n\"data\":[", total);
 
StartJoin(returnval,work,bufsize);

for (int i = startIndex; (i < endIndex) || (i < total); i++)
  {
  char hostName[CF_MAXVARSIZE] = {0};
  char keyHash[CF_MAXVARSIZE] = {0};

  RandomizeString(WORD_LEN * 3, hostName, sizeof(hostName));
  RandomizeString(WORD_LEN * 3, keyHash, sizeof(keyHash));

  snprintf(work, sizeof(work), "[\"%s\", \"%s\", \"%s\"]\n,", hostName, "10.0.0.100", keyHash);
    
  p = strcatUnsafe(p,work);
  }

ReplaceTrailingChar(returnval, ',', '\0');
EndJoin(returnval,"]}",bufsize);

return true;
}
/*****************************************************************************/
/*
 * cfpr_report_bundlesseen (sssbssbll)
*/
int Nova2PHP_bundle_report_test(char *hostkey,char *bundle,int regex,HostClassFilter *hostClassFilter,PageInfo *page,char *returnval,int bufsize)

{ char *p = returnval;
  char work[CF_BUFSIZE] = {0};
  char header[CF_BUFSIZE] = {0};
  int truncated = false;

  int startIndex = page->resultsPerPage*(page->pageNum - 1);
  int endIndex = page->resultsPerPage*page->pageNum;
  const char *total_env = getenv("CFENGINE_TEST_OVERRIDE_TOTAL_RECORDS");

  int total = ( total_env == NULL ) ? endIndex : atoi(total_env);

// Max limit to avoid segfault
if(total > 30000)
  {
  total = 30000;
  }

snprintf(header,sizeof(header),
	 "\"meta\":{\"count\" : %d,"
	 "\"header\": {\"Host\":0,\"Bundle\":1,\"Last Verified\":2,\"Hours Ago\":3,\"Avg Interval\":4,\"Uncertainty\":5,"
	 "\"Note\":{\"index\":6,\"subkeys\":{\"action\":0,\"hostkey\":1,\"reporttype\":2,\"rid\":3,\"nid\":4}}"
	 "}", total);

StartJoin(returnval,"{\"data\":[",bufsize);

for (int i = startIndex; (i < endIndex) || (i < total); i++)
  {
  char hostName[CF_MAXVARSIZE];
  char bundle[CF_MAXVARSIZE];
  char keyHash[CF_MAXVARSIZE];
  char nid[CF_MAXVARSIZE];

  RandomizeString(WORD_LEN*3, hostName, sizeof(hostName));
  RandomizeString(WORD_LEN*2, bundle, sizeof(bundle));
  RandomizeString(WORD_LEN*7, keyHash, sizeof(keyHash));
  RandomizeString(WORD_LEN*4, nid, sizeof(nid));
    
  snprintf(work, sizeof(work),
	   "[\"%s\",\"%s\",%ld,%.2lf,%.2lf,%.2lf,"
	   "[\"add\",\"%s\",%d,\"%s\",\"\"]],",
	   hostName,
	   bundle,
	   time(NULL),
	   (double)(rand()%100)/100,
	   (double)(rand()%100)/100,
	   (double)(rand()%100)/100,
	   keyHash,
	   CFREPORT_BUNDLE,
	   nid);

  p = strcatUnsafe(p, work);
  }
 
ReplaceTrailingChar(returnval,',','\0');
EndJoin(returnval,"]",bufsize);

Nova_AddReportHeader(header, truncated, work, sizeof(work)-1);
Join(returnval, work, bufsize);
EndJoin(returnval, "}}\n", bufsize);

return true;
}

/*****************************************************************************/
