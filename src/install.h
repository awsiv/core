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

JsonHeaderTable *NewJsonHeaderTable(const char *title, JsonElement *header, JsonElement *rows);
void DeleteJsonHeaderTable(JsonHeaderTable *table);

#endif  /* CFENGINE_ENTERPRISE_INSTALL_H */
