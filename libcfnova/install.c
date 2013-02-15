/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "install.h"

#include "string_lib.h"
#include "sort.h"
#include "conversion.h"
#include "sequence.h"
#include "granules.h"
#include "matching.h"
#include "hashes.h"
#include "cfstream.h"
#include "buffer.h"

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

    RlistDestroy(hq->hosts);

    if (fnptr)
    {
        for (rp = hq->records; rp != NULL; rp = rp->next)
        {
            (*fnptr) (rp->item);
            rp->item = NULL;
        }

        RlistDestroy(hq->records);
    }
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

HubVariable *NewHubVariable(HubHost *hh, const char *type, const char *ns, const char *bundle, const char *lval, Rval rval, time_t t)
// NOTE: rval must be allocated by caller
{
    HubVariable *hp;

    hp = xmalloc(sizeof(HubVariable));

    hp->hh = hh;
    hp->rval = rval;
    hp->ns = SafeStringDuplicate(ns);
    hp->bundle = SafeStringDuplicate(bundle);
    hp->lval = xstrdup(lval);
    hp->dtype = xstrdup(type);
    hp->t = t;
    return hp;
}

/*****************************************************************************/

void DeleteHubVariable(HubVariable *hv)
{
    free(hv->ns);
    free(hv->bundle);
    free(hv->lval);
    free(hv->dtype);
    RvalDestroy(hv->rval);
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

unsigned int HubPromiseLogHash(const void *record, unsigned int max)
{
    const HubPromiseLog *record_a = (const HubPromiseLog *)record;

    Buffer *buf = BufferNew();

    if (record_a->handle)
    {
        BufferPrintf(buf, "%s", record_a->handle);
    }
    if (record_a->cause)
    {
        BufferPrintf(buf, "%s", record_a->cause);
    }

    unsigned int hash = OatHash(BufferData(buf), max);

    BufferDestroy(&buf);

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

HubFileChanges *NewHubFileChanges(HubHost *hh, char *file, time_t t, char *handle, char change_type, char *change_msg)
{
    HubFileChanges *hp;

    hp = xmalloc(sizeof(HubFileChanges));

    hp->hh = hh;
    hp->path = xstrdup(file);
    hp->t = t;
    hp->handle = xstrdup(handle);
    hp->diff_record = NULL;

    if(!NULL_OR_EMPTY(change_msg))
    {
        hp->msg = xstrdup(change_msg);
    }
    else
    {
        hp->msg = xstrdup("NA");
    }

    hp->type = change_type;

    return hp;
}

/*****************************************************************************/

void DeleteHubFileChanges(HubFileChanges *hp)
{
    free(hp->path);
    free(hp->handle);
    free(hp->msg);
    free(hp);
}

/*****************************************************************************/

HubFileDiff *NewHubFileDiff(HubHost *hh, char *promise_handle, char *file, char *diff, time_t t)
{
    HubFileDiff *hp;

    hp = xmalloc(sizeof(HubFileDiff));

    hp->hh = hh;

    hp->promise_handle = SafeStringDuplicate(promise_handle);
    hp->path = SafeStringDuplicate(file);
    hp->diff = SafeStringDuplicate(diff);
    hp->t = t;
    return hp;
}

/*****************************************************************************/

void DeleteHubFileDiff(HubFileDiff *hp)
{
    free(hp->promise_handle);
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

HubBundleSeen *NewHubBundleSeen(HubHost *hh, char *ns, char *rname, double comp, double avg, double dev, time_t t)
{
    HubBundleSeen *hp;

    hp = xmalloc(sizeof(HubBundleSeen));

    hp->hh = hh;
    hp->ns = xstrdup(ns);
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
    if (hp)
    {
        free(hp->ns);
        free(hp->bundle);
        free(hp);
    }
}

/*****************************************************************************/

HubPromise *NewHubPromise(const char *ns, char *bn, char *bt, Rlist *ba, char *pt, char *pr, Rlist *pe, char *cl, char *ha, char *co,
                          char *fn, int lno, Rlist *cons)
{
    HubPromise *hp = xmalloc(sizeof(HubPromise));

    hp->ns = SafeStringDuplicate(ns);
    hp->bundleName = SafeStringDuplicate(bn);
    hp->bundleType = SafeStringDuplicate(bt);
    hp->promiseType = SafeStringDuplicate(pt);
    hp->promiser = SafeStringDuplicate(pr);
    hp->classContext = SafeStringDuplicate(cl);
    hp->handle = SafeStringDuplicate(ha);
    hp->comment = SafeStringDuplicate(co);
    hp->file = SafeStringDuplicate(fn);
    hp->lineNo = lno;
    hp->popularity = 0;

    // NOTE: these are allocated by caller - taking ownership
    hp->promisees = pe;
    hp->bundleArgs = ba;
    hp->constraints = cons;

    return hp;
}

/*****************************************************************************/

void DeleteHubPromise(HubPromise * hp)
{
    free(hp->ns);
    free(hp->bundleName);
    free(hp->bundleType);
    free(hp->promiseType);
    free(hp->promiser);
    free(hp->classContext);
    free(hp->handle);
    free(hp->comment);
    free(hp->file);
    hp->lineNo = -1;

    RlistDestroy(hp->bundleArgs);
    RlistDestroy(hp->constraints);
    RlistDestroy(hp->promisees);

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

    RlistDestroy(hb->bundleArgs);

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

HubVital *NewHubVital(const char *hostkey, const char *id, const char *units, const char *description, time_t last_update)
{
    HubVital *v = xmalloc(sizeof(HubVital));

    v->hostkey = SafeStringDuplicate(hostkey);
    v->id = SafeStringDuplicate(id);
    v->units = SafeStringDuplicate(units);
    v->description = SafeStringDuplicate(description);
    v->last_update = last_update;
    v->q = NULL;

    return v;
}

void DeleteHubVital(HubVital *hv)
{
    if (hv)
    {
        free(hv->hostkey);
        free(hv->id);
        free(hv->units);
        free(hv->description);
        SeqDestroy(hv->q);

        free(hv);
    }
}

HubVitalPoint *NewHubVitalPoint(time_t t, double value)
{
    HubVitalPoint *point = xmalloc(sizeof(HubVitalPoint));

    point->t = t;
    point->value = value;

    return point;
}

int HubVitalPointCompare(const void *a, const void *b, void *user_data)
{
    const HubVitalPoint *ap = a;
    const HubVitalPoint *bp = b;
    if (ap && bp)
    {
        return ap->t - bp->t;
    }
    else
    {
        return 0;
    }
}

const HubVitalPoint *HubVitalLastValue(const HubVital *vital)
{
    if (vital->q && vital->q->length > 0)
    {
       return vital->q->data[vital->q->length - 1];
    }
    return NULL;
}

void DeleteHubVitalPoint(HubVitalPoint *point)
{
    free(point);
}

/*****************************************************************************/

HubUser *NewHubUser(bool external, const char *username, const char *name, const char *email, const Rlist *roles)
{
    HubUser *user = xmalloc(sizeof(HubUser));

    user->external = external;
    user->username = SafeStringDuplicate(username);
    user->name = SafeStringDuplicate(name);
    user->email = SafeStringDuplicate(email);
    user->roles = RlistCopy(roles);

    return user;
}

int HubUserCompare(const void *_hub_user_a, const void *_hub_user_b)
{
    const HubUser *a = _hub_user_a;
    const HubUser *b = _hub_user_b;

    if (a->name && !b->name)
    {
        return -1;
    }
    else if (!a->name && b->name)
    {
        return 1;
    }
    else if (a->name && b->name)
    {
        return StringSafeCompare(a->name, b->name) < 0;
    }
    else
    {
        return StringSafeCompare(a->username, b->username) < 0;
    }
}

void DeleteHubUser(void *_hub_user)
{
    HubUser *user = _hub_user;

    if (user)
    {
        free(user->username);
        free(user->name);
        free(user->email);
        RlistDestroy(user->roles);

        free(user);
    }
}

int HubUserHash(const HubUser *user)
{
    assert(user);
    assert(user->username);
    return OatHash(user->username, CF_HASHTABLESIZE);
}

bool HubUserEqual(const HubUser *a, const HubUser *b)
{
    if (a == b)
    {
        return true;
    }
    else if (!a || !b)
    {
        return false;
    }
    else
    {
        return StringSafeEqual(a->username, b->username)
                && a->external == b->external;
    }
}

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

HubSettings *NewHubSettings(void)
{
    HubSettings *settings = xcalloc(1, sizeof(HubSettings));

    settings->rbac_enabled = TRINARY_UNDEFINED;
    settings->bluehost_horizon = -1;

    settings->ldap.ad_domain = NULL;
    settings->ldap.base_dn = NULL;
    settings->ldap.enabled = TRINARY_UNDEFINED;
    settings->ldap.encryption = NULL;
    settings->ldap.host = NULL;
    settings->ldap.login_attribute = NULL;
    settings->ldap.mode = LDAP_MODE_UNDEFINED;
    settings->ldap.password = NULL;
    settings->ldap.port = -1;
    settings->ldap.port_ssl = -1;
    settings->ldap.username = NULL;
    settings->ldap.users_directory = NULL;

    settings->log_level = -1;

    return settings;
}

HubSettings *NewHubSettingsDefaults(void)
{
    HubSettings *settings = NewHubSettings();

    settings->rbac_enabled = TRINARY_TRUE;
    settings->bluehost_horizon = CF_BLUEHOST_THRESHOLD_DEFAULT;

    settings->ldap.enabled = TRINARY_FALSE;
    settings->ldap.encryption = SafeStringDuplicate("plain");
    settings->ldap.login_attribute = SafeStringDuplicate("uid");
    settings->ldap.mode = LDAP_MODE_STANDARD;
    settings->ldap.port = 389;
    settings->ldap.port_ssl = 636;

    settings->log_level = LOG_ERR;

    return settings;
}

void DeleteHubSettings(HubSettings *settings)
{
    if (settings)
    {
        free(settings->ldap.ad_domain);
        free(settings->ldap.base_dn);
        free(settings->ldap.encryption);
        free(settings->ldap.host);
        free(settings->ldap.login_attribute);
        free(settings->ldap.password);
        free(settings->ldap.username);
        free(settings->ldap.users_directory);

        free(settings);
    }
}

HubSettings *HubSettingsCopy(const HubSettings *settings)
{
    HubSettings *copy = NewHubSettings();

    copy->rbac_enabled = settings->rbac_enabled;
    copy->bluehost_horizon = settings->bluehost_horizon;

    copy->ldap.ad_domain = SafeStringDuplicate(settings->ldap.ad_domain);
    copy->ldap.base_dn = SafeStringDuplicate(settings->ldap.base_dn);
    copy->ldap.enabled = settings->ldap.enabled;
    copy->ldap.encryption = SafeStringDuplicate(settings->ldap.encryption);
    copy->ldap.host = SafeStringDuplicate(settings->ldap.host);
    copy->ldap.login_attribute = SafeStringDuplicate(settings->ldap.login_attribute);
    copy->ldap.mode = settings->ldap.mode;
    copy->ldap.password = SafeStringDuplicate(settings->ldap.password);
    copy->ldap.port = settings->ldap.port;
    copy->ldap.port_ssl = settings->ldap.port_ssl;
    copy->ldap.username = SafeStringDuplicate(settings->ldap.username);
    copy->ldap.users_directory = SafeStringDuplicate(settings->ldap.users_directory);

    copy->log_level = settings->log_level;

    return copy;
}

HubSettings *HubSettingsUpdate(const HubSettings *existing_settings, const HubSettings *new_settings)
{
    HubSettings *updated = HubSettingsCopy(existing_settings);

    if (new_settings->rbac_enabled != TRINARY_UNDEFINED)
    {
        updated->rbac_enabled = new_settings->rbac_enabled;
    }

    if (new_settings->bluehost_horizon != -1)
    {
        updated->bluehost_horizon = new_settings->bluehost_horizon;
    }

    if (new_settings->ldap.ad_domain)
    {
        free(updated->ldap.ad_domain);
        updated->ldap.ad_domain = SafeStringDuplicate(new_settings->ldap.ad_domain);
    }

    if (new_settings->ldap.base_dn)
    {
        free(updated->ldap.base_dn);
        updated->ldap.base_dn = SafeStringDuplicate(new_settings->ldap.base_dn);
    }

    if (new_settings->ldap.enabled != TRINARY_UNDEFINED)
    {
        updated->ldap.enabled = new_settings->ldap.enabled;
    }

    if (new_settings->ldap.encryption)
    {
        free(updated->ldap.encryption);
        updated->ldap.encryption = SafeStringDuplicate(new_settings->ldap.encryption);
    }

    if (new_settings->ldap.host)
    {
        free(updated->ldap.host);
        updated->ldap.host = SafeStringDuplicate(new_settings->ldap.host);
    }

    if (new_settings->ldap.login_attribute)
    {
        free(updated->ldap.login_attribute);
        updated->ldap.login_attribute = SafeStringDuplicate(new_settings->ldap.login_attribute);
    }

    if (new_settings->ldap.mode != LDAP_MODE_UNDEFINED)
    {
        updated->ldap.mode = new_settings->ldap.mode;
    }

    if (new_settings->ldap.password)
    {
        free(updated->ldap.password);
        updated->ldap.password = SafeStringDuplicate(new_settings->ldap.password);
    }

    if (new_settings->ldap.port != -1)
    {
        updated->ldap.port = new_settings->ldap.port;
    }

    if (new_settings->ldap.port_ssl != -1)
    {
        updated->ldap.port_ssl = new_settings->ldap.port_ssl;
    }

    if (new_settings->ldap.username)
    {
        free(updated->ldap.username);
        updated->ldap.username = SafeStringDuplicate(new_settings->ldap.username);
    }

    if (new_settings->ldap.users_directory)
    {
        free(updated->ldap.users_directory);
        updated->ldap.users_directory = SafeStringDuplicate(new_settings->ldap.users_directory);
    }

    if (new_settings->log_level != -1)
    {
        updated->log_level = new_settings->log_level;
    }

    return updated;
}

HubSettingsLDAPMode HubSettingsLDAPModeFromString(const char *ldap_mode)
{
    if (!ldap_mode)
    {
        return LDAP_MODE_UNDEFINED;
    }
    else if (StringSafeEqual("activeDirectory", ldap_mode))
    {
        return LDAP_MODE_AD;
    }
    else
    {
        return LDAP_MODE_STANDARD;
    }
}

const char *HubSettingsLDAPModeToString(HubSettingsLDAPMode mode)
{
    switch (mode)
    {
    case LDAP_MODE_UNDEFINED:
        return NULL;

    case LDAP_MODE_AD:
        return "activeDirectory";

    default:
        return "standard";
    }
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
        RlistAppend(&classRxIncludes, classRxInclude, RVAL_TYPE_SCALAR);
    }

    if (classRxExclude)
    {
        RlistAppend(&classRxExcludes, classRxExclude, RVAL_TYPE_SCALAR);
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
        RlistAppend(&(filter->classRxIncludes), classRxInclude, RVAL_TYPE_SCALAR);
    }

    if (classRxExclude)
    {
        RlistAppend(&(filter->classRxExcludes), classRxExclude, RVAL_TYPE_SCALAR);
    }
}

void HostClassFilterAddClassLists(HostClassFilter *filter, const Rlist *classRxIncludes, const Rlist *classRxExcludes)
{
    for (const Rlist *rp = classRxIncludes; rp; rp = rp->next)
    {
        HostClassFilterAddClasses(filter, RlistScalarValue(rp), NULL);
    }

    for (const Rlist *rp = classRxExcludes; rp; rp = rp->next)
    {
        HostClassFilterAddClasses(filter, NULL, RlistScalarValue(rp));
    }
}

static bool HostClassFilterMatchInclude(const HostClassFilter *filter, const char *classRx)
{
    bool include = false;
    for (const Rlist *rp = filter->classRxIncludes; rp; rp = rp->next)
    {
        if (StringMatch(RlistScalarValue(rp), classRx))
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
        if (StringMatch(RlistScalarValue(rp), classRx))
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
    RlistDestroy(filter->classRxIncludes);
    RlistDestroy(filter->classRxExcludes);

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
        RlistAppend(&(filter->bundleIncludes), bundleInclude, RVAL_TYPE_SCALAR);
    }

    if (bundleExclude)
    {
        RlistAppend(&(filter->bundleExcludes), bundleExclude, RVAL_TYPE_SCALAR);
    }
}

void PromiseFilterAddBundlesRx(PromiseFilter *filter, const char *bundleRxInclude, const char *bundleRxExclude)
{
    if (bundleRxInclude)
    {
        RlistAppend(&(filter->bundleRxIncludes), bundleRxInclude, RVAL_TYPE_SCALAR);
    }

    if (bundleRxExclude)
    {
        RlistAppend(&(filter->bundleRxExcludes), bundleRxExclude, RVAL_TYPE_SCALAR);
    }
}

void PromiseFilterAddNamespaces(PromiseFilter *filter, const char *namespaceInclude, const char *namespaceExclude)
{
    if (namespaceInclude)
    {
        RlistAppend(&(filter->namespaceIncludes), namespaceInclude, RVAL_TYPE_SCALAR);
    }

    if (namespaceExclude)
    {
        RlistAppend(&(filter->namespaceExcludes), namespaceExclude, RVAL_TYPE_SCALAR);
    }
}

void PromiseFilterAddNamespacesRx(PromiseFilter *filter, const char *namespaceRxInclude, const char *namespaceRxExclude)
{
    if (namespaceRxInclude)
    {
        RlistAppend(&(filter->namespaceRxIncludes), namespaceRxInclude, RVAL_TYPE_SCALAR);
    }

    if (namespaceRxExclude)
    {
        RlistAppend(&(filter->namespaceRxExcludes), namespaceRxExclude, RVAL_TYPE_SCALAR);
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

    RlistDestroy(filter->bundleIncludes);
    RlistDestroy(filter->bundleRxIncludes);
    RlistDestroy(filter->bundleExcludes);
    RlistDestroy(filter->bundleRxExcludes);

    RlistDestroy(filter->namespaceIncludes);
    RlistDestroy(filter->namespaceRxIncludes);
    RlistDestroy(filter->namespaceExcludes);
    RlistDestroy(filter->namespaceRxExcludes);

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

        RlistDestroy(*records_p);
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

        RlistDestroy(*records_p);
        *records_p = NULL;      // restored below
    }

    if (endEl)
    {
        for (rp = endEl->next; rp != NULL; rp = rp->next)
        {
            (*fnptr) (rp->item);
            rp->item = NULL;
        }

        RlistDestroy(endEl->next);
        endEl->next = NULL;
    }

    *records_p = startEl;
    page->totalResultCount = count;

    return true;
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

/*****************************************************************************/

HubScheduledReport *NewHubScheduledReport(const char *user, const char *email, const char *query_id, const char *query, const char *schedule, const char *title, const char *description)
{
    HubScheduledReport *sr;
    sr = xmalloc( sizeof( HubScheduledReport ) );

    sr->username = SafeStringDuplicate( user );
    sr->email = SafeStringDuplicate( email );
    sr->query = SafeStringDuplicate( query );
    sr->query_id = SafeStringDuplicate( query_id );
    sr->schedule = SafeStringDuplicate( schedule );
    sr->enabled = false;
    sr->last_run = 0;
    sr->output_type = 0;
    sr->title = SafeStringDuplicate(title);
    sr->description = SafeStringDuplicate(description);

    return sr;
}

/*****************************************************************************/

void DeleteHubScheduledReport(HubScheduledReport *sr)
{
    if( sr )
    {
        free( sr->username );
        free( sr->email );
        free( sr->query_id );
        free( sr->query );
        free( sr->schedule );
        free(sr->title);
        free(sr->description);
    }

    free( sr );
}

/*****************************************************************************/

bool FileRecordsEqual(const HubFileChanges *change_record, const HubFileDiff *diff_record)
{
    if (!change_record || !diff_record)
    {
        return false;
    }

    if (strcmp(change_record->hh->keyhash, diff_record->hh->keyhash) != 0)
    {
        return false;
    }

    return (strcmp(change_record->path, diff_record->path) == 0) && (change_record->t == diff_record->t);
}
/*****************************************************************************/
