#ifndef CFENGINE_NOVA_TOPICMAP_H
#define CFENGINE_NOVA_TOPICMAP_H

#include "cf3.defs.h"

/*****************************************************************************/
#define CF_TRIBE_SIZE 25
#define RBAC_ERROR_MSG "This result is not available to you with your current credentials"

typedef struct
{
    int real_id;
    int tribe_id;
    char *shortname;
    char *fullname;
    char *context;
    int distance_from_centre;
} GraphNode;

/*****************************************************************************/
void Nova_DumpTopics(void);
void Nova_FillInGoalComment(Item *ip);
const char *Nova_GetBundleComment(char *bundle);
void Nova_ShowTopic(char *qualified_topic);
void Nova_LookupUniqueAssoc(int pid, char *buffer, int bufsize);
JsonElement *Nova_ScanTheRest(int pid);
Item *Nova_SearchTopicMap(char *typed_topic,int type,int merge);
void Nova_SearchPromises(Item **l, char *search, int type);
void Nova_SearchHosts(Item **l, char *search, int type, char *user);
void Nova_SearchClasses(Item **l, char *search, int type, char *user);
void Nova_SearchVariables(Item **l, char *search, int type, char *user);
void Nova_SearchReports(Item **l, char *search);
Item *Nova_ScanLeadsAssociations(int pid, char *view);
JsonElement *Nova_ScanOccurrences(int this_id, char *username);
int Nova_GetTopicComment(char *topic_name, char *topic_context, char *buffer, int bufsize);

int Nova_GetTopicByTopicId(int pid, char *topic_name, char *topic_id, char *topic_type, char *buffer);
int Nova_AddTopicSearchBuffer(int pid, char *topic_name, char *topic_type, char *buffer, int bufsize);
int Nova_AddAssocSearchBuffer(char *from_assoc, char *to_assoc, char *buffer, int bufsize);
JsonElement *Nova_AddOccurrenceBuffer(char *context, char *locator, enum representations locator_type, char *represents);
void AtomizeTopicContext(AlphaList *context_list, char *topic_context);
char *Nova_TopicIdURL(int pid, char *s);
char *Nova_AssocURL(char *s);
char *Nova_URL(char *s, char *rep);
Item *Nova_NearestNeighbours(int topic_id, char *assoc_mask);
Item *Nova_GetTopicsInContext(char *context);
Item *Nova_GetBusinessGoals(char *handle);
int Nova_GetUniqueBusinessGoals(char *buffer, int bufsize);
JsonElement *Nova_summarize_all_goals(char *username);
JsonElement *Nova_GetApplicationServices(void);
JsonElement *Nova_GetMeasuredServices(void);
JsonElement *Nova_GetServiceHistogram(void);
JsonElement *Nova_GetServiceLevels(char *service);
Item *Nova_GetHandlesForGoal(int referred);
Item *Nova_GetStakeHolders(int topic_id);
JsonElement *Nova_PlotTopicCosmos(int topic, char *view, char *username);
int Nova_GetTribe(int *tribe_id, GraphNode *tribe_nodes, double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE], int pid,
                  char *v);
void Nova_EigenvectorCentrality(double A[CF_TRIBE_SIZE][CF_TRIBE_SIZE], double *v, int dim);
void Nova_MatrixOperation(double A[CF_TRIBE_SIZE][CF_TRIBE_SIZE], double *v, int dim);
int Nova_AlreadyInTribe(int node, int *tribe_id);
void Nova_InitVertex(GraphNode *tribe, int i);
char *Nova_StripString(char *source, char *substring);

#endif
