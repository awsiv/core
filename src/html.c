
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: html.c                                                              */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

void NovaHtmlHeader(FILE *fp,char *title,char *css,char *webdriver,char *header)

{
fprintf(fp,"<html>"
        "  <head>\n"
        "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\" />\n"
        "    <META HTTP-EQUIV=\"refresh\" CONTENT=\"300\">\n"
        "    <title>\n"
        "      %s\n"
        "    </title>\n"
        "    <link rel=\"stylesheet\" href=\"%s\" type=\"text/css\" media=\"screen\" />\n"
        "  </head>\n"
        "  <body>\n\n",title,css);

if (cf_strlen(header) > 0)
   {
   fprintf(fp,"%s\n",header);
   }

fprintf(fp,"<h1>%s</h1>\n",title);
fprintf(fp,"<div id=\"primary\">\n");
}

/*****************************************************************************/

void NovaHtmlFooter(FILE *fp,char *footer)

{
 /* This section should be conditional */

 if (strlen(footer) > 0)
    {
    fprintf(fp,"%s",footer);
    }
/* end */

fprintf(fp,"</div></body></html>\n");
}
