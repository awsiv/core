/*****************************************************************************/
/*                                                                           */
/* File: chew.c                                                              */
/*                                                                           */
/* Created: Sat Jan 26 20:12:14 2008                                         */
/*                                                                           */
/* Author:                                                                   */
/*                                                                           */
/* Revision: $Id$                                                            */
/*                                                                           */
/* Description:                                                              */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define CF_SCALAR 's'

#define false 0
#define true  1
#define CF_BUFSIZE 2048
#define CF_MAXVARSIZE 1024
#define cf_error 1

#define DOC_CONTROLLER_PREFIX "/knowledge/docs/"

/*******************************************************************/

typedef struct Rlist_ Rlist;

struct Rlist_
{
    void *item;
    char type;
    Rlist *state_ptr;           /* Points to "current" state/element of sub-list */
    Rlist *next;
};

typedef struct Item_ Item;

struct Item_
{
    char *name;
    Item *next;
};

/*************************************************************************/
/* Ontology                                                              */
/*************************************************************************/

typedef struct Topic_ Topic;
typedef struct TopicAssociation_ TopicAssociation;

struct Topic_
{
    int id;
    char *topic_context;
    char *topic_name;
    double evc;
    TopicAssociation *associations;
    char *comment;
    struct Occurrence *occurrences;
    Topic *next;
};

struct TopicAssociation_
{
    char *assoc_type;
    char *fwd_name;
    char *bwd_name;
    Rlist *associates;
    char *associate_topic_context;
    TopicAssociation *next;
};

/*****************************************************************************/

void Manual(char *prefix, char *str, char *file);
void ProcessFile(char *file, FILE *fin, char *context, char *prefix);
char *CanonifyName(char *str);
void AddTopic(Topic **list, char *name, char *type, int nr);
int TopicExists(Topic *list, char *topic_name, char *topic_context);
void AppendItem(Item **liststart, char *itemstring);
char ToLower(char ch);
char ToUpper(char ch);
char *ToUpperStr(char *str);
char *ToLowerStr(char *str);
char *GetTitle(char *base, char *type);
char *GetTopicType(Topic *list, char *topic_name);
void AddTopicAssociation(TopicAssociation **list, char *fwd_name, char *bwd_name, char *topic_context, char *associate,
                         int verify);
Topic *GetTopic(Topic *list, char *topic_name);
TopicAssociation *AssociationExists(TopicAssociation *list, char *fwd, char *bwd, int verify);
Rlist *RlistPrependScalarIdemp(Rlist **start, void *item, char type);
Rlist *KeyInRlist(Rlist *list, char *key);
Rlist *RlistPrepend(Rlist **start, void *item, char type);
Rlist *RlistFromSplitString(char *string, char sep);
int SubStrnCopyChr(char *to, char *from, int len, char sep);
int AddKeyAssociations(TopicAssociation **a, char *s, char *url,Item **script);

/*****************************************************************************/

main(int argc, char **argv)
{
// Prefix for Web pages:  http://cfengine.com/inside/manuals/
// Prefix for Nova, etc: /

    if (argv[3] && strcmp(argv[3], "web") == 0)
    {
        Manual(argv[1], argv[2], DOC_CONTROLLER_PREFIX);
    }
    else
    {
        Manual(argv[1], argv[2], DOC_CONTROLLER_PREFIX);
    }
}

/*****************************************************************************/

void Manual(char *context, char *file, char *prefix)
{
    FILE *fin;
    char name[2048];

/* Start with the main doc file */

    if ((fin = fopen(file, "r")) == NULL)
    {
        printf("File missing\n");
        exit(0);
    }

    strcpy(name, file);
    *(strchr(name, '.')) = '\0';        // strip extension

    ProcessFile(name, fin, context, prefix);

    fclose(fin);
}

/*****************************************************************************/

void ProcessFile(char *document, FILE *fin, char *context, char *prefix)
{
    char tmp[2048], line[2048], type[2048], url[2048], title[2048], *sp;
    char chapter[2048], section[2048], subsection[2048], script[2048], doctitle[2048];
    char ref[2048];
    Topic *tp, *topics = NULL;
    Item *ip, *scriptlog = NULL;
    int lineno = 0;

    strcpy(chapter, "Special Topics Guide");
    strcpy(section, "Special Topics Guide");

    while (!feof(fin))
    {
        fgets(line, 2047, fin);
        lineno++;

        if (sp = strstr(line, "<dt>&lsquo;<samp><code>"))
        {
            sscanf(sp + strlen("<dt>&lsquo;<samp><code>"), "%[^<]", title);
            sprintf(ref, " \"%s%s.html#%s\"\n", prefix, document, url);
            AppendItem(&scriptlog, ref);
            sprintf(script, "  represents => { \"Official CFEngine document\" }, about_topics => { \"%s\"};\n",ToLowerStr(title));
            AppendItem(&scriptlog, script);

            AddTopic(&topics, ToLowerStr(title), "documentation", lineno);

            if (tp = GetTopic(topics, title))
            {
                AddTopicAssociation(&(tp->associations), "is contained in", "contains", "files", document, true);
                AddKeyAssociations(&(tp->associations), document,ref,&scriptlog);
            }

        }

        if (sp = strstr(line, "<a name=\""))
        {
            memset(url, 0, 2048);
            sscanf(sp + strlen("<a name=\""), "%[^\"]", url);
        }

        if (sp = strstr(line, "<title>"))
        {
            memset(url, 0, 2048);
            sscanf(sp + strlen("<title>"), "%[^<]", title);

            strcpy(doctitle, title);

            if (strncmp(document, "st-", 3) == 0)
            {
                sprintf(ref, " \"%s%s.html\"\n", prefix, document);
                AppendItem(&scriptlog, ref);
                sprintf(script, "  represents => { \"Special Topics Guide\" }, about_topics => { \"%s\" };\n", ToLowerStr(title));
                AppendItem(&scriptlog, script);
            }
            else
            {
                sprintf(ref, " \"%s%s.html\"\n", prefix, document);
                AppendItem(&scriptlog, ref);
                sprintf(script, "  represents => { \"Official CFEngine document\"},  about_topics => { \"%s\" };\n", ToLowerStr(title));
                AppendItem(&scriptlog, script);
            }

            sprintf(ref, " \"%s%s.html\"\n", prefix, document);
            AppendItem(&scriptlog, ref);
            sprintf(script, "  represents => { \"Special Topics Guide\"},  about_topics => { \"%s\" };\n", ToLowerStr(title));            
            AppendItem(&scriptlog, script);
        }

        if (sp = strstr(line, "class=\""))
        {
            memset(type, 0, 2048);
            sscanf(sp + strlen("class=\""), "%[^\"]", type);

            if (strstr(type, "unnumbered") || strstr(type, "unnumberedsec"))
            {
                strcpy(title, GetTitle(sp, type));
                sprintf(ref, " \"%s%s.html#%s\"\n", prefix, document, url);
                AppendItem(&scriptlog, ref);
                sprintf(script, "  represents => { \"Text section\" }, about_topics => { \"%s\" };\n", ToLowerStr(title));
                AppendItem(&scriptlog, script);
                strcpy(subsection, title);

                AddTopic(&topics, title, section, lineno);

                if (tp = GetTopic(topics, title))
                {
                    AddTopicAssociation(&(tp->associations), "discusses", "is discussed in", "short topic", doctitle,
                                        true);
                    AddKeyAssociations(&(tp->associations), section, ref, &scriptlog);
                }
            }

            if (strstr(type, "subsection"))
            {
                strcpy(title, GetTitle(sp, type));

                sprintf(ref, " \"%s%s.html#%s\"\n", prefix, document, url);
                AppendItem(&scriptlog, ref);
                sprintf(script, "  represents => { \"manual %s\" }, about_topics => { \"%s\"};\n", type,ToLowerStr(title));
                AppendItem(&scriptlog, script);
                strcpy(subsection, title);

                AddTopic(&topics, subsection, section, lineno);
                continue;

                if ((tp = GetTopic(topics, subsection)) && (strlen(section) > 0))
                {
                    AddTopicAssociation(&(tp->associations), "is a subsection of", "has subs Nova2Txt_search_topicsection", "manual", section,
                                        true);
                    AddKeyAssociations(&(tp->associations), section, ref, &scriptlog);
                }
            }

            if (strstr(type, "section"))
            {
                strcpy(title, GetTitle(sp, type));
                sprintf(ref, " \"%s%s.html#%s\"\n", prefix, document, url);
                AppendItem(&scriptlog, ref);
                sprintf(script, "  represents => { \"manual section\" }, about_topics => { \"%s\"};\n", ToLowerStr(title));
                AppendItem(&scriptlog, script);

                script[0] = '\0';
                strcpy(section, title);
                strcpy(subsection, "");
                AddTopic(&topics, section, chapter, lineno);

                if (strcmp(title, section) != 0)
                {
                    if (tp = GetTopic(topics, subsection))
                    {
                        AddTopicAssociation(&(tp->associations), "discussed in", "discusses", "manual", chapter, true);
                        AddKeyAssociations(&(tp->associations), chapter, ref, &scriptlog);
                    }
                }

                continue;
            }

            if (strstr(type, "chapter"))
            {
                strcpy(title, GetTitle(sp, type));
                sprintf(ref, " \"%s%s.html#%s\"\n", prefix, document, url);
                AppendItem(&scriptlog, ref);

                sprintf(script, "  represents => { \"manual chapter\" }, about_topics => { \"%s\"};\n",ToLowerStr(title));
                AppendItem(&scriptlog, script);

                strcpy(chapter, title);
                strcpy(section, "");
                strcpy(subsection, "");
                AddTopic(&topics, chapter, title, lineno);

                if (strcmp(title, section) != 0)
                {
                    if (tp = GetTopic(topics, subsection))
                    {
                        AddTopicAssociation(&(tp->associations), "discussed in", "discusses", "Documentation", title,
                                            true);
                        AddKeyAssociations(&(tp->associations), section, ref,&scriptlog);
                    }
                }

                continue;
            }

            if (strstr(type, "smallexample"))
            {
                // Find the nearest section name for topic name

                memset(title, 0, 2048);

                if (strlen(subsection) != 0)
                {
                    strcpy(title, subsection);
                }
                else if (strlen(section) != 0)
                {
                    strcpy(title, section);
                }
                else
                {
                    strcpy(title, chapter);
                }

                sprintf(script, "examples:: \"%s%s.html#%s\"\n", prefix, document, url);
                AppendItem(&scriptlog, script);
                sprintf(script, " about_topics => { \"%s\" },\n", title);
                AppendItem(&scriptlog, script);
                sprintf(script, "  represents => { \"code example\" };\n");
                AppendItem(&scriptlog, script);
                continue;
            }

        }
    }

    printf("\nbundle knowledge %s\n\n{\n", CanonifyName(document));

    printf("topics:\n\n");

    printf("\"%s\";\n", document);

    for (tp = topics; tp != NULL; tp = tp->next)
    {
        TopicAssociation *ta;

        for (ta = tp->associations; ta != NULL; ta = ta->next)
        {
            if (strlen(tp->topic_context) > 0)
            {
                printf("%s::\n", CanonifyName(tp->topic_context));
            }

            printf("    \"%s\" ", tp->topic_name);

            Rlist *list = ta->associates;

            printf(" association => a(\"%s\",\"%s\",\"%s\")", ta->fwd_name, (char *) list->item, ta->bwd_name);

            printf(";\n");
        }
    }

    printf("\n\noccurrences:\n\n");

    for (ip = scriptlog; ip != NULL; ip = ip->next)
    {
        printf("%s", ip->name);
    }

    printf("}\n\n");
}

/******************************************************************/

char *GetTitle(char *base, char *type)
{
    static char tmp[CF_BUFSIZE];
    char buffer[CF_BUFSIZE];
    char *sp, *to;

    tmp[0] = '\0';
    memset(tmp, 0, CF_BUFSIZE);

    if (strstr(type, "unnumbered") || strstr(type, "unnumberedsec"))
    {
        sscanf(base + strlen("class=\"xx") + strlen(type), "%[^(\n]", buffer);
    }
    else
    {
        sscanf(base + strlen("class=\"xx") + strlen(type), "%*s %[^(\n]", buffer);
    }

    for (sp = buffer, to = tmp; *sp != '\0'; sp++)
    {
        switch (*sp)
        {
        case '<':

            while (*++sp != '>')
            {
                if (*sp == '\0')
                {
                    *to = '\0';
                    sp--;
                    continue;
                }
            }
            break;

        case '&':

            if (*(sp - 1) != '\\')
            {
                while (*++sp != ';')
                {
                    if (*sp == '\0')
                    {
                        *to = '\0';
                        sp--;
                        continue;
                    }
                }
            }

            break;

        case '\'':
        case '`':
            break;

        default:
            *to++ = *sp;
            break;
        }
    }

    if (sp = strstr(tmp, " (body template)"))
    {
        *sp = '\0';
    }

    sp = tmp + strlen(tmp) - 1;

    if (isspace(*sp))
    {
        *sp = '\0';
    }

    return tmp;
}

/*****************************************************************************/

char *CanonifyName(char *str)
{
    static char buffer[2048];
    char *sp;

    memset(buffer, 0, 2048);
    strcpy(buffer, str);

    for (sp = buffer; *sp != '\0'; sp++)
    {
        switch (*sp)
        {
        default:
            if (!isalnum((int) *sp))
            {
                *sp = '_';
            }

        }
    }

    return buffer;
}

/*****************************************************************************/

void AddTopic(Topic **list, char *name, char *type, int nr)
{
    Topic *tp;

    if (TopicExists(*list, name, type))
    {
//   fprintf(stderr,"Topic %s already defined at line %d\n",name,nr);
        return;
    }

    if ((tp = (Topic *) malloc(sizeof(Topic))) == NULL)
    {
        printf("Memory failure in AddTopic");
        exit(1);
    }

    if ((tp->topic_name = strdup(name)) == NULL)
    {
        printf("Memory failure in AddTopic");
        exit(1);
    }

    if ((tp->topic_context = strdup(type)) == NULL)
    {
        printf("Memory failure in AddTopic");
        exit(1);
    }

    tp->comment = NULL;
    tp->associations = NULL;
    tp->occurrences = NULL;
    tp->next = *list;
    *list = tp;
}

/*****************************************************************************/

int AddKeyAssociations(TopicAssociation **a, char *s, char *url,Item **occur)
{
    char *keywords[8000];
    char *exceptions[] = { "or", "and", "the", "there", "then", "what", "how", "ci", "at", NULL };

    char *otherwords[] = { "convergence", "promise", "scheduling", "workflow", "bundles", "hierarchy", "cloud",
        "package", "policy", "security", "virtualization", "scalability",
        "hierarchies", "file", NULL
    };
    char *sp;
    int i, j, skip;
    FILE *fp;
    char word[1024];

    if ((fp = fopen("words", "r")) == NULL)
    {
        return;
    }

    i = 0;

    while (!feof(fp))
    {
        memset(word, 0, 1024);
        fgets(word, 1023, fp);
        Chop(word);

        if (strlen(word) == 0)
        {
            continue;
        }

        keywords[i] = strdup(word);

        i++;

        if (i >= 8000)
        {
            break;
        }
    }

    keywords[i] = NULL;
    fclose(fp);

    for (i = 0; keywords[i] != NULL; i++)
    {
        skip = false;

        for (j = 0; exceptions[j] != NULL; j++)
        {
            if (strcmp(keywords[i], exceptions[j]) == 0)
            {
                skip = true;
                continue;
            }
        }

        if (skip)
        {
            continue;
        }

        //Check for canonified form too

        if (strcmp(ToLowerStr(s), keywords[i]) == 0)
        {
            continue;
        }

        if (sp = strstr(ToLowerStr(s), keywords[i]))
        {
            // Check for at least one space around the word

            if ((sp > s && *(sp - 1) != ' ') || !isspace(*(sp + strlen(keywords[i]))))
            {
                continue;
            }

            AddTopicAssociation(a, "seems to refer to", "seems be referred to in", "any", keywords[i], true);
        }

        if (strstr(CanonifyName(s), keywords[i]))
        {
            AddTopicAssociation(a, "seems to refer to", "seems be referred to in", "any", keywords[i], true);
        }
    }

    for (i = 0; otherwords[i] != NULL; i++)
    {
        if (strcmp(ToLowerStr(s), otherwords[i]) == 0)
        {
            continue;
        }

        if (sp = strstr(ToLowerStr(s), otherwords[i]))
        {
            // Check for at least one space around the word

            if ((sp > s && *(sp - 1) != ' ') || !isspace(*(sp + strlen(otherwords[i]))))
            {
//         continue;
            }

            AddTopicAssociation(a, "seems to refer to", "seems be referred to in", "any", otherwords[i], true);
        }
    }

}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int TopicExists(Topic *list, char *topic_name, char *topic_context)
{
    Topic *tp;

    for (tp = list; tp != NULL; tp = tp->next)
    {
        if (strcmp(tp->topic_name, topic_name) == 0)
        {
            if (topic_context && strcmp(tp->topic_context, topic_context) != 0)
            {
                //fprintf(stderr,"Scan: Topic \"%s\" exists, but its type \"%s\" does not match promised type \"%s\"\n",topic_name,tp->topic_context,topic_context);
            }

            return true;
        }
    }

    return false;
}

/*****************************************************************************/

Topic *GetTopic(Topic *list, char *topic_name)
{
    Topic *tp;

    for (tp = list; tp != NULL; tp = tp->next)
    {
        if (strcmp(topic_name, tp->topic_name) == 0)
        {
            return tp;
        }
    }

    return NULL;
}

/*****************************************************************************/

Topic *GetCanonizedTopic(Topic *list, char *topic_name)
{
    Topic *tp;

    for (tp = list; tp != NULL; tp = tp->next)
    {
        if (strcmp(topic_name, CanonifyName(tp->topic_name)) == 0)
        {
            return tp;
        }
    }

    return NULL;
}

/*****************************************************************************/

char *GetTopicType(Topic *list, char *topic_name)
{
    Topic *tp;

    for (tp = list; tp != NULL; tp = tp->next)
    {
        if (strcmp(topic_name, tp->topic_name) == 0)
        {
            return tp->topic_context;
        }
    }

    return NULL;
}

/*********************************************************************/

void AppendItem(Item **liststart, char *itemstring)
{
    Item *ip, *lp;
    char *sp, *spe = NULL;

    if ((ip = (Item *) malloc(sizeof(Item))) == NULL)
    {
        perror("malloc");
        exit(1);
    }

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
    }

    ip->name = strdup(itemstring);
    ip->next = NULL;
}

/*********************************************************************/
/* TOOLKIT : String                                                  */
/*********************************************************************/

char ToLower(char ch)
{
 if (isdigit((int) ch) || ispunct((int) ch) || isspace((int)ch))
    {
        return (ch);
    }

    if (islower((int) ch))
    {
        return (ch);
    }
    else
    {
        return (ch - 'A' + 'a');
    }
}

/*********************************************************************/

char ToUpper(char ch)
{
    if (isdigit((int) ch) || ispunct((int) ch) || isspace((int)ch))
    {
        return (ch);
    }

    if (isupper((int) ch))
    {
        return (ch);
    }
    else
    {
        return (ch - 'a' + 'A');
    }
}

/*********************************************************************/

char *ToUpperStr(char *str)
{
    static char buffer[CF_BUFSIZE];
    int i;

    memset(buffer, 0, CF_BUFSIZE);

    if (strlen(str) >= CF_BUFSIZE)
    {
        char *tmp;

        tmp = malloc(40 + strlen(str));
        exit(1);
    }

    for (i = 0; (str[i] != '\0') && (i < CF_BUFSIZE - 1); i++)
    {
        buffer[i] = ToUpper(str[i]);
    }

    buffer[i] = '\0';

    return buffer;
}

/*********************************************************************/

char *ToLowerStr(char *str)
{
    static char buffer[CF_BUFSIZE];
    int i;

    memset(buffer, 0, CF_BUFSIZE);

    if (strlen(str) >= CF_BUFSIZE - 1)
    {
        char *tmp;

        tmp = malloc(40 + strlen(str));
        printf("String too long in ToLowerStr: %s", str);
        exit(1);
    }

    for (i = 0; (str[i] != '\0') && (i < CF_BUFSIZE - 1); i++)
    {
        buffer[i] = ToLower(str[i]);
    }

    buffer[i] = '\0';

    return buffer;
}

/*****************************************************************************/

void AddTopicAssociation(TopicAssociation **list, char *fwd_name, char *bwd_name, char *topic_context, char *associates,
                         int verify)
{
    TopicAssociation *ta = NULL, *texist;
    char assoc_type[256];
    Rlist *rp;

    strncpy(assoc_type, CanonifyName(fwd_name), 255);

    if (associates == NULL)
    {
        printf("A topic must have at least one associate in association %s", fwd_name);
        return;
    }

    if ((texist = AssociationExists(*list, fwd_name, bwd_name, verify)) == NULL)
    {
        if ((ta = (TopicAssociation *) malloc(sizeof(TopicAssociation))) == NULL)
        {
            printf("malloc", "Memory failure in AddTopicAssociation");
            exit(1);
        }

        if ((ta->fwd_name = strdup(fwd_name)) == NULL)
        {
            printf("Memory failure in AddTopicAssociation");
            exit(1);
        }

        ta->bwd_name = NULL;

        if (bwd_name && ((ta->bwd_name = strdup(bwd_name)) == NULL))
        {
            exit(1);
        }

        if (assoc_type && (ta->assoc_type = strdup(assoc_type)) == NULL)
        {
            exit(1);
        }

        ta->associates = NULL;
        ta->associate_topic_context = NULL;
        ta->next = *list;
        *list = ta;
    }
    else
    {
        ta = texist;
    }

/* Association now exists, so add new members */

    RlistPrependScalarIdemp(&(ta->associates), associates, 's');
}

/*****************************************************************************/

TopicAssociation *AssociationExists(TopicAssociation *list, char *fwd, char *bwd, int verify)
{
    TopicAssociation *ta;
    int yfwd = false, ybwd = false;
    char l[CF_BUFSIZE], r[CF_BUFSIZE];

    if (fwd == NULL || (fwd && strlen(fwd) == 0))
    {
        exit(1);
    }

    for (ta = list; ta != NULL; ta = ta->next)
    {
        if (strcmp(fwd, ta->fwd_name) == 0)
        {
            yfwd = true;
        }
        else if (fwd)
        {
            strncpy(l, ToLowerStr(fwd), 255);
            strncpy(r, ToLowerStr(ta->fwd_name), 255);
        }

        if (bwd && strcmp(bwd, ta->bwd_name) == 0)
        {
            ybwd = true;
        }
        else if (bwd && ta->bwd_name)
        {
            strncpy(l, ToLowerStr(bwd), 255);
            strncpy(r, ToLowerStr(ta->bwd_name), 255);
        }

        if (ta->bwd_name && strcmp(fwd, ta->bwd_name) == 0)
        {
            return ta;
        }

        if (bwd && strcmp(bwd, ta->fwd_name) == 0)
        {
            return ta;
        }

        if (yfwd && ybwd)
        {
            return ta;
        }

        if (yfwd && !ybwd)
        {
            return ta;
        }

        if (!yfwd && ybwd)
        {
            return ta;
        }
    }

    return NULL;
}

/*******************************************************************/

Rlist *RlistPrependScalarIdemp(Rlist **start, void *item, char type)
{
    char *scalar = strdup((char *) item);

    if (!KeyInRlist(*start, (char *) item))
    {
        return RlistPrepend(start, scalar, type);
    }
    else
    {
        return NULL;
    }
}

/*******************************************************************/

Rlist *KeyInRlist(Rlist *list, char *key)
{
    Rlist *rp;

    for (rp = list; rp != NULL; rp = rp->next)
    {
        if (strcmp((char *) rp->item, key) == 0)
        {
            return rp;
        }
    }

    return NULL;
}

/*******************************************************************/

Rlist *RlistPrepend(Rlist **start, void *item, char type)
   /* heap memory for item must have already been allocated */
{
    Rlist *rp, *lp = *start;
    char *sp = NULL;

    if ((rp = (Rlist *) malloc(sizeof(Rlist))) == NULL)
    {
        exit(1);
    }

    rp->next = *start;
    rp->item = strdup(item);
    rp->type = type;            /* scalar, builtin function */

    rp->state_ptr = NULL;

    *start = rp;

    return rp;
}

/*******************************************************************/

Rlist *RlistFromSplitString(char *string, char sep)
 /* Splits a string containing a separator like "," 
    into a linked list of separate items, supports
    escaping separators, e.g. \, */
{
    Rlist *liststart = NULL;
    char *sp;
    char node[CF_MAXVARSIZE];
    int maxlen = strlen(string);

    if (string == NULL)
    {
        return NULL;
    }

    for (sp = string; *sp != '\0'; sp++)
    {
        if (*sp == '\0' || sp > string + maxlen)
        {
            break;
        }

        memset(node, 0, CF_MAXVARSIZE);

        sp += SubStrnCopyChr(node, sp, CF_MAXVARSIZE, sep);

        RlistPrepend(&liststart, node, CF_SCALAR);
    }

    return liststart;
}

/*********************************************************************/

int SubStrnCopyChr(char *to, char *from, int len, char sep)
{
    char *sp, *sto = to;
    int count = 0;

    memset(to, 0, len);

    if (from == NULL)
    {
        return 0;
    }

    if (from && strlen(from) == 0)
    {
        return 0;
    }

    for (sp = from; *sp != '\0'; sp++)
    {
        if (count > len - 1)
        {
            break;
        }

        if (*sp == '\\' && *(sp + 1) == sep)
        {
            *sto++ = *++sp;
        }
        else if (*sp == sep)
        {
            break;
        }
        else
        {
            *sto++ = *sp;
        }

        count++;
    }

    return count;
}

/*************************************************************************/

int Chop(char *str)             /* remove trailing spaces */
{
    int i;

    if ((str == NULL) || (strlen(str) == 0))
    {
        return;
    }

    for (i = strlen(str) - 1; i >= 0 && isspace((int) str[i]); i--)
    {
        str[i] = '\0';
    }
}
