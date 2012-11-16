#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "files_hashes.h"
#include "db_save.h"
#include "db_query.h"
#include "crypto.h"
#include "item_lib.h"
#include "bson_lib.h"
#include "datapack.h"
#include "cfstream.h"
#include "string_lib.h"

static void BlackStatusFlagRefresh(EnterpriseDB *dbconn, char *id);
static void Nova_RecordNetwork(EnterpriseDB *dbconnp, time_t now, double datarate, AgentConnection *conn);

/*****************************************************************************/

/*********************************************************************/

int Nova_QueryClientForReports(EnterpriseDB *dbconn, AgentConnection *conn, const char *menu, time_t since)
/*
 * Returns the number of plaintext bytes received (0 on error).
 **/
{
    int tosend, cipherlen = 0;
    char in[CF_BUFSIZE], out[CF_BUFSIZE], workbuf[CF_BUFSIZE], cfchangedstr[265];
    long length = 0;
    int more = true, header = true, current_report = -1;
    time_t now, then, time2 = 0, delta1 = 0, delta2 = 0;
    char keyHash[EVP_MAX_MD_SIZE * 4];
    double datarate;

    Item **reports = NewReportBook();

    snprintf(cfchangedstr, 255, "%s%s", CF_CHANGEDSTR1, CF_CHANGEDSTR2);

    workbuf[0] = '\0';

    now = time(NULL);

    snprintf(in, CF_BUFSIZE - CF_PROTO_OFFSET, "QUERY %s %ld %ld", menu, (long) since, now);
    cipherlen = EncryptString(conn->encryption_type, in, out, conn->session_key, strlen(in) + 1);
    snprintf(workbuf, CF_BUFSIZE, "SQUERY %4d", cipherlen);
    memcpy(workbuf + CF_PROTO_OFFSET, out, cipherlen);
    tosend = cipherlen + CF_PROTO_OFFSET;

/* Send proposition C0 - query */

    CfOut(cf_verbose, "", " -> Sending query at %s", cf_ctime(&now));

    if (SendTransaction(conn->sd, workbuf, tosend, CF_DONE) == -1)
    {
        CfOut(cf_error, "send", "Couldn't send data");
        return 0;
    }

    int total_plaintext_len = 0;

    while (more)
    {
        out[0] = '\0';

        if ((cipherlen = ReceiveTransaction(conn->sd, in, &more)) == -1)
        {
            CfOut(cf_error, "recv", " !! Failed to collect data");
            return 0;
        }

        if (strncmp(in, "BAD:", 4) == 0)
        {
            CfOut(cf_error, "", " !! Abort transmission: got \"%s\" from %s", in + 4, conn->remoteip);
            break;
        }

        int plaintext_len = DecryptString(conn->encryption_type, in, out, conn->session_key, cipherlen);

        total_plaintext_len += plaintext_len;

        // Check the header for timing of response - we can eventually use this to
        // measure the network performance

        if (header)
        {
            char validate[5];
            char timebuffer[26];

            header = false;     // First one is special
            sscanf(out, "%4s %ld %ld %ld", validate, &delta1, &time2, &length);

            if (strcmp(validate, "CFR:") != 0)
            {
                CfOut(cf_error, "", " !! Invalid report format");
                return 0;
            }

            then = now;
            now = time(NULL);
            delta2 = now - time2;

            CfOut(cf_verbose, "",
                  " -> Received reply of %ld bytes at %s -> Xfer time %jd seconds (processing time %ld seconds)",
                  length, cf_strtimestamp_local(now, timebuffer), (intmax_t) delta2, now - then);

            if (delta2 > 0)
            {
                datarate = (double) length / (double) delta2;
                CfOut(cf_verbose, "", " -> Data rate of %lf", datarate);
            }
            else
            {
                delta2 = (now - then) / 2;
                CfOut(cf_verbose, "", " -> Data rate was unmeasurable (instantaneous) - computing outer boundary");
                if (delta2 != 0)
                {
                    datarate = (double) length / (double) delta2;
                }
                else
                {
                    datarate = 0;
                }
            }

            // Promise to record data rate per host-digest and per IP

            Nova_RecordNetwork(dbconn, now, datarate, conn);
        }
        else if(strncmp ("QUERY complete", out, strlen("QUERY complete")) == 0)
        {
            continue;
        }
        else
        {
            // Put data into a separate linked list for each code
            current_report = Nova_StoreIncomingReports(out, reports, current_report);
        }
    }

    if (!ReportBookHasData(reports))
    {
        return 0;
    }
    
    HashPrintSafe(CF_DEFAULT_DIGEST, conn->digest, keyHash);

    UnpackReportBook(dbconn, keyHash, reports);

    /* nova agent < 2.2 - black status estimation */
    if (dbconn)
    {
        BlackStatusFlagRefresh(dbconn, keyHash);
    }

    DeleteReportBook(reports);

    CFDB_SaveLastHostUpdate(dbconn, keyHash);
    CFDB_SaveLastHostUpdateSize(dbconn, keyHash, total_plaintext_len);
    
    CfOut(cf_verbose, "", "Received %d bytes of report data", total_plaintext_len);

    return total_plaintext_len;
}

/*********************************************************************/

static void BlackStatusFlagRefresh(EnterpriseDB *dbconn, char *id)
{
    long delta_schedule = CFDB_GetDeltaAgentExecution(dbconn, id);
    time_t agent_last_run_time = CFDB_GetLastAgentExecution(dbconn, id);

    if (delta_schedule == 0)
    {
        time_t now = time(NULL);

        int black_threshold = (SECONDS_PER_MINUTE * 5) * CF_BLACKHOST_THRESHOLD; // 5 min assumption
        black_threshold += black_threshold * (CF_BLACKHOST_THRESHOLD_VARIATION * 0.01);
        delta_schedule = (long)(now - agent_last_run_time);
        bool is_blackhost = (delta_schedule > black_threshold)? true:false;

        CFDB_SaveExecutionStatus(dbconn, id, is_blackhost);

        CfDebug("Execution status (pre-estimation): black %s, agent last run time: %ld",
                (is_blackhost)? "true" : "false", agent_last_run_time);
    }
}

/*********************************************************************/

static void Nova_RecordNetwork(EnterpriseDB *dbconnp, time_t now,
                               double datarate, AgentConnection *conn)
// NOTE: NOT Thread-safe (use of HashPrint())
{
    Event e = { 0 }, newe = { 0 };
    e.Q = QDefinite(0);

// query

    bson query;
    bson_init(&query);
    bson_append_string(&query, cfr_keyhash, HashPrint(CF_DEFAULT_DIGEST, conn->digest));
    BsonFinish(&query);

// returned value

    bson field;
    BsonSelectReportFields(&field, 1, cfr_netmeasure);

    mongo_cursor *cursor = MongoFind(dbconnp, MONGO_DATABASE, &query, &field, 0, 0, CF_MONGO_SLAVE_OK);

// freeing query below
    bson_destroy(&field);

    if (MongoCursorNext(cursor))      // not more than one record
    {
        bson_iterator it1;

        bson_iterator_init(&it1, mongo_cursor_bson( cursor ) );

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (StringSafeCompare(bson_iterator_key(&it1), cfr_netmeasure) == 0)
            {
                bson_iterator it2;
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    if (StringSafeCompare(bson_iterator_key(&it2), cfr_time) == 0)
                    {
                        e.t = bson_iterator_int(&it2);
                    }

                    if (StringSafeCompare(bson_iterator_key(&it2), cfr_netmeasure_expect) == 0)
                    {
                        e.Q.expect = (double)bson_iterator_double(&it2);
                    }

                    if (StringSafeCompare(bson_iterator_key(&it2), cfr_netmeasure_var) == 0)
                    {
                        e.Q.var = (double)bson_iterator_double(&it2);
                    }

                    if (StringSafeCompare(bson_iterator_key(&it2), cfr_netmeasure_dq) == 0)
                    {
                        e.Q.dq = (double)bson_iterator_double(&it2);
                    }

                    if (StringSafeCompare(bson_iterator_key(&it2), cfr_netmeasure_q) == 0)
                    {
                        e.Q.q = (double)bson_iterator_double(&it2);
                    }
                }
            }
        }

    }

    mongo_cursor_destroy(cursor);

    newe.Q = QDefinite(0);
    newe.t = now;
    newe.Q = QAverage(e.Q, datarate, 0.5);

    // update DB with new measurement
    bson update;
    bson_init(&update);
    {
        bson_append_start_object(&update, "$set");
        {
            bson_append_start_object(&update, cfr_netmeasure);

            bson_append_int(&update, cfr_time, newe.t);
            bson_append_double(&update, cfr_netmeasure_expect, newe.Q.expect);
            bson_append_double(&update, cfr_netmeasure_var, newe.Q.var);
            bson_append_double(&update, cfr_netmeasure_dq, newe.Q.dq);
            bson_append_double(&update, cfr_netmeasure_q, newe.Q.q);

            bson_append_finish_object(&update);
        }
        bson_append_finish_object(&update);
    }
    BsonFinish(&update);

    MongoUpdate(dbconnp, MONGO_DATABASE, &query, &update, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&query);
    bson_destroy(&update);
}
