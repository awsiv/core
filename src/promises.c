/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: promises.c                                                          */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "env_context.h"
#include "vars.h"
#include "expand.h"

#ifdef HAVE_LIBMONGOC
#include "db_save.h"
#include "db_query.h"
#endif

#ifdef HAVE_ZONE_H
# include <zone.h>
#endif

PromiseIdent *PROMISER_LIST[CF_HASHTABLESIZE] = { NULL };

PromiseIdent *PROMISER_REGEXES = NULL;

static void Nova_DefineHubMaster(void);

/*****************************************************************************/

static void PrependPromiserList(PromiseIdent **list, char *s, Promise *pp)
{
    PromiseIdent *ptr;

    for (ptr = *list; ptr != NULL; ptr = ptr->next)
    {
        if (pp->offset.line == ptr->line_number)
        {
            if (strcmp(pp->audit->filename, ptr->filename) == 0)
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

void NewPromiser(Promise *pp)
{
    int hash;
    char unique[CF_BUFSIZE];

    if (strcmp(pp->agentsubtype, "methods") == 0)
    {
        return;                 // Method promisers are not real objects
    }

    if (strcmp(pp->agentsubtype, "delete_lines") == 0)
    {
        return;                 // Deletion cannot confict
    }

    if (IsNakedVar(pp->promiser, '$') || strcmp(pp->agentsubtype, "vars") == 0
        || strcmp(pp->agentsubtype, "classes") == 0)
    {
        snprintf(unique, CF_BUFSIZE, "%s: %s (%s)", pp->agentsubtype, pp->promiser, pp->bundle);
    }
    else
    {
        snprintf(unique, CF_BUFSIZE, "%s: %s", pp->agentsubtype, pp->promiser);
    }

    hash = GetHash(unique);

    if (IsRegex(pp->promiser) && (strchr(pp->promiser, '*') || strchr(pp->promiser, '+')))
    {
        PrependPromiserList(&PROMISER_REGEXES, unique, pp);
    }
    else
    {
        PrependPromiserList(&(PROMISER_LIST[hash]), unique, pp);
    }
}

/*****************************************************************************/

void AnalyzePromiseConflicts(void)
{
    int i;
    PromiseIdent *p1, *p2;
    Rlist *contexts1, *contexts2;
    Promise *pp = NULL;

// There should be no duplicate entries in these lists when we get here

    for (i = 0; i < CF_HASHTABLESIZE; i++)
    {
        if (PROMISER_LIST[i])
        {
            if (PROMISER_LIST[i]->next)
            {
                for (p1 = PROMISER_LIST[i]; p1 != NULL; p1 = p1->next)
                {
                    int count = 0;

                    for (p2 = PROMISER_LIST[i]; p2 != NULL; p2 = p2->next)
                    {
                        if (p1 == p2)
                        {
                            continue;
                        }

                        if (strcmp(p1->handle, p2->handle) == 0)
                        {
                            count++;
                        }
                    }

                    if (count < 1)      // Could be a hashing ghost
                    {
                        continue;
                    }

                    contexts1 = SplitContextExpression(p1->classes, pp);

                    count = 0;

                    for (p2 = p1->next; p2 != NULL; p2 = p2->next)
                    {
                        if (strcmp(p1->handle, p2->handle) != 0)        // Can be hash collisions
                        {
                            continue;
                        }

                        contexts2 = SplitContextExpression(p2->classes, pp);

                        if (Nova_ClassesIntersect(contexts1, contexts2))
                        {
                            if (count++ == 0)
                            {
                                CfOut(cf_inform, "",
                                      "I: The resource \"%s\" makes a number of possibly conflicting promises: ",
                                      p1->handle);
                                CfOut(cf_inform, "", "\n %d: In %s\n    Near line %d\n    In the context of \"%s\"\n",
                                      count, p1->filename, p1->line_number, p1->classes);
                                count++;
                            }
                            CfOut(cf_inform, "", "\n %d: In %s\n    Near line %d\n    In the context of \"%s\"\n",
                                  count, p2->filename, p2->line_number, p2->classes);
                        }

                        DeleteRlist(contexts2);
                    }

                    DeleteRlist(contexts1);
                }
            }

            // Check if there are any patterns that might overlap

            for (p1 = PROMISER_REGEXES; p1 != NULL; p1 = p1->next)
            {
                if (FullTextMatch(p1->handle, PROMISER_LIST[i]->handle))
                {
                    CfOut(cf_inform, "", " ! promiser \"%s\" might conflict with \"%s\" in file %s near line %d\n",
                          p1->handle, PROMISER_LIST[i]->handle, PROMISER_LIST[i]->filename,
                          PROMISER_LIST[i]->line_number);
                }
            }
        }
    }

/* Finally check if any of the patterns could overlap one another
   This is a much harder problem....

for (p1 = PROMISER_REGEXES; p1 != NULL; p1=p1->next)
   {
   for (p2 = PROMISER_REGEXES; p2 != NULL; p2=p2->next)
      {
      }
   }

*/
}

/*****************************************************************************/

void EnterpriseContext(void)
{
    if (IsDefinedClass("am_policy_hub"))
    {
        Nova_DefineHubMaster();
    }
}

/*****************************************************************************/
static void Nova_DefineHubMaster(void)
{
#ifdef HAVE_LIBMONGOC
    char master[CF_MAXVARSIZE] = { 0 };
    struct hostent *hp;
    struct sockaddr_in cin;

    master[0] = '\0';

    if (CFDB_QueryIsMaster())
    {
        NewClass("am_hub_master");
        snprintf(master, sizeof(master), "%s", POLICY_SERVER);
    }
    else
    {
        char *sp;

        CFDB_QueryMasterIP(master, sizeof(master));

        if ((sp = strchr(master, ':')))
        {
            *sp = '\0';
        }
    }

    if (!NULL_OR_EMPTY(master) && !IsIPV4Address(master) && !IsIPV6Address(master))
    {
        if ((hp = gethostbyname(master)) == NULL)
        {
            CfOut(cf_verbose, "", "Hostname lookup failed on node name \"%s\"\n", master);
        }
        else
        {
            memset(&cin, 0, sizeof(cin));
            cin.sin_addr.s_addr = ((struct in_addr *) (hp->h_addr))->s_addr;
            CfOut(cf_verbose, "", "Address given by nameserver: %s\n", inet_ntoa(cin.sin_addr));
            master[0] = '\0';
            strcpy(master, inet_ntoa(cin.sin_addr));
        }
    }

    NewScalar("sys", "hub_master", master, cf_str);
#endif
}

/*****************************************************************************/

void PreSanitizePromise(Promise *pp)
{
    Attributes a = { {0} };

    /* Attempt run-time-like analysis here before execution if possible */

    if (strcmp("processes", pp->agentsubtype) == 0)
    {
        return;
    }

    if (strcmp("storage", pp->agentsubtype) == 0)
    {
        return;
    }

    if (strcmp("packages", pp->agentsubtype) == 0)
    {
        return;
    }

    if (strcmp("files", pp->agentsubtype) == 0)
    {
        a = GetFilesAttributes(pp);
        FileSanityChecks(pp->promiser, a, pp);
        return;
    }

    if (strcmp("commands", pp->agentsubtype) == 0)
    {
        return;
    }

    if (strcmp("databases", pp->agentsubtype) == 0)
    {
        return;
    }

    if (strcmp("methods", pp->agentsubtype) == 0)
    {
        return;
    }

    if (strcmp("reports", pp->agentsubtype) == 0)
    {
        return;
    }
}

/*****************************************************************************/

const char *Nova_Version(void)
{
    return NOVA_VERSION;
}

/*****************************************************************************/

const char *Nova_NameVersion(void)
{
    return "CFEngine Nova " NOVA_VERSION;
}

/***************************************************************/

void NotePromiseCompliance(const Promise *pp, double val, PromiseState state, char *reason)
{
    CF_DB *dbp;
    FILE *fp;
    char promiseHandle[CF_MAXVARSIZE], newNoRepeatId[CF_BUFSIZE];
    static char oldNoRepeatId[CF_BUFSIZE] = { 0 };
    Event e, newe;
    time_t now = time(NULL);
    double vstatus;             /* end with a rough probability */

    CfDebug("Note Promise Compliance\n");

    strncpy(promiseHandle, PromiseID(pp), sizeof(promiseHandle) - 1);
    snprintf(newNoRepeatId, sizeof(newNoRepeatId), "%s:%s", promiseHandle, reason);

    if (strcmp(newNoRepeatId, oldNoRepeatId) == 0)
    {
        // TODO: this is a bit hackish, preferably we should fix this earlier in callstack
        return;
    }

    strncpy(oldNoRepeatId, newNoRepeatId, sizeof(oldNoRepeatId) - 1);

    switch (state)
    {
    case PROMISE_STATE_KEPT:
    case PROMISE_STATE_ANY:
        vstatus = 1.0;
        break;

    case PROMISE_STATE_REPAIRED:
        vstatus = 0.5;
        break;

    case PROMISE_STATE_NOTKEPT:
        vstatus = 0;
        break;
    default:
        return;
    }

    if (!OpenDB(&dbp, dbid_promise_compliance))
    {
        return;
    }

/* First record the classes that are in use */

    if (ReadDB(dbp, promiseHandle, &e, sizeof(e)))
    {
        newe.t = now;
        newe.Q = QAverage(e.Q, vstatus, 0.5);
    }
    else
    {
        newe.t = now;
        newe.Q.q = vstatus;
        newe.Q.expect = 0.5 * vstatus;  /* With no data it's 50/50 what we can say */
        newe.Q.var = 0.000;
    }

    WriteDB(dbp, promiseHandle, &newe, sizeof(newe));
    CloseDB(dbp);

/* Just noise to log variables and classes */

    if (pp->agentsubtype == NULL)
    {
        return;
    }

/* Now keep the next log */

    char name[CF_BUFSIZE];

    switch (state)
    {
    case PROMISE_STATE_REPAIRED:
        snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, CF_REPAIR_LOG);
        break;

    case PROMISE_STATE_NOTKEPT:
        snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, CF_NOTKEPT_LOG);
        break;

    default:
        return;
    }

    if ((fp = fopen(name, "a")) == NULL)
    {
        CfOut(cf_error, "fopen", " !! Could not open \"%s\"", name);
        return;
    }

    fprintf(fp, "%ld,%s,%s", (long) now, promiseHandle, reason);

    if (reason[strlen(reason) - 1] != '\n')
    {
        fprintf(fp, "\n");
    }

    fclose(fp);
    chmod(name, 0644);
}


void TrackValue(char *date, double kept, double repaired, double notkept)
{
    char month[CF_SMALLBUF], day[CF_SMALLBUF], year[CF_SMALLBUF], key[CF_SMALLBUF];
    CF_DB *dbp;
    PromiseValue value, new_value;

// Strip out the date resolution so we keep only each day of the year

    sscanf(date, "%*s %s %s %*s %s", month, day, year);
    snprintf(key, CF_SMALLBUF - 1, "%s %s %s", day, month, year);

    if (!OpenDB(&dbp, dbid_value))
    {
        return;
    }

    if (ReadDB(dbp, key, &value, sizeof(PromiseValue)))
    {
        new_value.kept = value.kept + kept;
        new_value.repaired = value.repaired + repaired;
        new_value.notkept = value.notkept + notkept;
    }
    else
    {
        new_value.kept = kept;
        new_value.repaired = repaired;
        new_value.notkept = notkept;
    }

    CfOut(cf_verbose, "", " -> recording value (%.4lf,%.4lf,%.4lf)", kept, repaired, notkept);
    WriteDB(dbp, key, &new_value, sizeof(PromiseValue));
    CloseDB(dbp);
}

/*****************************************************************************/

void LastSawBundle(const Bundle *bundle, double compliance)
{
    Event e, newe;
    time_t now = time(NULL);
    CF_DB *dbp;

    if (!OpenDB(&dbp, dbid_bundles))
    {
       return;
    }
    
    if (ReadDB(dbp, bundle->name, &e, sizeof(e)))
    {
        newe.Q = QAverage(e.Q, compliance, 0.7);
    }
    else
    {
        newe.Q = QDefinite(compliance);
    }

    newe.t = now;
    
    if (THIS_AGENT_TYPE == cf_agent)
    {
        char *fqname = BundleQualifiedName(bundle);
        WriteDB(dbp, fqname, &newe, sizeof(newe));
        free(fqname);
    }

    CloseDB(dbp);
}

/********************************************************************/

int Nova_ClassesIntersect(Rlist *contexts1, Rlist *contexts2)
{
    Rlist *rp1, *rp2;

    for (rp1 = contexts1; rp1 != NULL; rp1 = rp1->next)
    {
        for (rp2 = contexts2; rp2 != NULL; rp2 = rp2->next)
        {
            // Should strictly check the commutation on [.&] also

            if (strcmp(rp1->item, rp2->item) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

/*****************************************************************************/

void AddGoalsToDB(char *goal_patterns)
{
#ifdef HAVE_LIBMONGOC
    CFDB_SaveGoalsCache(goal_patterns);
#endif
}

/*****************************************************************************/
