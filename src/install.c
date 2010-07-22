
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: install.c                                                           */
/*                                                                           */
/* Created: Wed Jul 21 07:17:44 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

struct HubHost *NewHubHost(char *keyhash,char *hostname,char *ipaddr)

{ struct HubHost *hp;

if ((hp = malloc(sizeof(struct HubHost))) == NULL)
   {
   FatalError("Memory exhausted NewHubHost");
   }

if (keyhash)
   {
   hp->keyhash = strdup(keyhash);
   }
else
   {
   hp->keyhash = NULL;
   }

if (ipaddr)
   {
   hp->ipaddr = strdup(ipaddr);
   }
else
   {
   hp->ipaddr = NULL;
   }

if (hostname)
   {
   hp->hostname = strdup(hostname);
   }
else
   {
   hp->hostname = NULL;
   }

return hp;
}

/*****************************************************************************/

void DeleteHubHost(struct HubHost *hp)
{
if (hp->keyhash)
   {
   free(hp->keyhash);
   }

if (hp->ipaddr)
   {
   free(hp->ipaddr);
   }

if (hp->hostname)
   {
   free(hp->hostname);
   }

free(hp);
}

/*****************************************************************************/

struct HubSoftware *NewHubSoftware(char *name,char *version,char *arch)

{ struct HubSoftware *hp;

if ((hp = malloc(sizeof(struct HubSoftware))) == NULL)
   {
   FatalError("Memory exhausted NewHubSoftware");
   }

if (name)
   {
   hp->name = strdup(name);
   }
else
   {
   hp->name = NULL;
   }

if (version)
   {
   hp->version = strdup(version);
   }
else
   {
   hp->version = NULL;
   }

if (arch)
   {
   hp->arch = strdup(arch);
   }
else
   {
   hp->arch = NULL;
   }


return hp;
}

/*****************************************************************************/

void DeleteHubSoftware(struct HubSoftware *hs)
{
if (hs->name)
   {
   free(hs->name);
   }

if (hs->version)
   {
   free(hs->version);
   }

if (hs->arch)
   {
   free(hs->arch);
   }

free(hs);
}

/*****************************************************************************/

struct HubClass *NewHubClass(double p, double dev, time_t t)

{ struct HubClass *hp;

if ((hp = malloc(sizeof(struct HubClass))) == NULL)
   {
   FatalError("Memory exhausted NewHubSoftware");
   }

hp->prob = p;
hp->dev = dev;
hp->t = t;
return hp;
}

/*****************************************************************************/

void DeleteHubClass(struct HubClass *hc)
{
free(hc);
}

/*****************************************************************************/

struct HubTotalCompliance *NewHubTotalCompliance(time_t t,char *v,int k,int r,int n)

{ struct HubTotalCompliance *hp;
 
if ((hp = malloc(sizeof(struct HubTotalCompliance))) == NULL)
   {
   FatalError("Memory exhausted NewHubSoftware");
   }

hp->t = t;
hp->version = v;
hp->kept = k;
hp->repaired = r;
hp->notkept = n;
return hp;
}

/*****************************************************************************/

void DeleteHubTotalCompliance(struct HubTotalCompliance *ht)
{
free(ht);
}

/*****************************************************************************/

struct HubVariable *NewHubVariable(char *type,char *scope,char *lval,char *rval)

{ struct HubVariable *hp;
     
if ((hp = malloc(sizeof(struct HubVariable))) == NULL)
   {
   FatalError("Memory exhausted NewHubVariable");
   }

hp->type = strdup(type);
hp->scope = strdup(scope);
hp->rval = strdup(rval);
hp->lval = strdup(lval);

return hp;
}

/*****************************************************************************/

void DeleteHubVariable(struct HubVariable *hv)
{
free(hv);
}

/*****************************************************************************/

struct HubPromiseLog *NewHubPromiseLog(char *handle,time_t t)

{ struct HubPromiseLog *hp;
     
if ((hp = malloc(sizeof(struct HubPromiseLog))) == NULL)
   {
   FatalError("Memory exhausted NewHubPromiseLog");
   }

hp->handle = strdup(handle);
hp->t = t;
return hp;
}

/*****************************************************************************/

void DeleteHubPromiseLog(struct HubPromiseLog *hp)

{
free(hp->handle);
free(hp);
}

/*****************************************************************************/

struct HubLastSeen *NewHubLastSeen(char io,char *kh,char *rhost,char *ip,double ago,double avg,double dev,time_t t)

{ struct HubLastSeen *hp;
     
if ((hp = malloc(sizeof(struct HubLastSeen))) == NULL)
   {
   FatalError("Memory exhausted NewHubLastSeen");
   }

hp->io = io;  // '+' or '-'
hp->rhost = NewHubHost(kh,rhost,ip);
hp->hrsago;
hp->hrsavg;
hp->hrsdev;
hp->t = t;

return hp;
}

/*****************************************************************************/

void DeleteHubLastSeen(struct HubLastSeen *hp)
{
DeleteHubHost(hp->rhost);
free(hp); 
}

/*****************************************************************************/

struct HubMeter *NewHubMeter(char type,double kept,double repaired)

{ struct HubMeter *hp;
     
if ((hp = malloc(sizeof(struct HubMeter))) == NULL)
   {
   FatalError("Memory exhausted NewHubLastSeen");
   }

hp->type = type;
hp->kept = kept;
hp->repaired = repaired;
hp->notkept = 100.0 - kept - repaired;
return hp;
}

/*****************************************************************************/

void DeleteHubMeter(struct HubMeter *hp)
{
free(hp);
}

/*****************************************************************************/

struct HubPerformance *NewHubPerformance(char *event,time_t t,double q,double e,double d)

{ struct HubPerformance *hp;
     
if ((hp = malloc(sizeof(struct HubPerformance))) == NULL)
   {
   FatalError("Memory exhausted NewHubPerformance");
   }

hp->event = strdup(event);
hp->q = q;
hp->e = e;
hp->d = d;
hp->t = t;
return hp;
}

/*****************************************************************************/

void DeleteHubPerformance(struct HubPerformance *hp)
{
free(hp->event);
free(hp); 
}

/*****************************************************************************/

struct HubSetUid *NewHubSetUid(char *file)

{ struct HubSetUid *hp;
     
if ((hp = malloc(sizeof(struct HubSetUid))) == NULL)
   {
   FatalError("Memory exhausted NewHubSetuid");
   }

hp->path = file;
return hp;
}

/*****************************************************************************/

void DeleteHubSetUid(struct HubSetUid *hp)
{
free(hp->path);
free(hp); 
}

/*****************************************************************************/

struct HubPromiseCompliance *NewHubCompliance(char status,double e,double d,time_t t)

{ struct HubPromiseCompliance *hp;
     
if ((hp = malloc(sizeof(struct HubPromiseCompliance))) == NULL)
   {
   FatalError("Memory exhausted NewHubPromiseCompliance");
   }

hp->status = status;
hp->e = e;
hp->d = d;
hp->t = t;
return hp;
};

/*****************************************************************************/

void DeleteHubPromiseCompliance(struct HubPromiseCompliance *hp)
{
free(hp); 
}
