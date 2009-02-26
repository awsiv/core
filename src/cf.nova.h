/*****************************************************************************/
/*                                                                           */
/* File: cf.nova.h                                                           */
/*                                                                           */
/*****************************************************************************/

#define NOVA 1

/* knowledge.c */

void Nova_MapPromiseToTopic(FILE *fp,struct Promise *pp,char *version);
void Nova_ShowTopicRepresentation(FILE *fp);
void Nova_BundleReference(FILE *fp,char *bundle);
void Nova_TypeNode(FILE *fp,char *type);
void Nova_PromiseNode(FILE *fp,struct Promise *pp,int calltype);
void Nova_TypeNode(FILE *fp,char *type);
void Nova_BundleNode(FILE *fp,char *bundle);
void Nova_BodyNode(FILE *fp,char *body,int calltype);
char *Nova_PromiseID(struct Promise *pp);
void Nova_DependencyGraph(struct Topic *map);
void Nova_PlotTopicDependencies(int topic,double **adj,char **names,int dim);
void Nova_MapClassAssociations(FILE *fp, struct Promise *pp,char *promise_id);

/* monitoring.c */

void Nova_HistoryUpdate(char *key,struct Averages newvals);
void Nova_UpdateShiftAverage(struct Averages *shift_value,struct Averages *newvals);
void Nova_ResetShiftAverage(struct Averages *shift_value);
double ShiftAverage(double new,double old);

/* reporting.c */

void Nova_SummarizeCompliance(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeSetuid(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeFileChanges(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);

#define NOVA_USES "relies on promise from"
#define NOVA_GIVES "makes promise to"
#define NOVA_IMPACTS "has direct influence on"
#define NOVA_ISIMPACTED "is directly influenced by"
#define NOVA_HISTORYDB "history.db"
