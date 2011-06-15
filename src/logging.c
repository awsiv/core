/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: logging.c                                                           */
/*                                                                           */
/* Created: Sun Feb 14 11:19:26 2010                                         */
/*                                                                           */
/*****************************************************************************/

static int FACILITY;

void Nova_RemoteSyslog(struct Attributes a,struct Promise *pp)

{
  int sd,err,rfc3164_len = 1024;
  char message[CF_BUFSIZE];
  time_t now = time(NULL);
  int pri = a.transaction.log_priority | FACILITY;

#if defined(HAVE_GETADDRINFO)
  struct addrinfo query, *response, *ap;
  char strport[CF_MAXVARSIZE];

snprintf(strport,CF_MAXVARSIZE-1,"%u",(int)SYSLOGPORT);  
memset(&query,0,sizeof(struct addrinfo));   
query.ai_family = AF_UNSPEC;
query.ai_socktype = SOCK_DGRAM;

if ((err = getaddrinfo(SYSLOGHOST,strport,&query,&response)) != 0)
   {
   CfOut(cf_inform,"","Unable to find syslog_host or service: (%s/%s) %s",SYSLOGHOST,strport,gai_strerror(err));
   return;
   }

for (ap = response; ap != NULL; ap = ap->ai_next)
   {
   CfOut(cf_verbose,""," -> Connect to syslog %s = %s on port %s\n",SYSLOGHOST,sockaddr_ntop(ap->ai_addr),strport);
   
   if ((sd = socket(ap->ai_family,ap->ai_socktype,IPPROTO_UDP)) == -1)
      {
      CfOut(cf_inform,"socket","Couldn't open a socket");
      continue;      
      }
   else
      {
      char timebuffer[26];
      snprintf(message,rfc3164_len,"<%u>%.15s %s %s",pri,cf_strtimestamp_local(now,timebuffer)+4,VFQNAME,a.transaction.log_string);
      if (sendto(sd,message,strlen(message),0,ap->ai_addr,ap->ai_addrlen) == -1)
         {
         CfOut(cf_verbose,"sendto"," -> Couldn't send \"%s\" to syslog server \"%s\"\n",message,SYSLOGHOST);
         }
      else
         {
         CfOut(cf_verbose,""," -> Syslog message: \"%s\" to server \"%s\"\n",message,SYSLOGHOST);
         }
      close(sd);
      return;
      }
   }

#else
struct sockaddr_in addr;
char timebuffer[26];
sockaddr_pton(AF_INET,SYSLOGHOST,&addr);
addr.sin_port = htons(SYSLOGPORT);

if ((sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
   {
   CfOut(cf_error,"sendto"," !! Unable to send syslog datagram");
   return;
   }

snprintf(message,rfc3164_len,"<%u>%.15s %s %s",pri,cf_strtimestamp_local(now,timebuffer)+4,VFQNAME,a.transaction.log_string);

if (sendto(sd,message,strlen(message),0,(struct sockaddr *)&addr, sizeof(addr)) == -1)
   {
   CfOut(cf_error,"sendto"," !! Unable to send syslog datagram");
   return;
   }

CfOut(cf_verbose,""," -> Syslog message: \"%s\" to server %s\n",message,SYSLOGHOST);
close(sd);
#endif
}
