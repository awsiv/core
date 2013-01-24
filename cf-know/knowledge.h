#ifndef KNOWLEDGE_H
#define KNOWLEDGE_H

void Nova_StoreKMDB(Topic **topichash, Occurrence *occurrences, Inference *inferences);
void Nova_BundleReference(FILE *fp, char *bundle);
void Nova_TypeNode(FILE *fp, char *type);
void Nova_PromiseNode(FILE *fp, Promise *pp, int calltype);
void Nova_TypeNode(FILE *fp, char *type);
void Nova_BundleNode(FILE *fp, char *bundle);
void Nova_BodyNode(FILE *fp, char *body, int calltype);
void Nova_DependencyGraph(Topic *map);
void Nova_PlotTopicDependencies(int topic, double **adj, char **names, int dim);
int GetTopicPid(char *classified_topic);

#endif // KNOWLEDGE_H
