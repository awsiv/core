/*

This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: cmd_api.c                                                           */
/*                                                                           */
/* Created: Mon Aug 8 10:44:14 2011                                          */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "scorecards.h"
#include "db_query.h"

void Nova2Txt_getlicense(char *buffer, int bufsize)
{
    NewClass("am_policy_hub");
    CFDB_GetValue("license_report", buffer, bufsize, MONGO_SCRATCH);
}

/*****************************************************************************/

int Nova2Txt_summary_report(char *hostkey, char *handle, char *status, int regex, char *classreg)
/*
  Return current best-knowledge of average compliance for the class of hosts and promises selected
 */
{
    char buf1[CF_SMALLBUF], buf2[CF_SMALLBUF];
    HubPromiseCompliance *hp;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    time_t now = time(NULL), from = now, to = now - SECONDS_PER_WEEK;
    int code_blue = 0, tot_hosts, code_black = 0;
    double n, r, k, n_av, k_av, r_av, tot_promises;
    char *current_host = "x";

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    if (!status)                // any
    {
        status = "x";
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0, false, filter);
    DeleteHostClassFilter(filter);

    n = k = r = 0;
    n_av = k_av = r_av = 0;
    tot_promises = 1;
    tot_hosts = 0;

// For each promise record (from multiple matching hosts)

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hp = (HubPromiseCompliance *) rp->item;

        // If data have passed the time horizon, we should not claim to know their state
        // The system might or might not be compliant, hp->e is the expected schedule for this promise

        if (hp->t < now - CF_BLUEHOST_THRESHOLD_DEFAULT - hp->e)
        {
            if (current_host && strcmp(hp->hh->keyhash, current_host) != 0)     // New host
            {
                code_blue++;
                current_host = hp->hh->keyhash;
            }
            continue;
        }

        HostColour colour;
        CFDB_GetHostColour(hp->hh->keyhash, HOST_RANK_METHOD_COMPLIANCE, &colour);
        if (colour == HOST_COLOUR_BLACK)
        {
            if (current_host && strcmp(hp->hh->keyhash, current_host) != 0)     // New host
            {
                code_black++;
                current_host = hp->hh->keyhash;
            }
            continue;
        }

        // How do we know how many promises are supposed to be kept on a given host?
        // compute the average number per host      
        if (current_host && strcmp(hp->hh->keyhash, current_host) != 0) // New host
        {
            n_av += n / tot_promises;   // Average not kept on available hosts in class
            r_av += r / tot_promises;   // Average repaired on available hosts in class
            k_av += k / tot_promises;   // Average compliant/kept on available hosts in class

            n = k = r = 0;
            tot_promises = 1;
            tot_hosts++;        // The number of hosts in the class that have fresh data
            current_host = hp->hh->keyhash;
        }
        else
        {
            tot_promises++;
        }

        // Get minimax boundary of last measured time region so we can compute uncertainty

        if (hp->t < from)
        {
            from = hp->t;
        }

        if (hp->t > to)
        {
            to = hp->t;
        }

        // Get status of this matching promise (n,r,k = 1 if unique handle)

        switch (hp->status)
        {
        case PROMISE_STATE_KEPT:
            k++;
            break;
        case PROMISE_STATE_REPAIRED:
            r++;
            break;
        case PROMISE_STATE_NOTKEPT:
        default:
            n++;
            break;
        }
    }

    n_av += n / tot_promises;   // Average not kept on available hosts in class
    r_av += r / tot_promises;   // Average repaired on available hosts in class
    k_av += k / tot_promises;   // Average compliant/kept on available hosts in class

    if (tot_hosts == 0)
    {
        from = now - CF_BLUEHOST_THRESHOLD_DEFAULT;
        to = now;
    }
// Return current best-knowledge of average compliance for the class of hosts and promises selected

    if (CSV)
    {
        printf("%.2lf,"
               "%.2lf,"
               "%.2lf,"
               "%d,"
               "%d,"
               "%d,"
               "%s,"
               "%s,"
               "%s\n",
               k_av, n_av, r_av, tot_hosts, code_blue, code_black, classreg,
               cf_strtimestamp_local(from, buf1), cf_strtimestamp_local(to, buf2));
    }
    else
    {
        printf("Hosts with promises kept: %.2lf\n"
               "Hosts with promises not kept: %.2lf\n"
               "Hosts with promises repaired %.2lf\n"
               "Total number of hosts: %d\n"
               "Hosts that didn't report status: %d\n"
               "Hosts that failed to execute agent: %d\n"
               "Search class: %s\n"
               "First reports at: %s\n"
               "Last reports at: %s\n",
               k_av, n_av, r_av, tot_hosts, code_blue, code_black, classreg,
               cf_strtimestamp_local(from, buf1), cf_strtimestamp_local(to, buf2));
    }

    DeleteHubQuery(hq, DeleteHubPromiseCompliance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/
/* Search for answers                                                        */
/*****************************************************************************/

int Nova2Txt_promiselog(char *hostkey, char *handle, char *cause, PromiseLogState state, time_t from, time_t to, char *classreg,
                        PageInfo *page, char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE], canonifiedCause[CF_BUFSIZE] = { 0 };
    HubPromiseLog *hp;
    HubQuery *hq;
    Rlist *rp;
    int reportType;
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryPromiseLog(&dbconn, hostkey, state, handle, true, cause, from, to, true, filter);
    DeleteHostClassFilter(filter);

    PageRecords(&(hq->records), page, DeleteHubPromiseLog);

    snprintf(buffer, sizeof(buffer),
             "{\"meta\":{\"count\" : %d,"
             "\"header\":{\"Host\":0,\"Promise Handle\":1,\"Report\":2,\"Time\":3,"
             "\"Note\":{\"index\":4,\"subkeys\":{\"action\":0,\"hostkey\":1,\"reporttype\":2,\"rid\":3,\"nid\":4}}"
             "}},\"data\":[", page->totalResultCount);
    StartJoin(returnval, buffer, bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hp = (HubPromiseLog *) rp->item;
        EscapeJson(hp->cause, canonifiedCause, sizeof(canonifiedCause));
        if (strcmp(hp->nid, CF_NONOTE) == 0)
        {
            switch (state)
            {
            case PROMISE_LOG_STATE_REPAIRED:
                reportType = CFREPORT_REPAIRED;
                break;
            case PROMISE_LOG_STATE_NOTKEPT:
            default:
                reportType = CFREPORT_NOTKEPT;
                break;
            }
            snprintf(buffer, sizeof(buffer),
                     "[ \"%s\",\"%s\",\"%s\",%ld,"
                     "[ \"add\",\"%s\",%d,\"%s\",\"\"]"
                     "],", hp->hh->hostname, hp->handle, canonifiedCause, hp->t, hp->hh->keyhash, reportType, hp->oid);
        }
        else
        {
            snprintf(buffer, sizeof(buffer),
                     "[ \"%s\",\"%s\",\"%s\",%ld,"
                     "[ \"show\",\"\",\"\",\"\",\"%s\"]"
                     "],", hp->hh->hostname, hp->handle, canonifiedCause, hp->t, hp->nid);
        }
        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }
    }
    if (returnval[strlen(returnval) - 1] == ',')
    {
        returnval[strlen(returnval) - 1] = '\0';
    }
    EndJoin(returnval, "]}", bufsize);
    DeleteHubQuery(hq, DeleteHubPromiseLog);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_promiselog_summary(char *hostkey, char *handle, char *cause, PromiseLogState state, time_t from, time_t to,
                                char *classreg)
{
    HubPromiseLog *hp;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    Item *ip, *summary = NULL;
    int i = 0;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryPromiseLog(&dbconn, hostkey, state, handle, true, cause, from, to, false, filter);
    DeleteHostClassFilter(filter);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hp = (HubPromiseLog *) rp->item;
        ip = IdempPrependItem(&summary, hp->handle, hp->cause);
        ip->counter++;
    }

    DeleteHubQuery(hq, DeleteHubPromiseLog);

    CFDB_Close(&dbconn);

    if (summary == NULL)
    {
        printf("No data to report\n");
    }
    else
    {
        summary = SortItemListCounters(summary);

        printf("{\"meta\":{\"count\" : %d,"
               "\"header\":{\"Promise Handle\":0,\"Report\":1,\"Occurrences\":2" "}},\"data\":[", ListLen(summary));

        printf("Promise Handle       Frequency  Report");

        for (ip = summary; ip != NULL; ip = ip->next, i++)
        {
            printf("%20.20s %10d %s\n", ip->name, ip->counter, ip->classes);
        }

        DeleteItemList(summary);
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_software_report(char *hostkey, char *name, char *value, char *arch, int regex, char *type, char *classreg)
{
    HubSoftware *hs;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }
    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QuerySoftware(&dbconn, hostkey, type, name, value, arch, regex, filter, true);
    DeleteHostClassFilter(filter);

    if (!CSV)
    {
        printf("%25s %30s %20s %s\n", "Hostname", "Package", "Version", "Arch");
    }

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hs = (HubSoftware *) rp->item;
        if (CSV)
        {
            printf("%s,%s,%s,%s\n", hs->hh->hostname, hs->name, hs->version, Nova_LongArch(hs->arch));
        }
        else
        {
            printf("%25s %30s %20s %s\n", hs->hh->hostname, hs->name, hs->version, Nova_LongArch(hs->arch));
        }
    }

    DeleteHubQuery(hq, DeleteHubSoftware);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_classes_report(char *hostkey, char *name, int regex, char *classreg)
{
    HubClass *hc;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryClasses(&dbconn, hostkey, name, regex, (time_t) SECONDS_PER_WEEK, filter, true);
    DeleteHostClassFilter(filter);

    if (!CSV)
    {
        printf("Host  Class/Context  Probability  Uncertainty Last-Defined\n");
    }

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hc = (HubClass *) rp->item;

        if (CSV)
        {
            printf("%s,%s,%lf,%lf,%ld\n", hc->hh->hostname, hc->class, hc->prob, hc->dev, hc->t);
        }
        else
        {
            printf("%s %s %lf %lf %ld\n", hc->hh->hostname, hc->class, hc->prob, hc->dev, hc->t);
        }
    }

    DeleteHubQuery(hq, DeleteHubClass);

    CFDB_Close(&dbconn);
    return true;
}

/*****************************************************************************/

int Nova2Txt_vars_report(char *hostkey, char *scope, char *lval, char *rval, char *type, int regex, char *classreg)
{
    char rvalBuf[CF_MAXVARSIZE];
    HubVariable *hv;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryVariables(&dbconn, hostkey, scope, lval, rval, type, regex, filter);
    DeleteHostClassFilter(filter);

    if (!CSV)
    {
        printf("%25s %14s %25s %s\n", "Host", "Type", "lval", "rval");
    }

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        char typestr[CF_SMALLBUF];

        hv = (HubVariable *) rp->item;

        switch (*hv->dtype)
        {
        case 's':
            snprintf(typestr, CF_SMALLBUF, "string");
            break;
        case 'i':
            snprintf(typestr, CF_SMALLBUF, "int");
            break;
        case 'r':
            snprintf(typestr, CF_SMALLBUF, "real");
            break;
        case 'm':
            snprintf(typestr, CF_SMALLBUF, "menu");
            break;
        }

        if (strlen(hv->dtype) == 2)
        {
            strcat(typestr, " list");
        }

        if (strlen(hv->dtype) > 1)      // list
        {
            PrintRlist(rvalBuf, sizeof(rvalBuf), hv->rval.item);
        }
        else
        {
            snprintf(rvalBuf, sizeof(rvalBuf), "%s", (char *) hv->rval.item);
        }

        if (CSV)
        {
            printf("%s,%s,%s.%s,%s\n", hv->hh->hostname, typestr, hv->scope, hv->lval, rvalBuf);
        }
        else
        {
            printf("%25s %14s %s.%-25s %s\n", hv->hh->hostname, typestr, hv->scope, hv->lval, rvalBuf);
        }
    }

    DeleteHubQuery(hq, DeleteHubVariable);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_compliance_report(char *hostkey, char *version, time_t t, int k, int nk, int rep, char *cmp,
                               char *classreg)
{
    char buffer[CF_BUFSIZE];
    HubTotalCompliance *ht;
    HubQuery *hq;
    Rlist *rp;
    int icmp;
    mongo_connection dbconn;

    switch (*cmp)
    {
    case '<':
        icmp = CFDB_LESSTHANEQ;
        break;
    default:
        icmp = CFDB_GREATERTHANEQ;
        break;
    }

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryTotalCompliance(&dbconn, hostkey, version, t, k, nk, rep, icmp, true, filter);
    DeleteHostClassFilter(filter);

    if (!CSV)
    {
        printf("%25s %7s %10s %10s %s\n", "Host", "%Kept", "%Repaired", "%Not-Kept", "Last-verified");
    }

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        ht = (HubTotalCompliance *) rp->item;

        if (CSV)
        {
            printf("%s,%d%%,%d%%,%d%%,%s\n", ht->hh->hostname, ht->kept, ht->repaired, ht->notkept,
                   cf_strtimestamp_local(ht->t, buffer));
        }
        else
        {
            printf("%25s %6d%% %9d%% %9d%% %s\n", ht->hh->hostname, ht->kept, ht->repaired, ht->notkept,
                   cf_strtimestamp_local(ht->t, buffer));
        }
    }

    DeleteHubQuery(hq, DeleteHubTotalCompliance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_compliance_promises(char *hostkey, char *handle, char *status, int regex, char *classreg)
{
    char buffer[CF_BUFSIZE];
    HubPromiseCompliance *hp;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    if (!status)                // any
    {
        status = "x";
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0, true, filter);
    DeleteHostClassFilter(filter);

    if (!CSV)
    {
        printf("%25s %50s %10s %s %s %s\n", "Host", "Promise-handle", "Last state", "E(Q)", "Sigma", "Last-data");
    }

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hp = (HubPromiseCompliance *) rp->item;

        if (CSV)
        {
            printf("%s,%s,%s,%.2lf,%.2lf,%s\n", hp->hh->hostname, hp->handle, Nova_LongState(hp->status), hp->e, hp->d,
                   cf_strtimestamp_local(hp->t, buffer));
        }
        else
        {
            printf("%25s %50s %10s %.2lf %.2lf %s\n", hp->hh->hostname, hp->handle, Nova_LongState(hp->status), hp->e,
                   hp->d, cf_strtimestamp_local(hp->t, buffer));
        }
    }

    DeleteHubQuery(hq, DeleteHubPromiseCompliance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_lastseen_report(char *hostkey, char *lhash, char *lhost, char *laddress, time_t lago, int lregex,
                             char *classreg)
{
    HubLastSeen *hl;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    char inout[CF_SMALLBUF];

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryLastSeen(&dbconn, hostkey, lhash, lhost, laddress, lago, lregex, true, filter);
    DeleteHostClassFilter(filter);

    if (!CSV)
    {
        printf("%25s %11s %25s %10s %7s %12s %10s %s\n", "Seen-on-host", "Direction", "Remote-host", "Remote-IP",
               "Hrs-ago", "Avg-interval", "Uncertainty", "Remote-key");
    }

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hl = (HubLastSeen *) rp->item;

        switch (hl->direction)
        {
        case LAST_SEEN_DIRECTION_OUTGOING:
            snprintf(inout, CF_SMALLBUF, "by us (+)");
            break;
        case LAST_SEEN_DIRECTION_INCOMING:
            snprintf(inout, CF_SMALLBUF, "by them (-)");
            break;
        }

        if (CSV)
        {
            printf("%s,%s,%s,%s,%.2lf,%.2lf,%.2lf,%s\n",
                   hl->hh->hostname, inout, hl->rhost->hostname, hl->rhost->ipaddr,
                   hl->hrsago, hl->hrsavg, hl->hrsdev, hl->rhost->keyhash);

        }
        else
        {
            printf("%25s %11s %25s %10s %7.2lf %12.2lf %10.2lf %s\n",
                   hl->hh->hostname, inout, hl->rhost->hostname, hl->rhost->ipaddr,
                   hl->hrsago, hl->hrsavg, hl->hrsdev, hl->rhost->keyhash);
        }
    }

    DeleteHubQuery(hq, DeleteHubLastSeen);
    CFDB_Close(&dbconn);
    return true;
}

/*****************************************************************************/

int Nova2Txt_deadclient_report(char *hostkey, char *lhash, char *lhost, char *laddress, time_t lago, int lregex,
                               char *classreg)
{
    HubLastSeen *hl;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    time_t then;
    time_t now = time(NULL);

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryLastSeen(&dbconn, hostkey, lhash, lhost, laddress, lago, lregex, true, filter);
    DeleteHostClassFilter(filter);

    if (!CSV)
    {
        printf("%25s %25s %10s %7s %12s %10s %s\n", "Seen-on-host", "Remote-host", "Remote-IP", "Hrs-ago",
               "Avg-interval", "Uncertainty", "Remote-key");
    }

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hl = (HubLastSeen *) rp->item;

        switch (hl->direction)
        {
        case LAST_SEEN_DIRECTION_OUTGOING:
            break;

        case LAST_SEEN_DIRECTION_INCOMING:
            continue;
        }

        then = hl->t;

        if (then < now - CF_BLUEHOST_THRESHOLD_DEFAULT)
        {
            if (CSV)
            {
                printf("%s,%s,%s,%.2lf,%.2lf,%.2lf,%s\n",
                       hl->hh->hostname, hl->rhost->hostname, hl->rhost->ipaddr,
                       hl->hrsago, hl->hrsavg, hl->hrsdev, hl->rhost->keyhash);

            }
            else
            {
                printf("%25s %25s %10s %7.2lf %12.2lf %10.2lf %s\n",
                       hl->hh->hostname, hl->rhost->hostname, hl->rhost->ipaddr,
                       hl->hrsago, hl->hrsavg, hl->hrsdev, hl->rhost->keyhash);
            }
        }
    }

    DeleteHubQuery(hq, DeleteHubLastSeen);
    CFDB_Close(&dbconn);
    return true;
}

/*****************************************************************************/

int Nova2Txt_setuid_report(char *hostkey, char *file, int regex, char *classreg)
{
    HubSetUid *hS;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QuerySetuid(&dbconn, hostkey, file, regex, filter);
    DeleteHostClassFilter(filter);

    if (!CSV)
    {
        printf("%25s %s\n", "Host", "File");
    }

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hS = (HubSetUid *) rp->item;

        if (CSV)
        {
            printf("%s,%s\n", hS->hh->hostname, hS->path);
        }
        else
        {
            printf("%25s %s\n", hS->hh->hostname, hS->path);
        }
    }

    DeleteHubQuery(hq, DeleteHubSetUid);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_bundle_report(char *hostkey, char *bundle, int regex, char *classreg, PageInfo *page, char *returnval,
                           int bufsize)
{
    char buffer[CF_BUFSIZE] = { 0 };
    HubBundleSeen *hb;
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    char header[CF_BUFSIZE] = { 0 };
    int margin = 0, headerLen = 0, noticeLen = 0;
    int truncated = false;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryBundleSeen(&dbconn, hostkey, bundle, regex, filter, true);
    DeleteHostClassFilter(filter);

    PageRecords(&(hq->records), page, DeleteHubBundleSeen);
    snprintf(header, sizeof(header),
             "\"meta\":{\"count\" : %d,"
             "\"header\": {\"Host\":0,\"Bundle\":1,\"Last Verified\":2,\"Compliance\":3,\"Avg Compliance\":4,\"Uncertainty\":5,"
             "\"Note\":{\"index\":6,\"subkeys\":{\"action\":0,\"hostkey\":1,\"reporttype\":2,\"rid\":3,\"nid\":4}}"
             "}", page->totalResultCount);

    headerLen = strlen(header);
    noticeLen = strlen(CF_NOTICE_TRUNCATED);
    StartJoin(returnval, "{\"data\":[", bufsize);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hb = (HubBundleSeen *) rp->item;

        if (strcmp(hb->bundle, "QUERY") == 0)
        {
            continue;
        }

        if (strcmp(hb->nid, CF_NONOTE) == 0)
        {
            snprintf(buffer, sizeof(buffer), "[\"%s\",\"%s\",%ld,%.2lf,%.2lf,%.2lf,"
                     "[\"add\",\"%s\",%d,\"%s\",\"\"]],",
                     hb->hh->hostname, hb->bundle, hb->t,
                     hb->bundlecomp, hb->bundleavg, hb->bundledev, hb->hh->keyhash, CFREPORT_BUNDLE, hb->bundle);
        }
        else
        {
            snprintf(buffer, sizeof(buffer), "[\"%s\",\"%s\",%ld,%.2lf,%.2lf,%.2lf,"
                     "[\"show\",\"\",\"\",\"\",\"%s\"]],",
                     hb->hh->hostname, hb->bundle, hb->t, hb->bundlecomp, hb->bundleavg, hb->bundledev, hb->nid);
        }
        margin = headerLen + noticeLen + strlen(buffer);
        if (!JoinMargin(returnval, buffer, NULL, bufsize, margin))
        {
            truncated = true;
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    Nova_AddReportHeader(header, truncated, buffer, sizeof(buffer) - 1);
    Join(returnval, buffer, bufsize);
    EndJoin(returnval, "}}\n", bufsize);

    DeleteHubQuery(hq, DeleteHubBundleSeen);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_filechanges_report(char *hostkey, char *file, int regex, time_t t, char *cmp, char *classreg)
{
    char buffer[CF_SMALLBUF];
    HubFileChanges *hC;
    HubQuery *hq;
    Rlist *rp;
    int icmp;
    mongo_connection dbconn;

    switch (*cmp)
    {
    case '<':
        icmp = CFDB_LESSTHANEQ;
        break;
    default:
        icmp = CFDB_GREATERTHANEQ;
        break;
    }

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryFileChanges(&dbconn, hostkey, file, regex, t, icmp, true, filter, false);
    DeleteHostClassFilter(filter);

    if (!CSV)
    {
        printf("%s,%s,%s,%s\n", "Host", "File", "Changed-on", "Note");
    }

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hC = (HubFileChanges *) rp->item;

        if (CSV)
        {
            if (strcmp(hC->nid, CF_NONOTE) == 0)
            {
                printf("%s,%s,%s\n", hC->hh->hostname, hC->path, cf_strtimestamp_local(hC->t, buffer));
            }
            else
            {
                printf("%s,%s,%s,%s\n", hC->hh->hostname, hC->path, cf_strtimestamp_local(hC->t, buffer), hC->nid);
            }
        }
        else
        {
            if (strcmp(hC->nid, CF_NONOTE) == 0)
            {
                printf("%25s %40s  %20s\n", hC->hh->hostname, hC->path, cf_strtimestamp_local(hC->t, buffer));
            }
            else
            {
                printf("%25s %40s %20s %s\n", hC->hh->hostname, hC->path, cf_strtimestamp_local(hC->t, buffer),
                       hC->nid);
            }
        }
    }

    DeleteHubQuery(hq, DeleteHubFileChanges);
    CFDB_Close(&dbconn);
    return true;
}

/*****************************************************************************/

int Nova2Txt_filediffs_report(char *hostkey, char *file, char *diffs, int regex, time_t t, char *cmp, char *classreg)
{
    HubFileDiff *hd;
    HubQuery *hq;
    Rlist *rp;
    int icmp;
    mongo_connection dbconn;

    switch (*cmp)
    {
    case '<':
        icmp = CFDB_LESSTHANEQ;
        break;
    default:
        icmp = CFDB_GREATERTHANEQ;
        break;
    }

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryFileDiff(&dbconn, hostkey, file, diffs, regex, t, icmp, true, filter, false);
    DeleteHostClassFilter(filter);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hd = (HubFileDiff *) rp->item;

        if (CSV)
        {
            char *sp;

            // If the formatted difference contains new lines, we will break csv formatting, so purge

            for (sp = hd->diff; *sp != '\0'; sp++)
            {
                if (*sp == '\n')
                {
                    *sp = ';';
                }
            }

            printf("%s,%s,%ld,%s\n", hd->hh->hostname, hd->path, hd->t, Nova_FormatDiff(hd->diff));
        }
        else
        {
            printf("%s file %s changed on %ld\n%s\n", hd->hh->hostname, hd->path, hd->t, Nova_FormatDiff(hd->diff));
        }
    }

    DeleteHubQuery(hq, DeleteHubFileDiff);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/
/* Search for hosts with property X,Y,Z                                      */
/*****************************************************************************/

int Nova2Txt_hostinfo(char *hostkey, char *hostnameOut, char *ipaddrOut, int bufsize)
{
    char buffer1[CF_BUFSIZE], buffer2[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int count1 = 0, count2 = 0, tmpsize1, tmpsize2;
    mongo_connection dbconn;
    bson query;
    bson_buffer bb;

    if (NULL_OR_EMPTY(hostkey))
    {
        return false;
    }

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    bson_buffer_init(&bb);
    bson_append_string(&bb, cfr_keyhash, hostkey);
    bson_from_buffer(&query, &bb);

    hq = CFDB_QueryHosts(&dbconn, MONGO_DATABASE, cfr_keyhash, &query);
    bson_destroy(&query);

    hostnameOut[0] = '\0';
    ipaddrOut[0] = '\0';

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;

        snprintf(buffer1, CF_MAXVARSIZE - 1, "%s ", hh->hostname);
        snprintf(buffer2, CF_MAXVARSIZE - 1, "%s ", hh->ipaddr);

        tmpsize1 = strlen(buffer1);
        tmpsize2 = strlen(buffer2);

        if (count1 + tmpsize1 <= bufsize - 1)
        {
            strcat(hostnameOut, buffer1);
            count1 += tmpsize1;
        }

        if (count2 + tmpsize2 <= bufsize - 1)
        {
            strcat(ipaddrOut, buffer2);
            count2 += tmpsize2;
        }
    }

    DeleteHubQuery(hq, NULL);

    ReplaceTrailingChar(hostnameOut, ' ', '\0');
    ReplaceTrailingChar(ipaddrOut, ' ', '\0');

    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

int Nova2Txt_value_hosts(char *hostkey, char *day, char *month, char *year, char *classreg, char *returnval,
                         int bufsize)
{
    HubQuery *hq;
    Rlist *rp;
    mongo_connection dbconn;
    char buffer[CF_BUFSIZE] = { 0 };
    int counter = 0, n = 180;
    HubHost *hh;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryValueReport(&dbconn, hostkey, day, month, year, true, filter);
    DeleteHostClassFilter(filter);

    StartJoin(returnval, "[", bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;

        counter++;
        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }

        if (counter > n && counter % 6 == 0)
        {
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    DeleteHubQuery(hq, DeleteHubValue);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_software_hosts(char *hostkey, char *name, char *value, char *arch, int regex, char *type, char *classreg,
                            char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int counter = 0, n = 180;
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QuerySoftware(&dbconn, hostkey, type, name, value, arch, regex, filter, false);
    DeleteHostClassFilter(filter);

    StartJoin(returnval, "[", bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;

        counter++;
        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }

        if (counter > n && counter % 6 == 0)
        {
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    DeleteHubQuery(hq, DeleteHubSoftware);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_classes_hosts(char *hostkey, char *name, int regex, char *classreg, char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int counter = 0, n = 180;
    mongo_connection dbconn;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryClasses(&dbconn, hostkey, name, regex, (time_t) SECONDS_PER_WEEK, filter, false);
    DeleteHostClassFilter(filter);

    StartJoin(returnval, "[", bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;

        counter++;
        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }

        if (counter > n && counter % 6 == 0)
        {
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    DeleteHubQuery(hq, DeleteHubClass);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_vars_hosts(char *hostkey, char *scope, char *lval, char *rval, char *type, int regex, char *classreg,
                        char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int counter = 0, n = 180;
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryVariables(&dbconn, hostkey, scope, lval, rval, type, regex, filter);
    DeleteHostClassFilter(filter);

    StartJoin(returnval, "[", bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;
        counter++;
        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }

        if (counter > n && counter % 6 == 0)
        {
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    DeleteHubQuery(hq, DeleteHubVariable);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_compliance_hosts(char *hostkey, char *version, time_t t, int k, int nk, int rep, char *cmp, char *classreg,
                              char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int counter = 0, n = 180, icmp;
    mongo_connection dbconn;

    switch (*cmp)
    {
    case '<':
        icmp = CFDB_LESSTHANEQ;
        break;
    default:
        icmp = CFDB_GREATERTHANEQ;
        break;
    }

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryTotalCompliance(&dbconn, hostkey, version, t, k, nk, rep, icmp, false, filter);
    DeleteHostClassFilter(filter);

    StartJoin(returnval, "[", bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;
        counter++;
        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }

        if (counter > n && counter % 6 == 0)
        {
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    DeleteHubQuery(hq, DeleteHubTotalCompliance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_promise_hosts(char *hostkey, char *handle, char *status, int regex, char *classreg, char *returnval,
                           int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int counter = 0, n = 180;
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    if (!status)                // any
    {
        status = "x";
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0, false, filter);
    DeleteHostClassFilter(filter);

    StartJoin(returnval, "[", bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;
        counter++;
        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }

        if (counter > n && counter % 6 == 0)
        {
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    DeleteHubQuery(hq, DeleteHubPromiseCompliance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_lastseen_hosts(char *hostkey, char *lhash, char *lhost, char *laddress, time_t lago, int lregex,
                            char *classreg, char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int counter = 0, n = 180;
    mongo_connection dbconn;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryLastSeen(&dbconn, hostkey, lhash, lhost, laddress, lago, lregex, false, filter);
    DeleteHostClassFilter(filter);

    StartJoin(returnval, "[", bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;
        counter++;
        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }

        if (counter > n && counter % 6 == 0)
        {
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    DeleteHubQuery(hq, DeleteHubLastSeen);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_performance_hosts(char *hostkey, char *job, int regex, char *classreg, char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int counter = 0, n = 180;
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryPerformance(&dbconn, hostkey, job, regex, false, filter);
    DeleteHostClassFilter(filter);

    StartJoin(returnval, "[", bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;
        counter++;
        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }

        if (counter > n && counter % 6 == 0)
        {
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    DeleteHubQuery(hq, DeleteHubPerformance);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_setuid_hosts(char *hostkey, char *file, int regex, char *classreg, char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int counter = 0, n = 180;
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QuerySetuid(&dbconn, hostkey, file, regex, filter);
    DeleteHostClassFilter(filter);

    StartJoin(returnval, "[", bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;
        counter++;
        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }

        if (counter > n && counter % 6 == 0)
        {
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    DeleteHubQuery(hq, DeleteHubSetUid);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_bundle_hosts(char *hostkey, char *bundle, int regex, char *classreg, char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int counter = 0, n = 180;
    mongo_connection dbconn;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryBundleSeen(&dbconn, hostkey, bundle, regex, filter, false);
    DeleteHostClassFilter(filter);

    StartJoin(returnval, "[", bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;
        counter++;
        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }

        if (counter > n && counter % 6 == 0)
        {
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    DeleteHubQuery(hq, DeleteHubBundleSeen);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_filechanges_hosts(char *hostkey, char *file, int regex, time_t t, char *cmp, char *classreg,
                               char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int counter = 0, n = 180, icmp;
    mongo_connection dbconn;

/* BEGIN query document */
    switch (*cmp)
    {
    case '<':
        icmp = CFDB_LESSTHANEQ;
        break;
    default:
        icmp = CFDB_GREATERTHANEQ;
        break;
    }

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryFileChanges(&dbconn, hostkey, file, regex, t, icmp, false, filter, false);
    DeleteHostClassFilter(filter);

    StartJoin(returnval, "[", bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;
        counter++;
        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }

        if (counter > n && counter % 6 == 0)
        {
            break;
        }
    }

    ReplaceTrailingChar(returnval, ',', '\0');
    EndJoin(returnval, "]", bufsize);

    DeleteHubQuery(hq, DeleteHubFileChanges);

    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

int Nova2Txt_filediffs_hosts(char *hostkey, char *file, char *diffs, int regex, time_t t, char *cmp, char *classreg,
                             char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE];
    HubHost *hh;
    HubQuery *hq;
    Rlist *rp;
    int counter = 0, n = 180, icmp;
    mongo_connection dbconn;

    switch (*cmp)
    {
    case '<':
        icmp = CFDB_LESSTHANEQ;
        break;
    default:
        icmp = CFDB_GREATERTHANEQ;
        break;
    }

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryFileDiff(&dbconn, hostkey, file, diffs, regex, t, icmp, false, filter, false);
    DeleteHostClassFilter(filter);

    StartJoin(returnval, "[", bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;
        counter++;
        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }

        if (counter > n && counter % 6 == 0)
        {
            break;
        }
    }
    if (returnval[strlen(returnval) - 1] == ',')
    {
        returnval[strlen(returnval) - 1] = '\0';
    }

    EndJoin(returnval, "]\n", bufsize);
    DeleteHubQuery(hq, DeleteHubFileDiff);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

int Nova2Txt_promiselog_hosts(char *hostkey, char *handle, char *cause, PromiseLogState state, time_t from, time_t to,
                              char *classreg, char *returnval, int bufsize)
{
    HubHost *hh;
    HubQuery *hq;
    mongo_connection dbconn;
    Rlist *rp;
    int counter = 0, n = 180;
    char buffer[CF_BUFSIZE];

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryPromiseLog(&dbconn, hostkey, state, handle, true, cause, from, to, false, filter);
    DeleteHostClassFilter(filter);

    StartJoin(returnval, "[", bufsize);

    for (rp = hq->hosts; rp != NULL; rp = rp->next)
    {
        hh = (HubHost *) rp->item;
        counter++;
        snprintf(buffer, CF_MAXVARSIZE, "{\"hostkey\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\"},", hh->keyhash,
                 hh->hostname, hh->ipaddr);

        if (!Join(returnval, buffer, bufsize))
        {
            break;
        }

        if (counter > n && counter % 6 == 0)
        {
            break;
        }
    }
    if (returnval[strlen(returnval) - 1] == ',')
    {
        returnval[strlen(returnval) - 1] = '\0';
    }

    EndJoin(returnval, "]\n", bufsize);
    DeleteHubQuery(hq, DeleteHubFileDiff);

    CFDB_Close(&dbconn);

    return true;
}

/*****************************************************************************/

int Nova2Txt_list_all_goals(char *buffer, int bufsize)
{
    if (Nova_GetUniqueBusinessGoals(buffer, bufsize))
    {
        return true;
    }
    else
    {
        return false;
    }

}

/*****************************************************************************/

int Nova2Txt_get_bundle_count(char *buffer, int bufsize)
{
    mongo_connection dbconn;
    int bundleCount = 0;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return -1;
    }

    bundleCount = CFDB_QueryBundleCount(&dbconn);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    snprintf(buffer, bufsize, "%d", bundleCount);

    return true;
}

/*****************************************************************************/

int Nova2Txt_get_promise_count(char *buffer, int bufsize)
{
    mongo_connection dbconn;
    int promiseCount = 0;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return -1;
    }

    promiseCount = CFDB_QueryPromiseCount(&dbconn);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    snprintf(buffer, bufsize, "%d", promiseCount);

    return true;
}

/*****************************************************************************/
/* Topic Map                                                                 */
/*****************************************************************************/

int Nova2Txt_docroot(char *buffer, int bufsize)
{
    Nova_WebTopicMap_Initialize();
    snprintf(buffer, bufsize, "%s", DOCROOT);
    return true;
}

/*****************************************************************************/

int Nova2Txt_search_topics(char *search, int regex, char *buffer, int bufsize)
{
    int pid;

    if ((pid = Nova_SearchTopicMap(search, buffer, bufsize)))
    {
        return true;
    }

    return false;
}

/*****************************************************************************/

void Nova2Txt_show_topic(int id, char *buffer, int bufsize)
{
    char topic_name[CF_BUFSIZE], topic_id[CF_BUFSIZE], topic_context[CF_BUFSIZE];

    buffer[0] = '\0';

    if (Nova_GetTopicByTopicId(id, topic_name, topic_id, topic_context))
    {
        snprintf(buffer, bufsize, "{\"topic\":\"%s\",\"context\":\"%s\"}", topic_name, topic_context);
    }
    else
    {
        snprintf(buffer, bufsize, "No such topic\n");
    }
}

/*****************************************************************************/

void Nova2Txt_show_topic_leads(int id, char *buffer, int bufsize)
{
    Item *ip;
    Item *list = Nova_ScanLeadsAssociations(id, NULL);
    char work[CF_BUFSIZE];

    DebugListItemList(list);

    buffer[0] = '\0';
    strcpy(buffer, "[ ");

// name contains the association
// classes contains the related topic
// counter contains the topic id

    for (ip = list; ip != NULL; ip = ip->next)
    {
        if (ip == list)
        {
            snprintf(work, CF_BUFSIZE, "{ \"assoc\": \"%s\", \"topics\": [", ip->name);
            Join(buffer, work, bufsize);
        }

        snprintf(work, CF_BUFSIZE, "{ \"topic\": \"%s\", \"id\": %d },", ip->classes, ip->counter);
        Join(buffer, work, bufsize);

        if (ip->next && strcmp(ip->name, ip->next->name) != 0)
        {
            strcpy(buffer + strlen(buffer) - 1, "]},");
            snprintf(work, CF_BUFSIZE, "{ \"assoc\": \"%s\", \"topics\": [", ip->next->name);
            Join(buffer, work, bufsize);
        }
    }

    strcpy(buffer + strlen(buffer) - 1, "]} ]");
}

/*****************************************************************************/

void Nova2Txt_show_topic_hits(int id, char *buffer, int bufsize)
{
    buffer[0] = '\0';
    Nova_ScanOccurrences(id, buffer, bufsize);
}

/*****************************************************************************/

void Nova2Txt_show_topic_category(int id, char *buffer, int bufsize)
{
    buffer[0] = '\0';
    Nova_ScanTheRest(id, buffer, bufsize);
}

/*****************************************************************************/
/* Hosts stats                                                               */
/*****************************************************************************/

char *Nova2Txt_GetPromiseComment(char *handle)
{
    static char buffer[CF_BUFSIZE];
    mongo_connection dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    if (CFDB_QueryPromiseAttr(&dbconn, handle, cfp_comment, buffer, CF_BUFSIZE))
    {
        return buffer;
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return "No comment";
}

/*****************************************************************************/

void Nova2Txt_GetPromiseBody(char *name, char *type, char *returnval, int bufsize)
{
    char work[CF_BUFSIZE];
    mongo_connection dbconn;
    HubBody *hb;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return;
    }

    hb = CFDB_QueryBody(&dbconn, type, name);

    if (hb)
    {
        snprintf(returnval, CF_MAXVARSIZE - 1, "{");

        snprintf(work, CF_MAXVARSIZE - 1, "\"Type\":\"%s\",", hb->bodyType);
        Join(returnval, work, bufsize);

        snprintf(work, CF_MAXVARSIZE - 1, "\"Name\":\"%s\",", hb->bodyName);
        Join(returnval, work, bufsize);

        if (hb->bodyArgs)
        {
            snprintf(work, CF_MAXVARSIZE - 1, "\"Arguments\":\"%s\",", hb->bodyArgs);
            Join(returnval, work, bufsize);
        }

        if (hb->attr)
        {
            HubBodyAttr *ha;

            Join(returnval, "\"attributes\":[", bufsize);
            for (ha = hb->attr; ha != NULL; ha = ha->next)
            {
                snprintf(work, CF_MAXVARSIZE - 1, "{\"lval\":\"%s\",\"rval\":\"%s\",\"class_context\":\"%s\"},",
                         ha->lval, ha->rval, ha->classContext);
                Join(returnval, work, bufsize);
            }

            ReplaceTrailingChar(returnval, ',', '\0');
            EndJoin(returnval, "]", bufsize);
        }

        DeleteHubBody(hb);
        EndJoin(returnval, "}", bufsize);
    }

    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }
}

/*****************************************************************************/
/* Reports                                                                   */
/*****************************************************************************/
void Nova2Txt_select_reports(char *buffer, int bufsize)
{
    char work[CF_MAXVARSIZE];
    int i;

    buffer[0] = '\0';
    strcat(buffer, "[");

    for (i = 0; i < cfrep_unknown; i++)
    {
        snprintf(work, CF_MAXVARSIZE, "[\"%s\",\"%s\"]", BASIC_REPORTS[i].category, BASIC_REPORTS[i].name);
        if (i != cfrep_unknown - 1)
        {
            strcat(work, ",");
        }
        Join(buffer, work, bufsize);
    }

    ReplaceTrailingChar(buffer, ',', '\0');
    EndJoin(buffer, "]", bufsize);
}

/*****************************************************************************/

int Nova2Txt_list_promise_handles_with_comments(char *bundle, char *btype, char *returnval, int bufsize)
{
    mongo_connection dbconn;
    char work[CF_BUFSIZE];
    Rlist *rp;
    HubQuery *hq;
    HubPromise *hp;

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    hq = CFDB_QueryHandlesForBundlesWithComments(&dbconn, btype, bundle);

    CFDB_Close(&dbconn);

    returnval[0] = '\0';

    if (hq)
    {
        StartJoin(returnval, "[", bufsize);

        for (rp = hq->records; rp != NULL; rp = rp->next)
        {
            hp = (HubPromise *) rp->item;
            snprintf(work, CF_BUFSIZE - 1, "[\"%s\", \"%s\"],", (char *) hp->handle, (char *) hp->comment);
            Join(returnval, work, bufsize);
        }

        ReplaceTrailingChar(returnval, ',', '\0');
        EndJoin(returnval, "]", bufsize);

        DeleteHubQuery(hq, DeleteHubPromise);

        return true;
    }
    else                        // no result
    {
        return false;
    }

}

/*****************************************************************************/

void Nova2Txt_ComplianceSummaryGraph(char *policy, char *buffer, int bufsize)
{
    ComplianceSummaryGraph(NULL, policy, false, buffer, bufsize);
}

/*****************************************************************************/

int Nova2Txt_validate_policy(char *file, char *buffer, int bufsize)
{
    char cmd[CF_BUFSIZE];
    char tmp[CF_MAXVARSIZE];
    int retVal;
    FILE *pp;

    // NOTE: this is run as the web user

    snprintf(cmd, CF_BUFSIZE, "/var/cfengine/bin/cf-promises -n -f \"%s\"", file);

    if ((pp = cf_popen(cmd, "r")) == NULL)
    {
        snprintf(buffer, bufsize, "Could not run command \"%s\": %s", cmd, GetErrorStr());
        return -1;
    }

    buffer[0] = '\0';

    while (!feof(pp))
    {
        if (fgets(tmp, sizeof(tmp), pp))
        {
            Join(buffer, tmp, bufsize);
        }
    }

    retVal = cf_pclose(pp);
    return retVal;              // 0 on success
}
