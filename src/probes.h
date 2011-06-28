#ifndef CFENGINE_PROBES_H
#define CFENGINE_PROBES_H

#include "cf.nova.h"

/*
 * Type of callback collecting actual probe data.
 */
typedef void (*ProbeGatherData)(double *cf_this);

/*
 * Type of probe initialization function.
 *
 * Probe initialization function should either return callback and name of probe
 * provider in "name" argument, or NULL and error description in "error"
 * argument.
 *
 * Caller does not free data returned in "name" or "error".
 */
typedef ProbeGatherData (*ProbeInit)(const char **name, const char **error);

/*
 * Existing probes and their identifiers
 */

#define MON_IO_READS "io_reads"
#define MON_IO_WRITES "io_writes"
#define MON_IO_READDATA "io_readdata"
#define MON_IO_WRITTENDATA "io_writtendata"

ProbeGatherData MonIoInit(const char **name, const char **error);

#define MON_MEM_TOTAL "mem_total"
#define MON_MEM_FREE "mem_free"
#define MON_MEM_CACHED "mem_cached"
#define MON_MEM_SWAP "mem_swap"
#define MON_MEM_FREE_SWAP "mem_freeswap"

ProbeGatherData MonMemoryInit(const char **name, const char **error);

#endif
