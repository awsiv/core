/*
 * This file is (C) Cfengine AS. See COSL LICENSE for details.
 */

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*
 * Cumulative statitsics support and conversion to instant values
 */

typedef struct PrevUint32Value
   {
   char *name;
   char *subname;
   unsigned value;
   time_t timestamp;
   struct PrevUint32Value *next;
   } PrevUint32Value;

/* Globals */

static PrevUint32Value *values;

/* Implementation */

unsigned GetInstantUint32Value(const char *name, const char *subname, unsigned value, time_t timestamp)
{
PrevUint32Value *v;

for (v = values; v; v = v->next)
   {
   if (!strcmp(v->name, name) && !strcmp(v->subname, subname))
      {
      unsigned diff;
      unsigned difft;

      /* Check for wraparound */
      if (value < v->value)
         {
         diff = INT_MAX - v->value + value;
         }
      else
         {
         diff = value - v->value;
         }
      difft = timestamp - v->timestamp;

      v->value = value;
      v->timestamp = timestamp;

      return diff / difft;
      }
   }

v = malloc(sizeof(PrevUint32Value));
v->name = strdup(name);
v->subname = strdup(subname);
v->value = value;
v->timestamp = timestamp;
v->next = values;
values = v;
return (unsigned)-1;
}
