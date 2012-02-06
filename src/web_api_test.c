#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "cf.nova.web_api.h"

#define MAX_LEN  10
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
  while (*dest++ = *src++);
  return --dest;
}

/***************************************************************************************************************/
char *RandomizeString(int len, char *buffer, int buflen)

{ char *letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_2134lkahsdlkfhjsad982374lkjdflands8234987adslf567890asdf";
  int i;
  int max = strlen(letters) -1;

  buffer[0]='\0';

  for(i=0; i<len-1;i++)
    {
      int index = rand() % max;
      buffer[i] = letters[index];
      buffer[i+1] = '\0';
    }

  return buffer;
}

/***************************************************************************************************************/

int Nova2PHP_classes_report_test(char *hostkey,char *name,int regex,HostClassFilter *hostClassFilter,PageInfo *page, char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE]={0};
  char header[CF_BUFSIZE]={0};
  int headerLen=0;
  int noticeLen=0;
  int truncated = false;
  int i = 0;
  char testStr[CF_MAXVARSIZE];
  char *p = returnval;
  int totalLen = 0;
  int recordLen = 0;
  int total = page->resultsPerPage*page->pageNum;

snprintf(header,sizeof(header),
	 "\"meta\":{\"count\" : %d,"
	 "\"header\": {\"Host\":0,\"Class Context\":1,\"Occurs with Probability\":2,\"Uncertainty\":3,\"Last seen\":4"          "}", total);
 
headerLen = strlen(header);
noticeLen = strlen(CF_NOTICE_TRUNCATED);
StartJoin(returnval,"{\"data\":[",bufsize);

totalLen = headerLen + noticeLen + strlen(returnval) + 1000; 

for (i=0; i<total; i++)
  {
    if(!recordLen)
      {
	recordLen = strlen(buffer);
      }
    totalLen += recordLen;
    
    if(totalLen >= bufsize-1)
      {
	truncated = true;
	break;
      }
    
    testStr[0] = '\0';
    snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\",%lf,%lf,%ld],",
	     "myhost",
	     RandomizeString(MAX_LEN,testStr,sizeof(testStr)),
	     1.8,
	     1.5,
	     time(NULL));
    
    p = strcatUnsafe(p,buffer);	  
  }

ReplaceTrailingChar(returnval,',','\0');
EndJoin(returnval,"]",bufsize);

Nova_AddReportHeader(header,truncated,buffer,sizeof(buffer)-1);

Join(returnval,buffer,bufsize);
EndJoin(returnval,"}}\n",bufsize);
return true;
}

/***************************************************************************************************************/
int Nova2PHP_promise_list_test(PromiseFilter *promiseFilter ,char *returnval, int bufsize)

{ char work[CF_MAXVARSIZE] = {0};
 int total = 80000;
 int i = 0;
 char *p = returnval;
 char testStr1[CF_MAXVARSIZE];
 char testStr2[CF_MAXVARSIZE];
 char testStr3[CF_MAXVARSIZE];
 char testStr4[CF_MAXVARSIZE];
 char testStr5[CF_MAXVARSIZE];

StartJoin(returnval, "[", bufsize);
for ( i= 0; i<total; i++)
  {
    snprintf(work,sizeof(work),"[\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"],",
             RandomizeString(MAX_LEN *2,testStr1,sizeof(testStr1)),
             RandomizeString(MAX_LEN,testStr2,sizeof(testStr2)),
             RandomizeString(MAX_LEN*2,testStr3,sizeof(testStr3)),
             RandomizeString(MAX_LEN,testStr4,sizeof(testStr4)),
             RandomizeString(MAX_LEN,testStr5,sizeof(testStr5)));
    
    p = strcatUnsafe(p,work);
  }
 
ReplaceTrailingChar(returnval, ',', '\0');
EndJoin(returnval, "]", bufsize);
return true;
}
/*****************************************************************************/

int Nova2PHP_classes_summary_test(char **classes, char *buf, int bufsize)

{ char work[CF_MAXVARSIZE];
  int i = 0;
  char testStr1[CF_MAXVARSIZE];
  char testStr2[CF_MAXVARSIZE];
  char testStr3[CF_MAXVARSIZE];

StartJoin(buf, "{", bufsize);

Join(buf, "\"hosts\":[", bufsize);
  
for(i = 0; i<3000; i++)
  {
    snprintf(work, sizeof(work), "[\"%s\",\"%s\"]\n,", 
	     RandomizeString(MAX_LEN*4,testStr1,sizeof(testStr1)),
	     RandomizeString(MAX_LEN*2,testStr2,sizeof(testStr2)));
    
    if(!Join(buf, work, bufsize - 10))
      {
	break;
      }
  }

ReplaceTrailingChar(buf, ',', '\0');
EndJoin(buf,"]",bufsize);
  
Join(buf, ",\n\"classes\":[", bufsize - 10);
      
for(i = 0; i<10000; i++)
  {
    snprintf(work, sizeof(work), "[\"%s\",%d]\n,",
	     RandomizeString(MAX_LEN*3,testStr3,sizeof(testStr3)), i);
      
    if(!Join(buf, work, bufsize - 10))
      {
	break;
      }
  }
 
ReplaceTrailingChar(buf, ',', '\0');
EndJoin(buf,"]",bufsize);

EndJoin(buf, "}", bufsize);

return true;
}
/*****************************************************************************/
int Nova2PHP_show_hosts_test(char *hostNameRegex,char *ipRegex,char *classRegex,PageInfo *page,char *buf,int bufsize)

{ char work[CF_MAXVARSIZE];
  
  int i = 0;
  char *p = buf;
  char testStr1[CF_MAXVARSIZE];
  char testStr2[CF_MAXVARSIZE];
  int total = page->resultsPerPage*page->pageNum;


if(total > 60000)
  {
    total = 60000;
  }

snprintf(work,sizeof(work),
	 "{\"meta\":{\"count\" : %d,"
	 "\"header\": {\"Key Hash\":0,\"Host name\":1,\"IP address\":2"
	 "}},\n\"data\":[",total);
 
StartJoin(buf,work,bufsize);

for (i=0; i<total; i++)
  {
    snprintf(work, sizeof(work), 
	     "[\"%s\", \"%s\", \"%s\"]\n,",
	     RandomizeString(MAX_LEN*3,testStr1,sizeof(testStr1)),
	     "10.0.0.100", 
	     RandomizeString(MAX_LEN*7,testStr2,sizeof(testStr2)));
    
    p = strcatUnsafe(p,work);
  }

ReplaceTrailingChar(buf, ',', '\0');

EndJoin(buf,"]}",bufsize);
return true;
}
/*****************************************************************************/
int Nova2PHP_bundle_report_test(char *hostkey,char *bundle,int regex,HostClassFilter *hostClassFilter,PageInfo *page,char *returnval,int bufsize)

{ char buffer[CF_BUFSIZE]={0};
  char header[CF_BUFSIZE]={0};
  int margin = 0,headerLen=0,noticeLen=0;
  int truncated = false;
  char testStr1[CF_MAXVARSIZE];
  char testStr2[CF_MAXVARSIZE];
  char testStr3[CF_MAXVARSIZE];
  char testStr4[CF_MAXVARSIZE];
  int total = page->resultsPerPage*page->pageNum;
  char *p = returnval;
 
if(total > 35000)
  {
    total = 35000;
  }

snprintf(header,sizeof(header),
	 "\"meta\":{\"count\" : %d,"
	 "\"header\": {\"Host\":0,\"Bundle\":1,\"Last Verified\":2,\"Hours Ago\":3,\"Avg Interval\":4,\"Uncertainty\":5,"
	 "\"Note\":{\"index\":6,\"subkeys\":{\"action\":0,\"hostkey\":1,\"reporttype\":2,\"rid\":3,\"nid\":4}}"
	 "}",total);

headerLen = strlen(header);
noticeLen = strlen(CF_NOTICE_TRUNCATED);
StartJoin(returnval,"{\"data\":[",bufsize);

for (int i=0; i<total; i++)
  {
    snprintf(buffer,sizeof(buffer),"[\"%s\",\"%s\",%ld,%.2lf,%.2lf,%.2lf,"
	     "[\"add\",\"%s\",%d,\"%s\",\"\"]],",
	     RandomizeString(MAX_LEN*3,testStr1,sizeof(testStr1)),
	     RandomizeString(MAX_LEN*2,testStr2,sizeof(testStr2)),
	     time(NULL),
	     (double)(rand()%100)/100,
	     (double)(rand()%100)/100,
	     (double)(rand()%100)/100,
	     RandomizeString(MAX_LEN*7,testStr3,sizeof(testStr3)),
	     CFREPORT_BUNDLE,
	     RandomizeString(MAX_LEN*4,testStr4,sizeof(testStr4)));

    margin = headerLen + noticeLen + strlen(buffer);
    p = strcatUnsafe(p,buffer);
  }
 
ReplaceTrailingChar(returnval,',','\0');
EndJoin(returnval,"]",bufsize);

Nova_AddReportHeader(header,truncated,buffer,sizeof(buffer)-1);
Join(returnval,buffer,bufsize);
EndJoin(returnval,"}}\n",bufsize);
return true;
}

/*****************************************************************************/
