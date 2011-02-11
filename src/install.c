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

void PrependPromiserList(struct PromiseIdent **list,char *s,struct Promise *pp)

{ struct PromiseIdent *ptr;

for (ptr = *list; ptr != NULL; ptr = ptr->next)
   {
   if (pp->lineno == ptr->lineno)
      {
      if (strcmp(pp->audit->filename,ptr->filename) == 0)
         {
         return;
         }
      }
   }

if ((ptr = malloc(sizeof(struct PromiseIdent))) == NULL)
   {
   FatalError("MemoryAlloc NewPromiseId\n");
   }

ptr->filename = strdup(pp->audit->filename);

if (ptr->classes)
   {
   ptr->classes = strdup(pp->classes);
   }
else
   {
   ptr->classes = strdup("any");
   }

ptr->lineno = pp->lineno;
ptr->handle = strdup(s);
ptr->next = *list;
*list = ptr;
}


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
 free(hq);
}

/*****************************************************************************/

struct HubHost *NewHubHost(char *keyhash,char *ipaddr,char *hostname)

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

struct HubHost *GetHubHostIn(struct Rlist *host_list, char *keyhash)
{
  struct Rlist *lp;
  struct HubHost *hh;

  if(host_list == NULL)
    {
    return NULL;
    }

  
   for (lp = host_list; lp != NULL; lp=lp->next)
      {
      hh = (struct HubHost *)lp->item;

      if(strcmp(hh->keyhash,keyhash) == 0)
	{
	return hh;
	}
      }

   return NULL;
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
free(hv->scope);
free(hv->lval);
free(hv->dtype);

switch(hv->rtype)
   {
   case CF_SCALAR:
       if (hv->rval)
          {
          free(hv->rval);
          }
       break;
       
   case CF_LIST:
       DeleteRlist(hv->rval);
       break;
   }

free(hv);
}

/*****************************************************************************/

struct HubPromiseLog *NewHubPromiseLog(struct HubHost *hh,char *policy,char *handle,char *cause,time_t t,char *commentId,char *oid)

{ struct HubPromiseLog *hp;
 
if ((hp = malloc(sizeof(struct HubPromiseLog))) == NULL)
   {
   FatalError("Memory exhausted NewHubPromiseLog");
   }

hp->hh = hh;
hp->policy = strdup(policy);
hp->handle = strdup(handle);
hp->cause = strdup(cause);
hp->comment_id = strdup(commentId);
hp->oid = strdup(oid);
hp->t = t;
return hp;
}

/*****************************************************************************/

void DeleteHubPromiseLog(struct HubPromiseLog *hp)

{
free(hp->policy);
free(hp->handle);
free(hp->cause);
free(hp->comment_id);
free(hp->oid);
free(hp);
}

/*****************************************************************************/

struct HubLastSeen *NewHubLastSeen(struct HubHost *hh,char io,char *kh,char *rhost,char *ip,double ago,double avg,double dev,time_t t)

{ struct HubLastSeen *hp;
     
if ((hp = malloc(sizeof(struct HubLastSeen))) == NULL)
   {
   FatalError("Memory exhausted NewHubLastSeen");
   }

hp->hh = hh;
hp->io = io;  // '+' or '-'
hp->rhost = NewHubHost(kh,rhost,ip);
hp->hrsago = ago;
hp->hrsavg = avg;
hp->hrsdev = dev;
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

struct HubValue *NewHubValue(struct HubHost *hh,char *day,double kept,double repaired,double notkept)

{ struct HubValue *hp;
     
if ((hp = malloc(sizeof(struct HubValue))) == NULL)
   {
   FatalError("Memory exhausted NewHubValue");
   }

hp->hh = hh;
hp->day = strdup(day);
hp->kept = kept;
hp->repaired = repaired;
hp->notkept = notkept;
return hp;
}

/*****************************************************************************/

void DeleteHubValue(struct HubValue *hp)
{
free(hp->day);
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

struct HubFileChanges *NewHubFileChanges(struct HubHost *hh,char *file,time_t t,char *handle)

{ struct HubFileChanges *hp;
     
if ((hp = (struct HubFileChanges *)malloc(sizeof(struct HubFileChanges))) == NULL)
   {
   FatalError("Memory exhausted NewHubFileChange");
   }

hp->hh = hh;
hp->path = strdup(file);
hp->t = t;
hp->handle = strdup(handle);
return hp;
}

/*****************************************************************************/

void DeleteHubFileChanges(struct HubFileChanges *hp)
{
free(hp->path);
free(hp->handle);
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

struct HubBundleSeen *NewHubBundleSeen(struct HubHost *hh,char *rname,double ago,double avg,double dev,time_t t)

{ struct HubBundleSeen *hp;
     
if ((hp = malloc(sizeof(struct HubBundleSeen))) == NULL)
   {
   FatalError("Memory exhausted NewHubLastSeen");
   }

hp->hh = hh;
hp->bundle = strdup(rname);
hp->hrsago = ago;
hp->hrsavg = avg;
hp->hrsdev = dev;
hp->t = t;
return hp;
}

/*****************************************************************************/

void DeleteHubBundleSeen(struct HubBundleSeen *hp)
{
free(hp->bundle);
free(hp); 
}

/*****************************************************************************/

struct HubPromise *NewHubPromise(char *bn,char *bt,char *ba,char *pt, char *pr, char *pe, char *cl, char *ha, char *co, char *fn, int lno, char **cons)

{ struct HubPromise *hp;
     
if ((hp = malloc(sizeof(struct HubPromise))) == NULL)
   {
   FatalError("Memory exhausted NewHubPromise");
   }

 hp->bundleName = strdup(bn);
 hp->bundleType = strdup(bt);
 hp->bundleArgs = strdup(ba);
 hp->promiseType = strdup(pt);
 hp->promiser = strdup(pr);
 hp->promisee = strdup(pe);
 hp->classContext = strdup(cl);
 hp->handle = strdup(ha);
 hp->comment = strdup(co);
 hp->file = strdup(fn);
 hp->lineNo = lno;
 hp->constraints = cons; // allocated by caller

 return hp;
}

/*****************************************************************************/

void DeleteHubPromise(struct HubPromise *hp)

{
free(hp->bundleName);
free(hp->bundleType);
free(hp->bundleArgs);
free(hp->promiseType);
free(hp->promiser);
free(hp->promisee);
free(hp->classContext);
free(hp->handle);
free(hp->comment);
free(hp->file);
hp->lineNo = -1;
FreeStringArray(hp->constraints);

free(hp);
hp = NULL;
}

/*****************************************************************************/

struct HubBody *NewHubBody(char *bodyType,char *bodyName,char *bodyArgs)

{ struct HubBody *hb;

if ((hb = malloc(sizeof(struct HubBody))) == NULL)
   {
   FatalError("Memory exhausted NewHubBody");
   }

hb->bodyName = strdup(bodyName);
hb->bodyType = strdup(bodyType);

if(EMPTY(bodyArgs))
   {
   hb->bodyArgs = NULL;
   }
else
   {
   hb->bodyArgs = strdup(bodyArgs);
   }


hb->attr = NULL;

return hb;
}

/*****************************************************************************/

void DeleteHubBody(struct HubBody *hb)

{
free(hb->bodyName);
free(hb->bodyType);

if (hb->bodyArgs)
   {
   free(hb->bodyArgs);
   }

DeleteHubBodyAttributes(hb->attr);

free(hb);
}

/*****************************************************************************/

struct HubBodyAttr *NewHubBodyAttr(struct HubBody *hb,char *lval,char *rval,char *classContext)
/* Appends to existing attribs */

{ struct HubBodyAttr *ha,*curr;

if ((ha = malloc(sizeof(struct HubBodyAttr))) == NULL)
   {
   FatalError("Memory exhausted NewHubBodyAttr");
   }

ha->lval = strdup(lval);
ha->rval = strdup(rval);
ha->classContext = strdup(classContext);
ha->next = NULL;

if (!hb->attr)
   {
   hb->attr = ha;
   }
else
   {
   curr = hb->attr;
   
   while(curr->next)
      {
      curr = curr->next;
      }
   
   curr->next = ha;
   }

return ha;
}

/*****************************************************************************/

void DeleteHubBodyAttributes(struct HubBodyAttr *ha)
{
if (!ha)
   {
   return;
   }

if (ha->next)
   {
   DeleteHubBodyAttributes(ha->next);
   }

free(ha->lval);
free(ha->rval);
free(ha->classContext);
free(ha);
}

/*commenting*/
/*****************************************************************************/

struct HubComment *NewHubComment(char *user,char *msg,time_t t)

{ struct HubComment *hc;

if ((hc = malloc(sizeof(struct HubComment))) == NULL)
   {
   FatalError("Memory exhausted HubComment");
   }

hc->user = strdup(user);
hc->msg = strdup(msg);
hc->t = t;
hc->next=NULL;
return hc;
}

/*****************************************************************************/

void DeleteHubComment(struct HubComment *hc)
{
free(hc->user);
free(hc->msg);
free(hc);
}

/*****************************************************************************/

struct HubCommentInfo *NewHubCommentInfo(struct HubHost *hh,char *cid,char *user,char *msg,time_t t)

{ struct HubCommentInfo *hci;

if ((hci = malloc(sizeof(struct HubCommentInfo))) == NULL)
   {
   FatalError("Memory exhausted HubCommentInfo");
   }

hci->hh = hh;
hci->cid = strdup(cid);
hci->comment = NewHubComment(user,msg,t);
return hci;
}

/*****************************************************************************/

void DeleteHubCommentInfo(struct HubCommentInfo *hci)

{ struct HubComment *hc;

  free(hci->cid);
for (hc = hci->comment; hc != NULL; hc=hc->next)
   {
   DeleteHubComment(hc);
   }

free(hci);
}

/*****************************************************************************/
/*                        REPORT SORTING FUNCTIONS                           */
/*****************************************************************************/


int SortPromiseLog(void *p1, void *p2)
/**
 * For SortRlist() - sorts promise logs descending on time.
 **/
{
  struct HubPromiseLog *pl1, *pl2;

  pl1 = (struct HubPromiseLog *)p1;
  pl2 = (struct HubPromiseLog *)p2;

  if(pl1->t > pl2->t)
    {
    return true;
    }
  else
    {
    return false;
    }
}

/*****************************************************************************/

int SortBusinessValue(void *p1, void *p2)
/**
 * For SortRlist() - sorts business value descending on time.
 **/
{
  struct HubValue *hv1, *hv2;
  time_t t1, t2;

  hv1 = (struct HubValue *)p1;
  hv2 = (struct HubValue *)p2;

  DateStrToTime(hv1->day,&t1);
  DateStrToTime(hv2->day,&t2);

  if(t1 > t2)
    {
    return true;
    }
  else
    {
    return false;
    }
}

/*****************************************************************************/

int SortTotalCompliance(void *p1, void *p2)
/**
 * For SortRlist() - sorts total compliance descending on time.
 **/
{
  struct HubTotalCompliance *hc1, *hc2;

  hc1 = (struct HubTotalCompliance *)p1;
  hc2 = (struct HubTotalCompliance *)p2;

  if(hc1->t > hc2->t)
    {
    return true;
    }
  else
    {
    return false;
    }
}

/*****************************************************************************/

int SortFileChanges(void *p1, void *p2)
/**
 * For SortRlist() - sorts file changes descending on time.
 **/
{
  struct HubFileChanges *hc1, *hc2;

  hc1 = (struct HubFileChanges *)p1;
  hc2 = (struct HubFileChanges *)p2;

  if(hc1->t > hc2->t)
    {
    return true;
    }
  else
    {
    return false;
    }
}

/*****************************************************************************/

int SortFileDiff(void *p1, void *p2)
/**
 * For SortRlist() - sorts file diffs descending on time.
 **/
{
  struct HubFileDiff *hd1, *hd2;

  hd1 = (struct HubFileDiff *)p1;
  hd2 = (struct HubFileDiff *)p2;

  if(hd1->t > hd2->t)
    {
    return true;
    }
  else
    {
    return false;
    }
}

/*****************************************************************************/

int SortLastSeen(void *p1, void *p2)
/**
 * For SortRlist() - sorts last seen descending on time.
 **/
{
  struct HubLastSeen *hl1, *hl2;

  hl1 = (struct HubLastSeen *)p1;
  hl2 = (struct HubLastSeen *)p2;

  if(hl1->t > hl2->t)
    {
    return true;
    }
  else
    {
    return false;
    }
}

/*****************************************************************************/

int SortPerformance(void *p1, void *p2)
/**
 * For SortRlist() - sorts performance descending on time.
 **/
{
  struct HubPerformance *hp1, *hp2;

  hp1 = (struct HubPerformance *)p1;
  hp2 = (struct HubPerformance *)p2;

  if(hp1->t > hp2->t)
    {
    return true;
    }
  else
    {
    return false;
    }
}

/*****************************************************************************/

int SortPromiseCompliance(void *p1, void *p2)
/**
 * For SortRlist() - sorts promise compliance descending on time.
 **/
{
  struct HubPromiseCompliance *hp1, *hp2;

  hp1 = (struct HubPromiseCompliance *)p1;
  hp2 = (struct HubPromiseCompliance *)p2;

  if(hp1->t > hp2->t)
    {
    return true;
    }
  else
    {
    return false;
    }
}


/*****************************************************************************/

int SortClasses(void *p1, void *p2)
/**
 * For SortRlist() - sorts classes on name.
 **/
{
  struct HubClass *hc1, *hc2;

  hc1 = (struct HubClass *)p1;
  hc2 = (struct HubClass *)p2;

  if(strcmp(hc1->class,hc2->class) < 0)
    {
    return true;
    }
  else
    {
    return false;
    }
}

/*****************************************************************************/

int SortSoftware(void *p1, void *p2)
/**
 * For SortRlist() - sorts software on name.
 **/
{
  struct HubSoftware *hs1, *hs2;

  hs1 = (struct HubSoftware *)p1;
  hs2 = (struct HubSoftware *)p2;

  if(strcmp(hs1->name,hs2->name) < 0)
    {
    return true;
    }
  else
    {
    return false;
    }
}

/*****************************************************************************/

int SortBundleSeen(void *p1, void *p2)
/**
 * For SortRlist() - sorts software on name.
 **/
{
  struct HubBundleSeen *hb1, *hb2;

  hb1 = (struct HubBundleSeen *)p1;
  hb2 = (struct HubBundleSeen *)p2;

  if(strcmp(hb1->bundle,hb2->bundle) < 0)
    {
    return true;
    }
  else
    {
    return false;
    }
}

