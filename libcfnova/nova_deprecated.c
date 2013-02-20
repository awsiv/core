#include "nova_deprecated.h"

char *EscapeJson(char *s, char *out, int outSz)
{
    char *spt, *spf;
    int i = 0;

    memset(out, 0, outSz);

    for (spf = s, spt = out; (i < outSz - 2) && (*spf != '\0'); spf++, spt++, i++)
    {
        switch (*spf)
        {
        case '\"':
        case '\\':
        case '/':
            *spt++ = '\\';
            *spt = *spf;
            i += 2;
            break;
        case '\n':
            *spt++ = '\\';
            *spt = 'n';
            i += 2;
            break;
        case '\t':
            *spt++ = '\\';
            *spt = 't';
            i += 2;
            break;
        case '\r':
            *spt++ = '\\';
            *spt = 'r';
            i += 2;
            break;
        case '\b':
            *spt++ = '\\';
            *spt = 'b';
            i += 2;
            break;
        case '\f':
            *spt++ = '\\';
            *spt = 'f';
            i += 2;
            break;
        default:
            *spt = *spf;
            i++;
            break;
        }
    }

    return out;
}
