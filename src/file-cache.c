#include "file-cache.h"

#include "string_lib.h"
#include "log.h"
#include "files_names.h"

#include <dirent.h>
#include <unistd.h>
#include <assert.h>

struct FileCache_
{
    char *key_prefix;
    size_t key_prefix_length;
    char *directory;
};

FileCache *FileCacheNew(const char *identity, const char *cache_directory)
{
    if (access(cache_directory, W_OK | R_OK) != 0)
    {
        syslog(LOG_ERR, "Read/write permissions to cache directory %s not granted", cache_directory);
        return NULL;
    }

    FileCache *c = xmalloc(sizeof(FileCache));

    c->key_prefix = SafeStringDuplicate(identity);
    c->key_prefix_length = SafeStringLength(c->key_prefix);
    c->directory = SafeStringDuplicate(cache_directory);

    return c;
}

// non-destructive JoinPath
static char *JoinPathAlloced(const char *path, const char *leaf)
{
    char *joined_path = xcalloc(CF_BUFSIZE, sizeof(char));
    strncpy(joined_path, path, CF_BUFSIZE);
    return JoinPath(joined_path, leaf);
}


static char *GenerateEntryPath(FileCache *cache, const char *key)
{
    char entry_file[CF_MAXVARSIZE] = { 0 };
    snprintf(entry_file, CF_MAXVARSIZE, "%s_%s", cache->key_prefix, key);
    return JoinPathAlloced(cache->directory, entry_file);
}

void FileCachePut(FileCache *cache, const char *key, const void *value, size_t value_size)
{
    assert(cache);
    assert(key);
    assert(value);

    char tmp_path[L_tmpnam + 1] = { 0 };
    if (tmpnam(tmp_path) == NULL)
    {
        syslog(LOG_ERR, "Could not generate temporary name for file cache put");
        return;
    }
    FILE *entry_file = fopen(tmp_path, "w");
    if (!entry_file)
    {
        syslog(LOG_ERR, "Could not write temporary cache write entry %s", tmp_path);
        return;
    }

    size_t bytes_written = fwrite(value, 1, value_size, entry_file);
    fclose(entry_file);

    if (bytes_written != value_size)
    {
        syslog(LOG_ERR, "Bytes written for file cache entry (%ld) differs from size specified (%ld)",
               bytes_written, value_size);
        if (unlink(tmp_path) == 0)
        {
            syslog(LOG_DEBUG, "Removed temporary file cache entry %s", tmp_path);
        }
        else
        {
            syslog(LOG_DEBUG, "Error removing temporary file cache entry %s, error code (%d)",
                   tmp_path, errno);
        }
    }

    // commit
    char *entry_path = GenerateEntryPath(cache, key);
    if (rename(tmp_path, entry_path) == 0)
    {
        syslog(LOG_DEBUG, "Comitted cache entry %s", entry_path);
    }
    else
    {
        syslog(LOG_ERR, "Unable to commit temporary file cache entry %s", entry_path);
        if (unlink(entry_path) == 0)
        {
            syslog(LOG_DEBUG, "Removed temporary file cache entry %s", tmp_path);
        }
        else
        {
            syslog(LOG_DEBUG, "Error removing temporary file cache entry %s, error code (%d)",
                   tmp_path, errno);
        }
    }
    free(entry_path);
}

FileCacheEntry FileCacheGet(FileCache *cache, const char *key)
{
    assert(cache);
    assert(key);

    char *entry_path = GenerateEntryPath(cache, key);

    size_t bytes_expected = 0;
    size_t last_modified = 0;
    {
        struct stat buf = { 0 };
        if (stat(entry_path, &buf) == 0)
        {
            bytes_expected = buf.st_size;
            last_modified = buf.st_mtim.tv_sec;
        }
        else
        {
            syslog(LOG_DEBUG, "File cache miss on %s", entry_path);
            free(entry_path);
            return (FileCacheEntry) { NULL, 0, 0 };
        }
    }

    FILE *entry_file = fopen(entry_path, "r");
    if (!entry_file)
    {
        syslog(LOG_ERR, "Could not open file cache entry %s for reading", entry_path);
        free(entry_path);
        return (FileCacheEntry) { NULL, 0, 0 };
    }

    FileCacheEntry entry = { 0 };
    entry.value = xmalloc(bytes_expected);

    size_t bytes_read = fread(entry.value, 1, bytes_expected, entry_file);
    if (bytes_read == bytes_expected)
    {
        entry.value_size = bytes_read;
        entry.last_modified = last_modified;
        syslog(LOG_DEBUG, "Retrieved cached version at %s", entry_path);
        return entry;
    }
    else
    {
        syslog(LOG_ERR, "Bytes read from cache entry %s is %lu, expected %lu", entry_path,
               bytes_read, bytes_expected);
        free(entry.value);
        entry.value = NULL;
        return entry;
    }
}

void FileCacheRemoveExpired(FileCache *cache, time_t expiration_date)
{
    assert(cache);
    DIR *dir = opendir(cache->directory);
    if (!dir)
    {
        syslog(LOG_ERR, "Unable to open file cache (%s) directory (%s) while removing expired entries",
               cache->key_prefix, cache->directory);
        return;
    }

    {
        struct dirent *entry = NULL;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strncmp(cache->key_prefix, entry->d_name, cache->key_prefix_length) != 0)
            {
                continue;
            }

            char *entry_path = JoinPathAlloced(cache->directory, entry->d_name);
            struct stat buf = { 0 };
            if (stat(entry_path, &buf) == 0)
            {
                if (buf.st_mtim.tv_sec < expiration_date)
                {
                    if (unlink(entry_path) == 0)
                    {
                        syslog(LOG_DEBUG, "Removed expired file cache entry %s", entry_path);
                    }
                    else
                    {
                        syslog(LOG_DEBUG, "Error removing expired file cache entry %s, error code (%d)",
                               entry_path, errno);
                    }
                }
            }
            else
            {
                syslog(LOG_ERR, "Unable to stat file cache entry %s", entry_path);
            }

            free(entry_path);
        }
    }

    closedir(dir);
}

void FileCacheDestroy(FileCache *cache)
{
    if (cache)
    {
        FileCacheRemoveExpired(cache, time(NULL));
        free(cache->key_prefix);
        free(cache->directory);
        free(cache);
    }
}

void FileCacheEntryDestroy(FileCacheEntry *entry)
{
    free(entry->value);
    entry->last_modified = 0;
    entry->value_size = 0;
}
