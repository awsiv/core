/*
 * This file is (C) Cfengine AS. See COSL LICENSE for details.
 */

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "probes.h"

/************************************************************************/

/* Generic "Key: Numeric Value" parser */

typedef void (*MemoryFieldCallback)(const char *field, off_t value, void *param);

static bool ParseKeyNumericValue(FILE *fd, MemoryFieldCallback callback, void *param)

{ char buf[1024];

while (fgets(buf, 1024, fd))
   {
   char *s = strchr(buf, ':');
   unsigned long long value;
   
   if (!s)
      {
      /* Malformed file */
      return false;
      }
   
   *s = 0;
   
   if (1 == sscanf(s + 1, "%llu", &value) != 1)
      {
      /* Malformed file */
      return false;
      }
   
   (*callback)(buf, value, param);
   }

return !ferror(fd);
}

/************************************************************************/

/* Getting data from /proc/meminfo -  All values are in bytes */

typedef struct MemoryInfo
   {
   off_t total;
   off_t free;
   off_t cached;
   off_t swap;
   off_t free_swap;
   } MemoryInfo;

#define KB 1024

static void AcceptMemoryField(const char *field, off_t value, void *param)

{ MemoryInfo *info = param;

value *= KB;

if (!strcmp(field, "MemTotal"))
   {
   info->total = value;
   }
else if (!strcmp(field, "MemFree"))
   {
   info->free = value;
   }
else if (!strcmp(field, "Cached"))
   {
   info->cached = value;
   }
else if (!strcmp(field, "SwapTotal"))
   {
   info->swap = value;
   }
else if (!strcmp(field, "SwapFree"))
   {
   info->free_swap = value;
   }
}

/************************************************************************/

static void MonMeminfoGatherData(double *cf_this)

{ FILE *fh;
  MemoryInfo info = { 0 };

if (!(fh = fopen("/proc/meminfo", "r")))
   {
   CfOut(cf_error, "fopen", "Unable to open /proc/meminfo");
   return;
   }

if (ParseKeyNumericValue(fh, &AcceptMemoryField, &info))
   {
   int total_slot = NovaRegisterSlot(MON_MEM_TOTAL, "Total system memory", "megabytes",
                                     512.0f, 4096.0f, true);
   int free_slot = NovaRegisterSlot(MON_MEM_FREE, "Free system memory", "megabytes",
                                    0.0f, 4096.0f, true);
   int cached_slot = NovaRegisterSlot(MON_MEM_CACHED, "Size of disk cache", "megabytes",
                                      0.0f, 4096.0f, true);
   int swap_slot = NovaRegisterSlot(MON_MEM_SWAP, "Total swap size", "megabytes",
                                    0.0f, 4096.0f, true);
   int free_swap_slot = NovaRegisterSlot(MON_MEM_FREE_SWAP, "Free swap size", "megabytes",
                                         0.0f, 8192.0f, true);

   if (total_slot != -1)
      {
      cf_this[total_slot] = ((double)info.total) / KB / KB;
      }
   if (free_slot != -1)
      {
      cf_this[free_slot] = ((double)info.free) / KB / KB;
      }
   if (cached_slot != -1)
      {
      cf_this[cached_slot] = ((double)info.cached) / KB / KB;
      }
   if (swap_slot != -1)
      {
      cf_this[swap_slot] = ((double)info.swap) / KB / KB;
      }
   if (free_swap_slot != -1)
      {
      cf_this[free_swap_slot] = ((double)info.free_swap) / KB / KB;
      }
   }
else
   {
   CfOut(cf_error, "", "Unable to parse /proc/meminfo");
   }

fclose(fh);
}

/************************************************************************/

ProbeGatherData MonMemoryInit(const char **name, const char **error)
{
if (access("/proc/meminfo", R_OK) == 0)
   {
   *name = "Linux /proc/meminfo statistics";
   *error = NULL;
   return &MonMeminfoGatherData;
   }
else
   {
   *name = NULL;
   *error = "/proc/meminfo is not readable";
   return NULL;
   }
}
