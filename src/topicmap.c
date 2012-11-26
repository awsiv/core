/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "files_names.h"
#include "db_query.h"
#include "bson_lib.h"
#include "item_lib.h"
#include "sort.h"
#include "matching.h"
#include "cfstream.h"
#include "string_lib.h"
#include "communication.h"

#include <assert.h>

/*****************************************************************************/

static int Nova_NewVertex(GraphNode *tribe, int node, int distance, int real, char *name, char *context);

/*****************************************************************************/

typedef struct Hit_ Hit;

/*****************************************************************************/

struct Hit_
{
    char *occurrence_context;
    char *locator;                 /* Promiser */
    enum representations rep_type;
    char *represents;
    Hit *next;
};

/*****************************************************************************/

enum { cftcp4, cftcp6, cfudp4, cfudp6, CFDIM} session;

struct servhist
{
   char name[CF_SMALLBUF];
   char port[CF_BUFSIZE];
   int freq[CFDIM];
   
};

/*****************************************************************************/

#define CF_SERVICES_LIMIT 64
#include "cf.nova.web_api.h"
#include "db_query.h"
#include "web_rbac.h"

/*****************************************************************************/

static void NewHit(Hit **list,char *context, char *locator, enum representations locator_type, char *represents);
static void DeleteHitList(Hit *list);
static Hit *HitExists(Hit *list, char *locator, enum representations rep_type, char *context);
static int MergeExistingContexts(Item **list, char *topic_name, char *topic_context, int merge);
static int Nova_GetTopicIdForPromiseHandle(int handle_id, char *buffer, int bufsize);
static void GetPortFrequencies(EnterpriseDB *dbconn, char *variable, struct servhist *services, const int type);
static void GetClassHostFrequencies(char *srv, int *h1, int *h2, int *h3, int *l1, int *l2, int *l3, int *normal);

/*****************************************************************************/
/* SEARCH workers                                                            */
/*****************************************************************************/

void Nova_SearchHosts(Item **list, char *search, int type, char *username)
{
    EnterpriseDB conn;
    HubQuery *hq;
    HubHost *hh;
    Rlist *rp;
    char *addr, *name;
    char url[CF_BUFSIZE];

    if (!CFDB_Open(&conn))
    {
        return;
    }

    if (strncmp(search, "SHA=", 4) == 0)
       {
       printf("Lookfor hosts by key %s\n", search);

       hq = CFDB_QueryHostByHostKey(&conn, search);
       }
    else
       {
       HubQuery *hqHostClassFilter = CFDB_HostClassFilterFromUserRBAC(username);
       
       // ?? ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);
       
       HostClassFilter *filter = (HostClassFilter *) HubQueryGetFirstRecord(hqHostClassFilter);

       if (IsIPV6Address((char *)search) || IsIPV4Address((char *)search))
          {
          addr = search;
          name = NULL;
          }
       else
          {
          addr = NULL;
          name = search;
          }
       
       hq = CFDB_QueryHostsByAddress(&conn, name, addr, filter);

       for (rp = hq->hosts; rp != NULL; rp = rp->next)
          {
          hh = (HubHost *) rp->item;
          snprintf(url, sizeof(url), "assets/host_assets/%s\n", hh->keyhash);
          PrependItem(list, hh->hostname, url);
          PrependFullItem(list, hh->hostname, url, CF_CATEGORY_HOSTS, 0);
          }

       DeleteHubQuery(hq, free);
       DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
       }
        
    CFDB_Close(&conn);

}

/*****************************************************************************/

void Nova_SearchClasses(Item **list, char *search, int type, char *username)
{
 char url[CF_BUFSIZE], text[CF_BUFSIZE];

/*

    EnterpriseDB conn;
    HubQuery *hq;
    HubHost *hh;
    Rlist *rp;
    char *addr, *name;

  Return URL to hosts matching search string (classes)
  CFDB_HostsWithClass(Rlist **return_list, char *class_name, char *return_format)

    HostClassFilter *filter = NewHostClassFilter(NULL, NULL);
    HostClassFilterAddClasses(filter, class_name, NULL);

    HubQuery *hq = CFDB_QueryHostsByHostClassFilter(&conn, filter);

    // Return a link to a list of matching hosts, and url
    PrependItem(list, hh->hostname, url);
    
    DeleteHostClassFilter(filter);
*/

    snprintf(url, CF_BUFSIZE, "/search/index/name/%s/hosts_only/true/host/All/report/contexts", search);
    snprintf(text, CF_BUFSIZE, "Generate a list of hosts defining classes like \"%s\"", search);
    PrependFullItem(list, text, url, CF_CATEGORY_CLASSES, 0);
}

/*****************************************************************************/

void Nova_SearchVariables(Item **list, char *search, int type, char *username)
{
// Return URL to hosts matching search string (classes)

 char text[CF_BUFSIZE];
 snprintf(text, CF_BUFSIZE, "Generate a list of hosts with variables matching %s", search);
 PrependFullItem(list, text, "/advancedreports", CF_CATEGORY_VARS, 0);
}

/*****************************************************************************/

void Nova_SearchReports(Item **list, char *search)
{
 char text[CF_BUFSIZE], url[CF_BUFSIZE];
   int s,e;

 for (int i = 0; i < cfrep_unknown; i++)
    {
    if (BlockTextMatch(search, BASIC_REPORTS[i].name_old, &s, &e) || BlockTextMatch(search, BASIC_REPORTS[i].name, &s, &e))
       {
       snprintf(url, CF_BUFSIZE, "%s/%s", REPORT_CONTROLLER_PREFIX, BASIC_REPORTS[i].id);
       snprintf(text, CF_BUFSIZE, "Generate a %s report", BASIC_REPORTS[i].name);
       PrependFullItem(list, text, url, CF_CATEGORY_REPORTS, 0);
       }
    }
}

/*****************************************************************************/

Item *Nova_SearchTopicMap(char *search_topic,int search_type,int merge)
{
    bson_iterator it1;
    EnterpriseDB conn;
    char topic_name[CF_BUFSIZE];
    char topic_context[CF_BUFSIZE];
    int topic_id;
    Item *list = NULL;

    if (!CFDB_Open(&conn))
    {
        return NULL;
    }

    // Return topics matching search string (classes)
    
    bson query;

    bson_init(&query);

    DeClassifyTopic(search_topic, topic_name, topic_context);

    if (search_type == CF_SEARCH_REGEX)
       {
       if (!NULL_OR_EMPTY(search_topic))
          {          
          bson_append_regex(&query, cfk_topicname, topic_name, "");

          if (strcmp(topic_context,"any") != 0)
             {
             bson_append_regex(&query, cfk_topiccontext, topic_context, "");
             }
          }
       }
    else // CF_SEARCH_EXACT
       {
       if (!NULL_OR_EMPTY(search_topic))
          {          
          bson_append_string(&query, cfk_topicname, topic_name);
          if (strcmp(topic_context,"any") != 0)
             {
             bson_append_string(&query, cfk_topiccontext, topic_context);
             }
          }       
       }
    
    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;
    BsonSelectReportFields(&fields, 7,
                           cfk_topicname,
                           cfk_topicid,
                           cfk_topiccontext,
                           cfk_associations,
                           cfk_associd,
                           cfk_assoccontext,
                           cfk_assocname);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_TOPICS, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))   // loops over documents
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        topic_name[0] = '\0';
        topic_context[0] = '\0';
        topic_id = 0;

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            /* Query specific search/marshalling */

            if (strcmp(bson_iterator_key(&it1), cfk_topicname) == 0)
            {
                strncpy(topic_name, bson_iterator_string(&it1), CF_BUFSIZE - 1);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_topiccontext) == 0)
            {
                strncpy(topic_context, bson_iterator_string(&it1), CF_BUFSIZE - 1);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_topicid) == 0)
            {
                topic_id = (int) bson_iterator_int(&it1);
            }
        }

        if (!MergeExistingContexts(&list, topic_name, topic_context,merge))
        {
            PrependFullItem(&list, topic_name, topic_context, topic_id, 0);
        }
    }

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);

    return list;
}

/*****************************************************************************/
/* The main panels                                                           */
/*****************************************************************************/

void Nova_DumpTopics()
{
    bson_iterator it1;
    EnterpriseDB conn;
    char topic_name[CF_BUFSIZE];
    char topic_context[CF_BUFSIZE];
    int topic_id;
    Item *ip, *nn;

    if (!CFDB_Open(&conn))
    {
        return;
    }

/* BEGIN query document */
    bson query;

    bson_empty(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;

    BsonSelectReportFields(&fields, 7,
                           cfk_topicname,
                           cfk_topicid,
                           cfk_topiccontext,
                           cfk_associations,
                           cfk_associd,
                           cfk_assoccontext,
                           cfk_assocname);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_TOPICS, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))   // loops over documents
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        topic_name[0] = '\0';
        topic_context[0] = '\0';
        topic_id = 0;

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            /* Query specific search/marshalling */

            if (strcmp(bson_iterator_key(&it1), cfk_topicname) == 0)
            {
                strncpy(topic_name, bson_iterator_string(&it1), CF_BUFSIZE - 1);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_topiccontext) == 0)
            {
                strncpy(topic_context, bson_iterator_string(&it1), CF_BUFSIZE - 1);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_topicid) == 0)
            {
                topic_id = (int) bson_iterator_int(&it1);
            }
        }

        printf("Topic: %s::%s, id: %d \n", topic_context, topic_name, topic_id);

        nn = Nova_ScanLeadsAssociations(topic_id, NULL);

        printf("   {\n");
        for (ip = nn; ip != NULL; ip = ip->next)
        {
            printf("   (%s) \"%s\"\n", ip->name, ip->classes);
        }
        printf("   }\n");

        DeleteItemList(nn);
    }

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);
}

/*****************************************************************************/

void Nova_ShowTopic(char *qualified_topic)
{
     char topic_name[CF_BUFSIZE], topic_context[CF_BUFSIZE], topic_id[CF_BUFSIZE], bundle[CF_BUFSIZE];
    int id;
    Writer *writer = NULL;
    JsonElement *json = NULL;

    DeClassifyTopic(qualified_topic, topic_name, topic_context);
    id = Nova_GetTopicIdForTopic(qualified_topic);

    printf("Search: %s\n",topic_name);

    if (id == 0)
       {
       printf("Found nothing\n");
       return;
       }

    Nova_GetTopicByTopicId(id, topic_name, topic_id, topic_context,bundle);
    printf("Found (%d): \"%s::%s = %s\" in bundle %s \n", id, topic_context, topic_name, topic_id, bundle);

    writer = StringWriter();
    json = Nova2PHP_show_all_context_leads(topic_name, "dummy_user");
    JsonElementPrint(writer, json, 1);
    JsonElementDestroy(json);
    printf("\nAssociations: %s\n", StringWriterData(writer));
    WriterClose(writer);

    json = Nova_ScanOccurrences(id, "dummy_user");
    
    if (json)
    {
        writer = NULL;
        writer = StringWriter();

        JsonElementPrint(writer, json, 1);
        JsonElementDestroy(json);
        printf("\nOccurrences: %s\n\n", StringWriterData(writer));

        WriterClose(writer);
    }

    json = Nova2PHP_list_promises_with_promisee(qualified_topic, "admin");

    if (json)
    {
        writer = NULL;
        writer = StringWriter();

        JsonElementPrint(writer, json, 1);
        JsonElementDestroy(json);
        printf("\nPromisee/stakeholders: %s\n\n", StringWriterData(writer));

        WriterClose(writer);
    }
}

/*****************************************************************************/

int Nova_GetTopicIdForTopic(char *typed_topic)
{
    char topic[CF_BUFSIZE], type[CF_BUFSIZE];
    bson_iterator it1;
    EnterpriseDB conn;
    int topic_id = 0;

    DeClassifyTopic(ToLowerStr(typed_topic), topic, type); // Linker trouble - copy this from core

    if (!CFDB_Open(&conn))
    {
        return false;
    }

/* BEGIN query document */
    bson query;

    bson_init(&query);
    bson_append_string(&query, cfk_topicname, topic);

    if (strlen(type) > 0 && strcmp(type, "any") != 0)
    {
        bson_append_string(&query, cfk_topiccontext, type);
    }

    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;

    BsonSelectReportFields(&fields, 3, cfk_topicname, cfk_topicid, cfk_topiccontext);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_TOPICS, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))   // loops over documents
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (strcmp(bson_iterator_key(&it1), cfk_topicid) == 0)
            {
                topic_id = (int) bson_iterator_int(&it1);
                break;
            }
        }

        if (topic_id > 0)
           {
           break;
           }
    }

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);
    return topic_id;
}


/*****************************************************************************/

static int Nova_GetTopicIdForPromiseHandle(int handle_id, char *buffer, int bufsize)
{
    Item *ip, *nn = Nova_ScanLeadsAssociations(handle_id, NOVA_HANDLE);

    char topic_name[CF_BUFSIZE], topic_id[CF_BUFSIZE], topic_context[CF_BUFSIZE], bundle[CF_BUFSIZE];
    Nova_GetTopicByTopicId(handle_id, topic_name, topic_id, topic_context, bundle);
    
    for (ip = nn; ip != NULL; ip = ip->next)
       {
       char declass[CF_BUFSIZE];
       sscanf(ip->classes, "%*[^:]::%[^\n]",declass);
       snprintf(buffer,bufsize,"%s",declass);
       return ip->counter;
       }

    return 0;
}

/*****************************************************************************/

int Nova_GetTopicByTopicId(int search_id, char *topic_name, char *topic_id, char *topic_context, char *bundle)
{
    bson_iterator it1;
    EnterpriseDB conn;
    int topicid = 0;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

/* BEGIN query document */
    bson query;

    bson_init(&query);
    bson_append_int(&query, cfk_topicid, search_id);
    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;
    BsonSelectReportFields(&fields, 4, cfk_topicname, cfk_topicid, cfk_topiccontext, cfk_bundle);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_TOPICS, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))   // loops over documents
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        topic_name[0] = '\0';
        topic_context[0] = '\0';
        topicid = 0;

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (strcmp(bson_iterator_key(&it1), cfk_topicname) == 0)
            {
                strncpy(topic_name, bson_iterator_string(&it1), CF_BUFSIZE - 1);
                strncpy(topic_id, Name2Id((char *) bson_iterator_string(&it1)), CF_BUFSIZE - 1);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_topiccontext) == 0)
            {
                strncpy(topic_context, bson_iterator_string(&it1), CF_BUFSIZE - 1);
            }

            if (bundle != NULL && strcmp(bson_iterator_key(&it1), cfk_bundle) == 0)
            {
                strncpy(bundle, bson_iterator_string(&it1), CF_BUFSIZE - 1);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_topicid) == 0)
            {
                topicid = (int) bson_iterator_int(&it1);
            }
        }
    }

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);
    return topicid;
}

/*****************************************************************************/

JsonElement *Nova_ScanTheRest(int pid)
/* Find other topics in the same context */
{
    char this_name[CF_BUFSIZE], this_id[CF_BUFSIZE], this_context[CF_BUFSIZE];
    Item *worklist, *ip;
    int id = 0;
    JsonElement *json_obj_out = JsonObjectCreate(2);

    id = Nova_GetTopicByTopicId(pid, this_name, this_id, this_context, NULL);
    if (!id)
    {
        return json_obj_out;
    }

    // Find other topics that have this topic as their category (sub topics)
    worklist = Nova_GetTopicsInContext(this_id);

    JsonElement *json_arr_subtopics = JsonArrayCreate(100);
    for (ip = worklist; ip != NULL; ip = ip->next)
    {
        JsonElement *json_obj_subtopic = JsonObjectCreate(3);
        JsonObjectAppendString(json_obj_subtopic, "context", ip->classes);
        JsonObjectAppendString(json_obj_subtopic, "topic", ip->name);
        JsonObjectAppendInteger(json_obj_subtopic, "id", ip->counter);

        JsonArrayAppendObject(json_arr_subtopics, json_obj_subtopic);
    }
    DeleteItemList(worklist);

    // Find other topics in the same context
    worklist = Nova_GetTopicsInContext(this_context);

    JsonElement *json_arr_othertopics = JsonArrayCreate(100);
    for (ip = worklist; ip != NULL; ip = ip->next)
    {
        JsonElement *json_obj_othertopic = JsonObjectCreate(3);
        JsonObjectAppendString(json_obj_othertopic, "context", ip->classes);
        JsonObjectAppendString(json_obj_othertopic, "topic", ip->name);
        JsonObjectAppendInteger(json_obj_othertopic, "id", ip->counter);

        JsonArrayAppendObject(json_arr_othertopics, json_obj_othertopic);
    }
    DeleteItemList(worklist);

    JsonElement *json_obj_topic = JsonObjectCreate(4);
    JsonObjectAppendString(json_obj_topic, "context", this_context);
    JsonObjectAppendString(json_obj_topic, "name", this_name);
    JsonObjectAppendInteger(json_obj_topic, "id", id);
    JsonObjectAppendArray(json_obj_topic, "sub_topics", json_arr_subtopics);

    JsonObjectAppendObject(json_obj_out, "topic", json_obj_topic);
    JsonObjectAppendArray(json_obj_out, "other_topics", json_arr_othertopics);

    return json_obj_out;
}

/*****************************************************************************/

Item *Nova_ScanLeadsAssociations(int search_id, char *assoc_mask)
/* Look for neighbours and retain/sort link names - return JSON array

  [
   {
   assoc: "is related to",
   topics: [  { topic: "name", id: 7},   { topic: "name2", id: 8}   ]
   }, ...
  ]

*/
{
    bson_iterator it1, it2, it3;
    EnterpriseDB conn;
    char assoc_name[CF_BUFSIZE];
    char afwd[CF_BUFSIZE];
    char assoc_context[CF_BUFSIZE];
    Item *list = NULL;
    int assoc_id;

    if (!CFDB_Open(&conn))
    {
        return NULL;
    }

/* BEGIN query document */
    bson query;

    bson_init(&query);
    bson_append_int(&query, cfk_topicid, search_id);
    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;
    BsonSelectReportFields(&fields, 4, cfk_associations, cfk_associd, cfk_assoccontext, cfk_assocname);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_TOPICS, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))   // loops over documents
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (strcmp(bson_iterator_key(&it1), cfk_associations) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator_subiterator(&it2, &it3);

                    assoc_id = 0;
                    assoc_name[0] = '\0';
                    assoc_context[0] = '\0';
                    afwd[0] = '\0';

                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfk_associd) == 0)
                        {
                            assoc_id = bson_iterator_int(&it3);
                        }

                        if (strcmp(bson_iterator_key(&it3), cfk_assocname) == 0)
                        {
                            strncpy(assoc_name, bson_iterator_string(&it3), CF_BUFSIZE - 1);
                        }

                        if (strcmp(bson_iterator_key(&it3), cfk_fwd) == 0)
                        {
                            strncpy(afwd, bson_iterator_string(&it3), CF_BUFSIZE - 1);
                        }

                        if (strcmp(bson_iterator_key(&it3), cfk_assoccontext) == 0)
                        {
                            strncpy(assoc_context, bson_iterator_string(&it3), CF_BUFSIZE - 1);
                        }
                    }

                    if (assoc_mask == NULL || FullTextMatch(assoc_mask, afwd))
                    {
                        char qualified[CF_BUFSIZE];

                        snprintf(qualified, CF_BUFSIZE, "%s::%s", assoc_context, assoc_name);
                        PrependFullItem(&list, afwd, qualified, assoc_id, 0);
                    }
                }
            }
        }
    }

    list = SortItemListNames(list);

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);
    return list;
}

/*****************************************************************************/

JsonElement *Nova_ScanOccurrences(int this_id, char *username)
{
    enum representations locator_type;
    char topic_name[CF_BUFSIZE] = { 0 },
         topic_id[CF_BUFSIZE] = { 0 },
         topic_context[CF_BUFSIZE] = { 0 };
    char locator[CF_BUFSIZE], context[CF_BUFSIZE], represents[CF_BUFSIZE], topic[CF_BUFSIZE], text[CF_BUFSIZE];
    bson_iterator it1;
    EnterpriseDB conn;
    Hit *hits = NULL, *hp;

    
// Do we want to prune using the topic context?

/* Match occurrences that could overlap with the current context.
   Take a simple approach - if we are in topic_context, then
   topic_context&anything is within topic_context, but topic_context|anything
   must be too big. BUT anything must!= "any"
*/

    if (!CFDB_Open(&conn))
    {
        return JsonArrayCreate(1);
    }

    Nova_GetTopicByTopicId(this_id, topic_name, topic_id, topic_context, NULL);

    if (strlen(topic_name) == 0)
    {
        return JsonArrayCreate(1);
    }
    
    // Using a regex here is greedy, but it helps to brainstorm
    bson query;

    bson_init(&query);

    // The following search is greedy, but probably best, else we could try for a distinct word
    // snprintf(searchstr,CF_BUFSIZE,"^%s$|\\s+%s\\s+",topic_name,topic_name);

    bson_append_regex(&query, cfk_occurtopic, topic_name, "");

    if (strcmp("any", topic_context) != 0)
       {
       // We no longer search for a specific context, but rather aggregate all contexts
       // bson_append_string(&bb, cfk_occurtopic, topic_context);
       }
    
    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;
    BsonSelectReportFields(&fields, 5,
                           cfk_occurcontext,
                           cfk_occurlocator,
                           cfk_occurtype,
                           cfk_occurrep,
                           cfk_occurtopic);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_OCCURRENCES, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))   // loops over documents
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        locator[0] = '\0';
        topic[0] = '\0';
        context[0] = '\0';
        represents[0] = '\0';
        locator_type = cfk_literal;

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            /* Query specific search/marshalling */

            if (strcmp(bson_iterator_key(&it1), cfk_occurlocator) == 0)
            {
                strncpy(locator, bson_iterator_string(&it1), CF_BUFSIZE - 1);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_occurcontext) == 0)
            {
                strncpy(context, bson_iterator_string(&it1), CF_BUFSIZE - 1);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_occurrep) == 0)
            {
                strncpy(represents, bson_iterator_string(&it1), CF_BUFSIZE - 1);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_occurtopic) == 0)
            {
                strncpy(topic, bson_iterator_string(&it1), CF_BUFSIZE - 1);
            }
            
            if (strcmp(bson_iterator_key(&it1), cfk_occurtype) == 0)
            {
                locator_type = (int) bson_iterator_int(&it1);
            }
        }

        if (RBACPruneKnowledge(topic, NULL, username))
        {
            NewHit(&hits, "RBAC settings", RBAC_ERROR_MSG, cfk_literal, topic);
        }
        else
        {
            char topic_short[CF_BUFSIZE],tc[CF_BUFSIZE];
            DeClassifyTopic(topic, topic_short, tc);
            snprintf(text,CF_BUFSIZE,"%s -- about %s",represents,topic_short);
            NewHit(&hits,context, locator, locator_type, text);
        }
    }

    JsonElement *json_array = JsonArrayCreate(10);

    for (hp = hits; hp != NULL; hp= hp->next)
    {
        JsonArrayAppendObject(json_array, Nova_AddOccurrenceBuffer(hp->occurrence_context,
                                                                 hp->locator, hp->rep_type,
                                                                 hp->represents));
    }

    DeleteHitList(hits);

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);

    return json_array;
}

/*************************************************************************/

int Nova_GetTopicComment(char *topic_name, char *topic_context, char *buffer, int bufsize)
{
    char searchstring[CF_BUFSIZE];
    bson_iterator it1;
    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

    if (topic_context)
       {
       snprintf(searchstring, CF_BUFSIZE, "%s::%s", topic_context, topic_name);
       }
    else
       {
       snprintf(searchstring, CF_BUFSIZE, "%s", topic_name);
       }
    
    bson query;

    bson_init(&query);
    bson_append_string(&query, cfk_occurtopic, searchstring);
    bson_append_string(&query, cfk_occurrep, "Comment");
    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;
    BsonSelectReportFields(&fields, 3,
                           cfk_occurtopic,
                           cfk_occurcontext,
                           cfk_occurlocator);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_OCCURRENCES, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    snprintf(buffer, bufsize, "No available comment");

    while (MongoCursorNext(cursor))
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (strcmp(bson_iterator_key(&it1), cfk_occurlocator) == 0)
            {
                strncpy(buffer, bson_iterator_string(&it1), bufsize - 1);
                mongo_cursor_destroy(cursor);
                CFDB_Close(&conn);
                return true;
            }
        }
    }
    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);
    return false;
}

/*************************************************************************/

Item *Nova_GetStakeHolders(int topic_id)
{
 return Nova_NearestNeighbours(topic_id, NOVA_STAKEHOLDER);
}

/*************************************************************************/

Item *Nova_GetBusinessGoals(char *handle)
{
    char querytopic[CF_BUFSIZE];
    Item *worklist = NULL, *ip;
    int pid;

    snprintf(querytopic, CF_BUFSIZE, "bundles::%s", handle);
    pid = Nova_GetTopicIdForTopic(querytopic);

    worklist = Nova_NearestNeighbours(pid, NOVA_GOAL);

    for (ip = worklist; ip != NULL; ip = ip->next)
    {
        Nova_FillInGoalComment(ip);
    }

    worklist = SortItemListNames(worklist);
    return worklist;
}

/*************************************************************************/

int Nova_GetUniqueBusinessGoals(char *buffer, int bufsize)
{
    Rlist *rp;
    bson_iterator it1;
    char topic_name[CF_MAXVARSIZE] = { 0 };
    int topic_id = 0;
    char work[CF_BUFSIZE] = { 0 };
    char searchstring[CF_BUFSIZE] = { 0 };
    Rlist *goal_patterns = NULL;
    char db_goal_patterns[CF_BUFSIZE] = { 0 };

    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

    if ( CFDB_GetValue( &conn, "goal_patterns", db_goal_patterns, sizeof(db_goal_patterns), MONGO_SCRATCH ) )
    {
        goal_patterns = SplitStringAsRList(db_goal_patterns, ',');
    }

    for (rp = goal_patterns; rp != NULL; rp = rp->next)
    {
        snprintf(work, CF_MAXVARSIZE - 1, "handles::%s|goals::.*|", (char *) rp->item);
        strcat(searchstring, work);
    }

    if (strlen(searchstring) > 1)
    {
        searchstring[strlen(searchstring) - 1] = '\0';
    }
    else
    {
        snprintf(searchstring, CF_MAXVARSIZE - 1, "handles::goal_.*|goals::.*");
    }   

/* BEGIN query document */
    bson query;

    bson_init(&query);
    bson_append_regex(&query, cfk_occurtopic, searchstring, "");
    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;
    BsonSelectReportFields(&fields, 5,
                           cfk_occurcontext,
                           cfk_occurlocator,
                           cfk_occurtype,
                           cfk_occurrep,
                           cfk_occurtopic);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_OCCURRENCES, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    JsonElement *json_array_out = JsonArrayCreate(100);
    char description[CF_BUFSIZE] = {0};
    char refer[CF_BUFSIZE] = {0};
    int referred = false;
    Item *check = NULL;
    
    while (MongoCursorNext(cursor))   // loops over documents
    {
        JsonElement *json_obj = JsonObjectCreate(4);
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        topic_id = 0;
        
        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {   
            if (strcmp(bson_iterator_key(&it1), cfk_occurlocator) == 0)
            {
                snprintf(description,CF_BUFSIZE-1,"%s", bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1), cfk_occurtopic) == 0)
            {
                snprintf(topic_name, CF_MAXVARSIZE, "%s", bson_iterator_string(&it1));
                topic_id = Nova_GetTopicIdForTopic(topic_name);
                referred = Nova_GetTopicIdForPromiseHandle(topic_id,refer,CF_BUFSIZE);
            }
        }

        char topic[CF_BUFSIZE],context[CF_BUFSIZE];
        DeClassifyTopic(topic_name, topic, context);
        
        if (referred != 0 && !IsItemIn(check,topic))
        {
            JsonObjectAppendString(json_obj, "description", description);
            JsonObjectAppendString(json_obj, "name", refer);
            JsonObjectAppendString(json_obj, "handle", topic);
            JsonObjectAppendInteger(json_obj, "pid", referred);

            Item *nn = Nova_NearestNeighbours(referred, KM_INVOLVES_CERT_F);
           
            if (nn)
            {
                JsonElement *json_array = JsonArrayCreate(50);
               
                for (Item *ip = nn; ip != NULL; ip = ip->next)
                {
                    if (ip->counter == referred || ip->counter == topic_id)
                    {
                        continue;
                    }
                  
                JsonElement *json_service = JsonObjectCreate(3);
                JsonObjectAppendString(json_service, "topic", ip->name);
                JsonObjectAppendString(json_service, "context", ip->classes);
                JsonObjectAppendInteger(json_service, "topic_id", ip->counter);
                JsonArrayAppendObject(json_array, json_service);
                }

                PrependItem(&check, topic, NULL);
                JsonObjectAppendArray(json_obj, "comprises", json_array);
            }

            JsonArrayAppendObject(json_array_out, json_obj);
        }           
    }
    
    DeleteItemList(check);
    Writer *writer = StringWriter();
    JsonElementPrint(writer, json_array_out, 1);
    JsonElementDestroy(json_array_out);
    snprintf(buffer, bufsize,"%s", StringWriterData(writer));
    WriterClose(writer);

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);

    return true;
}

/*************************************************************************/

JsonElement *Nova_summarize_all_goals(char *username)
{
    // Based on Nova_GetUniqueBusinessGoals(buffer, bufsize))
    Rlist *rp;
    bson_iterator it1;
    char topic_name[CF_MAXVARSIZE] = { 0 };
    int topic_id = 0;
    char work[CF_BUFSIZE] = { 0 };
    char searchstring[CF_BUFSIZE] = { 0 };
    Rlist *goal_patterns = NULL;
    char db_goal_patterns[CF_BUFSIZE] = { 0 };

    EnterpriseDB conn;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

    if ( CFDB_GetValue( &conn, "goal_patterns", db_goal_patterns, sizeof(db_goal_patterns), MONGO_SCRATCH ) )
    {
        goal_patterns = SplitStringAsRList(db_goal_patterns, ',');
    }

    for (rp = goal_patterns; rp != NULL; rp = rp->next)
    {
        snprintf(work, CF_MAXVARSIZE - 1, "handles::%s|goals::.*|", (char *) rp->item);
        strcat(searchstring, work);
    }

    if (strlen(searchstring) > 1)
    {
        searchstring[strlen(searchstring) - 1] = '\0';
    }
    else
    {
        snprintf(searchstring, CF_MAXVARSIZE - 1, "handles::goal_.*|goals::.*");
    }

/* BEGIN query document */
    bson query;

    bson_init(&query);
    bson_append_regex(&query, cfk_occurtopic, searchstring, "");
    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;
    BsonSelectReportFields(&fields, 5,
                           cfk_occurcontext,
                           cfk_occurlocator,
                           cfk_occurtype,
                           cfk_occurrep,
                           cfk_occurtopic);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_OCCURRENCES, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    JsonElement *json_array_out = JsonArrayCreate(100);
    char description[CF_BUFSIZE] = {0};
    char refer[CF_BUFSIZE] = {0};
    int referred = false;
    Item *check = NULL;

    while (MongoCursorNext(cursor))   // loops over documents
    {
        JsonElement *json_obj = JsonObjectCreate(7);
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        topic_id = 0;

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (strcmp(bson_iterator_key(&it1), cfk_occurlocator) == 0)
            {
                snprintf(description,CF_BUFSIZE-1,"%s", bson_iterator_string(&it1));
            }
            else if (strcmp(bson_iterator_key(&it1), cfk_occurtopic) == 0)
            {
                snprintf(topic_name, CF_MAXVARSIZE, "%s", bson_iterator_string(&it1));
                topic_id = Nova_GetTopicIdForTopic(topic_name);
                referred = Nova_GetTopicIdForPromiseHandle(topic_id,refer,CF_BUFSIZE);
            }
        }

        char topic[CF_BUFSIZE],context[CF_BUFSIZE];
        DeClassifyTopic(topic_name, topic, context);

        if (referred != 0 && !IsItemIn(check,topic))
        {
            JsonObjectAppendString(json_obj, "description", description);
            JsonObjectAppendString(json_obj, "name", refer);
            JsonObjectAppendString(json_obj, "handle", topic);
            JsonObjectAppendInteger(json_obj, "pid", referred);

            // Now get the compliance / involvement summary for this

            char name[CF_MAXVARSIZE];
            snprintf(name, CF_MAXVARSIZE, "handles::%s", topic);
            Item *ip, *handles = Nova_GetHandlesForGoal(referred);
            int count_people = 0;
            double average_compliance = 0, total_compliance = 0, count = 0;
            double host_count = 0;
            Item *people = Nova_GetStakeHolders(referred);
            handles = SortItemListNames(handles);

            for (ip = handles; ip != NULL; ip = ip->next)
            {
                HubPromiseCompliance *hp;
                HubQuery *hq;
                Rlist *rp;
                time_t now = time(NULL);

                hq = CFDB_QueryWeightedPromiseCompliance(&conn, NULL, ip->name, 'x', 0, now,
                                                         NULL, NULL, PROMISE_CONTEXT_MODE_ALL, NULL,
                                                         QUERY_FLAG_DISABLE_ALL);
                average_compliance = 0;
                host_count = 0;

                // Average over all hosts

                for (rp = hq->records; rp != NULL; rp = rp->next)
                {
                    hp = (HubPromiseCompliance *) rp->item;
                    average_compliance += hp->e;
                    host_count++;
                }

                if (host_count)
                {
                    total_compliance += average_compliance / host_count;
                }

                count++;
            }

            DeleteItemList(handles);

            for (ip = people; ip != NULL; ip = ip->next)
            {
                if (strstr(ip->name, "@") || strcmp(ip->classes, "users") == 0)
                {
                    count_people++;
                }
            }

            DeleteItemList(people);

            if (count == 0)
            {
                count++;
            }

            JsonObjectAppendInteger(json_obj, "total_compliance", (int)(total_compliance / count + 0.5));
            JsonObjectAppendInteger(json_obj, "people", (int)count_people);
            JsonObjectAppendInteger(json_obj, "hosts", (int)host_count);
        }

        JsonArrayAppendObject(json_array_out, json_obj);
    }

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);

    return json_array_out;
}

/*************************************************************************/

JsonElement *Nova_GetServiceHistogram()
{
    EnterpriseDB dbconn;
    int srv = 0;
    struct servhist serv_array[CF_SERVICES_LIMIT] = {{{0}}};

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    GetPortFrequencies(&dbconn, "listening_tcp4_ports", serv_array, (int)cftcp4);
    GetPortFrequencies(&dbconn, "listening_tcp6_ports", serv_array, (int)cftcp6);
    GetPortFrequencies(&dbconn, "listening_udp4_ports", serv_array, (int)cfudp4);
    GetPortFrequencies(&dbconn, "listening_udp6_ports", serv_array, (int)cfudp6);    
    
    if (!CFDB_Close(&dbconn))
    {
        CfOut(cf_verbose, "", "!! Could not close connection to report database");
    }

    JsonElement *json_array_out = JsonArrayCreate(100);
    
    for (srv = 0; serv_array[srv].port != NULL && strlen(serv_array[srv].port) > 0; srv++)
       {
       JsonElement *json_obj = JsonObjectCreate(6);
       JsonObjectAppendString(json_obj, "port", serv_array[srv].port);
       JsonObjectAppendString(json_obj, "name", serv_array[srv].name);
       JsonObjectAppendInteger(json_obj, "tcp4", serv_array[srv].freq[cftcp4]);
       JsonObjectAppendInteger(json_obj, "tcp6", serv_array[srv].freq[cftcp6]);
       JsonObjectAppendInteger(json_obj, "udp4", serv_array[srv].freq[cfudp4]);
       JsonObjectAppendInteger(json_obj, "udp6", serv_array[srv].freq[cfudp6]);
       JsonArrayAppendObject(json_array_out, json_obj); 
       }

    return json_array_out;
}

/*************************************************************************/

static void GetPortFrequencies(EnterpriseDB *dbconn, char *variable, struct servhist serv_array[CF_SERVICES_LIMIT], const int type)
{
    HubVariable *hv;
    HubQuery *hq;
    Rlist *rp, *rp2;
    char *portname;
    int i;

    hq = CFDB_QueryVariables(dbconn, NULL, NULL, NULL, variable, NULL, NULL,
                             0, time(NULL), NULL, PROMISE_CONTEXT_MODE_ALL, NULL,
                             QUERY_FLAG_IS_REGEX);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
    hv = (HubVariable *) rp->item;

    if (strlen(hv->dtype) > 1)      // list
    {
        for (rp2 = hv->rval.item; rp2 != NULL; rp2 = rp2->next)
        {
            int found = false;

            if (atoi(rp2->item) == 0)
            {
                continue;
            }

            portname = "misc";

            for (int j = 0; j < ATTR; j++)
            {
                if (strcmp(rp2->item, ECGSOCKS[j].portnr) == 0)
                {
                    portname = ECGSOCKS[j].name;
                }
            }

          
            for (i = 0; (i < CF_SERVICES_LIMIT-1 && strlen(serv_array[i].port) > 0); i++)
            {
                if (strlen(serv_array[i].port) == 0)
                {
                    break;
                }

                char cmp[CF_SMALLBUF];
                snprintf(cmp,CF_SMALLBUF-1,"%s_in", portname);

                if (strcmp(cmp, serv_array[i].name) == 0)
                {
                    serv_array[i].freq[type]++;
                    
                    if (strcmp(serv_array[i].port, rp2->item) != 0)
                    {
                        Join(serv_array[i].port, ",", CF_BUFSIZE);
                        Join(serv_array[i].port, rp2->item, CF_BUFSIZE);
                    }
                    
                found = true;
                break;
                }
             }
          
             if (!found)
             {
                 serv_array[i].freq[type]++;
                 strncpy(serv_array[i].port, rp2->item, CF_BUFSIZE);
                 snprintf(serv_array[i].name,CF_SMALLBUF-1,"%s_in", portname);
             }
          }
       }
    }

    DeleteHubQuery(hq, DeleteHubVariable); 
}

/*************************************************************************/

JsonElement *Nova_GetServiceLevels(char *service)
{
    int high1 = 0, low1 = 0, high2 = 0, low2 = 0, high3 = 0, low3 = 0, normal = 0;

    GetClassHostFrequencies(service, &high1, &high2, &high3, &low1, &low2, &low3, &normal);

    JsonElement *json_obj = JsonObjectCreate(7);
    JsonObjectAppendInteger(json_obj, "low3", low3);
    JsonObjectAppendInteger(json_obj, "low2", low2);
    JsonObjectAppendInteger(json_obj, "low1", low1);
    JsonObjectAppendInteger(json_obj, "normal", normal);
    JsonObjectAppendInteger(json_obj, "high1", high1);
    JsonObjectAppendInteger(json_obj, "high2", high2);
    JsonObjectAppendInteger(json_obj, "high3", high3);       
    
    return json_obj;
}


/*************************************************************************/

static void GetClassHostFrequencies(char *srv, int *h1, int *h2, int *h3, int *l1, int *l2, int *l3, int *normal)
{
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;
    unsigned long bluehost_threshold;
    time_t now = time(NULL);

    CFDB_GetBlueHostThreshold(&bluehost_threshold);

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    // Classifiers of service level

    char classname[CF_MAXVARSIZE];
    const char *h1n = "_high_dev1";
    const char *h2n = "_high_dev2";
    const char *h3n = "_high_anomaly";
    const char *n = "_normal";
    const char *h = "_high";
    const char *l = "_low";
    const char *hn = "_low_normal";
    const char *ln = "_low_normal";
    const char *l1n = "_low_dev1";
    const char *l2n = "_low_dev2";
    const char *l3n = "_low_anomaly";
    int offset = strlen(srv);

    snprintf(classname, CF_MAXVARSIZE-1, "%s_.*", srv);

    hq = CFDB_QueryClasses(&dbconn, NULL, classname, now - (time_t)bluehost_threshold,
                           now, NULL, PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_IS_REGEX);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        HubClass *hc = (HubClass *) rp->item;
        
        if (strcmp(hc->class+offset, h1n) == 0)
        {
            (*h1)++;
        }
        else if (strcmp(hc->class+offset, h2n) == 0)
        {
            (*h2)++;
        }
        else if (strcmp(hc->class+offset, h3n) == 0)
        {
            (*h3)++;
        }
        else if (strcmp(hc->class+offset, n) == 0 ||
                 strcmp(hc->class+offset, h) == 0 ||
                 strcmp(hc->class+offset, hn) == 0 ||
                 strcmp(hc->class+offset, ln) == 0 ||
                 strcmp(hc->class+offset, l) == 0)
        {
            (*normal)++;
        }
        else if (strcmp(hc->class+offset, l1n) == 0)
        {
            (*l1)++;
        }
        else if (strcmp(hc->class+offset, l2n) == 0)
        {
            (*l2)++;
        }
        else if (strcmp(hc->class+offset, l3n) == 0)
        {
            (*l3)++;
        }

    }

    // Since dev1 is defined if dev2 etc, we have to correct the counts
    // One of these groups will do nothing

    *normal = *normal - *h1;
    *h1 = *h1 - *h2;
    *h2 = *h2 - *h3;

    *normal = *normal - *l1;
    *l1 = *l1 - *l2;
    *l2 = *l2 - *l3;

    
    CFDB_Close(&dbconn);
}

/*************************************************************************/

JsonElement *Nova_GetApplicationServices()
{
    bson_iterator it1;
    EnterpriseDB conn;
    Item *list = NULL;
    int topic_id;
    char *topic, *context;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

/* BEGIN query document */
    bson query;

    bson_init(&query);
    bson_append_regex(&query, cfk_topiccontext, "^services$", "");
    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;
    BsonSelectReportFields(&fields, 7,
                           cfk_topicname,
                           cfk_topicid,
                           cfk_topiccontext,
                           cfk_associations,
                           cfk_associd,
                           cfk_assoccontext,
                           cfk_assocname);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_TOPICS, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))   // loops over documents
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        topic_id = 0;
        topic = NULL;
        context = NULL;

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (strcmp(bson_iterator_key(&it1), cfk_topicname) == 0)
            {
                topic = (char *)bson_iterator_string(&it1);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_topiccontext) == 0)
            {
                context = (char *)bson_iterator_string(&it1);
            }
            
            if (strcmp(bson_iterator_key(&it1), cfk_topicid) == 0)
            {
                topic_id = bson_iterator_int(&it1);
            }
        }

        
        if (topic && topic_id && context)
        {
            PrependFullItem(&list, topic, context, topic_id, 0);
        }

    }

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn); 

    list = SortItemListNames(list);

    JsonElement *json_array_out = JsonArrayCreate(100);

    for (Item *ip = list; ip != NULL; ip=ip->next)
    {
        JsonElement *json_obj = JsonObjectCreate(3);
        char comment[CF_BUFSIZE];
       
        Nova_GetTopicComment(ip->name,"promisers",comment,sizeof(comment));
        JsonObjectAppendString(json_obj, "description", comment);
        JsonObjectAppendString(json_obj, "name", ip->name);
        JsonObjectAppendInteger(json_obj, "topic_id", ip->counter);

        // Now get the technical port services that provide this, i.e. "is provided by"

        Item *nn = Nova_NearestNeighbours(ip->counter, KM_MEASURES_CERT_B);
           
        if (nn)
        {
            JsonElement *json_array = JsonArrayCreate(50);
          
            for (Item *ips = nn; ips != NULL; ips = ips->next)
            {
                if (ips->counter == ip->counter)
                {
                    continue;
                }
             
                JsonElement *json_service = JsonObjectCreate(3);
                JsonObjectAppendString(json_service, "topic", ips->name);
                JsonObjectAppendString(json_service, "context", ips->classes);
                JsonObjectAppendInteger(json_service, "topic_id", ips->counter);
                JsonArrayAppendObject(json_array, json_service);
            }

            JsonObjectAppendArray(json_obj, "measured_by", json_array);
        }
              
        JsonArrayAppendObject(json_array_out, json_obj); 
    }
    
    return json_array_out;
}

/*************************************************************************/

JsonElement *Nova_GetMeasuredServices()
{
    bson_iterator it1;
    EnterpriseDB conn;
    Item *list = NULL;
    int topic_id;
    char *topic, *context;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

/* BEGIN query document */
    bson query;

    bson_init(&query);
    bson_append_regex(&query, cfk_topiccontext, "service_measurements", "");
    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;
    BsonSelectReportFields(&fields, 7,
                           cfk_topicname,
                           cfk_topicid,
                           cfk_topiccontext,
                           cfk_associations,
                           cfk_associd,
                           cfk_assoccontext,
                           cfk_assocname);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_TOPICS, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))   // loops over documents
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        topic_id = 0;
        topic = NULL;
        context = NULL;

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (strcmp(bson_iterator_key(&it1), cfk_topicname) == 0)
            {
                topic = (char *)bson_iterator_string(&it1);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_topiccontext) == 0)
            {
                context = (char *)bson_iterator_string(&it1);
            }
            
            if (strcmp(bson_iterator_key(&it1), cfk_topicid) == 0)
            {
                topic_id = bson_iterator_int(&it1);
            }
        }

        
        if (topic && topic_id && context)
        {
            PrependFullItem(&list, topic, context, topic_id, 0);
        }

    }

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn); 

    list = SortItemListNames(list);

    JsonElement *json_array_out = JsonArrayCreate(100);

    for (Item *ip = list; ip != NULL; ip=ip->next)
    {
        JsonElement *json_obj = JsonObjectCreate(3);
        char comment[CF_BUFSIZE];
       
        Nova_GetTopicComment(ip->name,"promisers",comment,sizeof(comment));
        JsonObjectAppendString(json_obj, "description", comment);
        JsonObjectAppendString(json_obj, "name", ip->name);
        JsonObjectAppendInteger(json_obj, "topic_id", ip->counter);

        // Now get the technical port services that provide this, i.e. "is provided by"

        Item *nn = Nova_NearestNeighbours(ip->counter, KM_MEASURES_CERT_F);
           
        if (nn)
        {
            JsonElement *json_array = JsonArrayCreate(50);
          
            for (Item *ips = nn; ips != NULL; ips = ips->next)
            {
                if (ips->counter == ip->counter)
                {
                    continue;
                }
             
                JsonElement *json_service = JsonObjectCreate(3);
                JsonObjectAppendString(json_service, "topic", ips->name);
                JsonObjectAppendString(json_service, "context", ips->classes);
                JsonObjectAppendInteger(json_service, "topic_id", ips->counter);
                JsonArrayAppendObject(json_array, json_service);
            }

            JsonObjectAppendArray(json_obj, "measures", json_array);
        }
              
        JsonArrayAppendObject(json_array_out, json_obj); 
    }
    
    return json_array_out;
}

/*************************************************************************/

Item *Nova_GetHandlesForGoal(int topic_id)

{
    return Nova_NearestNeighbours(topic_id,NOVA_GOAL_INV);
}

/*************************************************************************/

JsonElement *Nova_AddOccurrenceBuffer(char *context, char *locator, enum representations locator_type, char *represents)
{
    JsonElement *json_obj = JsonObjectCreate(4);
    JsonObjectAppendString(json_obj, "context", context);
    JsonObjectAppendString(json_obj, "ref", locator);
    JsonObjectAppendString(json_obj, "represents", represents);
    JsonObjectAppendInteger(json_obj, "type", locator_type);

    return json_obj;
}

/*************************************************************************/

char *Nova_URL(char *s, char *rep)
{
    static char buf[CF_MAXVARSIZE];

    snprintf(buf, CF_MAXVARSIZE - 1, "<a href=\"%s\">%s</a>: ", s, rep);
    return buf;
}

/*************************************************************************/

void Nova_FillInGoalComment(Item *ip)
{
    char searchstring[CF_MAXVARSIZE];
    bson_iterator it1;
    EnterpriseDB conn;
    char canonified_goal[CF_BUFSIZE] = { 0 };

// Get comment goals.* or targets.%s etc

    searchstring[0] = '\0';
    ReplaceChar(ip->name, canonified_goal, CF_BUFSIZE, ' ', '_');

    snprintf(searchstring, CF_MAXVARSIZE - 1, "%s|%s", ip->name, canonified_goal);

    if (!CFDB_Open(&conn))
    {
        return;
    }

    bson query;

    bson_init(&query);
    bson_append_regex(&query, cfk_occurcontext, searchstring, "");
    bson_append_regex(&query, cfk_occurtopic, searchstring, "");
    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;
    BsonSelectReportFields(&fields, 5,
                           cfk_occurcontext,
                           cfk_occurlocator,
                           cfk_occurtype,
                           cfk_occurtopic,
                           cfk_occurrep);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_OCCURRENCES, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))   // loops over documents
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            /* Query specific search/marshalling */

            if (strcmp(bson_iterator_key(&it1), cfk_occurlocator) == 0)
            {
                ip->classes = xstrdup(bson_iterator_string(&it1));
                mongo_cursor_destroy(cursor);
                return;
            }
        }
    }

    mongo_cursor_destroy(cursor);
    ip->classes = xstrdup("No description found");
}

/*************************************************************************/

const char *Nova_GetBundleComment(char *bundle)
{
    static char buf[CF_BUFSIZE];
    bson_iterator it1;
    EnterpriseDB conn;

// Get comment goals.* or targets.%s etc

    if (!CFDB_Open(&conn))
    {
        return NULL;
    }

    bson query;

    bson_init(&query);
    bson_append_string(&query, cfk_occurrep, "description");
    bson_append_string(&query, cfk_occurcontext, bundle);
    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;
    BsonSelectReportFields(&fields, 1, cfk_occurlocator);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_OCCURRENCES, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    CFDB_Close(&conn);

    while (MongoCursorNext(cursor))   // loops over documents
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            /* Query specific search/marshalling */

            if (strcmp(bson_iterator_key(&it1), cfk_occurlocator) == 0)
            {
                strncpy(buf, bson_iterator_string(&it1), CF_BUFSIZE - 1);
                mongo_cursor_destroy(cursor);
                return buf;
            }
        }
    }
    mongo_cursor_destroy(cursor);
    return "";
}

/*********************************************************************/

char *Nova_StripString(char *source, char *substring)
{
    char *replace = xmalloc(strlen(source) + 1);
    Rlist *rp, *new = SplitStringAsRList(source, '.');

    replace[0] = '\0';

    for (rp = new; rp != NULL; rp = rp->next)
    {
        if (strcmp(rp->item, substring) == 0)
        {
            continue;
        }

        strcat(replace, rp->item);

        if (rp->next)
        {
            strcat(replace, ".");
        }
    }

    DeleteRlist(new);

    return replace;
}

/*****************************************************************************/
/* Plot cosmos                                                               */
/*****************************************************************************/

JsonElement *Nova_PlotTopicCosmos(int topic, char *view, char *user)
    
/* This assumes that we have the whole graph in a matrix */
    
{
    GraphNode tribe_nodes[CF_TRIBE_SIZE] = { { 0 } };
    int tribe_id[CF_TRIBE_SIZE] = { 0 },
        tribe_size = 0;
    double tribe_evc[CF_TRIBE_SIZE] = { 0 };
    double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE] = { { 0 } };

// RBAC - not so important here as the actual content will be blocked later
    
/* Count the  number of nodes in the solar system, to max number based on Dunbar's limit */

    if ((tribe_size = Nova_GetTribe(tribe_id, tribe_nodes, tribe_adj, topic, view)))
    {
        Nova_EigenvectorCentrality(tribe_adj, tribe_evc, CF_TRIBE_SIZE);
        return Nova_DrawTribe(tribe_id, tribe_nodes, tribe_adj, tribe_size, tribe_evc, topic);
    }
    else
    {
        JsonElement *json_out = JsonArrayCreate(0);
        return json_out;
    }
}

/*************************************************************************/
/* Local patch computation                                               */
/*************************************************************************/

int Nova_GetTribe(int *tribe_id, GraphNode *tribe_nodes, double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE], int pid,
                  char *view_pattern)
/* This function generates a breadth-first connected sub-graph of the full graph
   and identifies the orbits and distances, up to a maximum of Dunbar's tribe-size */
{
 char topic_name[CF_BUFSIZE], topic_context[CF_BUFSIZE], topic_id[CF_BUFSIZE];
    char *a_name, *a_context, view[CF_MAXVARSIZE];
    int a_pid;
    GraphNode neighbours1[CF_TRIBE_SIZE] = { { 0 } },
              neighbours2[CF_TRIBE_SIZE][CF_TRIBE_SIZE] = { { { 0 } } };
    int tribe_counter = 0, secondary_boundary, tertiary_boundary, i, j;
    Item *ip, *nn = NULL;

    for (i = 0; i < CF_TRIBE_SIZE; i++)
    {
        Nova_InitVertex(tribe_nodes, i);
        tribe_id[i] = -1;

        for (j = 0; j < CF_TRIBE_SIZE; j++)
        {
            tribe_adj[i][j] = 0;
        }
    }

    if (view_pattern && strcmp(view_pattern, "influence") == 0)
    {
        snprintf(view, CF_MAXVARSIZE, "%s|%s|%s|%s|%s|%s|%s", NOVA_GIVES, NOVA_USES, NOVA_IMPACTS, NOVA_ISIMPACTED,
                 NOVA_BUNDLE_DATA, NOVA_BUNDLE_DATA_INV_B, NOVA_BUNDLE_DATA_INV_P);
    }
    else
    {
        snprintf(view, CF_MAXVARSIZE, ".*");
    }

    tribe_id[0] = pid;
    topic_name[0] = '\0';
    topic_context[0] = '\0';
    
    if (!Nova_NewVertex(tribe_nodes, 0, 0, pid, topic_name, topic_context))
    {
        return false;
    }

/* Probe sub-graph */

    tribe_counter = 1;

// Nearest neighbours

    nn = Nova_NearestNeighbours(pid, view);

    // If there are no neighbours in the current context, look for other contexts    

    if (nn == NULL)
    {
        Item *backup, *ip;
        Nova_GetTopicByTopicId(pid, topic_name, topic_id, topic_context, NULL);
        backup = Nova_SearchTopicMap(topic_name,CF_SEARCH_EXACT,false);

        for (ip = backup; ip != NULL; ip=ip->next)
        {
            PrependFullItem(&nn, ip->name, ip->classes, ip->counter, 0);
        }
        
        DeleteItemList(backup);
    }
    
    for (ip = nn; ip != NULL; ip = ip->next)
    {
        a_name = ip->name;
        a_context = ip->classes;
        a_pid = ip->counter;

        if (Nova_AlreadyInTribe(a_pid, tribe_id))
        {
            continue;
        }

        if (Nova_NewVertex(tribe_nodes, tribe_counter, 1, a_pid, a_name, a_context))
        {
            neighbours1[tribe_counter].real_id = a_pid;
            tribe_id[tribe_counter] = a_pid;
            tribe_adj[tribe_counter][0] = 1;
            tribe_adj[0][tribe_counter] = 1;
            tribe_counter++;
        }

        CfDebug("NEAREST NEIGHOUR (%d): %s::%s at %d\n", tribe_counter, a_context, a_name, a_pid);

        if (tribe_counter >= CF_TRIBE_SIZE - 1)
        {
            break;
        }
    }

    DeleteItemList(nn);

    secondary_boundary = tribe_counter;

    if (tribe_counter < CF_TRIBE_SIZE - 1 && secondary_boundary > 0)
    {
        for (i = 0; i < secondary_boundary; i++)
        {
            nn = Nova_NearestNeighbours(neighbours1[i].real_id, view);

            for (ip = nn; ip != NULL; ip = ip->next)
            {
                if (tribe_counter >= CF_TRIBE_SIZE - 1)
                {
                    break;
                }

                a_name = ip->name;
                a_context = ip->classes;
                a_pid = ip->counter;

                if (Nova_AlreadyInTribe(a_pid, tribe_id))
                {
                    continue;
                }

                CfDebug("  2nd NEIGHOUR (%d): %s::%s at %d\n", tribe_counter, a_context, a_name, a_pid);

                if (Nova_NewVertex(tribe_nodes, tribe_counter, 2, a_pid, a_name, a_context))
                {
                    neighbours2[i][tribe_counter].real_id = a_pid;
                    tribe_id[tribe_counter] = a_pid;
                    tribe_adj[tribe_counter][i] = 1;
                    tribe_adj[i][tribe_counter] = 1;
                    tribe_counter++;
                }

                if (tribe_counter >= CF_TRIBE_SIZE - 1)
                {
                    break;
                }
            }

            DeleteItemList(nn);

            if (tribe_counter >= CF_TRIBE_SIZE - 1)
            {
                break;
            }
        }
    }

    tertiary_boundary = tribe_counter;

    if (tribe_counter < CF_TRIBE_SIZE - 1 && tertiary_boundary > 0)
    {
        for (i = 0; i < secondary_boundary; i++)
        {
            for (j = secondary_boundary; j < tertiary_boundary; j++)
            {
                if (tribe_counter >= CF_TRIBE_SIZE - 1)
                {
                    break;
                }

                nn = Nova_NearestNeighbours(neighbours2[i][j].real_id, view);

                for (ip = nn; ip != NULL; ip = ip->next)
                {
                    a_name = ip->name;
                    a_context = ip->classes;
                    a_pid = ip->counter;

                    if (a_pid == neighbours1[i].real_id)
                    {
                        continue;
                    }

                    if (Nova_AlreadyInTribe(a_pid, tribe_id))
                    {
                        continue;
                    }

                    CfDebug("     3rd NEIGHBOUR (%d): %s::%s at %d\n", tribe_counter, a_context, a_name, a_pid);

                    if (Nova_NewVertex(tribe_nodes, tribe_counter, 3, a_pid, a_name, a_context))
                    {
                        tribe_id[tribe_counter] = a_pid;
                        tribe_adj[tribe_counter][j] = 1;
                        tribe_adj[j][tribe_counter] = 1;
                        tribe_counter++;
                    }

                    if (tribe_counter >= CF_TRIBE_SIZE - 1)
                    {
                        break;
                    }
                }

                DeleteItemList(nn);

                if (tribe_counter >= CF_TRIBE_SIZE - 1)
                {
                    break;
                }
            }

            if (tribe_counter >= CF_TRIBE_SIZE - 1)
            {
                break;
            }
        }
    }

    return tribe_counter;
}

/*************************************************************************/
/* Level                                                                 */
/*************************************************************************/

void Nova_EigenvectorCentrality(double A[CF_TRIBE_SIZE][CF_TRIBE_SIZE], double *v, int dim)
{
    int i, n;
    double max = 0.0001;

    for (i = 0; i < dim; i++)
    {
        v[i] = 1.0;
    }

    for (n = 0; n < 10; n++)
    {
        Nova_MatrixOperation(A, v, dim);
    }

    for (i = 0; i < dim; i++)
    {
        if (v[i] > max)
        {
            max = v[i];
        }
    }

// Renormalize

    if (max > 0.0001)
    {
        for (i = 0; i < dim; i++)
        {
            v[i] = v[i] / max;
        }
    }
}

/*************************************************************************/

void Nova_MatrixOperation(double A[CF_TRIBE_SIZE][CF_TRIBE_SIZE], double *v, int dim)
{
    int i, j;
    double max = 0.000000001;
    double vp[CF_TRIBE_SIZE] = { 0 };

    for (i = 0; i < dim; i++)
    {
        for (j = 0; j < dim; j++)
        {
            vp[i] += A[i][j] * v[j];
        }

        if (vp[i] > max)
        {
            max = vp[i];
        }
    }

    if (max > 0.0001)
    {
        for (i = 0; i < dim; i++)
        {
            v[i] = vp[i] / max;

            if (isnan(v[i]))
            {
                v[i] = 0;
            }
        }
    }
}

/*************************************************************************/

int Nova_AlreadyInTribe(int node, int *tribe_id)
{
    int i;

    for (i = 0; tribe_id[i] > 0; i++)
    {
        if (tribe_id[i] == node)
        {
            return true;
        }
    }

    return false;
}

/*************************************************************************/

void Nova_InitVertex(GraphNode *tribe, int i)
{
    tribe[i].real_id = 0;
    tribe[i].shortname = NULL;
    tribe[i].fullname = NULL;
    tribe[i].context = NULL;
    tribe[i].distance_from_centre = 0;
}

/*************************************************************************/

static int Nova_NewVertex(GraphNode *tribe, int node, int distance, int real, char *topic_name, char *topic_context)
{
    char sshort[CF_BUFSIZE] = { 0 };
    char topic_id[CF_BUFSIZE] = { 0 };
    int j = 0;

    CfDebug("NEWVERT(%d,%d,%s:%s)\n", distance, real, topic_context, topic_name);

/* If more than a few nodes, don't waste visual space on repeated topics */

    if (strlen(topic_name) == 0)
    {
    Nova_GetTopicByTopicId(real, topic_name, topic_id, topic_context, NULL);
    }

    sscanf(topic_name, "%32[^\n]", sshort);

    if (strlen(sshort) == 0)
    {
        return false;
    }

    if (node > 5)
    {
        for (j = 0; j < node; j++)
        {
            if (strcmp(tribe[j].shortname, sshort) == 0)
            {
                CfDebug("A similar topic already exists, so don't waste the space: %s\n", sshort);
                return false;
            }
        }
    }

    tribe[node].real_id = real;
    tribe[node].shortname = xstrdup(sshort);
    tribe[node].fullname = xstrdup(topic_name);
    tribe[node].context = xstrdup(topic_context);
    tribe[node].distance_from_centre = distance;
    return true;
}

/*********************************************************************/

Item *Nova_NearestNeighbours(int search_id, char *assoc_mask)

/* This function is like ScanLeads, except different output */

{
    bson_iterator it1, it2, it3;
    EnterpriseDB conn;
    Item *list = NULL;
    char assoc_name[CF_BUFSIZE];
    char afwd[CF_BUFSIZE], abwd[CF_BUFSIZE];
    char assoc_context[CF_BUFSIZE];
    int assoc_id;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

/* BEGIN query document */
    bson query;

    bson_init(&query);
    bson_append_int(&query, cfk_topicid, search_id);
    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;
    BsonSelectReportFields(&fields, 7,
                           cfk_topicname,
                           cfk_topicid,
                           cfk_topiccontext,
                           cfk_associations,
                           cfk_associd,
                           cfk_assoccontext,
                           cfk_assocname);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_TOPICS, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))   // loops over documents
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (strcmp(bson_iterator_key(&it1), cfk_associations) == 0)
            {
                bson_iterator_subiterator(&it1, &it2);

                while (bson_iterator_next(&it2))
                {
                    bson_iterator_subiterator(&it2, &it3);

                    assoc_id = 0;
                    assoc_name[0] = '\0';
                    assoc_context[0] = '\0';
                    afwd[0] = '\0';
                    abwd[0] = '\0';

                    while (bson_iterator_next(&it3))
                    {
                        if (strcmp(bson_iterator_key(&it3), cfk_associd) == 0)
                        {
                            assoc_id = bson_iterator_int(&it3);
                        }

                        if (strcmp(bson_iterator_key(&it3), cfk_assocname) == 0)
                        {
                            strncpy(assoc_name, bson_iterator_string(&it3), CF_BUFSIZE - 1);
                        }

                        if (strcmp(bson_iterator_key(&it3), cfk_fwd) == 0)
                        {
                            strncpy(afwd, bson_iterator_string(&it3), CF_BUFSIZE - 1);
                        }

                        if (strcmp(bson_iterator_key(&it3), cfk_bwd) == 0)
                        {
                            strncpy(abwd, bson_iterator_string(&it3), CF_BUFSIZE - 1);
                        }

                        if (strcmp(bson_iterator_key(&it3), cfk_assoccontext) == 0)
                        {
                            strncpy(assoc_context, bson_iterator_string(&it3), CF_BUFSIZE - 1);
                        }
                    }

                    CfDebug(" - NEIGH topic %d has association %s %s::%s (%d)\n", assoc_id, afwd, assoc_context,
                            assoc_name, assoc_id);

                    if (assoc_mask == NULL || StringMatch(assoc_mask, afwd))
                    {
                        PrependFullItem(&list, assoc_name, assoc_context, assoc_id, 0);
                    }
                }
            }
        }
    }

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);
    return list;
}

/*********************************************************************/

Item *Nova_GetTopicsInContext(char *context)
{
    bson_iterator it1;
    EnterpriseDB conn;
    Item *list = NULL;
    char topic_name[CF_BUFSIZE];
    char topic_context[CF_BUFSIZE];
    int topic_id;

    if (!CFDB_Open(&conn))
    {
        return false;
    }

/* BEGIN query document */
    bson query;

    bson_init(&query);
    bson_append_string(&query, cfk_topiccontext, context);
    BsonFinish(&query);

/* BEGIN RESULT DOCUMENT */
    bson fields;
    BsonSelectReportFields(&fields, 3, cfk_topicname, cfk_topicid, cfk_topiccontext);

/* BEGIN SEARCH */

    mongo_cursor *cursor = MongoFind(&conn, MONGO_KM_TOPICS, &query, &fields, 0, 0, CF_MONGO_SLAVE_OK);

    bson_destroy(&query);
    bson_destroy(&fields);

    while (MongoCursorNext(cursor))   // loops over documents
    {
        bson_iterator_init(&it1, mongo_cursor_bson(cursor));

        topic_name[0] = '\0';
        topic_context[0] = '\0';
        topic_id = 0;

        while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
        {
            if (strcmp(bson_iterator_key(&it1), cfk_topicname) == 0)
            {
                strncpy(topic_name, bson_iterator_string(&it1), CF_BUFSIZE - 1);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_topiccontext) == 0)
            {
                strncpy(topic_context, bson_iterator_string(&it1), CF_BUFSIZE - 1);
            }

            if (strcmp(bson_iterator_key(&it1), cfk_topicid) == 0)
            {
                topic_id = (int) bson_iterator_int(&it1);
            }
        }

        PrependFullItem(&list, topic_name, topic_context, topic_id, 0);
    }

    mongo_cursor_destroy(cursor);
    CFDB_Close(&conn);
    return list;
}

/*****************************************************************************/

static int MergeExistingContexts(Item **list, char *name, char *context, int merge)
{
    Item *ip;

    if (!merge)
    {
        return false;
    }
    
    if ((ip = ReturnItemIn(*list,name)))
    {
        char *replace = xmalloc(strlen(ip->classes)+strlen(", .")+strlen(context));
        strcpy(replace,ip->classes);
        strcat(replace,", ");
        strcat(replace,context);
        free(ip->classes);
        ip->classes = replace;

        return true;
    }

    return false;
}

/*****************************************************************************/
/* The Hit Lists                                                             */
/*****************************************************************************/

static void NewHit(Hit **list,char *context, char *locator, enum representations locator_type, char *represents)
{
    Hit *hp = NULL;

    if ((hp = HitExists(*list, locator, locator_type, context)) == NULL)
    {
        hp = xcalloc(1, sizeof(Hit));

        hp->occurrence_context = xstrdup(ToLowerStr(context));
        hp->locator = xstrdup(locator);
        hp->represents = xstrdup(represents);
        hp->rep_type = locator_type;
        hp->next = *list;
        *list = hp;
    }

}

/*****************************************************************************/

static void DeleteHitList(Hit *hp)

{
    if (hp == NULL)
    {
        return;
    }
 
    if (hp->next)
    {
        DeleteHitList(hp->next);
    }

    free(hp->occurrence_context);
    free(hp->locator);
    free(hp->represents);
    free(hp);
}

/*****************************************************************************/

static Hit *HitExists(Hit *list, char *locator, enum representations rep_type, char *context)
{
    Hit *hp;

    for (hp = list; hp != NULL; hp = hp->next)
    {
        if (strcmp(locator, hp->locator) == 0)
        {
        if (strstr(hp->occurrence_context,context) == NULL)
           {
           // This context is unknown, but same reference
           char *replace = xmalloc(strlen(hp->occurrence_context)+strlen(", .")+strlen(context));

           strcpy(replace,hp->occurrence_context);
           strcat(replace,", ");
           strcat(replace,context);
           free(hp->occurrence_context);
           hp->occurrence_context = replace;
           }
        
            return hp;
        }
    }

    return NULL;
}

