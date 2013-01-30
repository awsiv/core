#ifndef CFENGINE_ONTOLOGY_H
#define CFENGINE_ONTOLOGY_H

void DeClassifyTopic(char *typdetopic, char *topic, char *type);
void Nova_MapPromiseToTopic(const ReportContext *report_context, const Promise *pp, const char *version);
char *Name2Id(char *s);

#endif
