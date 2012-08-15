/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#ifndef CFENGINE_ENTERPRISE_INSTALL_H
#define CFENGINE_ENTERPRISE_INSTALL_H

typedef struct
{
    JsonElement *header;
    JsonElement *data;
} JsonHeaderTable;


JsonHeaderTable *NewJsonHeaderTable(JsonElement *header, JsonElement *data);
void DeleteJsonHeaderTable(JsonHeaderTable *table);

#endif  /* CFENGINE_ENTERPRISE_INSTALL_H */
