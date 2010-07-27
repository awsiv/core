
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

struct HubQuery *NewHubQuery(struct Rlist *hosts,struct Rlist *records)

{ struct HubQuery *hq;

if ((hq = malloc(sizeof(struct HubQuery))) == NULL)
   {
   FatalError("Memory exhausted NewHubQuery");
   }

hq->hosts = hosts;
hq->records = records;
return hq;
}

/*****************************************************************************/

void DeleteHubQuery(struct HubQuery *hq,void (*fnptr)())

{ struct Rlist *rp;

for (rp = hq->hosts; rp != NULL; rp=rp->next)
   {
   DeleteHubHost(rp->item);
   rp->item = NULL;
   }
 
DeleteRlist(hq->hosts);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   (*fnptr)(rp->item);
   rp->item = NULL;
   }
 
DeleteRlist(hq->records);
}

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

struct HubSoftware *NewHubSoftware(struct HubHost *hh,char *name,char *version,char *arch)

{ struct HubSoftware *hp;

if ((hp = malloc(sizeof(struct HubSoftware))) == NULL)
   {
   FatalError("Memory exhausted NewHubSoftware");
   }

hp->hh = hh;

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

struct HubClass *NewHubClass(struct HubHost *hh,char *class,double p, double dev, time_t t)

{ struct HubClass *hp;

if ((hp = malloc(sizeof(struct HubClass))) == NULL)
   {
   FatalError("Memory exhausted NewHubSoftware");
   }

hp->hh = hh;

if (class)
   {
   hp->class = strdup(class);
   }
else
   {
   hp->class = NULL;
   }

hp->prob = p;
hp->dev = dev;
hp->t = t;
return hp;
}

/*****************************************************************************/

void DeleteHubClass(struct HubClass *hc)
{
if (hc->class)
   {
   free(hc->class);
   }

free(hc);
}

/*****************************************************************************/

struct HubTotalCompliance *NewHubTotalCompliance(struct HubHost *hh,time_t t,char *v,int k,int r,int n)

{ struct HubTotalCompliance *hp;
 
if ((hp = malloc(sizeof(struct HubTotalCompliance))) == NULL)
   {
   FatalError("Memory exhausted NewHubSoftware");
   }

hp->hh = hh;
hp->t = t;

hp->version = strdup(v);
hp->kept = k;
hp->repaired = r;
hp->notkept = n;
return hp;
}

/*****************************************************************************/

void DeleteHubTotalCompliance(struct HubTotalCompliance *ht)
{
if (ht->version)
   {
   free(ht->version);
   }

free(ht);
}

/*****************************************************************************/

struct HubVariable *NewHubVariable(struct HubHost *hh,char *type,char *scope,char *lval,void *rval,char rtype)

{ struct HubVariable *hp;
     
if ((hp = malloc(sizeof(struct HubVariable))) == NULL)
   {
   FatalError("Memory exhausted NewHubVariable");
   }

hp->hh = hh;
hp->rtype = rtype;
hp->scope = strdup(scope);
hp->rval = rval;
hp->lval = strdup(lval);
hp->dtype = strdup(type);
return hp;
}

/*****************************************************************************/

void DeleteHubVariable(struct HubVariable *hv)

{
free(hv->dtype);
free(hv->scope);
free(hv->lval);
DeleteRvalItem(hv->rval,hv->rtype);
free(hv);
}

/*****************************************************************************/

struct HubPromiseLog *NewHubPromiseLog(struct HubHost *hh,char *handle,time_t t)

{ struct HubPromiseLog *hp;
 
if ((hp = malloc(sizeof(struct HubPromiseLog))) == NULL)
   {
   FatalError("Memory exhausted NewHubPromiseLog");
   }

hp->hh = hh;
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

struct HubLastSeen *NewHubLastSeen(struct HubHost *hh,char io,char *kh,char *rhost,char *ip,double ago,double avg,double dev)

{ struct HubLastSeen *hp;
     
if ((hp = malloc(sizeof(struct HubLastSeen))) == NULL)
   {
   FatalError("Memory exhausted NewHubLastSeen");
   }

hp->hh = hh;
hp->io = io;  // '+' or '-'
hp->rhost = NewHubHost(kh,rhost,ip);
hp->hrsago;
hp->hrsavg;
hp->hrsdev;

return hp;
}

/*****************************************************************************/

void DeleteHubLastSeen(struct HubLastSeen *hp)
{
DeleteHubHost(hp->rhost);
free(hp); 
}

/*****************************************************************************/

struct HubMeter *NewHubMeter(struct HubHost *hh,char type,double kept,double repaired)

{ struct HubMeter *hp;
     
if ((hp = malloc(sizeof(struct HubMeter))) == NULL)
   {
   FatalError("Memory exhausted NewHubLastSeen");
   }

hp->hh = hh;
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

struct HubPerformance *NewHubPerformance(struct HubHost *hh,char *event,time_t t,double q,double e,double d)

{ struct HubPerformance *hp;
     
if ((hp = malloc(sizeof(struct HubPerformance))) == NULL)
   {
   FatalError("Memory exhausted NewHubPerformance");
   }

hp->hh = hh;
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

struct HubSetUid *NewHubSetUid(struct HubHost *hh,char *file)

{ struct HubSetUid *hp;
     
if ((hp = malloc(sizeof(struct HubSetUid))) == NULL)
   {
   FatalError("Memory exhausted NewHubSetuid");
   }

hp->hh = hh;
hp->path = strdup(file);
return hp;
}

/*****************************************************************************/

void DeleteHubSetUid(struct HubSetUid *hp)
{
free(hp->path);
free(hp); 
}

/*****************************************************************************/

struct HubFileChanges *NewHubFileChanges(struct HubHost *hh,char *file,time_t t)

{ struct HubFileChanges *hp;
     
if ((hp = (struct HubFileChanges *)malloc(sizeof(struct HubFileChanges))) == NULL)
   {
   FatalError("Memory exhausted NewHubFileChange");
   }

hp->hh = hh;
hp->path = strdup(file);
hp->t = t;
return hp;
}

/*****************************************************************************/

void DeleteHubFileChange(struct HubFileChanges *hp)
{
free(hp->path);
free(hp); 
}

/*****************************************************************************/

struct HubFileDiff *NewHubFileDiff(struct HubHost *hh,char *file,char *diff,time_t t)

{ struct HubFileDiff *hp;
     
if ((hp = malloc(sizeof(struct HubFileDiff))) == NULL)
   {
   FatalError("Memory exhausted NewHubFileDiff");
   }

hp->hh = hh;
hp->path = strdup(file);
hp->diff = strdup(diff);
hp->t = t;
return hp;
}

/*****************************************************************************/

void DeleteHubFileDiff(struct HubFileDiff *hp)
{
free(hp->path);
free(hp->diff);
free(hp); 
}

/*****************************************************************************/

struct HubPromiseCompliance *NewHubCompliance(struct HubHost *hh,char *handle,char status,double e,double d,time_t t)

{ struct HubPromiseCompliance *hp;
     
if ((hp = malloc(sizeof(struct HubPromiseCompliance))) == NULL)
   {
   FatalError("Memory exhausted NewHubPromiseCompliance");
   }

hp->hh = hh;
hp->handle = strdup(handle);
hp->status = status;
hp->e = e;
hp->d = d;
hp->t = t;
return hp;
};

/*****************************************************************************/

void DeleteHubPromiseCompliance(struct HubPromiseCompliance *hp)
{
free(hp->handle);
free(hp); 
}

/*****************************************************************************/

struct HubBundleSeen *NewHubBundleSeen(struct HubHost *hh,char *rname,double ago,double avg,double dev)

{ struct HubBundleSeen *hp;
     
if ((hp = malloc(sizeof(struct HubBundleSeen))) == NULL)
   {
   FatalError("Memory exhausted NewHubLastSeen");
   }

hp->hh = hh;
hp->bundle = strdup(rname);
hp->hrsago;
hp->hrsavg;
hp->hrsdev;

return hp;
}

/*****************************************************************************/

void DeleteHubBundleSeen(struct HubBundleSeen *hp)
{
free(hp->bundle);
free(hp); 
}

