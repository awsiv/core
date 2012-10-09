/*

This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "env_context.h"
#include "files_names.h"
#include "scorecards.h"
#include "db_query.h"
#include "conversion.h"
#include "bson_lib.h"

/*****************************************************************************/

int Nova2Txt_summary_report(char *hostkey, char *handle, char *status, bool regex, char *classreg)
/*
  Return current best-knowledge of average compliance for the class of hosts and promises selected
 */
{
    char buf1[CF_SMALLBUF], buf2[CF_SMALLBUF];
    HubPromiseCompliance *hp;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;
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

    hq = CFDB_QueryPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0, time(NULL), false, filter, PROMISE_CONTEXT_MODE_ALL);
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
        CFDB_GetHostColour(hp->hh->keyhash, HOST_RANK_METHOD_COMPLIANCE, &colour,
                           PROMISE_CONTEXT_MODE_ALL);

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

int Nova2Txt_software_report(char *hostkey, char *name, char *value, char *arch, bool regex, char *type, char *classreg)
{
    HubSoftware *hs;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }
    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QuerySoftware(&dbconn, hostkey, type, name, value, arch, regex,
                            filter, true, PROMISE_CONTEXT_MODE_ALL, NULL);
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


int Nova2Txt_vars_report(const char *hostkey, const char *ns, const char *bundle, const char *lval, const char *rval, const char *type, bool regex, char *classreg)
{
    char rvalBuf[CF_MAXVARSIZE];
    HubVariable *hv;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryVariables(&dbconn, hostkey, ns, bundle, lval, rval, type, regex,
                             0, time(NULL), filter, PROMISE_CONTEXT_MODE_ALL, NULL);

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
            printf("%s,%s,%s:%s.%s,%s\n", hv->hh->hostname, typestr, hv->ns, hv->bundle, hv->lval, rvalBuf);
        }
        else
        {
            printf("%25s %14s %s:%s.%-25s %s\n", hv->hh->hostname, typestr, hv->ns, hv->bundle, hv->lval, rvalBuf);
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

int Nova2Txt_compliance_report(char *hostkey, char *version, time_t from, time_t to, int k, int nk, int rep,
                               char *classreg)
{
    char buffer[CF_BUFSIZE];
    HubTotalCompliance *ht;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryTotalCompliance(&dbconn, hostkey, version, from, to, k, nk, rep, true, filter,
                                   PROMISE_CONTEXT_MODE_ALL, NULL);

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

int Nova2Txt_compliance_promises(char *hostkey, char *handle, char *status, bool regex, char *classreg)
{
    char buffer[CF_BUFSIZE];
    HubPromiseCompliance *hp;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;

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

    hq = CFDB_QueryPromiseCompliance(&dbconn, hostkey, handle, *status, regex, 0, time(NULL), true, filter, PROMISE_CONTEXT_MODE_ALL);
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
    EnterpriseDB dbconn;
    char inout[CF_SMALLBUF];

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryLastSeen(&dbconn, hostkey, lhash, lhost, laddress, lago, lregex,
                            0, time(NULL), true, filter, PROMISE_CONTEXT_MODE_ALL, NULL);

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
    EnterpriseDB dbconn;
    time_t then;
    time_t now = time(NULL);

/* BEGIN query document */

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryLastSeen(&dbconn, hostkey, lhash, lhost, laddress, lago, lregex,
                            0, time(NULL), true, filter, PROMISE_CONTEXT_MODE_ALL, NULL);

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

int Nova2Txt_setuid_report(char *hostkey, char *file, bool regex, char *classreg)
{
    HubSetUid *hS;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QuerySetuid(&dbconn, hostkey, file, regex, filter, PROMISE_CONTEXT_MODE_ALL, NULL);
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


int Nova2Txt_filechanges_report(char *hostkey, char *file, bool regex, time_t from, time_t to, char *classreg)
{
    char buffer[CF_SMALLBUF];
    HubFileChanges *hC;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryFileChanges(&dbconn, hostkey, file, regex, from, to, true,
                               filter, PROMISE_CONTEXT_MODE_ALL, NULL);

    DeleteHostClassFilter(filter);

    if (!CSV)
    {
        printf("%s,%s,%s\n", "Host", "File", "Changed-on");
    }

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hC = (HubFileChanges *) rp->item;

        if (CSV)
        {
            printf("%s,%s,%s\n", hC->hh->hostname, hC->path, cf_strtimestamp_local(hC->t, buffer));
        }
        else
        {
            printf("%25s %40s  %20s\n", hC->hh->hostname, hC->path, cf_strtimestamp_local(hC->t, buffer));
        }
    }

    DeleteHubQuery(hq, DeleteHubFileChanges);
    CFDB_Close(&dbconn);
    return true;
}

/*****************************************************************************/

int Nova2Txt_filediffs_report(char *hostkey, char *file, char *diffs, bool regex, time_t from, time_t to, char *classreg)
{
    HubFileDiff *hd;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    HostClassFilter *filter = NewHostClassFilter(classreg, NULL);

    hq = CFDB_QueryFileDiff(&dbconn, hostkey, file, diffs, regex, from, to, true,
                            filter, PROMISE_CONTEXT_MODE_ALL, NULL);

    DeleteHostClassFilter(filter);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        hd = (HubFileDiff *) rp->item;

        char *diff = NULL;
        {
            JsonElement *diff_json = Nova_FormatDiff(hd->diff);
            Writer *writer = StringWriter();
            JsonElementPrint(writer, diff_json, 2);
            diff = StringWriterClose(writer);

            JsonElementDestroy(diff_json);
        }

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

            printf("%s,%s,%ld,%s\n", hd->hh->hostname, hd->path, hd->t, diff);
        }
        else
        {
            printf("%s file %s changed on %ld\n%s\n", hd->hh->hostname, hd->path, hd->t, diff);
        }

        free(diff);
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
    EnterpriseDB dbconn;
    bson query;

    if (NULL_OR_EMPTY(hostkey))
    {
        return false;
    }

    if (!CFDB_Open(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not open connection to report database");
        return false;
    }

    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, hostkey);
    BsonFinish(&query);

    hq = CFDB_QueryHosts(&dbconn, MONGO_DATABASE, &query);
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

    DeleteHubQuery(hq, free);

    ReplaceTrailingChar(hostnameOut, ' ', '\0');
    ReplaceTrailingChar(ipaddrOut, ' ', '\0');

    CFDB_Close(&dbconn);

    return true;
}
