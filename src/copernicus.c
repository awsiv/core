/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: copernicus.c                                                        */
/*                                                                           */
/* Created: Wed Sep  2 04:41:11 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include <assert.h>

JsonElement *Nova_DrawTribe(int *tribe_id, GraphNode *tribe_node, double tribe_adj[CF_TRIBE_SIZE][CF_TRIBE_SIZE],
                            int tribe_size, double *tribe_evc, int topic)
/* FORMAT
    [{
    "id": "graphnode0",
    "name": "Host Portal",
    "adjacencies": [ {"nodeTo": "graphnode1"}, {"nodeFrom": "graphnode3"}  ],
    "data": {
        '$color':'#FFCC33',
        '$dim': 30,
        "$type": "gradientCircle"}
    },{}]
 */
{
    int i, j;
    double radius;
    char *colour;
    JsonElement *json_arr_out = JsonArrayCreate(10);

    for (i = 0; i < tribe_size; i++)
    {
        if (i == topic)
        {
            // The topic might not be central, so inflate its importance a little
            radius = 1.5 * CF_MIN_RADIUS + CF_RADIUS_SCALE * tribe_evc[i];
            colour = "#ffcc33";
        }
        else
        {
            radius = CF_MIN_RADIUS + CF_RADIUS_SCALE * tribe_evc[i];
            switch (tribe_node[i].distance_from_centre)
            {
            case 0:
                colour = "#ffcc33";
                break;
            case 1:
                colour = "#d2bfa5";
                break;
            case 2:
                colour = "#CCB99F";
                break;
            default:
                colour = "#787878";
                break;
            }
        }

        Writer *url = StringWriter();
        WriterWriteF(url, "/knowledge/knowledgemap/pid/%d", tribe_node[i].real_id);
        Writer *id = StringWriter();
        WriterWriteF(id, "g%d", i);
        Writer *rad = StringWriter();
        WriterWriteF(rad, "%.1lf", radius);

        JsonElement *json_obj_data = JsonObjectCreate(6);
        JsonObjectAppendString(json_obj_data, "$color", colour);
        JsonObjectAppendString(json_obj_data, "$dim", StringWriterData(rad));
        JsonObjectAppendString(json_obj_data, "$type", "gradientCircle");
        JsonObjectAppendString(json_obj_data, "context", tribe_node[i].context);
        JsonObjectAppendString(json_obj_data, "fullname", tribe_node[i].fullname);
        JsonObjectAppendString(json_obj_data, "link", StringWriterData(url));

        WriterClose(rad);
        WriterClose(url);

        JsonElement *json_obj = JsonObjectCreate(5);
        JsonObjectAppendString(json_obj, "id", StringWriterData(id));
        JsonObjectAppendString(json_obj, "name", tribe_node[i].shortname);
        JsonObjectAppendObject(json_obj, "data", json_obj_data);

        WriterClose(id);

        JsonElement *json_array_adjacencies = JsonArrayCreate(10);
        for (j = 0; j < tribe_size; j++)
        {
            if (tribe_adj[i][j])
            {
                Writer *tmp = StringWriter();
                WriterWriteF(tmp, "g%d", j);

                JsonElement *json_obj_nodeto = JsonObjectCreate(10);
                JsonObjectAppendString(json_obj_nodeto, "nodeTo", StringWriterData(tmp));
                JsonArrayAppendObject(json_array_adjacencies, json_obj_nodeto);

                WriterClose(tmp);
            }
        }
        JsonObjectAppendArray(json_obj, "adjacencies", json_array_adjacencies);

        JsonArrayAppendObject(json_arr_out, json_obj);
    }

    // Cleanup
    for (i = 0; i < tribe_size; i++)
    {
        if (tribe_node[i].fullname)
        {
            free(tribe_node[i].fullname);
        }
        if (tribe_node[i].shortname)
        {
            free(tribe_node[i].shortname);
        }
        if (tribe_node[i].context)
        {
            free(tribe_node[i].context);
        }
    }

    return json_arr_out;
}
