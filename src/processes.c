/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "promises.h"
#include "files_lib.h"
#include "files_names.h"

#ifdef HAVE_ZONE_H
# include <zone.h>
#endif

static void Nova_DoFileDiff(char *file, char *destination, struct stat sb, struct stat dsb, const char *handle);
static int Nova_GetFirstChangePosition(char *file, char *destination);
static int Nova_FileIsBinary(char *name, int size, int maxsize);
static void Nova_ReportFileChange(FILE *fp, char *file, char *destination, int maxsize);
static int Nova_LoadFileHunks(char *file, char *destination, FileLine **list1, FileLine **list2, int *len1, int *len2,
                              int maxsize);
static FileLine *AppendFileLine(FileLine **liststart, char *item, int pos);
static void DeleteFileLine(FileLine **liststart, FileLine *item);
static void DeleteAllFileLines(FileLine *list);

/*****************************************************************************/

void LogFileChange(char *file, int change, Attributes a, Promise *pp, const ReportContext *report_context)
{
    char destination[CF_BUFSIZE];
    struct stat sb, dsb;

    CfOut(cf_verbose, "", " -> Updating the difference engine with \"%s\"", file);

    if (cfstat(file, &sb) == -1)
    {
        CfOut(cf_verbose, "", "!! Cannot stat file \"%s\" - skipping", file);
        return;
    }

    if (Nova_FileIsBinary(file, sb.st_size, NOVA_MAXDIFFSIZE))
    {
        cfPS(cf_error, CF_FAIL, "", pp, a,
             "!! File \"%s\" is either too large to diff or contains binary chars -- skipping", file);
        return;
    }

    if (a.havedepthsearch)
    {
        CfOut(cf_error, "", "!! You may not use change detail logging on depth searches");
        PromiseRef(cf_error, pp);
        return;
    }

    if (GetRepositoryPath(file, a, destination))
    {
        JoinPath(destination, CanonifyName(file));
        MakeParentDirectory(destination, a.move_obstructions, report_context);
    }
    else
    {
        snprintf(destination, sizeof(destination), "%s_cfchanges", file);
    }


/* Check if the file already exists, if so do a diff */

    if (change && (cfstat(destination, &dsb) != -1))
    {
        const char *handle = PromiseID(pp);
        Nova_DoFileDiff(file, destination, sb, dsb, handle);
    }

/* Copy the current version to repository for versioning,
   keep a backup for forensics or binary change inspection */

    if (change || cfstat(destination, &dsb) == -1)
    {
        a.copy.servers = NULL;
        a.copy.backup = cfa_backup;
        a.copy.stealth = false;
        a.copy.verify = false;
        a.copy.preserve = false;

        CheckForFileHoles(&sb, pp);

        if (CopyRegularFile(file, destination, sb, dsb, a, pp, report_context))
        {
            CfOut(cf_verbose, "", " -> Cached change-file %s to repository location %s\n", file, destination);
            return;
        }
        else
        {
            CfOut(cf_verbose, "", " -> Cached change-file %s to repository location %s\n", file, destination);
            return;
        }
    }
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

static void Nova_DoFileDiff(char *file, char *destination, struct stat sb,
                            struct stat dsb, const char *handle)
{
    int pos;
    time_t now = time(NULL);
    char datestr[CF_MAXVARSIZE];
    FILE *fout;
    char logname[CF_BUFSIZE];

/* Start with a *cheap* diff algorithm. We don't need to use the Heckel/Myers
 because we are assuming small changes -- else we keep both files and users
 can use what they like to compare the files - diff cannot always show something
 helpful */

    CfOut(cf_verbose, "", " -> Nova Analysis of changes on file %s\n", file);

    strncpy(datestr, cf_ctime(&now), CF_MAXVARSIZE - 1);
    Chop(datestr);

    snprintf(logname, CF_BUFSIZE - 1, "%s%c%s", CFWORKDIR, FILE_SEPARATOR, NOVA_DIFF_LOG);
    MapName(logname);

    if ((fout = fopen(logname, "a")) == NULL)
    {
        CfOut(cf_error, "fopen", " !! Unable to open change log %s\n", logname);
    }

    fprintf(fout, "CHANGE %s\n", file);

    if (handle != NULL)
    {
        fprintf(fout, "promise handle: %s\n", handle);
    }

    pos = Nova_GetFirstChangePosition(file, destination);

    fprintf(fout, "%ld;File changed size from %jd to %jd, first difference at byte position %d/%jd of old\n",
            (long) now, (intmax_t) dsb.st_size, (intmax_t) sb.st_size, pos, (intmax_t) dsb.st_size);

    if (Nova_FileIsBinary(destination, dsb.st_size, NOVA_MAXDIFFSIZE))
    {
        // shouldn't happen, we test for this in calling function
        fprintf(fout, "+,0,(binary file)\n");
        CfOut(cf_error, "",
              "!! Cannot view content differences on a binary or huge files, you may inspect %s and %s manually", file,
              destination);
    }
    else
    {
        Nova_ReportFileChange(fout, file, destination, NOVA_MAXDIFFSIZE);
    }

    fprintf(fout, "END\n");

    fclose(fout);
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

static int Nova_GetFirstChangePosition(char *file, char *destination)
{
    FILE *fin1, *fin2;
    int pos = 0;
    int word1, word2;

    if ((fin1 = fopen(file, "r")) == NULL)
    {
        CfOut(cf_error, "fopen", " !! Unable to read the changed file %s", file);
        return 0;
    }

    if ((fin2 = fopen(destination, "r")) == NULL)
    {
        CfOut(cf_error, "fopen", " !! Unable to read the changed save-file %s", destination);
        fclose(fin1);
        return 0;
    }

    while (true)
    {
        word1 = fgetc(fin1);
        word2 = fgetc(fin2);
        pos++;

        if (word1 != word2)
        {
            break;
        }

        if (feof(fin1) || feof(fin2))
        {
            break;
        }
    }

    fclose(fin1);
    fclose(fin2);
    return pos;
}

/*****************************************************************************/

static int Nova_FileIsBinary(char *name, int size, int maxsize)
{
    char *file_buffer, *sp;
    int hasbinary = false, hasnewline = 1;

    file_buffer = xmalloc(maxsize);
    if (FileRead(name, file_buffer, maxsize) < 0)
    {
        free(file_buffer);
        return false;
    }

    for (sp = file_buffer; *sp != '\0' && sp < sp + maxsize; sp++)
    {
        if ((*sp > 0 && *sp < 9) || *sp == 11 || *sp == 12 || (*sp > 13 && *sp < 32) || *sp == 127)
        {
            hasbinary = true;
            break;
        }

        if (*sp == 10)
        {
            hasnewline++;
        }
    }

    free(file_buffer);

    if (hasbinary)
    {
        return true;
    }

    if ((sp - file_buffer) / hasnewline > CF_BUFSIZE)
    {
        return true;
    }

    return false;
}

/*****************************************************************************/

static void RemoveDuplicateLines(FileLine **lines1, FileLine **lines2)
{
    FileLine *it1 = *lines1;
    FileLine *it2 = *lines2;

    FileLine *next1, *next2;

    for (FileLine *flp1 = it1; flp1 != NULL; flp1 = next1)
    {
        next1 = flp1->next;

        for (FileLine *flp2 = it2; flp2 != NULL; flp2 = next2)
        {
            next2 = flp2->next;

            if ((*(flp1->text) == *(flp2->text)) && (strcmp(flp1->text, flp2->text) == 0))
            {
                DeleteFileLine(&it1, flp1);
                DeleteFileLine(&it2, flp2);
                break;
            }
        }
    }

    *lines1 = it1;
    *lines2 = it2;
}

static void Nova_ReportFileChange(FILE *fp, char *file, char *destination, int maxsize)
{
    FileLine *list1 = NULL, *list2 = NULL;
    FileLine *flp1, *flp2;
    int len1, len2;

    CfOut(cf_verbose, "", " -> Reporting on file changes to \"%s\"\n", file);

    if (!Nova_LoadFileHunks(file, destination, &list1, &list2, &len1, &len2, maxsize))
    {
        CfOut(cf_inform, "", " !! Unable to find reference files for diff of \"%s\"", file);
        return;
    }

    if (len1 > len2)
    {
        RemoveDuplicateLines(&list1, &list2);
    }
    else
    {
        RemoveDuplicateLines(&list2, &list1);
    }

    for (flp1 = list1; flp1 != NULL; flp1 = flp1->next)
    {
        if (EmptyString(flp1->text))
        {
            fprintf(fp, "+,%d,(blanks/space)\n", flp1->counter);
        }
        else
        {
            fprintf(fp, "+,%d,%s\n", flp1->counter, flp1->text);
        }
    }

    for (flp2 = list2; flp2 != NULL; flp2 = flp2->next)
    {
        if (EmptyString(flp2->text))
        {
            fprintf(fp, "-,%d,(blanks/space)\n", flp2->counter);
        }
        else
        {
            fprintf(fp, "-,%d,%s\n", flp2->counter, flp2->text);
        }
    }

    DeleteAllFileLines(list1);
    DeleteAllFileLines(list2);
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

static int Nova_LoadFileHunks(char *file, char *destination, FileLine **list1, FileLine **list2, int *len1, int *len2,
                              int maxsize)
{
    int pos = 0, size = 0, read1, read2, c1, c2;
    FILE *fin1, *fin2;
    char line1[CF_BUFSIZE], line2[CF_BUFSIZE];
    FileLine *flp1, *flp2, *last1 = 0, *last2 = 0;

    if ((fin1 = fopen(file, "r")) == NULL)
    {
        CfOut(cf_error, "fopen", " !! Unable to read the changed file %s", file);
    }

    if ((fin2 = fopen(destination, "r")) == NULL)
    {
        CfOut(cf_error, "fopen", " !! Unable to read the changed save-file %s", destination);
        fclose(fin1);
        return 0;
    }

    *len1 = 0;
    *len2 = 0;
    c1 = 0;
    c2 = 0;

    while (!feof(fin1) || !feof(fin2))
    {
        if ((read1 = CfReadLine(line1, CF_BUFSIZE - 1, fin1)))
        {
            c1++;
        }

        if ((read2 = CfReadLine(line2, CF_BUFSIZE - 1, fin2)))
        {
            c2++;
        }

        if (*len1 == 0 && *len2 == 0)
        {
            pos++;
        }

        size += strlen(line1);
        size += strlen(line2);

        if (size / 2 > maxsize)
        {
            CfOut(cf_inform, "", " !! Files exceeded maxiumum size %d during change reporting on \"%s\"", maxsize,
                  file);
            break;
        }

        if (*len1 == 0 && *len2 == 0 && (*line1 == *line2) && (strcmp(line1, line2) == 0))
        {
            continue;
        }

        if (read1)
        {
            last1 = AppendFileLine(list1, line1, c1);
            (*len1)++;
        }

        if (read2)
        {
            last2 = AppendFileLine(list2, line2, c2);
            (*len2)++;
        }

        line1[0] = '\0';
        line2[0] = '\0';
    }

    fclose(fin1);
    fclose(fin2);

    for (flp1 = last1; flp1 != NULL; flp1 = flp1->prev)
    {
    }

    for (flp2 = last2; flp2 != NULL; flp2 = flp2->prev)
    {
    }

/* Now prune the lists, first chop the ends */

    if (last1 && last2)
    {
        FileLine *next1, *next2;

        for (flp1 = last1, flp2 = last2; flp1 != NULL && flp2 != NULL; flp1 = next1, flp2 = next2)
        {
            if ((*(flp1->text) == *(flp2->text)) && strcmp(flp1->text, flp2->text) == 0)
            {
                next1 = flp1->prev;
                next2 = flp2->prev;
                DeleteFileLine(list1, flp1);
                DeleteFileLine(list2, flp2);
            }
            else
            {
                break;
            }
        }
    }

/* Now we are left with the central hunks */

    return pos;
}

/*****************************************************************************/

static FileLine *AppendFileLine(FileLine **liststart, char *item, int pos)
{
    FileLine *ip, *lp, *prev = NULL;

    ip = xmalloc(sizeof(Item));

    ip->text = xstrdup(item);

    if (*liststart == NULL)
    {
        *liststart = ip;
    }
    else
    {
        for (lp = *liststart; lp->next != NULL; lp = lp->next)
        {
        }

        lp->next = ip;

        if (lp->next != NULL)
        {
            prev = lp;
        }
    }

    ip->prev = prev;
    ip->next = NULL;
    ip->counter = pos;
    return ip;
}

/*****************************************************************************/

static void DeleteAllFileLines(FileLine *list)
{
    FileLine *flp, *next = list;

    for (flp = list; next != NULL; flp = next)
    {
        next = flp->next;

        if (flp->text != NULL)
        {
            free(flp->text);
        }

        free((char *) flp);
    }
}

/*********************************************************************/

static void DeleteFileLine(FileLine **liststart, FileLine *item)
{
    FileLine *ip, *sp;

    if (item == NULL || liststart == NULL)
    {
        return;
    }

    if (*liststart != NULL)
    {
        if (item->text != NULL)
        {
            free(item->text);
        }

        sp = item->next;

        if (item == *liststart)
        {
            *liststart = sp;

            if (sp)
            {
                sp->prev = NULL;
            }
        }
        else
        {
            for (ip = *liststart; ip && ip->next != item; ip = ip->next)
            {
            }

            ip->next = sp;

            if (sp && sp->next != NULL)
            {
                sp->prev = ip;
            }
        }

        free((char *) item);
    }
}
