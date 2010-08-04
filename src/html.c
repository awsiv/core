
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

void Nova_Header(char *title,char *device_type, char *buffer, int bufsize)
{
snprint(buffer,bufsize,

"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html>\n"
"  <head>\n"
"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\" />\n"
"    <meta http-equiv=\"refresh\" CONTENT=\"150\">\n"
"    <title>system_knowledge::mission status</title>\n"
"    <link rel=\"stylesheet\" href=\"/cf_enterprise.css\" type=\"text/css\" media=\"screen\" />\n"
"    <link rel=\"stylesheet\" href=\"hand_/cf_enterprise.css\" type=\"text/css\" media=\"handheld\" />\n"
"  </head>\n"
"  <body>\n"
"    <div id=\"top\">\n"
"      <div id=\"search\">\n"
"        <form method=\"post\" action=\"/index.php\">\n"
"          <p><input class=\"searchfield\" type=\"text\" name=\"search_string\" />&nbsp;Search</p>\n"
"        </form>\n"
"      </div> \n"
"          <a href=\"/index.php\"><h2>CFENGINE MISSION PORTAL <span id=\"subheading\"><b>overview</b></span></h2></a>\n"
"    </div>\n"
"<div id=\"primary\">");
}

/*****************************************************************************/

void Nova_Footer(char *buffer, int bufsize)
{
 snprint(buffer,bufsize,
         "</div>"
         "<div id=\"footer\">Copyright &copy; Cfengine AS</div></div>"
         "</body>"
         "</html>");
}

/*****************************************************************************/

void Nova_SpecialQuote(char *name,char *type)

{ char filename[CF_BUFSIZE],rootpath[CF_BUFSIZE];
  FILE *fin,*fout = stdout;
  struct Item *ip,*file = NULL;
  int have_title = false;

snprintf(filename,CF_BUFSIZE,"%s/%s",DOCROOT,name);

if ((fin = fopen(filename,"r")) == NULL)
   {
   fprintf(fout,"Nova Unable to open the local file fragment %s",filename);
   }
else
   {
   char line[CF_BUFSIZE],buffer[CF_BUFSIZE];

   while (!feof(fin))
      {
      line[0] = '\0';
      fgets(line,CF_BUFSIZE,fin);

      if (IsHtmlHeader(line))
         {
         continue;
         }

      if (strstr(line,"<h1>"))
         {
         have_title = true;
         }
      
      snprintf(buffer,CF_BUFSIZE-1,line,WEBDRIVER);
      AppendItem(&file,buffer,NULL);
      }
   
   fclose(fin);

   if (!have_title)
      {
      CfHtmlTitle(fout,name);
      }

   for (ip = file; ip != NULL; ip=ip->next)
      {      
      fprintf(fout,"%s",ip->name);
      }
   
   DeleteItemList(file);
   }
}

/*****************************************************************************/

void Nova_IncludeFile(char *name,char *buffer,int bufsize)

{ FILE *fin;
  char buf[CF_BUFSIZE];

if ((fin = fopen(name,"r")) == NULL)
   {
   CfOut(cf_error,"fopen","Can't open %s for reading\n",name);
   return;
   }

while(!feof(fin))
   {
   buf[0] = '\0';
   fgets(buf,CF_BUFSIZE-1,fin);
   Join(buffer,buf,CF_BUFSIZE);
   }

fclose(fin);
}


/* end */
