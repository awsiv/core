/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_ENTERPRISE_INSTALL_H
#define CFENGINE_ENTERPRISE_INSTALL_H

typedef struct
{
    char *title;
    JsonElement *header;
    JsonElement *rows;
} JsonHeaderTable;

typedef struct
{
    char *username;
    char *email;
    char *sql_query;
    time_t started_on;
    time_t completed_on;
    int total_secs;
    Writer *writer;
} HubScheduledReport;

JsonHeaderTable *NewJsonHeaderTable(const char *title, JsonElement *header, JsonElement *rows);
void DeleteJsonHeaderTable(JsonHeaderTable *table);

HubScheduledReport *NewScheduledReports(const char *user, const char *email, const char *sql, Writer *w);
void DeleteScheduledReport(HubScheduledReport *sr);

#endif  /* CFENGINE_ENTERPRISE_INSTALL_H */
