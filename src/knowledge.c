/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "files_names.h"
#include "item_lib.h"
#include "datapack.h"

#if defined(HAVE_LIBMONGOC)
#include "db_save.h"
#include "db_query.h"
#include "db_maintain.h"
#endif

/* From cfknow.c */ // FIX
int GetTopicPid(char *classified_topic);

static void Nova_ListAgents(void);
static void Nova_ListFunctions(void);
static void Nova_ListFunction(const FnCallType *f, int full);
static void Nova_ListPromiseTypes();
#ifdef HAVE_LIBMONGOC
static Rlist *Nova_GetTestMachines(void);
#endif

/*****************************************************************************/
#define CF_TEST_HOSTNAME "cfengine_auto_test"
/*****************************************************************************/

void Nova_StoreKMDB(Topic **topichash, Occurrence *occurrences, Inference *inferences)
{
#ifdef HAVE_LIBMONGOC

    Topic *tp;
    TopicAssociation *ta;
    Occurrence *op;
    Inference *ip;
    Item *itp, *bundles = NULL;
    char packNumStr[CF_MAXVARSIZE];
    EnterpriseDB dbconn = { 0 };
    int slot, assoc_id = 0;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

// remove existing data first

    bson b;

    MongoRemove(&dbconn, MONGO_KM_TOPICS, bson_empty(&b), NULL);

    /* Class types and topics */

    for (slot = 0; slot < CF_HASHTABLESIZE; slot++)
    {
        for (tp = topichash[slot]; tp != NULL; tp = tp->next)
        {
            bson insert_op;

            bson_init(&insert_op);
            bson_append_new_oid(&insert_op, "_id");
            bson_append_string(&insert_op, cfk_topicname, tp->topic_name);
            bson_append_string(&insert_op, cfk_topiccontext, tp->topic_context);
            bson_append_string(&insert_op, cfk_bundle, tp->bundle);
            bson_append_int(&insert_op, cfk_topicid, tp->id);

            IdempPrependItem(&bundles, tp->bundle, NULL);

            CfDebug("Add Topic(topic_name,topic_context,pid) values ('%s','%s','%d')\n", tp->topic_name,
                    tp->topic_context, tp->id);

            // Associations

            {
                bson_append_start_array(&insert_op, cfk_associations);

                for (ta = tp->associations; ta != NULL; ta = ta->next)
                {
                    for (itp = ta->associates; itp != NULL; itp = itp->next)
                    {
                        char to_context[CF_MAXVARSIZE], to_topic[CF_MAXVARSIZE];
                        int to_id = GetTopicPid(itp->name);

                        DeClassifyTopic(itp->name, to_topic, to_context);

                        CfDebug(" - Association: '%s::%s' (%d) %s '%s:%s' (%d)\n", tp->topic_context, tp->topic_name,
                                tp->id, ta->fwd_name, to_context, to_topic, to_id);
                        CfDebug(" - Hence  by implication : '%s::%s' (%d) %s '%s::%s' (%d)\n", to_context, to_topic, to_id,
                                ta->bwd_name, tp->topic_context, tp->topic_name, tp->id);

                        // Append variable list item to assocs

                        snprintf(packNumStr, sizeof(packNumStr), "%d", assoc_id++);
                        {
                            bson_append_start_object(&insert_op, packNumStr);
                            bson_append_string(&insert_op, cfk_fwd, ta->fwd_name);
                            bson_append_string(&insert_op, cfk_bwd, ta->bwd_name);
                            bson_append_string(&insert_op, cfk_assocname, to_topic);
                            bson_append_string(&insert_op, cfk_assoccontext, to_context);
                            bson_append_int(&insert_op, cfk_associd, to_id);
                            bson_append_finish_object(&insert_op);
                        }
                    }
                }
                bson_append_finish_object(&insert_op);
            }
            bson_finish(&insert_op);

            MongoInsert(&dbconn, MONGO_KM_TOPICS, &insert_op, NULL);
            bson_destroy(&insert_op);
        }
    }

// Knowledge bundle categories, quick lookup
    
    MongoRemove(&dbconn, MONGO_KM_BUNDLES, bson_empty(&b), NULL);
    
    for (itp = bundles; itp != NULL; itp = itp->next)
       {
       bson insert_op;
       bson_init(&insert_op);
       bson_append_new_oid(&insert_op, "_id");
       bson_append_string(&insert_op, cfk_bundle, itp->name);
       bson_append_string(&insert_op, cfk_topicid, "any");
       bson_finish(&insert_op);
       
       MongoInsert(&dbconn, MONGO_KM_BUNDLES, &insert_op, NULL);
       bson_destroy(&insert_op);
       }
    
    DeleteItemList(bundles);

// Occurrences

    MongoRemove(&dbconn, MONGO_KM_OCCURRENCES, bson_empty(&b), NULL);

    for (op = occurrences; op != NULL; op = op->next)
    {
    Rlist *rp1,*rp2;
    
        for (rp1 = op->represents; rp1 != NULL; rp1 = rp1->next)
        {
            for (rp2 = op->about_topics; rp2 != NULL; rp2 = rp2->next)
            {
            CfDebug("Add occurrence (topic,context,locator,locator_type,subtype) values ('%s','%s','%s','%d','%s')\n",
                   rp2->item, op->occurrence_context, op->locator, op->rep_type, (const char *) rp1->item);

            bson insert_op;
            bson_init(&insert_op);
            bson_append_new_oid(&insert_op, "_id");
            bson_append_string(&insert_op, cfk_occurlocator, op->locator);
            bson_append_string(&insert_op, cfk_occurcontext, op->occurrence_context);
            bson_append_int(&insert_op, cfk_occurtype, op->rep_type);
            bson_append_string(&insert_op, cfk_occurrep, rp1->item);
            bson_append_string(&insert_op, cfk_occurtopic, rp2->item);
            bson_append_string(&insert_op, cfk_bundle, op->bundle);

            bson_finish(&insert_op);

            MongoInsert(&dbconn, MONGO_KM_OCCURRENCES, &insert_op, NULL);

            bson_destroy(&insert_op);
            }
        }
    }
    
// Inferences

    MongoRemove(&dbconn, MONGO_KM_INFERENCES, bson_empty(&b), NULL);

    for (ip = inferences; ip != NULL; ip = ip->next)
    {
        CfDebug("Add inferences (precedent,qualifier,inference) values ('%s','%s','%s')\n", ip->precedent,
                ip->qualifier, ip->inference);

        bson insert_op;
        bson_init(&insert_op);
        bson_append_new_oid(&insert_op, "_id");
        bson_append_string(&insert_op, cfk_precedent, ip->precedent);
        bson_append_string(&insert_op, cfk_qualifier, ip->qualifier);
        bson_append_string(&insert_op, cfk_inference, ip->inference);

        bson_finish(&insert_op);

        MongoInsert(&dbconn, MONGO_KM_INFERENCES, &insert_op, NULL);
        bson_destroy(&insert_op);
    }

    CFDB_Close(&dbconn);
#endif
}

/*****************************************************************************/

void SyntaxCompletion(char *s)
{
    int i, j, k, l, m;
    const SubTypeSyntax *ss;
    const BodySyntax *bs, *bs2 = NULL;
    const FnCallType *fn;

    if (EnterpriseExpiry())
    {
        CfOut(cf_error, "", "Cfengine - autonomous configuration engine. This enterprise license has expired.\n");
        exit(1);
    }

    if (strncmp(s, "function", strlen("function")) == 0)
    {
        Nova_ListFunctions();
        return;
    }

    if (strncmp(s, "bundle", strlen("bundle")) == 0 || strncmp(s, "agent", strlen("agent")) == 0)
    {
        Nova_ListAgents();
        return;
    }

    if (strcmp(s, "promise") == 0)
    {
        Nova_ListPromiseTypes();
        return;
    }

/* Else search for the specific word in the syntax tree */

    for (i = 0; i < CF3_MODULES; i++)
    {
        if ((ss = CF_ALL_SUBTYPES[i]) == NULL)
        {
            continue;
        }

        /* ss[j] is an array of promise types */

        for (j = 0; ss[j].bundle_type != NULL; j++)
        {
            bs = (BodySyntax *) ss[j].bs;

            if (s && strcmp(s, ss[j].subtype) == 0)
            {
                printf("Promise type %s has possible contraints:\n\n", ss[j].subtype);

                for (k = 0; bs[k].lval != NULL; k++)
                {
                    printf("   %s\n", bs[k].lval);
                }

                if (strcmp(s, "classes") == 0)
                {
                    /* non-unique */
                    continue;
                }
                else
                {
                    return;
                }
            }

            for (k = 0; bs[k].lval != NULL; k++)
            {
                if (s && strcmp(s, bs[k].lval) == 0)
                {
                    printf("constraint %s (of promise type %s) has possible values:\n\n", bs[k].lval, ss[j].subtype);

                    switch (bs[k].dtype)
                    {
                    case cf_bundle:
                        printf("   %s  ~ defines a separate bundle of promises\n\n", bs[k].lval);
                        break;
                    case cf_body:
                        printf("   %s  ~ defined in a separate body, with elements\n\n", bs[k].lval);
                        bs2 = (BodySyntax *) bs[k].range;

                        for (l = 0; bs2[l].lval != NULL; l++)
                        {
                            printf("     %s", bs2[l].lval);
                            for (m = 0; m + strlen(bs2[l].lval) < 22; m++)
                            {
                                putchar(' ');
                            }

                            printf("~ (%s)\n", (const char *) bs2[l].range);
                        }

                        printf("\nDescription: %s\n", bs2[l].description);
                        break;

                    default:
                        printf("   %s  ~ (%s)\n", bs[k].lval, (const char *) bs[k].range);
                        printf("\nDescription: %s\n", bs[k].description);
                        break;
                    }

                    return;
                }

                /* Could be in a sub-body */

                if (bs[k].dtype == cf_body)
                {
                    bs2 = (BodySyntax *) bs[k].range;

                    for (l = 0; bs2[l].lval != NULL; l++)
                    {
                        if (strcmp(s, bs2[l].lval) == 0)
                        {
                            printf("body constraint %s is part of %s (in promise type %s) and has possible values:\n\n",
                                   bs2[l].lval, bs[k].lval, ss[j].subtype);

                            printf("     %s", bs2[l].lval);
                            for (m = 0; m + strlen(bs2[l].lval) < 22; m++)
                            {
                                putchar(' ');
                            }

                            printf("~ (%s)\n", (const char *) bs2[l].range);
                            printf("\nDescription: %s\n", bs2[l].description);
                            return;
                        }
                    }
                }
            }
        }
    }

/* Check functions */

    fn = FindFunction(s);
    if (fn)
    {
        Nova_ListFunction(fn, true);
        return;
    }

/* If nothing found */

    printf("Try one of the following:\n\n");
    printf("   function\n");
    printf("   promise type\n");
    printf("   bundle type\n");
    printf("   <syntax element>\n");
}

/*****************************************************************************/

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

static void Nova_ListAgents()
{
    int i;

    printf("Agent types are used as bundle types and control-body types:\n\n");

    for (i = 0; CF_ALL_BODIES[i].bundle_type != NULL; i++)
    {
        printf("   %s\n", CF_ALL_BODIES[i].bundle_type);
    }
}

/*****************************************************************************/

static void Nova_ListFunctions()
{
    int i;

    printf("In-built functions:\n\n");

    for (i = 0; CF_FNCALL_TYPES[i].name != NULL; i++)
    {
        Nova_ListFunction(CF_FNCALL_TYPES + i, false);
    }
}

/*****************************************************************************/

static void Nova_ListFunction(const FnCallType *f, int full)
{
    int j;
    const FnCallArg *args = f->args;

    switch (f->dtype)
    {
    case cf_str:
        printf("   (string)  ");
        break;
    case cf_int:
        printf("   (integer) ");
        break;
    case cf_class:
        printf("   (class)   ");
        break;
    case cf_slist:
        printf("   (slist)   ");
        break;
    case cf_ilist:
        printf("   (ilist)   ");
        break;
    case cf_rlist:
        printf("   (rlist)   ");
        break;
    case cf_rrange:
    case cf_irange:
        printf("   (range)   ");
        break;
    default:
        printf("   (unknown) ");
        break;
    }

    printf("   %s", f->name);

    for (j = 0; strlen(f->name) + j < 18; j++)
    {
        putchar(' ');
    }

    printf(" %s\n", f->description);

    if (full)
    {
        printf("\n");
        for (j = 0; args[j].pattern != NULL; j++)
        {
            printf("       arg %d : %s, in the range %s\n", j, args[j].description, args[j].pattern);
        }
    }
}

/*****************************************************************************/

static void Nova_ListPromiseTypes()
{
    int i;

    printf("promise types:\n\n");

    for (i = 0; CF_COMMON_SUBTYPES[i].subtype != NULL; i++)
    {
        printf("   %s  (%s)\n", CF_COMMON_SUBTYPES[i].subtype, CF_COMMON_SUBTYPES[i].bundle_type);
    }
}

/*********************************************************************/
/* Library documents                                                 */
/*********************************************************************/

char *ThisHashPrint(unsigned char digest[EVP_MAX_MD_SIZE + 1])
{
    unsigned int i;
    static char buffer[EVP_MAX_MD_SIZE * 4];

    sprintf(buffer, "SHA=  ");

    for (i = 0; i < CF_SHA256_LEN; i++)
    {
        sprintf((char *) (buffer + 4 + 2 * i), "%02x", digest[i]);
    }

    return buffer;
}

/*********************************************************************/
void ThisHashString(char *str, char *buffer, int len, unsigned char digest[EVP_MAX_MD_SIZE + 1])
{
    EVP_MD_CTX context;
    const EVP_MD *md = NULL;
    int md_len;

    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_digests();
    ERR_load_crypto_strings();

    md = EVP_get_digestbyname("sha256");

    if (md == NULL)
    {
        CfOut(cf_verbose, "", "!! Digest type not supported by OpenSSL library");
    }

    EVP_DigestInit(&context, md);
    EVP_DigestUpdate(&context, (unsigned char *) buffer, len);

    EVP_DigestUpdate(&context, (unsigned char *) str, strlen(str));

    EVP_DigestFinal(&context, digest, &md_len);
}

/*********************************************************************/

void Nova_GenerateTestData(int count)
{
#ifdef HAVE_LIBMONGOC
    Rlist *testmachines = NULL, *rp = NULL, *total = NULL;
    Item *ip;
    time_t from;
    char newkeyhash[CF_BUFSIZE] = { 0 }, newaddresses[CF_MAXVARSIZE] = { 0 }, newhostnames[CF_BUFSIZE] = { 0 }, noDot[CF_BUFSIZE] = { 0 };
    unsigned char digest[EVP_MAX_MD_SIZE + 1];
    int i = 0;
    int currReport = -1;

    Item *packedReports = NULL;
    char buffer[1000000] = { 0 }, buf[CF_BUFSIZE] = { 0 };
    int bufsize = 1000000;
    int countLen = 0;
    int hostCount = 0;
    int startFrom = 0, len = 0;
    EnterpriseDB conn;

    LICENSES = 1;
    snprintf(CFWORKDIR, sizeof(CFWORKDIR), "/var/cfengine");
    from = time(NULL) - SECONDS_PER_WEEK;
    Nova_PackAllReports(&packedReports, from, 0, cfd_menu_full);

    for (ip = packedReports; ip != NULL; ip = ip->next)
    {
        // skip the first line
        if (i == 0)
        {
            i++;
            continue;
        }
        ReplaceTrailingChar(ip->name, '\n', '\0');
        snprintf(buf, sizeof(buf), "%s\n", ip->name);
        Join(buffer, buf, bufsize);
    }

    DeleteItemList(packedReports);

    len = strlen(buffer);
    i = 0;

    Item **reports = NewReportBook();
    currReport = -1;

    while (countLen < len)
    {
        while (buffer[countLen] != '\n')
        {
            buf[i] = buffer[countLen];
            i++;
            countLen++;
        }
        buf[i] = '\0';
        countLen++;
        i = 0;
        currReport = Nova_StoreIncomingReports(buf, reports, currReport);
    }

    testmachines = Nova_GetTestMachines();

    for (rp = testmachines; rp != NULL; rp = rp->next)
    {
        startFrom++;
    }
    DeleteRlist(testmachines);

    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    for (hostCount = 0; hostCount < count; hostCount++)
    {
        snprintf(newhostnames, sizeof(newhostnames), "%s_%s_%d", CF_TEST_HOSTNAME, noDot, startFrom + hostCount + 1);
        snprintf(newaddresses, sizeof(newaddresses), "%d.%d.%d.%d", 10, 255 % (startFrom + hostCount + 1),
                 255 % (startFrom + hostCount + 2), 255 % (startFrom + hostCount + 3));

        ThisHashString(newhostnames, newaddresses, strlen(newaddresses), digest);
        snprintf(newkeyhash, sizeof(newkeyhash), "%s", ThisHashPrint(digest));

        snprintf(PUBKEY_DIGEST, sizeof(PUBKEY_DIGEST), "%s", newkeyhash);
        snprintf(VIPADDRESS, CF_MAXVARSIZE - 1, "%s", newaddresses);
        snprintf(VFQNAME, CF_MAXVARSIZE - 1, "%s", newhostnames);
        UnpackReportBook(&dbconn, newkeyhash, reports);

        if (CFDB_Open(&conn))
        {
            CFDB_SaveHostID(&conn, MONGO_DATABASE, cfr_keyhash, newkeyhash, newaddresses, newhostnames, NULL);
            CFDB_SaveHostID(&conn, MONGO_ARCHIVE, cfr_keyhash, newkeyhash, newaddresses, newhostnames, NULL);
            CFDB_Close(&conn);
        }

    }
    CFDB_Close(&dbconn);
    DeleteReportBook(reports);

    hostCount = 0;
    total = Nova_GetTestMachines();

    for (rp = total; rp != NULL; rp = rp->next)
    {
        hostCount++;
    }
    DeleteRlist(total);
    printf("%d test machines added\n", hostCount - startFrom);
#else
    fprintf(stderr, "Test data generation requires MongoDB support\n");
#endif
}

#ifdef HAVE_LIBMONGOC
/*********************************************************************/
static Rlist *Nova_GetTestMachines(void)
{
    mongo_cursor *cursor;
    bson_iterator it;
    bson query;
    EnterpriseDB conn;

    char keyhash[CF_MAXVARSIZE], addresses[CF_MAXVARSIZE], hostnames[CF_MAXVARSIZE];
    char temp[CF_MAXVARSIZE];
    int total_added = 0;
    Rlist *testmachines = NULL;

    if (!CFDB_Open(&conn))
    {
        return NULL;
    }

    cursor = mongo_find(&conn, MONGO_DATABASE, bson_empty(&query), 0, 0, 0, CF_MONGO_SLAVE_OK);

    while (mongo_cursor_next(cursor) == MONGO_OK)   // loops over documents  
    {
        bson_iterator_init(&it, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';

        while (bson_iterator_next(&it))
        {
            if (bson_iterator_type(&it) == BSON_STRING && strcmp(bson_iterator_key(&it), cfr_keyhash) == 0)
            {
                CFDB_ScanHubHost(&it, keyhash, addresses, hostnames);
            }
            else if (strcmp(bson_iterator_key(&it), cfr_host_array) == 0)
            {
                CFDB_ScanHubHost(&it, keyhash, addresses, hostnames);
            }
            else if (strcmp(bson_iterator_key(&it), "_id") == 0)
            {
                bson_iterator_oid(&it);
            }
        }

        strncpy(temp, hostnames, strlen(CF_TEST_HOSTNAME));
        temp[strlen(CF_TEST_HOSTNAME)] = '\0';
        if (strcmp(temp, CF_TEST_HOSTNAME) == 0)
        {
            IdempPrependRScalar(&testmachines, keyhash, CF_SCALAR);
        }

        if (strcmp(temp, CF_TEST_HOSTNAME) != 0 && strlen(hostnames) > 0)
        {
            total_added++;
        }
    }

    mongo_cursor_destroy(cursor);

    if (!CFDB_Close(&conn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    return testmachines;
}
#endif

/****************************************************************************************/
void Nova_RemoveTestData(void)
{
#ifdef HAVE_LIBMONGOC
    Rlist *testmachines = NULL, *rp = NULL;
    int i = 0;

    testmachines = Nova_GetTestMachines();

    for (rp = testmachines; rp != NULL; rp = rp->next)
    {
        CFDB_RemoveTestData(MONGO_DATABASE, (char *) rp->item);
        CFDB_RemoveTestData(MONGO_DATABASE_MON_MG, (char *) rp->item);
        CFDB_RemoveTestData(MONGO_DATABASE_MON_WK, (char *) rp->item);
        CFDB_RemoveTestData(MONGO_DATABASE_MON_YR, (char *) rp->item);
        CFDB_RemoveTestData(MONGO_LOGS_REPAIRED, (char *) rp->item);
        CFDB_RemoveTestData(MONGO_LOGS_REPAIRED, (char *) rp->item);
        CFDB_RemoveTestData(MONGO_LOGS_NOTKEPT, (char *) rp->item);
        CFDB_RemoveTestData(MONGO_ARCHIVE, (char *) rp->item);
        i++;
    }
    DeleteRlist(testmachines);

    printf("%d test machines removed\n", i);
#endif
}

/****************************************************************************************/

void Nova_UpdateTestData(void)
{
#ifdef HAVE_LIBMONGOC
    mongo_cursor *cursor;
    bson_iterator it;
    EnterpriseDB conn;

    char keyhash[CF_MAXVARSIZE], addresses[CF_MAXVARSIZE];
    char temp[CF_MAXVARSIZE], hostnames[CF_MAXVARSIZE], noDot[CF_BUFSIZE] = { 0 };

    int i = 0;

    if (!CFDB_Open(&conn))
    {
        return;
    }

    bson query;

    cursor = mongo_find(&conn, MONGO_DATABASE, bson_empty(&query), 0, 0, 0, CF_MONGO_SLAVE_OK);

    while (mongo_cursor_next(cursor) == MONGO_OK)   // loops over documents   
    {

        bson_iterator_init(&it, mongo_cursor_bson(cursor));

        keyhash[0] = '\0';
        hostnames[0] = '\0';
        addresses[0] = '\0';

        while (bson_iterator_next(&it))
        {
            if (bson_iterator_type(&it) == BSON_STRING && strcmp(bson_iterator_key(&it), cfr_keyhash) == 0)
            {
                CFDB_ScanHubHost(&it, keyhash, addresses, hostnames);
            }
            else if (strcmp(bson_iterator_key(&it), cfr_host_array) == 0)
            {
                CFDB_ScanHubHost(&it, keyhash, addresses, hostnames);
            }
            else if (strcmp(bson_iterator_key(&it), "_id") == 0)
            {
                bson_iterator_oid(&it);
            }
        }

        // generate new names, ip and keyhash
        ReplaceChar(hostnames, noDot, sizeof(noDot), '.', '_');
        strncpy(temp, noDot, strlen(CF_TEST_HOSTNAME));
        temp[strlen(CF_TEST_HOSTNAME)] = '\0';

        if (strcmp(temp, CF_TEST_HOSTNAME) == 0)
        {                        
            bson_init(&query);
            bson_append_string(&query, cfr_keyhash, keyhash);
            bson_finish(&query);

            bson set_op;

            bson_init(&set_op);
            {
                bson_append_start_object(&set_op, "$set");
                bson_append_int(&set_op, cfr_day, (long) time(NULL));
                bson_append_finish_object(&set_op);
            }
            bson_finish(&set_op);

            MongoUpdate(&conn, MONGO_DATABASE, &query, &set_op, MONGO_UPDATE_UPSERT, NULL);
            MongoCheckForError(&conn, "UpdateTestData", keyhash, NULL);

            bson_destroy(&set_op);
            bson_destroy(&query);
            i++;
        }
    }

    mongo_cursor_destroy(cursor);

    printf("%d test machines updated\n", i);

    if (!CFDB_Close(&conn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }
#endif
}

/*********************************************************************/
