/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "logging.h"

/*****************************************************************************/
/*                                                                           */
/* File: logging.c                                                           */
/*                                                                           */
/* Created: Sun Feb 14 11:19:26 2010                                         */
/*                                                                           */
/*****************************************************************************/

static char SYSLOG_HOST[CF_BUFSIZE] = "localhost";
static uint16_t SYSLOG_PORT = 514;


int FACILITY;

/* Delete this function once we build Nova against Core trunk/3.3 */
void Nova_RemoteSyslog(struct Attributes a, struct Promise *pp)
{
RemoteSysLog(a.transaction.log_priority, a.transaction.log_string);
}

/*****************************************************************************/

void SetSyslogHost(const char *host)
{
strlcpy(SYSLOG_HOST, host, CF_BUFSIZE);
}

void SetSyslogPort(uint16_t port)
{
SYSLOG_PORT = port;
}

void RemoteSysLog(int log_priority, const char *log_string)
{
  int sd,err,rfc3164_len = 1024;
  char message[CF_BUFSIZE];
  time_t now = time(NULL);
  int pri = log_priority | FACILITY;

#if defined(HAVE_GETADDRINFO)
  struct addrinfo query, *response, *ap;
  char strport[CF_MAXVARSIZE];

snprintf(strport,CF_MAXVARSIZE-1,"%u",(unsigned)SYSLOG_PORT);
memset(&query,0,sizeof(struct addrinfo));   
query.ai_family = AF_UNSPEC;
query.ai_socktype = SOCK_DGRAM;

if ((err = getaddrinfo(SYSLOG_HOST,strport,&query,&response)) != 0)
   {
   CfOut(cf_inform,"","Unable to find syslog_host or service: (%s/%s) %s",SYSLOG_HOST,strport,gai_strerror(err));
   return;
   }

for (ap = response; ap != NULL; ap = ap->ai_next)
   {
   CfOut(cf_verbose,""," -> Connect to syslog %s = %s on port %s\n",SYSLOG_HOST,sockaddr_ntop(ap->ai_addr),strport);
   
   if ((sd = socket(ap->ai_family,ap->ai_socktype,IPPROTO_UDP)) == -1)
      {
      CfOut(cf_inform,"socket","Couldn't open a socket");
      continue;      
      }
   else
      {
      char timebuffer[26];
      snprintf(message,rfc3164_len,"<%u>%.15s %s %s",pri,cf_strtimestamp_local(now,timebuffer)+4,VFQNAME, log_string);
      if (sendto(sd,message,strlen(message),0,ap->ai_addr,ap->ai_addrlen) == -1)
         {
         CfOut(cf_verbose,"sendto"," -> Couldn't send \"%s\" to syslog server \"%s\"\n",message,SYSLOG_HOST);
         }
      else
         {
         CfOut(cf_verbose,""," -> Syslog message: \"%s\" to server \"%s\"\n",message,SYSLOG_HOST);
         }
      close(sd);
      return;
      }
   }

#else
struct sockaddr_in addr;
char timebuffer[26];
sockaddr_pton(AF_INET,SYSLOG_HOST,&addr);
addr.sin_port = htons(SYSLOG_PORT);

if ((sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
   {
   CfOut(cf_error,"sendto"," !! Unable to send syslog datagram");
   return;
   }

snprintf(message,rfc3164_len,"<%u>%.15s %s %s",pri,cf_strtimestamp_local(now,timebuffer)+4,VFQNAME, log_string);

if (sendto(sd,message,strlen(message),0,(struct sockaddr *)&addr, sizeof(addr)) == -1)
   {
   CfOut(cf_error,"sendto"," !! Unable to send syslog datagram");
   return;
   }

CfOut(cf_verbose,""," -> Syslog message: \"%s\" to server %s\n",message,SYSLOG_HOST);
close(sd);
#endif
}
