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

void PrependPromiserList(PromiseIdent **list,char *s,Promise *pp)

{ PromiseIdent *ptr;

 for (ptr = *list; ptr != NULL; ptr = ptr->next)
    {
    if (pp->offset.line == ptr->line_number)
       {
       if (strcmp(pp->audit->filename,ptr->filename) == 0)
          {
          return;
          }
       }
    }

 ptr = xmalloc(sizeof(PromiseIdent));
 ptr->filename = xstrdup(pp->audit->filename);

 if (ptr->classes)
    {
    ptr->classes = xstrdup(pp->classes);
    }
 else
    {
    ptr->classes = xstrdup("any");
    }

 ptr->line_number = pp->offset.line;
 ptr->handle = xstrdup(s);
 ptr->next = *list;
 *list = ptr;
}


/*****************************************************************************/

#ifdef HAVE_LIBMONGOC
HubQuery *NewHubQuery(Rlist *hosts,Rlist *records)

{ HubQuery *hq;

 hq = xmalloc(sizeof(HubQuery));

 hq->hosts = hosts;
 hq->records = records;
 hq->errid = ERRID_SUCCESS;
 return hq;
}

/*****************************************************************************/

HubQuery *NewHubQueryErrid(Rlist *hosts,Rlist *records, cfapi_errid errid)

{ HubQuery *hq;

 hq = xmalloc(sizeof(HubQuery));

 hq->hosts = hosts;
 hq->records = records;
 hq->errid = errid;
 return hq;
}

/*****************************************************************************/

void DeleteHubQuery(HubQuery *hq,void (*fnptr)())

{ Rlist *rp;

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

int CountRecords(HubQuery *hq)

{
 if(hq == NULL)
    {
    return 0;
    }
 
 return RlistLen(hq->records);
}
#endif

/*****************************************************************************/

HubHost *NewHubHost(char *hubkey, char *keyhash,char *ipaddr,char *hostname)

{ HubHost *hp;

 hp = xmalloc(sizeof(HubHost));

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


HubHost *CreateEmptyHubHost(void)
{
 return (HubHost *)xcalloc(1, sizeof(HubHost));
}


HubHost *UpdateHubHost(HubHost *hubHost, char *keyhash,char *ipaddr,char *hostname)
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

HubHost *GetHubHostIn(Rlist *host_list, char *keyhash)
{
 Rlist *lp;
 HubHost *hh;

 if(host_list == NULL)
    {
    return NULL;
    }

  
 for (lp = host_list; lp != NULL; lp=lp->next)
    {
    hh = (HubHost *)lp->item;

    if(strcmp(hh->keyhash,keyhash) == 0)
       {
       return hh;
       }
    }

 return NULL;
}


/*****************************************************************************/

void DeleteHubHost(HubHost *hp)
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

HubSoftware *NewHubSoftware(HubHost *hh,char *name,char *version,char *arch,time_t timeseen)

{ HubSoftware *hp;

hp = xmalloc(sizeof(HubSoftware));

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

void DeleteHubSoftware(HubSoftware *hs)
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

HubClass *NewHubClass(HubHost *hh,char *class,double p, double dev, time_t t)

{ HubClass *hp;

hp = xmalloc(sizeof(HubClass));

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

void DeleteHubClass(HubClass *hc)
{
 if (hc->class)
    {
    free(hc->class);
    }

 free(hc);
}

/*****************************************************************************/

HubClassSum *NewHubClassSum(HubHost *hh,char *class,int frequency)

{ HubClassSum *hc;

hc = xmalloc(sizeof(HubClassSum));

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

void DeleteHubClassSum(HubClassSum *hc)
{
 if (hc->class)
    {
    free(hc->class);
    }

 free(hc);
}

/*****************************************************************************/

HubTotalCompliance *NewHubTotalCompliance(HubHost *hh,time_t t,char *v,int k,int r,int n)

{ HubTotalCompliance *hp;
 
hp = xmalloc(sizeof(HubTotalCompliance));

 hp->hh = hh;
 hp->t = t;

 hp->version = xstrdup(v);
 hp->kept = k;
 hp->repaired = r;
 hp->notkept = n;
 return hp;
}

/*****************************************************************************/

void DeleteHubTotalCompliance(HubTotalCompliance *ht)
{
 if (ht->version)
    {
    free(ht->version);
    }

 free(ht);
}

/*****************************************************************************/

HubCacheTotalCompliance *NewHubCacheTotalCompliance(char *policy, int slot, int hostCount, int totalHostCount, double kept, double repaired, double notkept, time_t genTime)

{ HubCacheTotalCompliance *tc;
 
tc = xmalloc(sizeof(HubCacheTotalCompliance));

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

void DeleteHubCacheTotalCompliance(HubCacheTotalCompliance *tc)
{
 free(tc->policy);
 free(tc);
}

/*****************************************************************************/

HubVariable *NewHubVariable(HubHost *hh,char *type,char *scope,char *lval,void *rval,char rtype,time_t t)
// NOTE: rval must be allocated by caller
{ HubVariable *hp;
     
hp = xmalloc(sizeof(HubVariable));

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

void DeleteHubVariable(HubVariable *hv)

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

HubPromiseLog *NewHubPromiseLog(HubHost *hh,char *handle,char *cause,time_t t,char *noteId,char *oid)

{ HubPromiseLog *hp;
 
hp = xmalloc(sizeof(HubPromiseLog));

 hp->hh = hh;
 hp->handle = xstrdup(handle);
 hp->cause = xstrdup(cause);
 hp->nid = xstrdup(noteId);
 hp->oid = xstrdup(oid);
 hp->t = t;
 return hp;
}

/*****************************************************************************/

void DeleteHubPromiseLog(HubPromiseLog *hp)

{
 free(hp->handle);
 free(hp->cause);
 free(hp->nid);
 free(hp->oid);
 free(hp);
}

/*****************************************************************************/

HubPromiseSum *NewHubPromiseSum(HubHost *hh,char *handle,char *cause,int occurences, int hostOccurences)

{ HubPromiseSum *hs;
 
hs = xmalloc(sizeof(HubPromiseSum));

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

void DeleteHubPromiseSum(HubPromiseSum *hs)

{
 free(hs->handle);

 if(hs->cause)
    {
    free(hs->cause);
    }

 free(hs);
}

/*****************************************************************************/

HubLastSeen *NewHubLastSeen(HubHost *hh,char io,char *kh,char *rhost,char *ip,double ago,double avg,double dev,time_t t)

{ HubLastSeen *hp;
     
hp = xmalloc(sizeof(HubLastSeen));

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

void DeleteHubLastSeen(HubLastSeen *hp)
{
 DeleteHubHost(hp->rhost);
 free(hp); 
}

/*****************************************************************************/

HubMeter *NewHubMeter(HubHost *hh,char type,double kept,double repaired)

{ HubMeter *hp;
     
 hp = xmalloc(sizeof(HubMeter));

 hp->hh = hh;
 hp->type = type;
 hp->kept = kept;
 hp->repaired = repaired;
 hp->notkept = 100.0 - kept - repaired;
 return hp;
}

/*****************************************************************************/

void DeleteHubMeter(HubMeter *hp)
{
 free(hp);
}

/*****************************************************************************/

HubValue *NewHubValue(HubHost *hh,char *day,double kept,double repaired,double notkept,char *noteid,char *handle)

{ HubValue *hp;
     
 hp = xmalloc(sizeof(HubValue));

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

void DeleteHubValue(HubValue *hp)
{
 free(hp->day);
 free(hp->nid);
 free(hp->handle);
 free(hp);
}

/*****************************************************************************/

HubPerformance *NewHubPerformance(HubHost *hh,char *event,time_t t,double q,double e,double d,char *noteid, char *handle)

{ HubPerformance *hp;
     
 hp = xmalloc(sizeof(HubPerformance));

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

void DeleteHubPerformance(HubPerformance *hp)
{
 free(hp->event);
 free(hp->nid);
 free(hp->handle);
 free(hp); 
}

/*****************************************************************************/

HubSetUid *NewHubSetUid(HubHost *hh,char *file)

{ HubSetUid *hp;
     
 hp = xmalloc(sizeof(HubSetUid));

 hp->hh = hh;
 hp->path = xstrdup(file);
 return hp;
}

/*****************************************************************************/

void DeleteHubSetUid(HubSetUid *hp)
{
 free(hp->path);
 free(hp); 
}

/*****************************************************************************/

HubFileChanges *NewHubFileChanges(HubHost *hh,char *file,time_t t,char *noteid,char *handle)

{ HubFileChanges *hp;
     
 hp = xmalloc(sizeof(HubFileChanges));

 hp->hh = hh;
 hp->path = xstrdup(file);
 hp->t = t;
 hp->handle = xstrdup(handle);
 hp->nid = xstrdup(noteid);
 return hp;
}

/*****************************************************************************/

void DeleteHubFileChanges(HubFileChanges *hp)
{
 free(hp->path);
 free(hp->handle);
 free(hp->nid);
 free(hp); 
}

/*****************************************************************************/

HubFileDiff *NewHubFileDiff(HubHost *hh,char *file,char *diff,time_t t)

{ HubFileDiff *hp;
     
 hp = xmalloc(sizeof(HubFileDiff));

 hp->hh = hh;
 hp->path = xstrdup(file);
 hp->diff = xstrdup(diff);
 hp->t = t;
 return hp;
}

/*****************************************************************************/

void DeleteHubFileDiff(HubFileDiff *hp)
{
 free(hp->path);
 free(hp->diff);
 free(hp); 
}

/*****************************************************************************/

HubPromiseCompliance *NewHubCompliance(HubHost *hh, char *handle, PromiseState status, double e, double d, time_t t)

{ HubPromiseCompliance *hp;
     
 hp = xmalloc(sizeof(HubPromiseCompliance));

 hp->hh = hh;
 hp->handle = xstrdup(handle);
 hp->status = status;
 hp->e = e;
 hp->d = d;
 hp->t = t;
 return hp;
};

/*****************************************************************************/

void DeleteHubPromiseCompliance(HubPromiseCompliance *hp)
{
 free(hp->handle);
 free(hp); 
}

/*****************************************************************************/

HubBundleSeen *NewHubBundleSeen(HubHost *hh,char *rname,double ago,double avg,double dev,time_t t,char *noteid)

{ HubBundleSeen *hp;
     
 hp = xmalloc(sizeof(HubBundleSeen));

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

void DeleteHubBundleSeen(HubBundleSeen *hp)
{
 free(hp->bundle);
 free(hp->nid);
 free(hp); 
}

/*****************************************************************************/

HubPromise *NewHubPromise(char *bn,char *bt,char *ba,char *pt, char *pr, char *pe, char *cl, char *ha, char *co, char *fn, int lno, char **cons)

{ HubPromise *hp;

hp = xcalloc(1, sizeof(HubPromise));

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

void DeleteHubPromise(HubPromise *hp)

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

HubBody *NewHubBody(char *bodyType,char *bodyName,char *bodyArgs)

{ HubBody *hb;

hb = xmalloc(sizeof(HubBody));

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

void DeleteHubBody(HubBody *hb)

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

HubBodyAttr *NewHubBodyAttr(HubBody *hb,char *lval,char *rval,char *classContext)
/* Appends to existing attribs */

{ HubBodyAttr *ha,*curr;

 ha = xmalloc(sizeof(HubBodyAttr));

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

void DeleteHubBodyAttributes(HubBodyAttr *ha)

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

/*****************************************************************************/

HubNote *NewHubNote(char *user,char *msg,time_t t)

{ HubNote *hc;

hc = xmalloc(sizeof(HubNote));

hc->user = xstrdup(user);
hc->msg = xstrdup(msg);
hc->t = t;
hc->next=NULL;
return hc;
}

/*****************************************************************************/

void DeleteHubNote(HubNote *hc)
{
free(hc->user);
free(hc->msg);
free(hc);
}

/*****************************************************************************/

HubNoteInfo *NewHubNoteInfo(HubHost *hh,char *nid,char *user,char *msg,time_t t,char *reportData, int reportType)

{ HubNoteInfo *hci;

hci = xmalloc(sizeof(HubNoteInfo));

hci->hh = hh;
hci->nid = xstrdup(nid);
hci->note = NewHubNote(user,msg,t);
hci->report = xstrdup(reportData);
hci->reportType = reportType;
return hci;
}

/*****************************************************************************/

void DeleteHubNoteInfo(HubNoteInfo *hci)

{ HubNote *hc;

free(hci->nid);
free(hci->report);
for (hc = hci->note; hc != NULL; hc=hc->next)
   {
   DeleteHubNote(hc);
   }

free(hci);
}

/*****************************************************************************/

HubVital *PrependHubVital(HubVital **first, char *id, char *units, char *description)

{ HubVital *hv;

hv = xmalloc(sizeof(HubVital));

hv->id = xstrdup(id);
hv->units = xstrdup(units);
hv->description = xstrdup(description);

hv->next = *first;
*first = hv;

return hv;
}

/*****************************************************************************/

void DeleteHubVital(HubVital *hv)

{
 HubVital *curr, *next;

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

HubUserRBAC *NewHubUserRBAC(char *userName, char *classRxInclude, char *classRxExclude, char *bundleRxInclude)
{
 HubUserRBAC *rbac = xmalloc(sizeof(HubUserRBAC));

 rbac->userName = SafeStringDuplicate(userName);
 rbac->classRxInclude = SafeStringDuplicate(classRxInclude);
 rbac->classRxExclude = SafeStringDuplicate(classRxExclude);
 rbac->bundleRxInclude = SafeStringDuplicate(bundleRxInclude);
 
 return rbac;
}

/*****************************************************************************/

void DeleteHubUserRBAC(HubUserRBAC *userRbac)
{
 free(userRbac->userName);
 free(userRbac->classRxInclude);
 free(userRbac->classRxExclude);
 free(userRbac->bundleRxInclude);
 free(userRbac);
}

/*****************************************************************************/

HubRole *NewHubRole(char *name, char *description, char *classRxInclude, char *classRxExclude, char *bundleRxInclude)
{
 HubRole *role = xmalloc(sizeof(HubRole));

 role->name = xstrdup(name);
 role->description = SafeStringDuplicate(description);
 role->classRxInclude = SafeStringDuplicate(classRxInclude);
 role->classRxExclude = SafeStringDuplicate(classRxExclude);
 role->bundleRxInclude = SafeStringDuplicate(bundleRxInclude);
 
 return role;
}

/*****************************************************************************/

void DeleteHubRole(HubRole *role)
{
 free(role->name);
 free(role->description);
 free(role->classRxInclude);
 free(role->classRxExclude);
 free(role->bundleRxInclude);
 free(role);
}

/*****************************************************************************/

#ifdef HAVE_LIBMONGOC
HostClassFilter *NewHostClassFilter(char *classRxInclude, char *classRxExclude)
{
 HostClassFilter *filter = xmalloc(sizeof(HostClassFilter));

 filter->classRxInclude = SafeStringDuplicate(classRxInclude);
 filter->classRxExclude = SafeStringDuplicate(classRxExclude);
 
 return filter;
}

/*****************************************************************************/

void DeleteHostClassFilter(HostClassFilter *filter)
{
 free(filter->classRxInclude);
 free(filter->classRxExclude);
 free(filter);
}
#endif

/*****************************************************************************/
/*                        REPORT SORTING FUNCTIONS                           */
/*****************************************************************************/

int SortPromiseLog(void *p1, void *p2)
/**
 * For SortRlist() - sorts promise logs descending on time.
 **/
{ HubPromiseLog *pl1, *pl2;

pl1 = (HubPromiseLog *)p1;
pl2 = (HubPromiseLog *)p2;

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
{ HubValue *hv1, *hv2;
  time_t t1, t2;

hv1 = (HubValue *)p1;
hv2 = (HubValue *)p2;

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
 HubTotalCompliance *hc1, *hc2;

 hc1 = (HubTotalCompliance *)p1;
 hc2 = (HubTotalCompliance *)p2;

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
 HubFileChanges *hc1, *hc2;

 hc1 = (HubFileChanges *)p1;
 hc2 = (HubFileChanges *)p2;

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
 HubFileDiff *hd1, *hd2;

 hd1 = (HubFileDiff *)p1;
 hd2 = (HubFileDiff *)p2;

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
 HubLastSeen *hl1, *hl2;

 hl1 = (HubLastSeen *)p1;
 hl2 = (HubLastSeen *)p2;

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
 HubPerformance *hp1, *hp2;

 hp1 = (HubPerformance *)p1;
 hp2 = (HubPerformance *)p2;

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
 HubPromiseCompliance *hp1, *hp2;

 hp1 = (HubPromiseCompliance *)p1;
 hp2 = (HubPromiseCompliance *)p2;

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
 HubClass *hc1, *hc2;

 hc1 = (HubClass *)p1;
 hc2 = (HubClass *)p2;

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
 HubClassSum *hc1, *hc2;

 hc1 = (HubClassSum *)p1;
 hc2 = (HubClassSum *)p2;

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
 HubSoftware *hs1, *hs2;

 hs1 = (HubSoftware *)p1;
 hs2 = (HubSoftware *)p2;

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
 HubBundleSeen *hb1, *hb2;

 hb1 = (HubBundleSeen *)p1;
 hb2 = (HubBundleSeen *)p2;

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
 HubPromise *hp1, *hp2;

 hp1 = (HubPromise *)p1;
 hp2 = (HubPromise *)p2;

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
 HubPromise *hp1, *hp2;

 hp1 = (HubPromise *)p1;
 hp2 = (HubPromise *)p2;

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

HubCacheTotalCompliance *GetHubCacheTotalComplianceSlot(Rlist *records, int slot)
{
 Rlist *rp;
 HubCacheTotalCompliance *tc;

 for(rp = records; rp != NULL; rp = rp->next)
    {
    tc = (HubCacheTotalCompliance *)rp->item;

    if(tc->slot == slot)
       {
       return tc;
       }
    }
  
 return NULL;
}

/*****************************************************************************/

int PageRecords(Rlist **records_p, PageInfo *page,void (*fnptr)())
/**
 * Unlinks and unallocates records not within the desired page.
 * Writes the total number of records given as input.
 **/
{
 Rlist *rp;
 Rlist *prevStartEl, *startEl, *endEl;
 int startIdx, endIdx;
 int count = 0;

 if(page->resultsPerPage <= 0 || page->pageNum <= 0)
    {
    page->totalResultCount = RlistLen(*records_p);
    return true;
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

void CountMarginRecordsVars(Rlist **records_p, PageInfo *page,int *start_count,int *end_count)
/**
 * Counts the total records for a scope
 * if it spans multiple pages
 **/
{ int startIdx=0, endIdx=0,count=0, head_count=0, tail_count=0;
  HubVariable *hv,*hv2,*hv3;
  Rlist *rp,*rp2,*rp3;
  int last_scope=false;
  char lscope[CF_MAXVARSIZE];

  rp = *records_p;

  startIdx = page->resultsPerPage*(page->pageNum - 1);
  endIdx = (page->resultsPerPage*page->pageNum) - 1;

  lscope[0] = '\0';

  for (rp = *records_p; (rp != NULL && !last_scope); rp=rp->next)
    {
      hv = (HubVariable *)rp->item;
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
	       hv3 = (HubVariable *)rp3->item;

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
	       hv2 = (HubVariable *)rp2->item;

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
