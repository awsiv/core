/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "cf3.defs.h"
#include "knowledge.h"

#include "files_names.h"
#include "item_lib.h"
#include "datapack.h"
#include "cfstream.h"
#include "fncall.h"
#include "string_lib.h"
#include "ontology.h"
#include "hashes.h"

#if defined(HAVE_LIBMONGOC)
#include "db_query.h"
#include "bson_lib.h"
#endif

/* From cfknow.c */ // FIX
int GetTopicPid(char *classified_topic);

static void Nova_ListAgents(void);
static void Nova_ListFunctions(void);
static void Nova_ListFunction(const FnCallType *f, int full);
static void Nova_ListPromiseTypes();

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
            BsonFinish(&insert_op);

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
       BsonFinish(&insert_op);
       
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
                    (char *)rp2->item, op->occurrence_context, op->locator, op->rep_type, (const char *) rp1->item);

            bson insert_op;
            bson_init(&insert_op);
            bson_append_new_oid(&insert_op, "_id");
            bson_append_string(&insert_op, cfk_occurlocator, op->locator);
            bson_append_string(&insert_op, cfk_occurcontext, op->occurrence_context);
            bson_append_int(&insert_op, cfk_occurtype, op->rep_type);
            bson_append_string(&insert_op, cfk_occurrep, rp1->item);
            bson_append_string(&insert_op, cfk_occurtopic, rp2->item);
            bson_append_string(&insert_op, cfk_bundle, op->bundle);
            BsonFinish(&insert_op);
            
            MongoInsert(&dbconn, MONGO_KM_OCCURRENCES, &insert_op, NULL);
            bson_destroy(&insert_op);

            if (strcmp(op->occurrence_context, "any") != 0)
               {
               char topic_name[CF_BUFSIZE], topic_context[CF_BUFSIZE], qualified[CF_BUFSIZE];
               DeClassifyTopic(rp2->item, topic_name, topic_context);
               snprintf(qualified, CF_BUFSIZE, "%s::%s", op->occurrence_context, topic_name);
               
               bson insert_op;
               bson_init(&insert_op);
               bson_append_new_oid(&insert_op, "_id");
               bson_append_string(&insert_op, cfk_occurlocator, op->locator);
               bson_append_string(&insert_op, cfk_occurcontext, topic_context);
               bson_append_int(&insert_op, cfk_occurtype, op->rep_type);
               bson_append_string(&insert_op, cfk_occurrep, rp1->item);
               bson_append_string(&insert_op, cfk_occurtopic, qualified);
               bson_append_string(&insert_op, cfk_bundle, op->bundle);
               BsonFinish(&insert_op);
               MongoInsert(&dbconn, MONGO_KM_OCCURRENCES, &insert_op, NULL);
               bson_destroy(&insert_op);
               }
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

        BsonFinish(&insert_op);

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
                    case DATA_TYPE_BUNDLE:
                        printf("   %s  ~ defines a separate bundle of promises\n\n", bs[k].lval);
                        break;
                    case DATA_TYPE_BODY:
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

                if (bs[k].dtype == DATA_TYPE_BODY)
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

    fn = FnCallTypeGet(s);
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
    case DATA_TYPE_STRING:
        printf("   (string)  ");
        break;
    case DATA_TYPE_INT:
        printf("   (integer) ");
        break;
    case DATA_TYPE_CONTEXT:
        printf("   (class)   ");
        break;
    case DATA_TYPE_STRING_LIST:
        printf("   (slist)   ");
        break;
    case DATA_TYPE_INT_LIST:
        printf("   (ilist)   ");
        break;
    case DATA_TYPE_REAL_LIST:
        printf("   (rlist)   ");
        break;
    case DATA_TYPE_REAL_RANGE:
    case DATA_TYPE_INT_RANGE:
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

/*****************************************************************************/
