#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "shared_kstat.h"

static kstat_ctl_t *kstat;

kstat_ctl_t *GetKstatHandle(void)
{
    if (!kstat)
    {
        kstat = kstat_open();
        if (!kstat)
        {
            CfOut(OUTPUT_LEVEL_ERROR, "kstat_open", "Unable to open Solaris kstat subsystem");
        }
    }
    else
    {
        kstat_chain_update(kstat);
    }

    return kstat;
}
