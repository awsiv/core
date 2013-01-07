#ifndef CFENGINE_FILECACHE_H
#define CFENGINE_FILECACHE_H

#include "cf.nova.h"

typedef struct FileCache_ FileCache;

typedef struct
{
    void *value;
    size_t value_size;
    time_t last_modified;
} FileCacheEntry;

FileCache *FileCacheNew(const char *identity, const char *cache_directory);
void FileCacheDestroy(FileCache *cache);

void FileCacheRemoveExpired(FileCache *cache, time_t expiration_date);

void FileCachePut(FileCache *cache, const char *key, const void *value, size_t value_size);
FileCacheEntry FileCacheGet(FileCache *cache, const char *key);

void FileCacheEntryDestroy(FileCacheEntry *entry);

#endif
