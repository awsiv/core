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

static void DateStrToTime(const char *inStr, time_t *t);

/*****************************************************************************/

void PrependPromiserList(struct PromiseIdent **list,char *s,struct Promise *pp)

{ struct PromiseIdent *ptr;

 for (ptr = *list; ptr != NULL; ptr = ptr->next)
    {
    if (pp->line_number == ptr->line_number)
       {
       if (strcmp(pp->audit->filename,ptr->filename) == 0)
          {
          return;
          }
       }
    }

 ptr = xmalloc(sizeof(struct PromiseIdent));
 ptr->filename = xstrdup(pp->audit->filename);

 if (ptr->classes)
    {
    ptr->classes = xstrdup(pp->classes);
    }
 else
    {
    ptr->classes = xstrdup("any");
    }

 ptr->line_number = pp->line_number;
 ptr->handle = xstrdup(s);
 ptr->next = *list;
 *list = ptr;
}


/*****************************************************************************/

struct HubQuery *NewHubQuery(struct Rlist *hosts,struct Rlist *records)

{ struct HubQuery *hq;

 hq = xmalloc(sizeof(struct HubQuery));

 hq->hosts = hosts;
 hq->records = records;
 hq->errid = ERRID_SUCCESS;
 return hq;
}

/*****************************************************************************/

void DeleteHubQuery(struct HubQuery *hq,void (*fnptr)())

{ struct Rlist *rp;

 if(!hq)
    {
    return;
    }

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

int CountRecords(struct HubQuery *hq)

{
 if(hq == NULL)
    {
    return 0;
    }
 
 return RlistLen(hq->records);
}

/*****************************************************************************/

struct HubHost *NewHubHost(char *hubkey, char *keyhash,char *ipaddr,char *hostname)

{ struct HubHost *hp;

 hp = xmalloc(sizeof(struct HubHost));

 if (hubkey)
    {
    hp->hubkey = xstrdup(hubkey);
    }
 else
    {
    hp->hubkey = NULL;
    }
 
 if (keyhash)
    {
    hp->keyhash = xstrdup(keyhash);
    }
 else
    {
    hp->keyhash = NULL;
    }

 if (ipaddr)
    {
    hp->ipaddr = xstrdup(ipaddr);
    }
 else
    {
    hp->ipaddr = NULL;
    }

 if (hostname)
    {
    hp->hostname = xstrdup(hostname);
    }
 else
    {
    hp->hostname = NULL;
    }

 return hp;
}


struct HubHost *CreateEmptyHubHost(void)
{
 return (struct HubHost *)xcalloc(1, sizeof(struct HubHost));
}


struct HubHost *UpdateHubHost(struct HubHost *hubHost, char *keyhash,char *ipaddr,char *hostname)
{
 free(hubHost->keyhash);
 free(hubHost->ipaddr);
 free(hubHost->hostname);

 hubHost->keyhash = xstrdup(keyhash);
 hubHost->ipaddr = xstrdup(ipaddr);
 hubHost->hostname = xstrdup(hostname);

 return hubHost;
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
 if(!hp)
    {
    return;
    }
 
  if (hp->hubkey)
    {
    free(hp->hubkey);
    }

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

struct HubSoftware *NewHubSoftware(struct HubHost *hh,char *name,char *version,char *arch,time_t timeseen)

{ struct HubSoftware *hp;

hp = xmalloc(sizeof(struct HubSoftware));

 hp->hh = hh;

 if (name)
    {
    hp->name = xstrdup(name);
    }
 else
    {
    hp->name = NULL;
    }

 if (version)
    {
    hp->version = xstrdup(version);
    }
 else
    {
    hp->version = NULL;
    }

 if (arch)
    {
    hp->arch = xstrdup(arch);
    }
 else
    {
    hp->arch = NULL;
    }

 hp->t = timeseen;

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

hp = xmalloc(sizeof(struct HubClass));

 hp->hh = hh;

 if (class)
    {
    hp->class = xstrdup(class);
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

struct HubClassSum *NewHubClassSum(struct HubHost *hh,char *class,int frequency)

{ struct HubClassSum *hc;

hc = xmalloc(sizeof(struct HubClassSum));

 hc->hh = hh;

 if (class)
    {
    hc->class = xstrdup(class);
    }
 else
    {
    hc->class = NULL;
    }

 hc->frequency = frequency;
 
 return hc;
}

/*****************************************************************************/

void DeleteHubClassSum(struct HubClassSum *hc)
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
 
hp = xmalloc(sizeof(struct HubTotalCompliance));

 hp->hh = hh;
 hp->t = t;

 hp->version = xstrdup(v);
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

struct HubCacheTotalCompliance *NewHubCacheTotalCompliance(char *policy, int slot, int hostCount, int totalHostCount, double kept, double repaired, double notkept, time_t genTime)

{ struct HubCacheTotalCompliance *tc;
 
tc = xmalloc(sizeof(struct HubCacheTotalCompliance));

 tc->policy = xstrdup(policy);
 tc->slot = slot;
 tc->hostCount = hostCount;
 tc->totalHostCount = totalHostCount;
 tc->kept = kept;
 tc->repaired = repaired;
 tc->notkept = notkept;
 tc->genTime = genTime;

 return tc;
}

/*****************************************************************************/

void DeleteHubCacheTotalCompliance(struct HubCacheTotalCompliance *tc)
{
 free(tc->policy);
 free(tc);
}

/*****************************************************************************/

struct HubVariable *NewHubVariable(struct HubHost *hh,char *type,char *scope,char *lval,void *rval,char rtype,time_t t)
// NOTE: rval must be allocated by caller
{ struct HubVariable *hp;
     
hp = xmalloc(sizeof(struct HubVariable));

 hp->hh = hh;
 hp->rtype = rtype;
 hp->rval = rval;
 hp->scope = xstrdup(scope);
 hp->lval = xstrdup(lval);
 hp->dtype = xstrdup(type);
 hp->t = t;
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

struct HubPromiseLog *NewHubPromiseLog(struct HubHost *hh,char *handle,char *cause,time_t t,char *noteId,char *oid)

{ struct HubPromiseLog *hp;
 
hp = xmalloc(sizeof(struct HubPromiseLog));

 hp->hh = hh;
 hp->handle = xstrdup(handle);
 hp->cause = xstrdup(cause);
 hp->nid = xstrdup(noteId);
 hp->oid = xstrdup(oid);
 hp->t = t;
 return hp;
}

/*****************************************************************************/

void DeleteHubPromiseLog(struct HubPromiseLog *hp)

{
 free(hp->handle);
 free(hp->cause);
 free(hp->nid);
 free(hp->oid);
 free(hp);
}

/*****************************************************************************/

struct HubPromiseSum *NewHubPromiseSum(struct HubHost *hh,char *handle,char *cause,int occurences, int hostOccurences)

{ struct HubPromiseSum *hs;
 
hs = xmalloc(sizeof(struct HubPromiseSum));

 hs->hh = hh;
 hs->handle = xstrdup(handle);

 if(cause)
    {
    hs->cause = xstrdup(cause);
    }
 else
    {
    hs->cause = NULL;
    }

 hs->occurences = occurences;
 hs->hostOccurences = hostOccurences;

 return hs;
}

/*****************************************************************************/

void DeleteHubPromiseSum(struct HubPromiseSum *hs)

{
 free(hs->handle);

 if(hs->cause)
    {
    free(hs->cause);
    }

 free(hs);
}

/*****************************************************************************/

struct HubLastSeen *NewHubLastSeen(struct HubHost *hh,char io,char *kh,char *rhost,char *ip,double ago,double avg,double dev,time_t t)

{ struct HubLastSeen *hp;
     
hp = xmalloc(sizeof(struct HubLastSeen));

 hp->hh = hh;
 hp->io = io;  // '+' or '-'
 hp->rhost = NewHubHost(NULL,kh,ip,rhost);
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
     
 hp = xmalloc(sizeof(struct HubMeter));

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

struct HubValue *NewHubValue(struct HubHost *hh,char *day,double kept,double repaired,double notkept,char *noteid,char *handle)

{ struct HubValue *hp;
     
 hp = xmalloc(sizeof(struct HubValue));

 hp->hh = hh;
 hp->day = xstrdup(day);
 hp->kept = kept;
 hp->repaired = repaired;
 hp->notkept = notkept;
 hp->nid = xstrdup(noteid);
 hp->handle = xstrdup(handle);
 return hp;
}

/*****************************************************************************/

void DeleteHubValue(struct HubValue *hp)
{
 free(hp->day);
 free(hp->nid);
 free(hp->handle);
 free(hp);
}

/*****************************************************************************/

struct HubPerformance *NewHubPerformance(struct HubHost *hh,char *event,time_t t,double q,double e,double d,char *noteid, char *handle)

{ struct HubPerformance *hp;
     
 hp = xmalloc(sizeof(struct HubPerformance));

 hp->hh = hh;
 hp->event = xstrdup(event);
 hp->q = q;
 hp->e = e;
 hp->d = d;
 hp->t = t;
 hp->nid = xstrdup(noteid);
 hp->handle = xstrdup(handle);
 return hp;
}

/*****************************************************************************/

void DeleteHubPerformance(struct HubPerformance *hp)
{
 free(hp->event);
 free(hp->nid);
 free(hp->handle);
 free(hp); 
}

/*****************************************************************************/

struct HubSetUid *NewHubSetUid(struct HubHost *hh,char *file)

{ struct HubSetUid *hp;
     
 hp = xmalloc(sizeof(struct HubSetUid));

 hp->hh = hh;
 hp->path = xstrdup(file);
 return hp;
}

/*****************************************************************************/

void DeleteHubSetUid(struct HubSetUid *hp)
{
 free(hp->path);
 free(hp); 
}

/*****************************************************************************/

struct HubFileChanges *NewHubFileChanges(struct HubHost *hh,char *file,time_t t,char *noteid,char *handle)

{ struct HubFileChanges *hp;
     
 hp = xmalloc(sizeof(struct HubFileChanges));

 hp->hh = hh;
 hp->path = xstrdup(file);
 hp->t = t;
 hp->handle = xstrdup(handle);
 hp->nid = xstrdup(noteid);
 return hp;
}

/*****************************************************************************/

void DeleteHubFileChanges(struct HubFileChanges *hp)
{
 free(hp->path);
 free(hp->handle);
 free(hp->nid);
 free(hp); 
}

/*****************************************************************************/

struct HubFileDiff *NewHubFileDiff(struct HubHost *hh,char *file,char *diff,time_t t)

{ struct HubFileDiff *hp;
     
 hp = xmalloc(sizeof(struct HubFileDiff));

 hp->hh = hh;
 hp->path = xstrdup(file);
 hp->diff = xstrdup(diff);
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
     
 hp = xmalloc(sizeof(struct HubPromiseCompliance));

 hp->hh = hh;
 hp->handle = xstrdup(handle);
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

struct HubBundleSeen *NewHubBundleSeen(struct HubHost *hh,char *rname,double ago,double avg,double dev,time_t t,char *noteid)

{ struct HubBundleSeen *hp;
     
 hp = xmalloc(sizeof(struct HubBundleSeen));

 hp->hh = hh;
 hp->bundle = xstrdup(rname);
 hp->hrsago = ago;
 hp->hrsavg = avg;
 hp->hrsdev = dev;
 hp->t = t;
 hp->nid = xstrdup(noteid);
 return hp;
}

/*****************************************************************************/

void DeleteHubBundleSeen(struct HubBundleSeen *hp)
{
 free(hp->bundle);
 free(hp->nid);
 free(hp); 
}

/*****************************************************************************/

struct HubPromise *NewHubPromise(char *bn,char *bt,char *ba,char *pt, char *pr, char *pe, char *cl, char *ha, char *co, char *fn, int lno, char **cons)

{ struct HubPromise *hp;

hp = xcalloc(1, sizeof(struct HubPromise));

if (bn)
   {
   hp->bundleName = xstrdup(bn);
   }
if (bt)
   {
   hp->bundleType = xstrdup(bt);
   }
if (ba)
   {
   hp->bundleArgs = xstrdup(ba);
   }
if (pt)
   {
   hp->promiseType = xstrdup(pt);
   }
if (pr)
   {
   hp->promiser = xstrdup(pr);
   }
if (pe)
   {
   hp->promisee = xstrdup(pe);
   }
if (cl)
   {
   hp->classContext = xstrdup(cl);
   }
if (ha)
   {
   hp->handle = xstrdup(ha);
   }
if (co)
   {
   hp->comment = xstrdup(co);
   }
if (fn)
   {
   hp->file = xstrdup(fn);
   }
hp->lineNo = lno;
hp->constraints = cons; // allocated by caller
hp->popularity = 0;  // optional

return hp;
}

/*****************************************************************************/

void DeleteHubPromise(struct HubPromise *hp)

{

 if(hp->bundleName)
    {
    free(hp->bundleName);
    }
 if(hp->bundleType)
    {
    free(hp->bundleType);
    }
 if(hp->bundleArgs)
    {
    free(hp->bundleArgs);
    }
 if(hp->promiseType)
    {
    free(hp->promiseType);
    }
 if(hp->promiser)
    {
    free(hp->promiser);
    }
 if(hp->promisee)
    {
    free(hp->promisee);
    }
 if(hp->classContext)
    {
    free(hp->classContext);
    }
 if(hp->handle)
    {
    free(hp->handle);
    }
 if(hp->comment)
    {
    free(hp->comment);
    }
 if(hp->file)
    {
    free(hp->file);
    }
 
hp->lineNo = -1;

if (hp->constraints)
   {
   FreeStringArray(hp->constraints);
   }

free(hp);
hp = NULL;
}

/*****************************************************************************/

struct HubBody *NewHubBody(char *bodyType,char *bodyName,char *bodyArgs)

{ struct HubBody *hb;

hb = xmalloc(sizeof(struct HubBody));

hb->bodyName = xstrdup(bodyName);
hb->bodyType = xstrdup(bodyType);

if (EMPTY(bodyArgs))
   {
   hb->bodyArgs = NULL;
   }
else
   {
   hb->bodyArgs = xstrdup(bodyArgs);
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

 ha = xmalloc(sizeof(struct HubBodyAttr));

 ha->lval = xstrdup(lval);
 ha->rval = xstrdup(rval);
 ha->classContext = xstrdup(classContext);
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

struct HubNote *NewHubNote(char *user,char *msg,time_t t)

{ struct HubNote *hc;

hc = xmalloc(sizeof(struct HubNote));

hc->user = xstrdup(user);
hc->msg = xstrdup(msg);
hc->t = t;
hc->next=NULL;
return hc;
}

/*****************************************************************************/

void DeleteHubNote(struct HubNote *hc)
{
free(hc->user);
free(hc->msg);
free(hc);
}

/*****************************************************************************/

struct HubNoteInfo *NewHubNoteInfo(struct HubHost *hh,char *nid,char *user,char *msg,time_t t,char *reportData, int reportType)

{ struct HubNoteInfo *hci;

hci = xmalloc(sizeof(struct HubNoteInfo));

hci->hh = hh;
hci->nid = xstrdup(nid);
hci->note = NewHubNote(user,msg,t);
hci->report = xstrdup(reportData);
hci->reportType = reportType;
return hci;
}

/*****************************************************************************/

void DeleteHubNoteInfo(struct HubNoteInfo *hci)

{ struct HubNote *hc;

free(hci->nid);
free(hci->report);
for (hc = hci->note; hc != NULL; hc=hc->next)
   {
   DeleteHubNote(hc);
   }

free(hci);
}

/*****************************************************************************/

struct HubVital *PrependHubVital(struct HubVital **first, char *id, char *units, char *description)

{ struct HubVital *hv;

hv = xmalloc(sizeof(struct HubVital));

hv->id = xstrdup(id);
hv->units = xstrdup(units);
hv->description = xstrdup(description);

hv->next = *first;
*first = hv;

return hv;
}

/*****************************************************************************/

void DeleteHubVital(struct HubVital *hv)

{
 struct HubVital *curr, *next;

 for(curr = hv; curr != NULL; curr = next)
    {
    free(curr->id);
    free(curr->units);
    free(curr->description);
    
    next = curr->next;
    free(curr);
    }
}

/*****************************************************************************/
/*                        REPORT SORTING FUNCTIONS                           */
/*****************************************************************************/

int SortPromiseLog(void *p1, void *p2)
/**
 * For SortRlist() - sorts promise logs descending on time.
 **/
{ struct HubPromiseLog *pl1, *pl2;

pl1 = (struct HubPromiseLog *)p1;
pl2 = (struct HubPromiseLog *)p2;

if (pl1->t > pl2->t)
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
{ struct HubValue *hv1, *hv2;
  time_t t1, t2;

hv1 = (struct HubValue *)p1;
hv2 = (struct HubValue *)p2;

DateStrToTime(hv1->day,&t1);
DateStrToTime(hv2->day,&t2);

if (t1 > t2)
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

int SortClassSum(void *p1, void *p2)
/**
 * For SortRlist() - sorts classes on frequency.
 **/
{
 struct HubClassSum *hc1, *hc2;

 hc1 = (struct HubClassSum *)p1;
 hc2 = (struct HubClassSum *)p2;

 if(hc1->frequency > hc2->frequency)
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
 * For SortRlist() - sorts bundles on name.
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

/*****************************************************************************/

int SortPromisePopularAscending(void *p1, void *p2)
/**
 * For SortRlist() - sorts least popular promises first.
 **/
{
 struct HubPromise *hp1, *hp2;

 hp1 = (struct HubPromise *)p1;
 hp2 = (struct HubPromise *)p2;

 if(hp1->popularity < hp2->popularity)
    {
    return true;
    }
 else
    {
    return false;
    }
}

/*****************************************************************************/

int SortPromisePopularDescending(void *p1, void *p2)
/**
 * For SortRlist() - sorts most popular promises first.
 **/
{
 struct HubPromise *hp1, *hp2;

 hp1 = (struct HubPromise *)p1;
 hp2 = (struct HubPromise *)p2;

 if(hp1->popularity > hp2->popularity)
    {
    return true;
    }
 else
    {
    return false;
    }
}

/*****************************************************************************/

struct HubCacheTotalCompliance *GetHubCacheTotalComplianceSlot(struct Rlist *records, int slot)
{
 struct Rlist *rp;
 struct HubCacheTotalCompliance *tc;

 for(rp = records; rp != NULL; rp = rp->next)
    {
    tc = (struct HubCacheTotalCompliance *)rp->item;

    if(tc->slot == slot)
       {
       return tc;
       }
    }
  
 return NULL;
}

/*****************************************************************************/

int PageRecords(struct Rlist **records_p, struct PageInfo *page,void (*fnptr)())
/**
 * Unlinks and unallocates records not within the desired page.
 * Writes the total number of records given as input.
 **/
{
 struct Rlist *rp;
 struct Rlist *prevStartEl, *startEl, *endEl;
 int startIdx, endIdx;
 int count = 0;

 if(page->resultsPerPage <= 0 || page->pageNum <= 0)
    {
    CfOut(cf_error, "", "!! Wrong parameters to PageRecords() - %d,%d",
	  page->resultsPerPage, page->pageNum);
    return false;
    }
  
 startIdx = page->resultsPerPage*(page->pageNum - 1);
 endIdx = (page->resultsPerPage*page->pageNum) - 1;

 prevStartEl = NULL;  // points to startEl
 startEl = *records_p;
 endEl = NULL;

 for(rp = *records_p; rp != NULL; rp = rp->next)
    {
    if(count == endIdx)
       {
       endEl = rp;
       }

    count++;

    if(count == startIdx && rp->next)  // next is startEl
       {
       prevStartEl = rp;
       startEl = rp->next;
       }
      
    }

 if(count <= startIdx ) // the page number is greater than the last one
    {
    for(rp = *records_p; rp != NULL; rp = rp->next)
       {
       (*fnptr)(rp->item);
       rp->item = NULL;
       }
    
    DeleteRlist(*records_p);
    *records_p = NULL;    
    page->totalResultCount = count;
    return false; 
    }
 // now unlink uneccessary elements at start and end
 if(prevStartEl)
    {
    prevStartEl->next = NULL;

    for(rp = *records_p; rp != NULL; rp = rp->next)
       {
       (*fnptr)(rp->item);
       rp->item = NULL;
       }
      
    DeleteRlist(*records_p);
    *records_p = NULL;  // restored below
    }
  

 if(endEl)
    {
    for(rp = endEl->next; rp != NULL; rp = rp->next)
       {
       (*fnptr)(rp->item);
       rp->item = NULL;
       }
      
    DeleteRlist(endEl->next);
    endEl->next = NULL;
    }
  
  
 *records_p = startEl;
 page->totalResultCount = count;
  
 return true;
}

/*****************************************************************************/

void CountMarginRecordsVars(struct Rlist **records_p, struct PageInfo *page,int *start_count,int *end_count)
/**
 * Counts the total records for a scope
 * if it spans multiple pages
 **/
{ int startIdx=0, endIdx=0,count=0, head_count=0, tail_count=0;
  struct HubVariable *hv,*hv2,*hv3;
  struct Rlist *rp,*rp2,*rp3;
  int last_scope=false;
  char lscope[CF_MAXVARSIZE];

  rp = *records_p;

  startIdx = page->resultsPerPage*(page->pageNum - 1);
  endIdx = (page->resultsPerPage*page->pageNum) - 1;

  lscope[0] = '\0';

  for (rp = *records_p; (rp != NULL && !last_scope); rp=rp->next)
    {
      hv = (struct HubVariable *)rp->item;
      if (strcmp(lscope,hv->scope) != 0)
	{
	  strcpy(lscope,hv->scope);
	  tail_count=0;
	}

      if(count == startIdx && rp->next)
	{
	  head_count=tail_count;
	  for (rp3 = rp; rp3 != NULL; rp3=rp3->next)
	     {
	       hv3 = (struct HubVariable *)rp3->item;

	       if (strcmp(lscope,hv3->scope) != 0)
		 {
		   break;
		 }
	       head_count++;
	     }
	 }

       if(count == endIdx && rp->next)
	 {
	   last_scope=true;
	   for (rp2 = rp; rp2 != NULL; rp2=rp2->next)
	     {
	       hv2 = (struct HubVariable *)rp2->item;

	       if (strcmp(lscope,hv2->scope) != 0)
		 {
		   break;
		 }
	       tail_count++;
	     }
	 }
       count++;
       if(!last_scope)
	 {
	   tail_count++;
	 }
    }
  
  *start_count = head_count;
  *end_count = tail_count;
}

/*************************************************************/

/*****************************************************************************/

static void DateStrToTime(const char *inStr, time_t *t)
/**
 * Takes a time str as "30 Sep 2010" and returns a time_t struct in
 * that day.
 */
{
  char monthStr[CF_SMALLBUF] = {0};
  int day = 0, month = 0, year = 0;
  struct tm tmTime = {0};
  time_t tTime = 0;
  
  sscanf(inStr,"%d %3s %d",&day,monthStr,&year);

  month = MonthLen2Int(monthStr,3);

  if(month == -1)
    {
    CfOut(cf_error, "", "!! Could not convert month to int in DateStrToTime()");
    *t = 0;
    return;
    }

  tmTime.tm_mday = day;
  tmTime.tm_mon = month - 1;
  tmTime.tm_year = year - 1900;
  tmTime.tm_isdst = -1;

  tTime = mktime(&tmTime);

  if(tTime == -1)
    {
    CfOut(cf_error, "mktime", "!! Could not convert \"%s\" from string to time_t", inStr);
    *t = 0;
    }
  else
    {
    *t = tTime;
    }
}
