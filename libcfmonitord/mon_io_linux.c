/*
 * This file is (C) Cfengine AS. See COSL LICENSE for details.
 */

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "cfstream.h"
#include "probes.h"
#include "files_names.h"
#include "string_lib.h"

/************************************************************************/

#define SYSFSBLOCK "/sys/class/block/"

#define KB 1024

/************************************************************************/

static void SysfsizeDeviceName(char *name)
{
    while ((name = strchr(name, '/')))
    {
        *name = '!';
    }
}

static int DiskSectorSize(const char *sysfsname)
{
    char sysfspath[CF_BUFSIZE];
    FILE *fh;
    int size = 512;             /* Long-time default value */

    if (snprintf(sysfspath, CF_BUFSIZE, SYSFSBLOCK "%s/queue/logical_block_size", sysfsname) >= CF_BUFSIZE)
    {
        /* FIXME: report overlong string */
        return -1;
    }

    if (!(fh = fopen(sysfspath, "r")))
    {
        /*
         * Older Linux systems don't work correctly with new 4K drives. It's safe to
         * report 512 here
         */
    }
    else
    {
        if (fscanf(fh, "%d", &size) != 1)
        {
            if (ferror(fh))
            {
                CfOut(cf_error, "fscanf", "Unable to read sector size for %s. Assuming 512 bytes.", sysfsname);
            }
            else
            {
                CfOut(cf_error, "", "Unable to read sector size for %s. Assuming 512 bytes.", sysfsname);
            }
        }
        fclose(fh);
    }

    return size;
}

static bool IsPartition(const char *sysfsname)
{
    char sysfspath[CF_BUFSIZE];

    if (snprintf(sysfspath, CF_BUFSIZE, "%s/partition", SYSFSBLOCK) >= CF_BUFSIZE)
    {
        /* FIXME: report overlong string */
        return false;
    }

    if (access(sysfspath, F_OK) == 0)
    {
        return true;
    }

    if (errno != ENOENT)
    {
        CfOut(cf_error, "access", "Unable to detect whether block device %s is a partition", sysfsname);
    }

    return false;
}

static bool IsStackedDevice(const char *sysfsname)
{
/* "Stacked" as LVM or cryptfs volume */
    char sysfspath[CF_BUFSIZE];
    DIR *dir;
    struct dirent *dirent;

    if (snprintf(sysfspath, CF_BUFSIZE, "%s/slaves", SYSFSBLOCK) >= CF_BUFSIZE)
    {
        /* FIXME: report overlong string */
        return false;
    }

    if (!(dir = opendir(sysfspath)))
    {
        /* We don't have any information */
        return false;
    }

    errno = 0;
    while ((dirent = readdir(dir)))
    {
        if (strcmp(dirent->d_name, ".") && strcmp(dirent->d_name, ".."))
        {
            /* Found a file inside. It's a stacked device. */
            closedir(dir);
            return true;
        }
    }

    if (errno)
    {
        CfOut(cf_error, "readdir", "Unable to read 'slaves' sysfs subdirectory for %s", sysfsname);
    }

    closedir(dir);
    return false;
}

static bool IsPhysicalDevice(const char *sysfsname)
{
    return !IsPartition(sysfsname) && !IsStackedDevice(sysfsname);
}

/************************************************************************/

static void MonIoDiskstatsGatherData(double *cf_this)
{
    char buf[CF_BUFSIZE];
    FILE *fh;
    time_t now = time(NULL);

    unsigned long long totalreads = 0, totalwrites = 0;
    unsigned long long totalreadbytes = 0, totalwrittenbytes = 0;

    if (!(fh = fopen("/proc/diskstats", "r")))
    {
        CfOut(cf_error, "fopen", "Error trying to open /proc/diskstats");
        return;
    }

/* Read per-disk statistics */

    while (fgets(buf, CF_BUFSIZE, fh))
    {
        char diskname[CF_BUFSIZE];
        unsigned long reads, writes, readsectors, writtensectors;

        /* Format sanity check */
        if (!strchr(buf, '\n'))
        {
            CfOut(cf_error, "",
                  "/proc/diskstats format error: read overlong string (> " TOSTRING(CF_BUFSIZE - 1) " bytes)");
            goto err;
        }

        if (StripTrailingNewline(buf, CF_EXPANDSIZE) == -1)
        {
            CfOut(cf_error, "", "StripTrailingNewline was called on an overlong string");
        }

        if (sscanf(buf, "%*u %*u %s %lu %*u %lu %*u %lu %*u %lu",
                   diskname, &reads, &readsectors, &writes, &writtensectors) != 5)
        {
            CfOut(cf_error, "", "Wrong /proc/diskstats line format: %s", buf);
            continue;
        }

        SysfsizeDeviceName(diskname);

        if (!reads && !readsectors && !writes && !writtensectors)
        {
            /* Not a disk. Something virtual: loopback or similar */
            continue;
        }

        if (!IsPhysicalDevice(diskname))
        {
            continue;
        }

        reads = GetInstantUint32Value(diskname, "reads", reads, now);
        writes = GetInstantUint32Value(diskname, "writes", writes, now);
        readsectors = GetInstantUint32Value(diskname, "readsectors", readsectors, now);
        writtensectors = GetInstantUint32Value(diskname, "writtensectors", writtensectors, now);

        if (reads != (unsigned) -1)
        {
            int sectorsize = DiskSectorSize(diskname);

            totalreads += reads;
            totalwrites += writes;
            totalreadbytes += ((unsigned long long) readsectors) * sectorsize;
            totalwrittenbytes += ((unsigned long long) writtensectors) * sectorsize;
        }
    }

    if (ferror(fh))
    {
        CfOut(cf_error, "fgets", "Error reading /proc/diskstats");
        goto err;
    }

/* Summarize */
    int reads_slot = NovaRegisterSlot(MON_IO_READS, "Number of I/O reads", "reads per second",
                                      0.0, 1000.0, true);
    int writes_slot = NovaRegisterSlot(MON_IO_WRITES, "Number of I/O writes", "writes per second",
                                       0.0f, 1000.0f, true);
    int readdata_slot = NovaRegisterSlot(MON_IO_READDATA, "Amount of data read", "megabytes/s",
                                         0.0, 1000.0, true);
    int writtendata_slot = NovaRegisterSlot(MON_IO_WRITTENDATA, "Amount of data written", "megabytes",
                                            0.0, 1000.0, true);

    if (reads_slot != -1 && totalreads != 0)
    {
        cf_this[reads_slot] = totalreads;
    }
    if (writes_slot != -1 && totalwrites != 0)
    {
        cf_this[writes_slot] = totalwrites;
    }
    if (readdata_slot != -1 && totalreadbytes != 0)
    {
        cf_this[readdata_slot] = ((double) totalreadbytes) / KB / KB;
    }
    if (writtendata_slot != -1 && totalwrittenbytes != 0)
    {
        cf_this[writtendata_slot] = ((double) totalwrittenbytes) / KB / KB;
    }

  err:
    fclose(fh);
}

/************************************************************************/

static void MonIoPartitionsGatherData(double *cf_this)
{
}

/************************************************************************/

ProbeGatherData MonIoInit(const char **name, const char **error)
{
    if (access("/proc/diskstats", R_OK) == 0)
    {
        *name = "Linux 2.6 /proc/diskstats statistics";
        *error = NULL;
        return &MonIoDiskstatsGatherData;
    }
    else if (access("/proc/partitions", R_OK) == 0)
    {
        *name = "Linux 2.4 /proc/partitions statistics";
        *error = NULL;
        return &MonIoPartitionsGatherData;
    }
    else
    {
        *name = NULL;
        *error = "/proc/diskstats and /proc/partitions are not available";
        return NULL;
    }
}
