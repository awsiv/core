
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "nova_reports.h"

#include "cfstream.h"
#include "cmd_api.h"
#include "conversion.h"
#include "datapack.h"
#include "db_common.h"
#include "db_maintain.h"
#include "db_save.h"
#include "env_context.h"
#include "files_interfaces.h"
#include "item_lib.h"
#include "logging.h"
#include "scope.h"
#include "string_lib.h"
#include "sort.h"
#include "files_names.h"
#include "nova_reporting.h"

 /* These should be coordinated with cfreport.c - but not copied */

enum cf_format
{
    cfx_entry,
    cfx_event,
    cfx_host,
    cfx_pm,
    cfx_ip,
    cfx_date,
    cfx_q,
    cfx_av,
    cfx_dev,
    cfx_version,
    cfx_ref,
    cfx_filename,
    cfx_index,
    cfx_min,
    cfx_max,
    cfx_end,
    cfx_kept,
    cfx_repaired,
    cfx_notrepaired
};

static const char *NRX[][2] =
{
    {"<entry>\n", "\n</entry>\n"},
    {"<event>\n", "\n</event>\n"},
    {"<hostname>\n", "\n</hostname>\n"},
    {"<pm>\n", "\n</pm>\n"},
    {"<ip>\n", "\n</ip>\n"},
    {"<date>\n", "\n</date>\n"},
    {"<q>\n", "\n</q>\n"},
    {"<expect>\n", "\n</expect>\n"},
    {"<sigma>\n", "\n</sigma>\n"},
    {"<version>\n", "\n</version>\n"},
    {"<ref>\n", "\n</ref>\n"},
    {"<filename>\n", "\n</filename>\n"},
    {"<index>\n", "\n</index>\n"},
    {"<min>\n", "\n</min>\n"},
    {"<max>\n", "\n</max>\n"},
    {"<end>\n", "\n</end>\n"},
    {"<kept>\n", "\n</kept>\n"},
    {"<repaired>\n", "\n</repaired>\n"},
    {"<notrepaired>\n", "\n</notrepaired>\n"},
    {NULL, NULL}
};

static const char *NRH[][2] =
{
    {"<tr>", "</tr>\n\n"},
    {"<td>", "</td>\n"},
    {"<td>", "</td>\n"},
    {"<td bgcolor=#add8e6>", "</td>\n"},
    {"<td bgcolor=#e0ffff>", "</td>\n"},
    {"<td bgcolor=#f0f8ff>", "</td>\n"},
    {"<td bgcolor=#fafafa>", "</td>\n"},
    {"<td bgcolor=#ededed>", "</td>\n"},
    {"<td bgcolor=#e0e0e0>", "</td>\n"},
    {"<td bgcolor=#add8e6>", "</td>\n"},
    {"<td bgcolor=#e0ffff>", "</td>\n"},
    {"<td bgcolor=#fafafa><small>", "</small></td>\n"},
    {"<td bgcolor=#fafafa><small>", "</small></td>\n"},
    {"<td>", "</td>\n"},
    {"<td>", "</td>\n"},
    {"<td>", "</td>\n"},
    {"<td>", "</td>\n"},
    {"<td>", "</td>\n"},
    {"<td>", "</td>\n"},
    {"<td>", "</td>\n"},
    {NULL, NULL}
};

static const char *NUMBER_TXT[] =
{
    "zero",
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine",
    "ten",
    "eleven",
    "twelve",
    "thirteen",
    "fourteen",
    "fifteen",
    "sixteen",
    NULL
};

#ifdef HAVE_LIBMONGOC

static int Nova_ImportHostReportsFromStream(EnterpriseDB *dbconn, char *header, FILE *fin);

static int Nova_ImportHostReports(EnterpriseDB *dbconnp, const char *filePath)
/*
 * Import from text file to Mongo database.
 * NOTE: Should only be called on Nova hub.
 */
{
    EnterpriseDB dbconn;
    char header[CF_MAXVARSIZE];
    FILE *fin;

    if (LICENSES == 0)
    {
        CfOut(cf_error, "", " !! The license has expired");
        return false;
    }

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    if ((fin = fopen(filePath, "r")) == NULL)
    {
        CfOut(cf_error, "fopen", "!! Cannot open import file %s", filePath);
        CFDB_Close(&dbconn);
        return false;
    }

    if (CfReadLine(header, sizeof(header), fin) == -1)
    {
        FatalError("Error in CfReadLine");
    }

    Nova_ImportHostReportsFromStream(&dbconn, header, fin);

    fclose(fin);
    CFDB_Close(&dbconn);

    return true;
}

/*********************************************************************/

static int Nova_ImportHostReportsFromStream(EnterpriseDB *dbconn, char *header, FILE *fin)
{
    char keyHash[CF_MAXVARSIZE] = { 0 }, ipAddr[CF_MAXVARSIZE] = { 0 }, hostName[CF_MAXVARSIZE] = { 0 };
    char buf[CF_BUFSIZE];
    char headerText[CF_SMALLBUF], reportType[CF_SMALLBUF];
    char validate[5];
    time_t delta1, genTime;
    long length;
    int currReport = -1;

    // OK to leave hostname blank - reverse lookup later
    if (sscanf(header, "%32s %32s %255s %255s %255s", headerText, reportType, keyHash, ipAddr, hostName) < 4)
    {
        CfOut(cf_error, "", "!! Error parsing first line of report header");
        return false;
    }

    if (CfReadLine(buf, sizeof(buf), fin) == -1)
    {
        FatalError("Error in CfReadLine");
    }

    if (sscanf(buf, "%4s %ld %ld %ld", validate, &delta1, &genTime, &length) != 4)
    {
        CfOut(cf_error, "", "!! Error parsing second line of report header");
        return false;
    }

    if (strcmp(validate, "CFR:") != 0)
    {
        CfOut(cf_error, "", " !! Invalid report format in - second line is %s not CFR:", validate);
        return false;
    }

    cf_strtimestamp_utc(genTime, buf);

    CfOut(cf_inform, "", " -> Importing Nova %s reports from host %s with UTC timestamp %s", reportType, keyHash, buf);

    Item **reports = NewReportBook();

    {
        ssize_t num_read = 0;
        while ((num_read = CfReadLine(buf, sizeof(buf), fin)) > 0)
        {
            CfDebug("%s\n", buf);
            currReport = Nova_StoreIncomingReports(buf, reports, currReport);
        }

        if (num_read == -1)
        {
            FatalError("Error in CfReadLine");
        }
    }

    CFDB_SaveHostID(dbconn, MONGO_DATABASE, cfr_keyhash, keyHash, ipAddr, hostName);
    CFDB_SaveHostID(dbconn, MONGO_ARCHIVE, cfr_keyhash, keyHash, ipAddr, hostName);

    UnpackReportBook(dbconn, keyHash, reports);
    DeleteReportBook(reports);

    return true;
}

#endif /*  HAVE_LIBMONGOC */


void Nova_ImportReports(const char *input_file)
{
#ifdef HAVE_LIBMONGOC
    if (IsDefinedClass("am_policy_hub", NULL) || getenv("CFENGINE_TEST"))
    {
        EnterpriseDB dbconn;

        if (!CFDB_Open(&dbconn))
        {
            CfOut(cf_error, "", "!! Could not connect to database -- skipping import");
            exit(0);
        }

        CFDB_EnsureIndices(&dbconn);

        if (Nova_ImportHostReports(&dbconn, input_file))
        {
            exit(0);
        }
        else
        {
            exit(1);
        }
    }
    else
    {
        CfOut(cf_error, "", "Importing reports is only possible on Nova policy hubs");
    }

#else /* NOT HAVE_LIBMONGOC */
    CfOut(cf_error, "", "Importing reports is only possible on Nova policy hubs");
#endif
}

/*
 * Export from dbm files to one text file.
 */

int Nova_ExportReports(char *reportName)
{
    Item *reports = NULL, *ip;
    enum cfd_menu reportType;
    time_t from;
    char filePath[CF_MAXVARSIZE];
    FILE *fout;

    if (LICENSES == 0)
    {
        CfOut(cf_error, "", " !! The license has expired");
        return false;
    }

    reportType = String2Menu(reportName);

    switch (reportType)
    {
    case cfd_menu_delta:
        from = time(NULL) - 60 * 10;    // delta = last 10 minutes
        break;
    case cfd_menu_full:
        from = time(NULL) - SECONDS_PER_WEEK;   // full = last week
        break;
    default:
        CfOut(cf_error, "", "!! Nova_ExportReports: reportType is not delta or full but %d", reportType);
        return false;
    }

    snprintf(filePath, sizeof(filePath), "%s/reports/nova_export.nov", CFWORKDIR);
    MapName(filePath);

    if ((fout = fopen(filePath, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "!! Cannot open export file path %s", filePath);
        return false;
    }

    CfOut(cf_inform, "", " -> Saving all Nova reports to %s", filePath);

    fprintf(fout, "%s %s %s %s %s\n", NOVA_EXPORT_HEADER, reportName, PUBKEY_DIGEST, VIPADDRESS, VFQNAME);

    Nova_PackAllReports(&reports, from, 0, reportType);

    for (ip = reports; ip != NULL; ip = ip->next)
    {
        ReplaceTrailingChar(ip->name, '\n', '\0');
        fprintf(fout, "%s\n", ip->name);
    }

    DeleteItemList(reports);
    fclose(fout);

    return true;
}

void Nova_CommandAPI(char *lsdata, char *name, char *phandle, char *hostkey, char *classregex)
{
#ifdef HAVE_LIBMONGOC
    int plen = strlen(phandle);
    int slen = strlen(lsdata);
    int hlen = strlen(hostkey);
    int clen = strlen(classregex);

    if (hlen > 0 && slen == 0 && plen == 0 && clen == 0)
    {
        // Show host details
        char name[CF_MAXVARSIZE] = { 0 }, address[CF_MAXVARSIZE] = { 0 };

        Nova2Txt_hostinfo(hostkey, name, address, CF_MAXVARSIZE);
        printf(" -> Hostname: %s\n", name);
        printf(" -> Recent IP Addresses: %s\n", address);
        return;
    }

    if (strcmp(lsdata, "promises") == 0)
    {
        Nova2Txt_compliance_promises(hostkey, phandle, NULL, true, classregex);
        return;
    }
    else if (strcmp(lsdata, "compliance") == 0)
    {
        Nova2Txt_compliance_report(hostkey, name, 0, time(NULL), -1, -1, -1, classregex);
        return;
    }
    else if (strcmp(lsdata, "summary") == 0)
    {
        Nova2Txt_summary_report(hostkey, phandle, NULL, true, classregex);
        return;
    }
    else if (strcmp(lsdata, "dead-clients") == 0)
    {
        Nova2Txt_deadclient_report(hostkey, NULL, NULL, NULL, -1, true, classregex);
        return;
    }
    else if (strcmp(lsdata, "last-seen") == 0)
    {
        Nova2Txt_lastseen_report(hostkey, NULL, NULL, NULL, -1, true, classregex);
        return;
    }
    else if (strcmp(lsdata, "software") == 0)
    {
        Nova2Txt_software_report(hostkey, name, "", "", true, cfr_software, classregex);
        return;
    }
    else if (strcmp(lsdata, "vars") == 0)
    {
        char lval[CF_MAXVARSIZE] = { 0 };
        char scope[CF_MAXVARSIZE] = { 0 };
        char *sp = NULL;

        if (name && (sp = strchr(name, '.')))
        {
            if (*(sp + 1) == '*')       // If it looks like a regex, don't split on .
            {
                Nova2Txt_vars_report(hostkey, NULL, NULL, name, NULL, NULL, true, classregex);
                return;
            }
            else
            {
                sscanf(name, "%[^.].%s", scope, lval);

                char ns[CF_MAXVARSIZE] = { 0 };
                char bundle[CF_MAXVARSIZE] = { 0 };
                if (strlen(scope) > 0)
                {
                    SplitScopeName(scope, ns, bundle);
                }

                Nova2Txt_vars_report(hostkey, ns, bundle, lval, NULL, NULL, true, classregex);
                return;
            }
        }

        Nova2Txt_vars_report(hostkey, NULL, NULL, NULL, NULL, NULL, true, NULL);
        return;
    }
    else if (strcmp(lsdata, "file_changes") == 0)
    {
        Nova2Txt_filechanges_report(hostkey, name, true, 0, time(NULL), classregex);
        return;
    }
    else if (strcmp(lsdata, "file_diffs") == 0)
    {
        Nova2Txt_filediffs_report(hostkey, name, NULL, true, 0, time(NULL), classregex);
        return;
    }
    else if (strcmp(lsdata, "setuid") == 0)
    {
        Nova2Txt_setuid_report(hostkey, name, true, classregex);
        return;
    }

    printf("Unknown report\n");
#endif
}

void SummarizeValue(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web)
{
    CF_DB *dbp;
    CF_DBC *dbcp;
    int ksize, vsize;
    void *value;
    char *key;
    FILE *fout;
    time_t now = time(NULL);
    PromiseValue pt;
    Item *ip, *data = NULL;
    char timebuffer[26];

// Strip out the date resolution so we keep only each day of the year

    if (!OpenDB(&dbp, dbid_value))
    {
        return;
    }

    if (NewDBCursor(dbp, &dbcp))
    {
        while (NextDB(dbp, dbcp, &key, &ksize, &value, &vsize))
        {
            if (value == NULL)
            {
                continue;
            }

            if (sizeof(pt) < vsize)
            {
                CfOut(cf_error, "", "Invalid entry in values database. Expected size: %zu, actual size: %d", sizeof(pt), vsize);
                continue;
            }

            char name[CF_BUFSIZE];
            memcpy(&pt, value, MIN(vsize, sizeof(pt)));
            snprintf(name, CF_BUFSIZE, "<td>%.4lf</td><td>%.4lf</td><td>%.4lf</td>", pt.kept, pt.repaired, pt.notkept);
            AppendItem(&data, key, name);
        }
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);

    if ((fout = fopen("value_report.html", "w")) == NULL)
    {
        CfOut(cf_error, "fopen", " !! Unable to write value report");
        return;
    }

    char name[CF_BUFSIZE];
    snprintf(name, sizeof(name), "Value return from Cfengine on %s", VFQNAME);
    fprintf(fout, "<div id=\"reporttext\">");
    fprintf(fout, "<p>Last measured on %s", cf_strtimestamp_local(now, timebuffer));
    fprintf(fout, "<table class=\"border\">\n");

    fprintf(fout, "<tr><th>Day</th><th>Promises Kept</th><th>Repairs</th><th>Not kept</th></tr>\n");

    for (ip = data; ip != NULL; ip = ip->next)
    {
        fprintf(fout, "<tr><td>%s</td>%s</tr>\n", ip->name, ip->classes);
    }

    fprintf(fout, "</table></div>\n");
    fclose(fout);

    DeleteItemList(data);
}

void SummarizeVariables(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web)
{
    char line[CF_BUFSIZE];
    FILE *fin, *fout;
    char name[CF_MAXVARSIZE];

    snprintf(name, sizeof(name), "%s/state/vars.out", CFWORKDIR);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen", "Cannot open variable notes %s (try again later)", name);
        return;
    }

    if (html)
    {
        snprintf(name, sizeof(name), "variables.html");
    }
    else
    {
        return;
    }

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        return;
    }

    if (html && !embed)
    {
        snprintf(name, sizeof(name), "Main variables instantiated on %s", VFQNAME);
        fprintf(fout, "<div id=\"reporttext\">\n");
    }

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);
        fprintf(fout, "%s", line);
    }

    fclose(fin);

    if (html && !embed)
    {
        fprintf(fout, "</div>\n");
    }

    fclose(fout);
}

void CSV2XML(Rlist *list)
{
    Rlist *rp, *rline, *rl;
    int i;

    for (rp = list; rp != NULL; rp = rp->next)
    {
        FILE *fin, *fout;
        char *sp, name[CF_MAXVARSIZE], line[CF_BUFSIZE];

        if ((fin = fopen(ScalarValue(rp), "r")) == NULL)
        {
            CfOut(cf_inform, "fopen", "Cannot open CSV file %s", ScalarValue(rp));
            continue;
        }

        strncpy(name, rp->item, CF_MAXVARSIZE - 1);

        for (sp = name; *sp != '.' && *sp != '\0'; sp++)
        {
        }

        *sp = '\0';

        strcat(name, ".xml");

        if ((fout = fopen(name, "w")) == NULL)
        {
            CfOut(cf_inform, "fopen", "Cannot open XML file %s", ScalarValue(rp));
            continue;
        }

        CfOut(cf_verbose, "", "Converting %s to %s\n", ScalarValue(rp), name);

        fprintf(fout, "<?xml version=\"1.0\"?>\n<output>\n");

        while (!feof(fin))
        {
            line[0] = '\0';
            fgets(line, CF_BUFSIZE - 1, fin);
            rline = SplitStringAsRList(line, ',');

            fprintf(fout, " <line>\n");

            i = 1;

            for (rl = rline; rl != NULL; rl = rl->next)
            {
                if (NUMBER_TXT[i])
                {
                    fprintf(fout, "   <%s>%s</%s>\n", NUMBER_TXT[i], (char *) rl->item, NUMBER_TXT[i]);
                    i++;
                }
                else
                {
                    CfOut(cf_error, "", "Too many fields in csv file %s\n", ScalarValue(rp));
                    break;
                }
            }

            fprintf(fout, " </line>\n");

            DeleteRlist(rline);
        }

        fprintf(fout, "</output>\n");

        fclose(fin);
        fclose(fout);
    }
}

void SummarizeCompliance(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web)
/* /var/cfengine/promise.log */
{
    FILE *fin, *fout;
    char name[CF_BUFSIZE], line[CF_BUFSIZE];
    Item *ip, *file = NULL;
    char start[32], end[32];
    char version[CF_MAXVARSIZE];
    double kept, repaired, notrepaired;
    int i = 0;
    double av_day_kept = 0, av_day_repaired = 0;
    double av_week_kept = 0, av_week_repaired = 0;
    double av_hour_kept = 0, av_hour_repaired = 0;

    snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, CF_PROMISE_LOG);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the source log %s", name);
        return;
    }

    if (html)
    {
        snprintf(name, CF_BUFSIZE, "compliance.html");
    }
    else if (xml)
    {
        snprintf(name, CF_BUFSIZE, "compliance.xml");
    }
    else if (csv)
    {
        snprintf(name, CF_BUFSIZE, "compliance.csv");
    }
    else
    {
        snprintf(name, CF_BUFSIZE, "compliance.txt");
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);
        PrependItem(&file, line, NULL);
    }

    fclose(fin);

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        return;
    }

    if (html && !embed)
    {
        snprintf(name, CF_BUFSIZE, "Policy compliance summary on %s", VFQNAME);
        fprintf(fout, "<div id=\"reporttext\">\n");
        fprintf(fout, "<table class=border cellpadding=5>\n");
        fprintf(fout, "%s", NRH[cfx_entry][cfb]);
        fprintf(fout, "%s %s %s", NRH[cfx_date][cfb], "Start check", NRH[cfx_date][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_end][cfb], "End check", NRH[cfx_end][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_version][cfb], "Policy version", NRH[cfx_version][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_kept][cfb], "%% scheduled promises kept", NRH[cfx_kept][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_repaired][cfb], "%% scheduled promises repaired", NRH[cfx_repaired][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_notrepaired][cfb], "%% scheduled promises ignored",
                NRH[cfx_notrepaired][cfe]);
        fprintf(fout, "%s", NRH[cfx_entry][cfe]);
    }
    else if (XML)
    {
        fprintf(fout, "<?xml version=\"1.0\"?>\n<output>\n");
    }

    for (ip = file; ip != NULL; ip = ip->next)
    {
        long startt, endt;
        time_t st, et;
        char timebuffer[26];

        kept = repaired = notrepaired = 0;
        memset(start, 0, 32);
        memset(end, 0, 32);
        memset(version, 0, 255);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        // Complex parsing/extraction

        sscanf(ip->name, "%ld,%ld", &startt, &endt);
        st = (time_t) startt;
        et = (time_t) endt;
        snprintf(start, 31, "%s", cf_strtimestamp_local(st, timebuffer));
        snprintf(end, 31, "%s", cf_strtimestamp_local(et, timebuffer));

        if (!strstr(ip->name, "Outcome of version"))
        {
            continue;
        }

        sscanf(strstr(ip->name, "Outcome of version") + strlen("Outcome of version"), "%64[^:]", version);

        sscanf(strstr(ip->name, "Promises observed") + strlen("Promises observed"),
               "%*[^0-9]%lf%*[^0-9]%lf%*[^0-9]%lf",
               &kept, &repaired, &notrepaired);

        if (i < 12 * 24)
        {
            av_day_kept = GAverage((double) kept, av_day_kept, 0.5);
            av_day_repaired = GAverage((double) repaired, av_day_repaired, 0.5);
        }

        if (i < 12 * 2)
        {
            av_hour_kept = GAverage((double) kept, av_hour_kept, 0.5);
            av_hour_repaired = GAverage((double) repaired, av_hour_repaired, 0.5);
        }

        av_week_kept = GAverage((double) kept, av_week_kept, 0.1);
        av_week_repaired = GAverage((double) repaired, av_week_repaired, 0.1);

        if (xml)
        {
            fprintf(fout, "%s", NRX[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRX[cfx_date][cfb], start, NRX[cfx_date][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_end][cfb], end, NRX[cfx_end][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_version][cfb], version, NRX[cfx_version][cfe]);
            fprintf(fout, "%s %d %s", NRX[cfx_kept][cfb], (int)(kept + 0.5), NRX[cfx_kept][cfe]);
            fprintf(fout, "%s %d %s", NRX[cfx_repaired][cfb], (int)(repaired + 0.5), NRX[cfx_repaired][cfe]);
            fprintf(fout, "%s %d %s", NRX[cfx_notrepaired][cfb], (int)(notrepaired + 0.5), NRX[cfx_notrepaired][cfe]);
            fprintf(fout, "%s", NRX[cfx_entry][cfe]);
        }
        else if (html)
        {
            fprintf(fout, "%s", NRH[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRH[cfx_date][cfb], start, NRH[cfx_date][cfe]);
            fprintf(fout, "%s %s %s", NRH[cfx_end][cfb], end, NRH[cfx_end][cfe]);
            fprintf(fout, "%s %s %s", NRH[cfx_version][cfb], version, NRH[cfx_version][cfe]);
            fprintf(fout, "%s %d%% %s", NRH[cfx_kept][cfb], (int)(kept + 0.5), NRH[cfx_kept][cfe]);
            fprintf(fout, "%s %d%% %s", NRH[cfx_repaired][cfb], (int)(repaired + 0.5), NRH[cfx_repaired][cfe]);
            fprintf(fout, "%s %d%% %s", NRH[cfx_notrepaired][cfb], (int)(notrepaired + 0.5), NRH[cfx_notrepaired][cfe]);
            fprintf(fout, "%s", NRH[cfx_entry][cfe]);
        }
        else if (csv)
        {
            fprintf(fout, "%s,%s,%s,%d,%d,%d", start, end, version,
                    (int)(kept + 0.5), (int)(repaired + 0.5), (int)(notrepaired + 0.5));
        }
        else
        {
            fprintf(fout, "%s", ip->name);
        }

        if (++i > 12 * 24 * 7)
        {
            break;
        }
    }

    if (html && !embed)
    {
        fprintf(fout, "</table>");
        fprintf(fout, "</div>");
    }

    if (XML)
    {
        fprintf(fout, "</output>\n");
    }

    fclose(fout);
    DeleteItemList(file);

    METER_KEPT[meter_compliance_week] = av_week_kept;
    METER_REPAIRED[meter_compliance_week] = av_week_repaired;
    METER_KEPT[meter_compliance_day] = av_day_kept;
    METER_REPAIRED[meter_compliance_day] = av_day_repaired;
    METER_KEPT[meter_compliance_hour] = av_week_kept;
    METER_REPAIRED[meter_compliance_hour] = av_hour_repaired;
}

void SummarizePerPromiseCompliance(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot,
                                   char *web)
{
    FILE *fout;
    double lsea = SECONDS_PER_WEEK * 52;        /* expire after a year */
    Event entry;
    Item *ip, *htmlreport = NULL;
    int ksize, vsize;
    CF_DB *dbp;
    CF_DBC *dbcp;
    char *key;
    void *stored;

/* Open the db */

    if (!OpenDB(&dbp, dbid_promise_compliance))
    {
        CfOut(cf_verbose, "", "!! Could not open promise compliancedatabase");
        return;
    }

    char name[CF_BUFSIZE];

    if (html)
    {
        snprintf(name, CF_BUFSIZE, "promise_compliance.html");
    }
    else if (xml)
    {
        snprintf(name, CF_BUFSIZE, "promise_compliance.xml");
    }
    else if (csv)
    {
        snprintf(name, CF_BUFSIZE, "promise_compliance.csv");
    }
    else
    {
        snprintf(name, CF_BUFSIZE, "promise_compliance.txt");
    }

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        CloseDB(dbp);
        return;
    }

    if (html && !embed)
    {
        snprintf(name, CF_BUFSIZE, "Promise compliance history on %s", VFQNAME);
        fprintf(fout, "<div id=\"reporttext\">\n");
        fprintf(fout, "<table class=border cellpadding=5>\n");
        fprintf(fout, "%s", NRH[cfx_entry][cfb]);
        fprintf(fout, "%s %s %s", NRH[cfx_date][cfb], "Last checked", NRH[cfx_date][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_event][cfb], "Promise handle/file reference", NRH[cfx_event][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_q][cfb], "Status", NRH[cfx_q][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_av][cfb], "Average", NRH[cfx_av][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_dev][cfb], "Margin", NRH[cfx_dev][cfe]);
        fprintf(fout, "%s", NRH[cfx_entry][cfe]);
    }
    else if (XML)
    {
        fprintf(fout, "<?xml version=\"1.0\"?>\n<output>\n");
    }

/* Acquire a cursor for the database. */

    if (!NewDBCursor(dbp, &dbcp))
    {
        CfOut(cf_inform, "", " !! Unable to scan class db");
        CloseDB(dbp);
        return;
    }

/* Initialize the key/data return pair. */

    while (NextDB(dbp, dbcp, &key, &ksize, &stored, &vsize))
    {
        double measure, av, var;
        time_t then, lastseen, now = time(NULL);
        char tbuf[CF_BUFSIZE], eventname[CF_BUFSIZE];

        if (sizeof(entry) < vsize)
        {
            CfOut(cf_error, "", "Invalid value in promise compliance database. Expected size: %zu, actual size: %d", sizeof(entry), vsize);
            continue;
        }

        strcpy(eventname, (char *) key);

        memcpy(&entry, stored, MIN(vsize, sizeof(entry)));

        then = entry.t;
        measure = entry.Q.q;
        av = entry.Q.expect;
        var = entry.Q.var;
        lastseen = now - then;

        snprintf(tbuf, CF_BUFSIZE - 1, "%s", cf_ctime(&then));

        if (then > 0 && lastseen > lsea)
        {
            CfOut(cf_verbose, "", " -> Promise usage record \"%s\" expired, removing...\n", eventname);
            DBCursorDeleteEntry(dbcp);
        }
        else
        {
            if (xml)
            {
                fprintf(fout, "%s", NRX[cfx_entry][cfb]);
                fprintf(fout, "%s %s %s", NRX[cfx_date][cfb], tbuf, NRX[cfx_date][cfe]);
                fprintf(fout, "%s <a href=\"promises.cf.html#%s\">%s</a> %s", NRX[cfx_event][cfb], eventname, eventname,
                        NRX[cfx_event][cfe]);

                if (measure == 1.0)
                {
                    fprintf(fout, "%s %s %s", NRX[cfx_q][cfb], "compliant", NRX[cfx_q][cfe]);
                }
                else if (measure == 0.5)
                {
                    fprintf(fout, "%s %s %s", NRX[cfx_q][cfb], "repaired", NRX[cfx_q][cfe]);
                }
                else if (measure == 0.0)
                {
                    fprintf(fout, "%s %s %s", NRX[cfx_q][cfb], "non-compliant", NRX[cfx_q][cfe]);
                }

                fprintf(fout, "%s %.1lf %s", NRX[cfx_av][cfb], av * 100.0, NRX[cfx_av][cfe]);
                fprintf(fout, "%s %.1lf %s", NRX[cfx_dev][cfb], sqrt(var) * 100.0, NRX[cfx_dev][cfe]);

                fprintf(fout, "%s", NRX[cfx_entry][cfe]);
            }
            else if (html)
            {
                char b1[CF_MAXVARSIZE], b2[CF_MAXVARSIZE], b3[CF_MAXVARSIZE], b4[CF_MAXVARSIZE], b5[CF_MAXVARSIZE],
                    b6[CF_MAXVARSIZE];
                char vbuff[CF_BUFSIZE];

                snprintf(b1, CF_MAXVARSIZE - 1, "%s %s %s %s", NRH[cfx_entry][cfb], NRH[cfx_date][cfb], tbuf,
                         NRH[cfx_date][cfe]);
                snprintf(b2, CF_MAXVARSIZE - 1, "%s <a href=\"promises.cf.html#%s\">%s</a> %s", NRH[cfx_event][cfb],
                         eventname, eventname, NRH[cfx_event][cfe]);

                if (measure == 1.0)
                {
                    snprintf(b3, CF_MAXVARSIZE - 1, "%s %s %s", NRH[cfx_q][cfb], "compliant", NRH[cfx_q][cfe]);
                }
                else if (measure == 0.5)
                {
                    snprintf(b3, CF_MAXVARSIZE - 1, "%s %s %s", NRH[cfx_q][cfb], "repaired", NRH[cfx_q][cfe]);
                }
                else if (measure == 0.0)
                {
                    snprintf(b3, CF_MAXVARSIZE - 1, "%s %s %s", NRH[cfx_q][cfb], "non-compliant", NRH[cfx_q][cfe]);
                }

                snprintf(b4, CF_MAXVARSIZE - 1, "%s %.1lf %s", NRH[cfx_av][cfb], av * 100.0, NRH[cfx_av][cfe]);
                snprintf(b5, CF_MAXVARSIZE - 1, "%s %.1lf %s", NRH[cfx_dev][cfb], sqrt(var) * 100.0, NRH[cfx_dev][cfe]);
                snprintf(b6, CF_MAXVARSIZE - 1, "%s", NRH[cfx_entry][cfe]);
                snprintf(vbuff, CF_BUFSIZE - 1, "%s %s %s %s %s %s", b1, b2, b3, b4, b5, b6);

                PrependItem(&htmlreport, vbuff, NULL);
                htmlreport->time = then;
            }
            else
            {
                if (measure == 1.0)
                {
                    fprintf(fout, "%s,%s,compliant,%.1lf,%.1lf", tbuf, eventname, av * 100.0, sqrt(var) * 100.0);
                }
                else if (measure == 0.5)
                {
                    fprintf(fout, "%s,%s,repaired,%.1lf,%.1lf", tbuf, eventname, av * 100.0, sqrt(var) * 100.0);
                }
                else if (measure == 0.0)
                {
                    fprintf(fout, "%s,%s,non-compliant,%.1lf,%.1lf", tbuf, eventname, av * 100.0, sqrt(var) * 100.0);
                }

            }
        }
    }

    DeleteDBCursor(dbp, dbcp);
    CloseDB(dbp);

    if (html && !embed)
    {
        htmlreport = SortItemListTimes(htmlreport);

        for (ip = htmlreport; ip != NULL; ip = ip->next)
        {
            fprintf(fout, "%s", ip->name);
        }

        fprintf(fout, "</table></div>");
        DeleteItemList(htmlreport);
    }

    if (XML)
    {
        fprintf(fout, "</output>\n");
    }

    fclose(fout);
}

void SummarizeSetuid(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web)
{
    FILE *fin, *fout;
    char name[CF_BUFSIZE], line[CF_BUFSIZE];
    Item *ip, *file = NULL;
    char start[32];

    snprintf(name, CF_BUFSIZE, "%s/cfagent.%s.log", CFWORKDIR, VSYSNAME.nodename);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen", "Cannot open the source log %s", name);
        return;
    }

    if (html)
    {
        snprintf(name, CF_BUFSIZE, "setuid.html");
    }
    else if (xml)
    {
        snprintf(name, CF_BUFSIZE, "setuid.xml");
    }
    else if (csv)
    {
        snprintf(name, CF_BUFSIZE, "setuid.csv");
    }
    else
    {
        snprintf(name, CF_BUFSIZE, "setuid.txt");
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);
        PrependItem(&file, line, NULL);
    }

    fclose(fin);

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        return;
    }

    if (html && !embed)
    {
        snprintf(name, CF_BUFSIZE, "Known setuid programs on %s", VFQNAME);
        fprintf(fout, "<div id=\"reporttext\">\n");
        fprintf(fout, "<table class=border cellpadding=5>\n");
        fprintf(fout, "%s", NRH[cfx_entry][cfb]);
        fprintf(fout, "%s %s %s", NRH[cfx_filename][cfb], "Filename", NRH[cfx_filename][cfe]);
        fprintf(fout, "%s", NRH[cfx_entry][cfe]);
    }
    else if (XML)
    {
        fprintf(fout, "<?xml version=\"1.0\"?>\n<output>\n");
    }

    file = SortItemListNames(file);

    for (ip = file; ip != NULL; ip = ip->next)
    {
        memset(start, 0, 32);
        memset(name, 0, 255);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        if (xml)
        {
            fprintf(fout, "%s", NRX[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRX[cfx_filename][cfb], ip->name, NRX[cfx_end][cfe]);
            fprintf(fout, "%s", NRX[cfx_entry][cfe]);
        }
        else if (html)
        {
            fprintf(fout, "%s", NRH[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRH[cfx_filename][cfb], ip->name, NRH[cfx_end][cfe]);
            fprintf(fout, "%s", NRH[cfx_entry][cfe]);
        }
        else if (csv)
        {
            fprintf(fout, "%s", ip->name);
        }
        else
        {
            fprintf(fout, "%s", ip->name);
        }
    }

    if (html && !embed)
    {
        fprintf(fout, "</table></div>\n");
    }

    if (XML)
    {
        fprintf(fout, "</output>\n");
    }

    fclose(fout);
    DeleteItemList(file);
}

void SummarizeFileChanges(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web)
{
    FILE *fin, *fout;
    char name[CF_BUFSIZE], line[CF_BUFSIZE], datestr[CF_MAXVARSIZE], size[CF_MAXVARSIZE];
    char no[CF_SMALLBUF], change[CF_BUFSIZE], reformat[CF_BUFSIZE], output[2 * CF_BUFSIZE], aggregate[CF_BUFSIZE];
    Item *ip, *file = NULL;
    char pm;
    long start;
    time_t now;
    int i = 0, truncate;

    snprintf(name, CF_BUFSIZE - 1, "%s/state/%s", CFWORKDIR, CF_FILECHANGE);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen", "Cannot open the source log %s", name);
        return;
    }

    if (html)
    {
        snprintf(name, CF_BUFSIZE, "file_changes.html");
    }
    else if (xml)
    {
        snprintf(name, CF_BUFSIZE, "file_changes.xml");
    }
    else if (csv)
    {
        snprintf(name, CF_BUFSIZE, "file_changes.csv");
    }
    else
    {
        snprintf(name, CF_BUFSIZE, "file_changes.txt");
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);
        PrependItem(&file, line, NULL);
    }

    fclose(fin);

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        return;
    }

    if (html && !embed)
    {
        snprintf(name, CF_BUFSIZE, "File change events recorded on %s", VFQNAME);
        fprintf(fout, "<div id=\"reporttext\">\n");
        fprintf(fout, "<table class=border cellpadding=5>\n");
        fprintf(fout, "%s", NRH[cfx_entry][cfb]);
        fprintf(fout, "%s %s %s", NRH[cfx_date][cfb], "Change detected", NRH[cfx_date][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_filename][cfb], "File change", NRH[cfx_filename][cfe]);
        fprintf(fout, "%s", NRH[cfx_entry][cfe]);
    }
    else if (XML)
    {
        fprintf(fout, "<?xml version=\"1.0\"?>\n<output>\n");
    }

    for (ip = file; ip != NULL; ip = ip->next)
    {
        char timebuffer[26];

        memset(name, 0, 255);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        sscanf(ip->name, "%ld,%1023[^\n]", &start, name);
        now = (time_t) start;
        snprintf(datestr, CF_MAXVARSIZE - 1, "%s", cf_strtimestamp_local(now, timebuffer));
        Chop(datestr, CF_EXPANDSIZE);

        if (xml)
        {
            fprintf(fout, "%s", NRX[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRX[cfx_date][cfb], datestr, NRX[cfx_date][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_filename][cfb], name, NRX[cfx_end][cfe]);
            fprintf(fout, "%s", NRX[cfx_entry][cfe]);
        }
        else if (html)
        {
            fprintf(fout, "%s", NRH[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRH[cfx_date][cfb], datestr, NRH[cfx_date][cfe]);
            fprintf(fout, "%s %s %s", NRH[cfx_filename][cfb], name, NRH[cfx_end][cfe]);
            fprintf(fout, "%s", NRH[cfx_entry][cfe]);
        }
        else if (csv)
        {
            fprintf(fout, "%s,%s", datestr, name);
        }
        else
        {
            fprintf(fout, "%s", ip->name);
        }

        if (++i > 12 * 24 * 7)
        {
            break;
        }
    }

    if (html && !embed)
    {
        fprintf(fout, "</table></div>");
    }

    if (XML)
    {
        fprintf(fout, "</output>\n");
    }

    fclose(fout);
    DeleteItemList(file);

/* Detail log */

    file = NULL;

    snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, NOVA_DIFF_LOG);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen", "Cannot open the source log %s", name);
        return;
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);

        if (strncmp(line, "CHANGE", strlen("CHANGE")) != 0)
        {
            continue;
        }

        name[0] = '\0';
        sscanf(line, "CHANGE %[^\n]", name);

        fgets(line, CF_BUFSIZE - 1, fin);
        /* check if promise handle is included into log record */
        if (strncmp(line, "promise handle:", strlen("promise handle:")) == 0)
        {
            fgets(line, CF_BUFSIZE - 1, fin);
        }

        sscanf(line, "%128[^;];%[^\n]", datestr, size);

        if (strncmp(datestr, "END", strlen("END")) == 0)
        {
            continue;
        }

        memset(aggregate, 0, CF_BUFSIZE);
        snprintf(aggregate, CF_BUFSIZE - 1, "%s<br>", aggregate);
        output[0] = '\0';

        truncate = false;

        while (!feof(fin))
        {
            line[0] = '\0';
            fgets(line, CF_BUFSIZE - 1, fin);

            if (strncmp(line, "END", strlen("END")) == 0)
            {
                break;
            }

            no[0] = '\0';
            change[0] = '\0';
            sscanf(line, "%c,%[^,],%1024[^\n]", &pm, no, change);

            if (!truncate)
            {
                if (xml)
                {
                    snprintf(reformat, CF_BUFSIZE - 1, "<pm>%c</pm><line>%s</line> <event>%s</event>\n", pm, no,
                             change);

                    if (!JoinSuffix(aggregate, reformat))
                    {
                        truncate = true;
                    }
                }
                else if (html)
                {
                    snprintf(reformat, CF_BUFSIZE - 1,
                             "<span id=\"pm\">%c</span><span id=\"line\">%s</span><span id=\"change\">%s</span><br>",
                             pm, no, change);

                    if (!JoinSuffix(aggregate, reformat))
                    {
                        truncate = true;
                    }
                }
                else
                {
                    snprintf(reformat, CF_BUFSIZE - 1, "   %s\n", line);
                    if (!JoinSuffix(aggregate, reformat))
                    {
                    }
                }
            }
        }

        if (xml)
        {
            snprintf(output, CF_BUFSIZE - 1,
                     "%s"
                     "%s %s %s"
                     "%s %s %s"
                     "%s %s %s"
                     "%s\n",
                     NRX[cfx_entry][cfb],
                     NRX[cfx_date][cfb], datestr, NRX[cfx_date][cfe],
                     NRX[cfx_filename][cfb], name, NRX[cfx_end][cfe],
                     NRX[cfx_event][cfb], aggregate, NRX[cfx_event][cfe], NRX[cfx_entry][cfe]);
        }
        else if (html)
        {
            if (truncate)
            {
                snprintf(output, CF_BUFSIZE - 1,
                         "%s"
                         "%s %s %s"
                         "%s %s %s"
                         "%s %s <br>Truncated, full record at %s:%s/cfdiff.log %s"
                         "%s\n",
                         NRH[cfx_entry][cfb],
                         NRH[cfx_date][cfb], datestr, NRH[cfx_date][cfe],
                         NRH[cfx_filename][cfb], name, NRH[cfx_end][cfe],
                         NRH[cfx_event][cfb], aggregate, VFQNAME, CFWORKDIR, NRH[cfx_event][cfe], NRH[cfx_entry][cfe]);
            }
            else
            {
                snprintf(output, CF_BUFSIZE - 1,
                         "%s"
                         "%s %s %s"
                         "%s %s %s"
                         "%s %s %s"
                         "%s\n",
                         NRH[cfx_entry][cfb],
                         NRH[cfx_date][cfb], datestr, NRH[cfx_date][cfe],
                         NRH[cfx_filename][cfb], name, NRH[cfx_end][cfe],
                         NRH[cfx_event][cfb], aggregate, NRH[cfx_event][cfe], NRH[cfx_entry][cfe]);
            }
        }
        else
        {
            snprintf(output, CF_BUFSIZE - 1, "%s %s %s", datestr, name, aggregate);
        }

        if (strlen(aggregate) > 0)
        {
            PrependItem(&file, output, NULL);
            aggregate[0] = '\0';
        }
    }

    fclose(fin);

    if (html)
    {
        snprintf(name, CF_BUFSIZE, "file_diffs.html");
    }
    else if (xml)
    {
        snprintf(name, CF_BUFSIZE, "file_diffs.xml");
    }
    else if (csv)
    {
        snprintf(name, CF_BUFSIZE, "file_diffs.csv");
    }
    else
    {
        snprintf(name, CF_BUFSIZE, "file_diffs.txt");
    }

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        return;
    }

    if (html && !embed)
    {
        snprintf(name, CF_BUFSIZE, "File difference events recorded on %s", VFQNAME);
        fprintf(fout, "<div id=\"reporttext\">\n");
        fprintf(fout, "<table class=border cellpadding=5>\n");
        fprintf(fout, "%s", NRH[cfx_entry][cfb]);
        fprintf(fout, "%s %s %s", NRH[cfx_date][cfb], "Time of change event", NRH[cfx_date][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_filename][cfb], "Filename", NRH[cfx_filename][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_event][cfb], "Delta", NRH[cfx_event][cfe]);
        fprintf(fout, "%s", NRH[cfx_entry][cfe]);
    }
    else if (XML)
    {
        fprintf(fout, "<?xml version=\"1.0\"?>\n<output>\n");
    }

    for (i = 0, ip = file; ip != NULL; ip = ip->next)
    {
        fprintf(fout, "%s", ip->name);
    }

    if (html && !embed)
    {
        fprintf(fout, "</table></div>");
    }

    if (XML)
    {
        fprintf(fout, "</output>\n");
    }

    fclose(fout);
    DeleteItemList(file);
}

void SummarizePromiseRepaired(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot,
                              char *web)
{
    FILE *fin, *fout;
    char name[CF_BUFSIZE], line[CF_BUFSIZE];
    char handle[CF_MAXVARSIZE];
    time_t date;
    long then;
    Item *ip, *file = NULL;
    int i = 0;

    snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, CF_REPAIR_LOG);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen", "Cannot open the source log %s", name);
        return;
    }

    if (html)
    {
        snprintf(name, CF_BUFSIZE, "promise_repair.html");
    }
    else if (xml)
    {
        snprintf(name, CF_BUFSIZE, "promise_repair.xml");
    }
    else if (csv)
    {
        snprintf(name, CF_BUFSIZE, "promise_repair.csv");
    }
    else
    {
        snprintf(name, CF_BUFSIZE, "promise_repair.txt");
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);
        PrependItem(&file, line, NULL);
    }

    fclose(fin);

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        return;
    }

    if (html && !embed)
    {
        snprintf(name, CF_BUFSIZE, "Promises repaired on %s", VFQNAME);
        fprintf(fout, "<div id=\"reporttext\">\n");
        fprintf(fout, "<table class=border cellpadding=5>\n");
        fprintf(fout, "%s", NRH[cfx_entry][cfb]);
        fprintf(fout, "%s %s %s", NRH[cfx_date][cfb], "Time", NRH[cfx_date][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_event][cfb], "Handle", NRH[cfx_event][cfe]);
        fprintf(fout, "%s", NRH[cfx_entry][cfe]);
    }
    else if (XML)
    {
        fprintf(fout, "<?xml version=\"1.0\"?>\n<output>\n");
    }

    for (ip = file; ip != NULL; ip = ip->next)
    {
        memset(name, 0, 255);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        sscanf(ip->name, "%ld,%512[^,]", &then, handle);
        date = (time_t) then;

        if (xml)
        {
            fprintf(fout, "%s", NRX[cfx_entry][cfb]);
            fprintf(fout, "%s %" PRIdMAX " %s", NRX[cfx_date][cfb], (intmax_t)date, NRX[cfx_date][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_event][cfb], handle, NRX[cfx_event][cfe]);
            fprintf(fout, "%s", NRX[cfx_entry][cfe]);
        }
        else if (html)
        {
            fprintf(fout, "%s", NRH[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRH[cfx_date][cfb], cf_ctime(&date), NRH[cfx_date][cfe]);
            fprintf(fout, "%s <a href=\"promises.cf.html#%s\">%s</a> %s", NRH[cfx_event][cfb], handle, handle,
                    NRH[cfx_event][cfe]);
            fprintf(fout, "%s", NRH[cfx_entry][cfe]);
        }
        else if (csv)
        {
            fprintf(fout, "%s", ip->name);
        }
        else
        {
            fprintf(fout, "%s", ip->name);
        }

        if (++i > 12 * 24 * 7)
        {
            break;
        }
    }

    if (html && !embed)
    {
        fprintf(fout, "</table></div>");
    }

    if (XML)
    {
        fprintf(fout, "</output>\n");
    }

    fclose(fout);
    DeleteItemList(file);
}

void SummarizePromiseNotKept(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web)
{
    FILE *fin, *fout;
    char name[CF_BUFSIZE], line[CF_BUFSIZE];
    char handle[CF_MAXVARSIZE];
    time_t date;
    long then;
    Item *ip, *file = NULL;
    char start[32];
    int i = 0;

    snprintf(name, CF_BUFSIZE - 1, "%s/%s", CFWORKDIR, CF_NOTKEPT_LOG);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen", "Cannot open the source log %s", name);
        return;
    }

    if (html)
    {
        snprintf(name, CF_BUFSIZE, "promise_notkept.html");
    }
    else if (xml)
    {
        snprintf(name, CF_BUFSIZE, "promise_notkept.xml");
    }
    else if (csv)
    {
        snprintf(name, CF_BUFSIZE, "promise_notkept.csv");
    }
    else
    {
        snprintf(name, CF_BUFSIZE, "promise_notkept.txt");
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);
        PrependItem(&file, line, NULL);
    }

    fclose(fin);

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        return;
    }

    if (html && !embed)
    {
        snprintf(name, CF_BUFSIZE, "Promises not kept on %s", VFQNAME);
        fprintf(fout, "<div id=\"reporttext\">\n");
        fprintf(fout, "<table class=border cellpadding=5>\n");
        fprintf(fout, "%s", NRH[cfx_entry][cfb]);
        fprintf(fout, "%s %s %s", NRH[cfx_date][cfb], "Time", NRH[cfx_date][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_event][cfb], "Handle", NRH[cfx_event][cfe]);
        fprintf(fout, "%s", NRH[cfx_entry][cfe]);
    }
    else if (XML)
    {
        fprintf(fout, "<?xml version=\"1.0\"?>\n<output>\n");
    }

    for (ip = file; ip != NULL; ip = ip->next)
    {
        memset(start, 0, 32);
        memset(name, 0, 255);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        sscanf(ip->name, "%ld,%512[^,]", &then, handle);
        date = (time_t) then;

        if (xml)
        {
            fprintf(fout, "%s", NRX[cfx_entry][cfb]);
            fprintf(fout, "%s %" PRIdMAX " %s", NRX[cfx_date][cfb], (intmax_t)date, NRX[cfx_date][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_event][cfb], handle, NRX[cfx_event][cfe]);
            fprintf(fout, "%s", NRX[cfx_entry][cfe]);
        }
        else if (html)
        {
            fprintf(fout, "%s", NRH[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRH[cfx_date][cfb], cf_ctime(&date), NRH[cfx_date][cfe]);
            fprintf(fout, "%s <a href=\"promises.cf.html#%s\">%s</a> %s", NRH[cfx_event][cfb], handle, handle,
                    NRH[cfx_event][cfe]);
            fprintf(fout, "%s", NRH[cfx_entry][cfe]);
        }
        else if (csv)
        {
            fprintf(fout, "%s", ip->name);
        }
        else
        {
            fprintf(fout, "%s", ip->name);
        }

        if (++i > 12 * 24 * 7)
        {
            break;
        }
    }

    if (html && !embed)
    {
        fprintf(fout, "</table></div>");
    }

    if (XML)
    {
        fprintf(fout, "</output>\n");
    }

    fclose(fout);
    DeleteItemList(file);
}

void SummarizeSoftware(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web)
{
    FILE *fin, *fout;
    char name[CF_MAXVARSIZE], version[CF_MAXVARSIZE], arch[CF_MAXVARSIZE], mgr[CF_MAXVARSIZE], line[CF_BUFSIZE];
    Item *ip, *file = NULL;
    int i = 0;

    snprintf(name, CF_MAXVARSIZE - 1, "%s/state/%s", CFWORKDIR, SOFTWARE_PACKAGES_CACHE);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen",
              "Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",
              name);
        return;
    }

    if (html)
    {
        snprintf(name, sizeof(name), "software_packages.html");
    }
    else if (xml)
    {
        snprintf(name, sizeof(name), "software_packages.xml");
    }
    else
    {
        snprintf(name, sizeof(name), "software_packages.csv");
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);
        if (!IsItemIn(file, line))
        {
            PrependItem(&file, line, NULL);
        }
    }

    fclose(fin);

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        return;
    }

    if (html && !embed)
    {
        snprintf(name, sizeof(name), "Software versions installed %s", VFQNAME);
        fprintf(fout, "<div id=\"reporttext\">\n");
        fprintf(fout, "<table class=border cellpadding=5>\n");
        fprintf(fout, "%s", NRH[cfx_entry][cfb]);
        fprintf(fout, "%s %s %s", NRH[cfx_filename][cfb], "Package", NRH[cfx_filename][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_version][cfb], "Version", NRH[cfx_version][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_ref][cfb], "Architecture", NRH[cfx_ref][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_event][cfb], "Manager", NRH[cfx_event][cfe]);
        fprintf(fout, "%s", NRH[cfx_entry][cfe]);
    }
    else if (XML)
    {
        fprintf(fout, "<?xml version=\"1.0\"?>\n<output>\n");
    }

    for (ip = file; ip != NULL; ip = ip->next)
    {
        memset(name, 0, CF_MAXVARSIZE);
        memset(version, 0, CF_MAXVARSIZE);
        memset(arch, 0, CF_MAXVARSIZE);
        memset(mgr, 0, CF_MAXVARSIZE);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        sscanf(ip->name, "%250[^,],%250[^,],%250[^,],%250[^\n]", name, version, arch, mgr);

        if (xml)
        {
            fprintf(fout, "%s", NRX[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRX[cfx_filename][cfb], name, NRX[cfx_filename][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_version][cfb], version, NRX[cfx_version][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_ref][cfb], arch, NRX[cfx_ref][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_event][cfb], mgr, NRX[cfx_event][cfe]);
            fprintf(fout, "%s", NRX[cfx_entry][cfe]);
        }
        else if (html)
        {
            fprintf(fout, "%s", NRH[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRH[cfx_filename][cfb], name, NRH[cfx_filename][cfe]);
            fprintf(fout, "%s %s %s", NRH[cfx_version][cfb], version, NRH[cfx_version][cfe]);
            fprintf(fout, "%s %s %s", NRH[cfx_ref][cfb], arch, NRH[cfx_ref][cfe]);
            fprintf(fout, "%s %s %s", NRH[cfx_event][cfb], mgr, NRH[cfx_event][cfe]);
            fprintf(fout, "%s", NRH[cfx_entry][cfe]);
        }
        else
        {
            fprintf(fout, "%s", ip->name);
        }

        if (++i > 12 * 24 * 7)
        {
            break;
        }
    }

    if (html && !embed)
    {
        fprintf(fout, "</table></div>\n");
    }

    if (XML)
    {
        fprintf(fout, "</output>\n");
    }

    fclose(fout);
    DeleteItemList(file);
}

void SummarizeUpdates(int xml, int html, int csv, int embed, char *stylesheet, char *head, char *foot, char *web)
{
    FILE *fin, *fout;
    char name[CF_MAXVARSIZE], version[CF_MAXVARSIZE], arch[CF_MAXVARSIZE], mgr[CF_MAXVARSIZE], line[CF_BUFSIZE];
    Item *ip, *file = NULL;
    int i = 0, count = 0;

    CfOut(cf_verbose, "", "Creating available patch report...\n");

    snprintf(name, CF_MAXVARSIZE - 1, "%s/state/%s", CFWORKDIR, NOVA_PATCHES_AVAIL);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen",
              "Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",
              name);
        return;
    }

    if (html)
    {
        snprintf(name, CF_MAXVARSIZE, "software_patches_avail.html");
    }
    else if (xml)
    {
        snprintf(name, CF_MAXVARSIZE, "software_patches_avail.xml");
    }
    else
    {
        snprintf(name, CF_MAXVARSIZE, "software_patches_avail.csv");
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);
        if (!IsItemIn(file, line))
        {
            PrependItem(&file, line, NULL);
        }
    }

    fclose(fin);

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        return;
    }

    if (html && !embed)
    {
        snprintf(name, sizeof(name), "Software updates available and outstanding for %s", VFQNAME);
        fprintf(fout, "<div id=\"reporttext\">\n");
        fprintf(fout, "<table class=border cellpadding=5>\n");
        fprintf(fout, "%s", NRH[cfx_entry][cfb]);
        fprintf(fout, "%s %s %s", NRH[cfx_filename][cfb], "Package", NRH[cfx_filename][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_version][cfb], "Version", NRH[cfx_version][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_ref][cfb], "Architecture", NRH[cfx_ref][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_event][cfb], "Manager", NRH[cfx_event][cfe]);
        fprintf(fout, "%s", NRH[cfx_entry][cfe]);
    }
    else if (XML)
    {
        fprintf(fout, "<?xml version=\"1.0\"?>\n<output>\n");
    }

    for (ip = file; ip != NULL; ip = ip->next)
    {
        memset(name, 0, CF_MAXVARSIZE);
        memset(version, 0, CF_MAXVARSIZE);
        memset(arch, 0, CF_MAXVARSIZE);
        memset(mgr, 0, CF_MAXVARSIZE);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        sscanf(ip->name, "%250[^,],%250[^,],%250[^,],%250[^\n]", name, version, arch, mgr);

        if (xml)
        {
            fprintf(fout, "%s", NRX[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRX[cfx_filename][cfb], name, NRX[cfx_filename][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_version][cfb], version, NRX[cfx_version][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_ref][cfb], arch, NRX[cfx_ref][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_event][cfb], mgr, NRX[cfx_event][cfe]);
            fprintf(fout, "%s", NRX[cfx_entry][cfe]);
        }
        else if (html)
        {
            fprintf(fout, "%s", NRH[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRH[cfx_filename][cfb], name, NRH[cfx_filename][cfe]);
            fprintf(fout, "%s %s %s", NRH[cfx_version][cfb], version, NRH[cfx_version][cfe]);
            fprintf(fout, "%s %s %s", NRH[cfx_ref][cfb], arch, NRH[cfx_ref][cfe]);
            fprintf(fout, "%s %s %s", NRH[cfx_event][cfb], mgr, NRH[cfx_event][cfe]);
            fprintf(fout, "%s", NRH[cfx_entry][cfe]);
        }
        else
        {
            fprintf(fout, "%s", ip->name);
        }

        count++;

        if (++i > 12 * 24 * 7)
        {
            break;
        }
    }

    if (html && !embed)
    {
        fprintf(fout, "</table></div>\n");
    }

    if (XML)
    {
        fprintf(fout, "</output>\n");
    }

    fclose(fout);
    DeleteItemList(file);
    file = NULL;

/* Now show installed patch level */

    CfOut(cf_verbose, "", "Creating patch status report...\n");

    snprintf(name, sizeof(name), "%s/state/%s", CFWORKDIR, NOVA_PATCHES_INSTALLED);
    MapName(name);

    if ((fin = fopen(name, "r")) == NULL)
    {
        CfOut(cf_inform, "fopen",
              "Cannot open the source log %s - you need to run a package discovery promise to create it in cf-agent",
              name);
        return;
    }

    if (html)
    {
        snprintf(name, sizeof(name), "software_patch_status.html");
    }
    else if (xml)
    {
        snprintf(name, sizeof(name), "software_patch_status.xml");
    }
    else
    {
        snprintf(name, sizeof(name), "software_patch_status.csv");
    }

/* Max 2016 entries - at least a week */

    while (!feof(fin))
    {
        line[0] = '\0';
        fgets(line, CF_BUFSIZE - 1, fin);

        if (!IsItemIn(file, line))
        {
            PrependItem(&file, line, NULL);
        }
    }

    fclose(fin);

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        return;
    }

    if (html && !embed)
    {
        snprintf(name, sizeof(name), "Software patches installed on %s", VFQNAME);
        fprintf(fout, "<div id=\"reporttext\">\n");
        fprintf(fout, "<table class=border cellpadding=5>\n");
        fprintf(fout, "%s", NRH[cfx_entry][cfb]);
        fprintf(fout, "%s %s %s", NRH[cfx_filename][cfb], "Package", NRH[cfx_filename][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_version][cfb], "Version", NRH[cfx_version][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_ref][cfb], "Architecture", NRH[cfx_ref][cfe]);
        fprintf(fout, "%s %s %s", NRH[cfx_event][cfb], "Manager", NRH[cfx_event][cfe]);
        fprintf(fout, "%s", NRH[cfx_entry][cfe]);
    }
    else if (XML)
    {
        fprintf(fout, "<?xml version=\"1.0\"?>\n<output>\n");
    }

    for (ip = file; ip != NULL; ip = ip->next)
    {
        memset(name, 0, CF_MAXVARSIZE);
        memset(version, 0, CF_MAXVARSIZE);
        memset(arch, 0, CF_MAXVARSIZE);
        memset(mgr, 0, CF_MAXVARSIZE);

        if (strlen(ip->name) == 0)
        {
            continue;
        }

        sscanf(ip->name, "%250[^,],%250[^,],%250[^,],%250[^\n]", name, version, arch, mgr);

        if (xml)
        {
            fprintf(fout, "%s", NRX[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRX[cfx_filename][cfb], name, NRX[cfx_filename][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_version][cfb], version, NRX[cfx_version][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_ref][cfb], arch, NRX[cfx_ref][cfe]);
            fprintf(fout, "%s %s %s", NRX[cfx_event][cfb], mgr, NRX[cfx_event][cfe]);
            fprintf(fout, "%s", NRX[cfx_entry][cfe]);
        }
        else if (html)
        {
            fprintf(fout, "%s", NRH[cfx_entry][cfb]);
            fprintf(fout, "%s %s %s", NRH[cfx_filename][cfb], name, NRH[cfx_filename][cfe]);
            fprintf(fout, "%s %s %s", NRH[cfx_version][cfb], version, NRH[cfx_version][cfe]);
            fprintf(fout, "%s %s %s", NRH[cfx_ref][cfb], arch, NRH[cfx_ref][cfe]);
            fprintf(fout, "%s %s %s", NRH[cfx_event][cfb], mgr, NRH[cfx_event][cfe]);
            fprintf(fout, "%s", NRH[cfx_entry][cfe]);
        }
        else
        {
            fprintf(fout, "%s", ip->name);
        }

        if (++i > 12 * 24 * 7)
        {
            break;
        }
    }

    if (html && !embed)
    {
        fprintf(fout, "</table></div>\n");
    }

    if (XML)
    {
        fprintf(fout, "</output>\n");
    }

    fclose(fout);
    DeleteItemList(file);
}

void GrandSummary(void)
{
    char name[CF_BUFSIZE];
    FILE *fout;

    Nova_SummarizeComms();

    snprintf(name, CF_BUFSIZE - 1, "%s/reports/comp_key", CFWORKDIR);
    MapName(name);

    if ((fout = fopen(name, "w")) == NULL)
    {
        CfOut(cf_error, "fopen", "Cannot open the destination file %s", name);
        return;
    }

    fprintf(fout, "Week: %.4lf %.4lf\n", METER_KEPT[meter_compliance_week], METER_REPAIRED[meter_compliance_week]);
    fprintf(fout, "Day: %.4lf %.4lf\n", METER_KEPT[meter_compliance_day], METER_REPAIRED[meter_compliance_day]);
    fprintf(fout, "Hour: %.4lf %.4lf\n", METER_KEPT[meter_compliance_hour], METER_REPAIRED[meter_compliance_hour]);
    fprintf(fout, "Perf: %.4lf %.4lf\n", METER_KEPT[meter_perf_day], METER_REPAIRED[meter_perf_day]);
    fprintf(fout, "Soft: %.4lf %.4lf\n", METER_KEPT[meter_other_day], METER_REPAIRED[meter_other_day]);
    fprintf(fout, "Comms: %.4lf %.4lf\n", METER_KEPT[meter_comms_hour], METER_REPAIRED[meter_comms_hour]);
    fprintf(fout, "Anom: %.4lf %.4lf\n", METER_KEPT[meter_anomalies_day], METER_REPAIRED[meter_anomalies_day]);

    fclose(fout);
}
