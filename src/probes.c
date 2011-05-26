/*
 * This file is (C) Cfengine AS. See COSL LICENSE for details.
 */

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "probes.h"

/* Structs */

typedef struct Probe
   {
   const char *name;
   ProbeInit init;
   } Probe;

/* Constants */

static const struct Probe NOVA_PROBES[] =
   {
   { "Input/output", &MonIoInit },
   { "Memory", &MonMemoryInit },
   };

/* Globals */

static ProbeGatherData NOVA_PROBES_GATHERERS[sizeof(NOVA_PROBES)/sizeof(NOVA_PROBES[0])];

/****************************************************************************/

void Nova_MonOtherInit(void)
{
int i;

CfOut(cf_verbose, "", "Starting initialization of static Nova monitoring probes.");

for (i = 0; i < sizeof(NOVA_PROBES) / sizeof(NOVA_PROBES[0]); ++i)
   {
   const Probe *probe = &NOVA_PROBES[i];
   const char *provider;
   const char *error;

   if ((NOVA_PROBES_GATHERERS[i] = (probe->init)(&provider, &error)))
      {
      CfOut(cf_verbose, "", " * %s: %s.", probe->name, provider);
      }
   else
      {
      CfOut(cf_verbose, "", " * %s: Disabled: %s.", probe->name, error);
      }
   }

CfOut(cf_verbose, "", "Initialization of static Nova monitoring probes is finished.");
}

/****************************************************************************/

void Nova_MonOtherGatherData(double *cf_this)
{
int i;
CfOut(cf_verbose, "", "Gathering data from static Nova monitoring probes.");

for (i = 0; i < sizeof(NOVA_PROBES) / sizeof(NOVA_PROBES[0]); ++i)
   {
   const char *probename = NOVA_PROBES[i].name;
   ProbeGatherData gatherer = NOVA_PROBES_GATHERERS[i];

   if (gatherer)
      {
      CfOut(cf_verbose, "", " * %s", probename);
      (*gatherer)(cf_this);
      }
   }
CfOut(cf_verbose, "", "Gathering data from static Nova monitoring probes is finished.");
}

