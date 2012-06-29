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
#include "string_lib.h"
#include <assert.h>

static void DateStrToTime(const char *inStr, time_t *t);

HubQuery *NewHubQuery(Rlist *hosts, Rlist *records)
{
    HubQuery *hq;

    hq = xmalloc(sizeof(HubQuery));

    hq->hosts = hosts;
    hq->records = records;
    hq->errid = ERRID_SUCCESS;
    return hq;
}

/*****************************************************************************/

HubQuery *NewHubQueryErrid(Rlist *hosts, Rlist *records, cfapi_errid errid)
{
    HubQuery *hq;

    hq = xmalloc(sizeof(HubQuery));

    hq->hosts = hosts;
    hq->records = records;
    hq->errid = errid;
    return hq;
}

/*****************************************************************************/

void DeleteHubQuery(HubQuery *hq, void (*fnptr) ())
{
    Rlist *rp;

    if (!hq)
    {
        return;
    }

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        DeleteHubHost(rp->item);
        rp->item = NULL;
    }

    DeleteRlist(hq->hosts);

    if (fnptr)
    {
        for (rp = hq->records; rp != NULL; rp = rp->next)
        {
            (*fnptr) (rp->item);
            rp->item = NULL;
        }
    }

    DeleteRlist(hq->records);
    free(hq);
}

/*****************************************************************************/

int CountRecords(HubQuery *hq)
{
    if (hq == NULL)
    {
        return 0;
    }

    return RlistLen(hq->records);
}

/*****************************************************************************/

void *HubQueryGetFirstRecord(HubQuery *hq)
{
    if (hq->records == NULL)
    {
        return NULL;
    }

    return hq->records->item;
}

HubHost *NewHubHost(const char *hubkey, const char *keyhash, const char *ipaddr, const char *hostname)
{
    HubHost *hp;

    hp = xmalloc(sizeof(HubHost));

    hp->hubkey = SafeStringDuplicate(hubkey);
    hp->keyhash = SafeStringDuplicate(keyhash);
    hp->ipaddr = SafeStringDuplicate(ipaddr);
    hp->hostname = SafeStringDuplicate(hostname);
    hp->colour = HOST_COLOUR_BLUE;

    return hp;
}

HubHost *CreateEmptyHubHost(void)
{
    return (HubHost *) xcalloc(1, sizeof(HubHost));
}

HubHost *UpdateHubHost(HubHost *hubHost, char *keyhash, char *ipaddr, char *hostname)
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

    if (host_list == NULL)
    {
        return NULL;
    }

    for (lp = host_list; lp != NULL; lp = lp->next)
    {
        hh = (HubHost *) lp->item;

        if (strcmp(hh->keyhash, keyhash) == 0)
        {
            return hh;
        }
    }

    return NULL;
}

bool HubHostEqual(const void *hubhost_a, const void *hubhost_b)
{
    assert(hubhost_a);
    assert(hubhost_b);

    const HubHost *a = (const HubHost *)hubhost_a;
    const HubHost *b = (const HubHost *)hubhost_b;

    return StringSafeEqual(a->keyhash, b->keyhash);
}

unsigned int HubHostHash(const void *hubhost)
{
    assert(hubhost);

    const HubHost *hh = (const HubHost *)hubhost;
    assert(hh->keyhash);
    // TODO: find a real hash function
    return SafeStringLength(hh->keyhash);
}

/*****************************************************************************/

void DeleteHubHost(HubHost *hp)
{
    if (!hp)
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

HubSoftware *NewHubSoftware(HubHost *hh, char *name, char *version, char *arch, time_t timeseen)
{
    HubSoftware *hp;

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

HubClass *NewHubClass(HubHost *hh, char *class, double p, double dev, time_t t)
{
    HubClass *hp;

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

HubClassSum *NewHubClassSum(HubHost *hh, char *class, int frequency)
{
    HubClassSum *hc;

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

HubTotalCompliance *NewHubTotalCompliance(HubHost *hh, time_t t, char *v, int k, int r, int n)
{
    HubTotalCompliance *hp;

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

HubHostComplianceShifts *NewHubHostComplianceShifts(const char *hostkey)
{
    HubHostComplianceShifts *record = xcalloc(sizeof(HubHostComplianceShifts), 1);

    record->hostkey = SafeStringDuplicate(hostkey);

    return record;
}

void DeleteHubHostComplianceShifts(HubHostComplianceShifts *record)
{
    if (record)
    {
        free(record->hostkey);
    }
    free(record);
}

/*****************************************************************************/

HubCacheTotalCompliance *NewHubCacheTotalCompliance(char *policy, int slot, int hostCount, int totalHostCount,
                                                    double kept, double repaired, double notkept, time_t genTime)
{
    HubCacheTotalCompliance *tc;

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

HubVariable *NewHubVariable(HubHost *hh, char *type, char *scope, char *lval, Rval rval, time_t t)
// NOTE: rval must be allocated by caller
{
    HubVariable *hp;

    hp = xmalloc(sizeof(HubVariable));

    hp->hh = hh;
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
    DeleteRvalItem(hv->rval);
    free(hv);
}

/*****************************************************************************/

HubPromiseLog *NewHubPromiseLog(HubHost *hh, char *handle, char *cause, time_t t)
{
    HubPromiseLog *hp;

    hp = xmalloc(sizeof(HubPromiseLog));

    hp->hh = hh;
    hp->handle = xstrdup(handle);
    hp->cause = xstrdup(cause);
    hp->t = t;
    return hp;
}

unsigned int HubPromiseLogHash(const void *record)
{
    const HubPromiseLog *record_a = (const HubPromiseLog *)record;

    unsigned int hash = 0;
    if (record_a->handle)
    {
        hash += OatHash(record_a->handle);
    }
    if (record_a->cause)
    {
        hash += OatHash(record_a->cause);
    }
    return hash;
}

bool HubPromiseLogEqual(const void *a, const void *b)
{
    const HubPromiseLog *record_a = (const HubPromiseLog *)a;
    const HubPromiseLog *record_b = (const HubPromiseLog *)b;

    return StringSafeEqual(record_a->handle, record_b->handle) &&
            StringSafeEqual(record_a->cause, record_b->cause);
}

/*****************************************************************************/

void DeleteHubPromiseLog(HubPromiseLog *hp)
{
    free(hp->handle);
    free(hp->cause);
    free(hp);
}

/*****************************************************************************/

HubPromiseSum *NewHubPromiseSum(HubHost *hh, char *handle, char *cause, int occurences, int hostOccurences)
{
    HubPromiseSum *hs;

    hs = xmalloc(sizeof(HubPromiseSum));

    hs->hh = hh;
    hs->handle = xstrdup(handle);

    if (cause)
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

// NOTE: non-standard compare function returns true (1) if a < b
int HubPromiseSumCompare(void *hps_a, void *hps_b)
{
    const HubPromiseSum *a = (const HubPromiseSum *)hps_a;
    const HubPromiseSum *b = (const HubPromiseSum *)hps_b;

    if (!(a && b))
    {
        return 0;
    }

    return a->occurences > b->occurences;
}


void DeleteHubPromiseSum(HubPromiseSum *hs)
{
    free(hs->handle);

    if (hs->cause)
    {
        free(hs->cause);
    }

    free(hs);
}

/*****************************************************************************/

HubLastSeen *NewHubLastSeen(HubHost *hh, LastSeenDirection direction, char *kh, char *rhost,
                            char *ip, double ago, double avg, double dev, time_t t)
{
    HubLastSeen *hp;

    hp = xmalloc(sizeof(HubLastSeen));

    hp->hh = hh;
    hp->direction = direction;
    hp->rhost = NewHubHost(NULL, kh, ip, rhost);
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

HubMeter *NewHubMeter(HubHost *hh, char type, double kept, double repaired)
{
    HubMeter *hp;

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

HubValue *NewHubValue(HubHost *hh, char *day, double kept, double repaired, double notkept, char *handle)
{
    HubValue *hp;

    hp = xmalloc(sizeof(HubValue));

    hp->hh = hh;
    hp->day = xstrdup(day);
    hp->kept = kept;
    hp->repaired = repaired;
    hp->notkept = notkept;
    hp->handle = xstrdup(handle);
    return hp;
}

/*****************************************************************************/

void DeleteHubValue(HubValue *hp)
{
    free(hp->day);
    free(hp->handle);
    free(hp);
}

/*****************************************************************************/

HubPerformance *NewHubPerformance(HubHost *hh, char *event, time_t t, double q, double e, double d, char *handle)
{
    HubPerformance *hp;

    hp = xmalloc(sizeof(HubPerformance));

    hp->hh = hh;
    hp->event = xstrdup(event);
    hp->q = q;
    hp->e = e;
    hp->d = d;
    hp->t = t;
    hp->handle = xstrdup(handle);
    return hp;
}

/*****************************************************************************/

void DeleteHubPerformance(HubPerformance *hp)
{
    free(hp->event);
    free(hp->handle);
    free(hp);
}

/*****************************************************************************/

HubSetUid *NewHubSetUid(HubHost *hh, char *file)
{
    HubSetUid *hp;

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

HubFileChanges *NewHubFileChanges(HubHost *hh, char *file, time_t t, char *handle)
{
    HubFileChanges *hp;

    hp = xmalloc(sizeof(HubFileChanges));

    hp->hh = hh;
    hp->path = xstrdup(file);
    hp->t = t;
    hp->handle = xstrdup(handle);
    hp->diff_record = NULL;

    return hp;
}

/*****************************************************************************/

void DeleteHubFileChanges(HubFileChanges *hp)
{
    free(hp->path);
    free(hp->handle);
    free(hp);
}

/*****************************************************************************/

HubFileDiff *NewHubFileDiff(HubHost *hh, char *file, char *diff, time_t t)
{
    HubFileDiff *hp;

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
{
    HubPromiseCompliance *hp;

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

HubBundleSeen *NewHubBundleSeen(HubHost *hh, char *rname, double comp, double avg, double dev, time_t t)
{
    HubBundleSeen *hp;

    hp = xmalloc(sizeof(HubBundleSeen));

    hp->hh = hh;
    hp->bundle = xstrdup(rname);
    hp->bundlecomp = comp;
    hp->bundleavg = avg;
    hp->bundledev = dev;
    hp->t = t;
    return hp;
}

/*****************************************************************************/

void DeleteHubBundleSeen(HubBundleSeen *hp)
{
    free(hp->bundle);
    free(hp);
}

/*****************************************************************************/

HubPromise *NewHubPromise(char *bn, char *bt, Rlist *ba, char *pt, char *pr, char *pe, char *cl, char *ha, char *co,
                          char *fn, int lno, Rlist *cons)
{
    HubPromise *hp = xmalloc(sizeof(HubPromise));

    hp->bundleName = SafeStringDuplicate(bn);
    hp->bundleType = SafeStringDuplicate(bt);
    hp->promiseType = SafeStringDuplicate(pt);
    hp->promiser = SafeStringDuplicate(pr);
    hp->promisee = SafeStringDuplicate(pe);
    hp->classContext = SafeStringDuplicate(cl);
    hp->handle = SafeStringDuplicate(ha);
    hp->comment = SafeStringDuplicate(co);
    hp->file = SafeStringDuplicate(fn);
    hp->lineNo = lno;
    hp->popularity = 0;

    // NOTE: these are allocated by caller - taking ownership
    hp->bundleArgs = ba;
    hp->constraints = cons;

    return hp;
}

/*****************************************************************************/

void DeleteHubPromise(HubPromise * hp)
{
    free(hp->bundleName);
    free(hp->bundleType);
    free(hp->promiseType);
    free(hp->promiser);
    free(hp->promisee);
    free(hp->classContext);
    free(hp->handle);
    free(hp->comment);
    free(hp->file);
    hp->lineNo = -1;

    DeleteRlist(hp->bundleArgs);
    DeleteRlist(hp->constraints);

    free(hp);
}

/*****************************************************************************/

HubPromiseBundle *NewHubPromiseBundle(char *bundleName, char *bundleType, Rlist *bundleArgs)
{
    HubPromiseBundle *hb = xmalloc(sizeof(HubPromiseBundle));

    hb->bundleName = SafeStringDuplicate(bundleName);
    hb->bundleType = SafeStringDuplicate(bundleType);

    // NOTE: allocated by caller - taking ownership
    hb->bundleArgs = bundleArgs;

    return hb;
}

/*****************************************************************************/

void DeleteHubPromiseBundle(HubPromiseBundle * hb)
{
    free(hb->bundleName);
    free(hb->bundleType);

    DeleteRlist(hb->bundleArgs);

    free(hb);
}

/*****************************************************************************/

HubBody *NewHubBody(char *bodyType, char *bodyName, char *bodyArgs)
{
    HubBody *hb;

    hb = xmalloc(sizeof(HubBody));

    hb->bodyName = xstrdup(bodyName);
    hb->bodyType = xstrdup(bodyType);

    if (NULL_OR_EMPTY(bodyArgs))
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

void DeleteHubBody(HubBody * hb)
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

HubBodyAttr *NewHubBodyAttr(HubBody * hb, char *lval, char *rval, char *classContext)
/* Appends to existing attribs */
{
    HubBodyAttr *ha, *curr;

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

        while (curr->next)
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

HubNote *NewHubNote(char *user, char *msg, time_t t)
{
    HubNote *hc;

    hc = xmalloc(sizeof(HubNote));

    hc->user = xstrdup(user);
    hc->msg = xstrdup(msg);
    hc->t = t;
    hc->next = NULL;
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

HubNoteInfo *NewHubNoteInfo(HubHost *hh, char *nid, char *user, char *msg, time_t t, char *reportData, int reportType)
{
    HubNoteInfo *hci;

    hci = xmalloc(sizeof(HubNoteInfo));

    hci->hh = hh;
    hci->nid = xstrdup(nid);
    hci->note = NewHubNote(user, msg, t);
    hci->report = xstrdup(reportData);
    hci->reportType = reportType;
    return hci;
}

/*****************************************************************************/

void DeleteHubNoteInfo(HubNoteInfo *hci)
{
    HubNote *hc;

    free(hci->nid);
    free(hci->report);
    for (hc = hci->note; hc != NULL; hc = hc->next)
    {
        DeleteHubNote(hc);
    }

    free(hci);
}

/*****************************************************************************/

HubVital *PrependHubVital(HubVital **first, char *id, char *units, char *description)
{
    HubVital *hv;

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

    for (curr = hv; curr != NULL; curr = next)
    {
        free(curr->id);
        free(curr->units);
        free(curr->description);

        next = curr->next;
        free(curr);
    }
}

/*****************************************************************************/

HubUserRBAC *NewHubUserRBAC(const char *userName, const char *classRxInclude,
                            const char *classRxExclude, const char *bundleRxInclude,
                            const char *bundleRxExclude)
{
    HubUserRBAC *rbac = xmalloc(sizeof(HubUserRBAC));

    rbac->userName = SafeStringDuplicate(userName);
    rbac->classRxInclude = SafeStringDuplicate(classRxInclude);
    rbac->classRxExclude = SafeStringDuplicate(classRxExclude);
    rbac->bundleRxInclude = SafeStringDuplicate(bundleRxInclude);
    rbac->bundleRxExclude = SafeStringDuplicate(bundleRxExclude);

    return rbac;
}

/*****************************************************************************/

void DeleteHubUserRBAC(HubUserRBAC *userRbac)
{
    free(userRbac->userName);
    free(userRbac->classRxInclude);
    free(userRbac->classRxExclude);
    free(userRbac->bundleRxInclude);
    free(userRbac->bundleRxExclude);
    free(userRbac);
}

/*****************************************************************************/

HubRole *NewHubRole(char *name, char *description, char *classRxInclude, char *classRxExclude, char *bundleRxInclude,
                    char *bundleRxExclude)
{
    HubRole *role = xmalloc(sizeof(HubRole));

    role->name = xstrdup(name);
    role->description = SafeStringDuplicate(description);
    role->classRxInclude = SafeStringDuplicate(classRxInclude);
    role->classRxExclude = SafeStringDuplicate(classRxExclude);
    role->bundleRxInclude = SafeStringDuplicate(bundleRxInclude);
    role->bundleRxExclude = SafeStringDuplicate(bundleRxExclude);

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
    free(role->bundleRxExclude);
    free(role);
}

HostClassFilter *NewHostClassFilter(const char *classRxInclude, const char *classRxExclude)
{
    Rlist *classRxIncludes = NULL;
    Rlist *classRxExcludes = NULL;

    if (classRxInclude)
    {
        AppendRlist(&classRxIncludes, classRxInclude, CF_SCALAR);
    }

    if (classRxExclude)
    {
        AppendRlist(&classRxExcludes, classRxExclude, CF_SCALAR);
    }

    return NewHostClassFilterLists(classRxIncludes, classRxExcludes);
}

HostClassFilter *NewHostClassFilterLists(Rlist *classRxIncludes, Rlist *classRxExcludes)
{
    HostClassFilter *filter = xmalloc(sizeof(HostClassFilter));

    filter->classRxIncludes = classRxIncludes;
    filter->classRxExcludes = classRxExcludes;

    return filter;
}

void HostClassFilterAddClasses(HostClassFilter *filter, const char *classRxInclude, const char *classRxExclude)
{
    if (classRxInclude)
    {
        AppendRlist(&(filter->classRxIncludes), classRxInclude, CF_SCALAR);
    }

    if (classRxExclude)
    {
        AppendRlist(&(filter->classRxExcludes), classRxExclude, CF_SCALAR);
    }
}

void HostClassFilterAddClassLists(HostClassFilter *filter, const Rlist *classRxIncludes, const Rlist *classRxExcludes)
{
    for (const Rlist *rp = classRxIncludes; rp; rp = rp->next)
    {
        HostClassFilterAddClasses(filter, ScalarValue(rp), NULL);
    }

    for (const Rlist *rp = classRxExcludes; rp; rp = rp->next)
    {
        HostClassFilterAddClasses(filter, NULL, ScalarValue(rp));
    }
}

static bool HostClassFilterMatchInclude(const HostClassFilter *filter, const char *classRx)
{
    bool include = false;
    for (const Rlist *rp = filter->classRxIncludes; rp; rp = rp->next)
    {
        if (StringMatch(ScalarValue(rp), classRx))
        {
            include = true;
        }
        else
        {
            return false;
        }
    }
    return include;
}

static bool HostClassFilterMatchExclude(const HostClassFilter *filter, const char *classRx)
{
    bool exclude = false;
    for (const Rlist *rp = filter->classRxExcludes; rp; rp = rp->next)
    {
        if (StringMatch(ScalarValue(rp), classRx))
        {
            exclude = true;
        }
        else
        {
            return false;
        }
    }
    return exclude;
}

bool HostClassFilterMatch(const HostClassFilter *filter, const char *class_rx)
{
    char class_rx_anchored[CF_MAXVARSIZE] = { 0 };
    AnchorRegex(class_rx, class_rx_anchored, sizeof(class_rx_anchored));

    return HostClassFilterMatchInclude(filter, class_rx_anchored) &&
          !HostClassFilterMatchExclude(filter, class_rx_anchored);
}

/*****************************************************************************/

void DeleteHostClassFilter(HostClassFilter *filter)
{
    DeleteRlist(filter->classRxIncludes);
    DeleteRlist(filter->classRxExcludes);

    free(filter);
}

/*****************************************************************************/

PromiseFilter *NewPromiseFilter(void)
{
    PromiseFilter *filter = xcalloc(1, sizeof(PromiseFilter));

    return filter;
}

void PromiseFilterAddPromiseBody(PromiseFilter *filter, const char *handleInclude, const char *promiserInclude)
{
    assert(!filter->handleInclude && "PromiseFilterAddPromiseBody: handle is already set");
    assert(!filter->promiserInclude && "PromiseFilterAddPromiseBody: promiser is already set");

    filter->handleInclude = SafeStringDuplicate(handleInclude);
    filter->promiserInclude = SafeStringDuplicate(promiserInclude);
}

void PromiseFilterAddPromiseBodyRx(PromiseFilter *filter, const char *handleRxInclude, const char *promiserRxInclude)
{
    assert(!filter->handleRxInclude && "PromiseFilterAddPromiseBodyRx: handle is already set");
    assert(!filter->promiserRxInclude && "PromiseFilterAddPromiseBodyRx: promiser is already set");

    filter->handleRxInclude = SafeStringDuplicate(handleRxInclude);
    filter->promiserRxInclude = SafeStringDuplicate(promiserRxInclude);
}

void PromiseFilterAddPromiseType(PromiseFilter *filter, const char *promiseTypeInclude)
{
    assert(!filter->promiseTypeInclude && "PromiseFilterAddPromiseType: promiseTypeInclude is already set");

    filter->promiseTypeInclude = SafeStringDuplicate(promiseTypeInclude);
}

void PromiseFilterAddPromiseTypeRx(PromiseFilter *filter, const char *promiseTypeRxInclude)
{
    assert(!filter->promiseTypeRxInclude && "PromiseFilterAddPromiseTypeRx: promiseTypeRxInclude is already set");

    filter->promiseTypeRxInclude = SafeStringDuplicate(promiseTypeRxInclude);
}

void PromiseFilterAddBundleType(PromiseFilter *filter, const char *bundleTypeInclude)
{
    assert(!filter->bundleTypeInclude && "PromiseFilterAddBundleType: bundleTypeInclude is already set");

    filter->bundleTypeInclude = SafeStringDuplicate(bundleTypeInclude);
}

void PromiseFilterAddBundleTypeRx(PromiseFilter *filter, const char *bundleTypeRxInclude)
{
    assert(!filter->bundleTypeRxInclude && "PromiseFilterAddBundleTypeRx: bundleTypeRxInclude is already set");

    filter->bundleTypeRxInclude = SafeStringDuplicate(bundleTypeRxInclude);
}

void PromiseFilterAddBundles(PromiseFilter *filter, const char *bundleInclude, const char *bundleExclude)
{
    if (bundleInclude)
    {
        AppendRlist(&(filter->bundleIncludes), bundleInclude, CF_SCALAR);
    }

    if (bundleExclude)
    {
        AppendRlist(&(filter->bundleExcludes), bundleExclude, CF_SCALAR);
    }
}

void PromiseFilterAddBundlesRx(PromiseFilter *filter, const char *bundleRxInclude, const char *bundleRxExclude)
{
    if (bundleRxInclude)
    {
        AppendRlist(&(filter->bundleRxIncludes), bundleRxInclude, CF_SCALAR);
    }

    if (bundleRxExclude)
    {
        AppendRlist(&(filter->bundleRxExcludes), bundleRxExclude, CF_SCALAR);
    }
}

/*****************************************************************************/

void DeletePromiseFilter(PromiseFilter *filter)
{
    free(filter->handleInclude);
    free(filter->handleRxInclude);
    free(filter->promiserInclude);
    free(filter->promiserRxInclude);
    free(filter->promiseTypeInclude);
    free(filter->promiseTypeRxInclude);
    free(filter->bundleTypeRxInclude);
    free(filter->bundleTypeInclude);
    free(filter->promiseTypeRxInclude);
    DeleteRlist(filter->bundleIncludes);
    DeleteRlist(filter->bundleRxIncludes);
    DeleteRlist(filter->bundleExcludes);
    DeleteRlist(filter->bundleRxExcludes);

    free(filter);
}

/*****************************************************************************/
/*                        REPORT SORTING FUNCTIONS                           */
/*****************************************************************************/

void HubQuerySortPromiseBundles(HubQuery *hqPromiseBundles)
{
    assert(hqPromiseBundles);
    if (hqPromiseBundles->records)
    {
        hqPromiseBundles->records = SortRlist(hqPromiseBundles->records, SortPromiseBundle);
    }
}

int SortPromiseLog(void *p1, void *p2)
/**
 * For SortRlist() - sorts promise logs descending on time.
 **/
{
    HubPromiseLog *pl1, *pl2;

    pl1 = (HubPromiseLog *) p1;
    pl2 = (HubPromiseLog *) p2;

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
{
    HubValue *hv1, *hv2;
    time_t t1, t2;

    hv1 = (HubValue *) p1;
    hv2 = (HubValue *) p2;

    DateStrToTime(hv1->day, &t1);
    DateStrToTime(hv2->day, &t2);

    if (t1 > t2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int SortTotalCompliance(void *p1, void *p2)
/**
 * For SortRlist() - sorts total compliance descending on time.
 **/
{
    HubTotalCompliance *hc1, *hc2;

    hc1 = (HubTotalCompliance *) p1;
    hc2 = (HubTotalCompliance *) p2;

    if (hc1->t > hc2->t)
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

    hc1 = (HubFileChanges *) p1;
    hc2 = (HubFileChanges *) p2;

    if (hc1->t > hc2->t)
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

    hd1 = (HubFileDiff *) p1;
    hd2 = (HubFileDiff *) p2;

    if (hd1->t > hd2->t)
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

    hl1 = (HubLastSeen *) p1;
    hl2 = (HubLastSeen *) p2;

    if (hl1->t > hl2->t)
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

    hp1 = (HubPerformance *) p1;
    hp2 = (HubPerformance *) p2;

    if (hp1->t > hp2->t)
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

    hp1 = (HubPromiseCompliance *) p1;
    hp2 = (HubPromiseCompliance *) p2;

    if (hp1->t > hp2->t)
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

    hc1 = (HubClass *) p1;
    hc2 = (HubClass *) p2;

    if (strcmp(hc1->class, hc2->class) < 0)
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

    hc1 = (HubClassSum *) p1;
    hc2 = (HubClassSum *) p2;

    if (hc1->frequency > hc2->frequency)
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

    hs1 = (HubSoftware *) p1;
    hs2 = (HubSoftware *) p2;

    if (strcmp(hs1->name, hs2->name) < 0)
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

    hb1 = (HubBundleSeen *) p1;
    hb2 = (HubBundleSeen *) p2;

    if (strcmp(hb1->bundle, hb2->bundle) < 0)
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

    hp1 = (HubPromise *) p1;
    hp2 = (HubPromise *) p2;

    if (hp1->popularity < hp2->popularity)
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

    hp1 = (HubPromise *) p1;
    hp2 = (HubPromise *) p2;

    if (hp1->popularity > hp2->popularity)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*****************************************************************************/

int SortPromiseBundle(void *p1, void *p2)
/**
 * For SortRlist() - sorts bundles from policy definition by type.
 **/
{
    HubPromiseBundle *hb1 = (HubPromiseBundle *) p1;
    HubPromiseBundle *hb2 = (HubPromiseBundle *) p2;

    if (strcmp(hb1->bundleType, hb2->bundleType) < 0)
    {
        return true;
    }
    else if (strcmp(hb1->bundleType, hb2->bundleType) > 0)
    {
        return false;
    }

    // same bundle type - sort on bundle name
    if (strcmp(hb1->bundleName, hb2->bundleName) < 0)
    {
        return true;
    }
    else if (strcmp(hb1->bundleName, hb2->bundleName) > 0)
    {
        return false;
    }

    return true;                // identical
}

/*****************************************************************************/

HubCacheTotalCompliance *GetHubCacheTotalComplianceSlot(Rlist *records, int slot)
{
    Rlist *rp;
    HubCacheTotalCompliance *tc;

    for (rp = records; rp != NULL; rp = rp->next)
    {
        tc = (HubCacheTotalCompliance *) rp->item;

        if (tc->slot == slot)
        {
            return tc;
        }
    }

    return NULL;
}

/*****************************************************************************/

int PageRecords(Rlist **records_p, PageInfo *page, void (*fnptr) ())
/**
 * Unlinks and unallocates records not within the desired page.
 * Writes the total number of records given as input.
 **/
{
    Rlist *rp;
    Rlist *prevStartEl, *startEl, *endEl;
    int startIdx, endIdx;
    int count = 0;

    if (page->resultsPerPage <= 0 || page->pageNum <= 0)
    {
        page->totalResultCount = RlistLen(*records_p);
        return true;
    }

    startIdx = page->resultsPerPage * (page->pageNum - 1);
    endIdx = (page->resultsPerPage * page->pageNum) - 1;
    prevStartEl = NULL;         // points to startEl
    startEl = *records_p;
    endEl = NULL;

    for (rp = *records_p; rp != NULL; rp = rp->next)
    {
        if (count == endIdx)
        {
            endEl = rp;
        }

        count++;

        if (count == startIdx && rp->next)      // next is startEl
        {
            prevStartEl = rp;
            startEl = rp->next;
        }

    }

    if (count <= startIdx)      // the page number is greater than the last one
    {
        for (rp = *records_p; rp != NULL; rp = rp->next)
        {
            (*fnptr) (rp->item);
            rp->item = NULL;
        }

        DeleteRlist(*records_p);
        *records_p = NULL;
        page->totalResultCount = count;
        return false;
    }
    // now unlink uneccessary elements at start and end
    if (prevStartEl)
    {
        prevStartEl->next = NULL;

        for (rp = *records_p; rp != NULL; rp = rp->next)
        {
            (*fnptr) (rp->item);
            rp->item = NULL;
        }

        DeleteRlist(*records_p);
        *records_p = NULL;      // restored below
    }

    if (endEl)
    {
        for (rp = endEl->next; rp != NULL; rp = rp->next)
        {
            (*fnptr) (rp->item);
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

void CountMarginRecordsVars(Rlist **records_p, PageInfo *page, int *start_count, int *end_count)
/**
 * Counts the total records for a scope
 * if it spans multiple pages
 **/
{
    int startIdx = 0, endIdx = 0, count = 0, head_count = 0, tail_count = 0;
    HubVariable *hv, *hv2, *hv3;
    Rlist *rp, *rp2, *rp3;
    int last_scope = false;
    char lscope[CF_MAXVARSIZE];

    rp = *records_p;

    startIdx = page->resultsPerPage * (page->pageNum - 1);
    endIdx = (page->resultsPerPage * page->pageNum) - 1;

    lscope[0] = '\0';

    for (rp = *records_p; (rp != NULL && !last_scope); rp = rp->next)
    {
        hv = (HubVariable *) rp->item;
        if (strcmp(lscope, hv->scope) != 0)
        {
            strcpy(lscope, hv->scope);
            tail_count = 0;
        }

        if (count == startIdx && rp->next)
        {
            head_count = tail_count;
            for (rp3 = rp; rp3 != NULL; rp3 = rp3->next)
            {
                hv3 = (HubVariable *) rp3->item;

                if (strcmp(lscope, hv3->scope) != 0)
                {
                    break;
                }
                head_count++;
            }
        }

        if (count == endIdx && rp->next)
        {
            last_scope = true;
            for (rp2 = rp; rp2 != NULL; rp2 = rp2->next)
            {
                hv2 = (HubVariable *) rp2->item;

                if (strcmp(lscope, hv2->scope) != 0)
                {
                    break;
                }
                tail_count++;
            }
        }
        count++;
        if (!last_scope)
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
    char monthStr[CF_SMALLBUF] = { 0 };
    int day = 0, month = 0, year = 0;
    struct tm tmTime = { 0 };
    time_t tTime = 0;

    sscanf(inStr, "%d %3s %d", &day, monthStr, &year);

    month = MonthLen2Int(monthStr, 3);

    if (month == -1)
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

    if (tTime == -1)
    {
        CfOut(cf_error, "mktime", "!! Could not convert \"%s\" from string to time_t", inStr);
        *t = 0;
    }
    else
    {
        *t = tTime;
    }
}
