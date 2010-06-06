
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
if (strlen(footer) > 0)
   {
   fprintf(fp,"%s",footer);
   }

//fprintf(fp,"</body></html>\n");

fprintf(fp,"\n");
}

/* end */
