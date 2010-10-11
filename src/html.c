
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

{ char *stylesheet_normal = "nova.css";
  char *stylesheet_phone = "nova_hand.css";
  char *css,*reload;
  
if (strstr(device_type,"mobile"))
   {
   css = stylesheet_phone;
   }
else
   {
   css = stylesheet_normal;
   }

if (strstr(device_type,"reload"))
   {
   reload = "    <meta http-equiv=\"refresh\" CONTENT=\"150\">\n";
   }
else
   {
   reload ="";
   }

snprintf(buffer,bufsize,


"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
"<head>\n"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n"
"<title>Cfengine Mission Portal - %s</title>\n"
"<link href=\"css/resets.css\" rel=\"stylesheet\" media=\"screen\" />\n"
"<link href=\"css/960.css\" rel=\"stylesheet\" media=\"screen\" />\n"
"<link href=\"css/style.css\" rel=\"stylesheet\" media=\"screen\" />\n"
"<script src=\"scripts/jquery-1.4.2.min.js\" type=\"text/javascript\"></script>\n"
"<script src=\"scripts/jquery-ui-1.8.2.custom.min.js\" type=\"text/javascript\"></script>\n"
"<script src=\"scripts/jquery.tablesorter.min.js\" type=\"text/javascript\"></script>\n"
"<script src=\"scripts/picnet.jquery.tablefilter.js\" type=\"text/javascript\"></script>\n"
"<script src=\"scripts/jquery.tablesorter.pager.js\" type=\"text/javascript\"></script>\n"
"</head>\n"
"\n"
"<body>\n"
"<div id=\"wrapper\">\n"
"   <div class=\"container_12\">\n"
"    	<div id=\"header\">\n"
"            <div class=\"grid_6 sitetitle\">CFENGINE MISSION PORTAL <span id=\"subheading\"><b>%s</b></span></div>\n"
"\n"
"  <div id=\"searchbox\" class=\"grid_4\"><form action=\"knowledge.php\" method=\"post\"><input type=\"text\" name=\"search\" class=\"align\"/><input type=\"submit\" class=\"searchbtn\" value=\"\"/></form></div>\n"
"            <div id=\"historynav\" class=\"grid_2 alignright\">\n"
"                <a href=\"javascript:history.go(-1)\"><img src=\"images/icon_arrow_back.png\"/></a>\n"
"                <a href=\"index.php\"><img src=\"images/home.png\"/></a>\n"
"                <a href=\"javascript:history.go(+1)\"><img src=\"images/icon_arrow_forward.png\"/></a>\n"
"            </div>\n"
"            <div class=\"clear\"></div>\n"
"        </div>\n", title, title);

}

/*****************************************************************************/

void Nova_Footer(char *buffer, int bufsize)
{
snprintf(buffer,bufsize,
         "<div id=\"footer\">Copyright &copy; Cfengine AS</div>\n"
         "</body>"
         "</html>");
}

/*****************************************************************************/

void Nova_TabMenu(char *buffer, int bufsize)
{
snprintf(buffer,bufsize,
         "<div id=\"banner\">"
         "<ul>"
         "<li><a href=\"index.php\">Mission summary<br></a></li>"
         "<li><a href=\"helm.php\">Mission planning</a></li>"
         "<li><a href=\"status.php\">Mission status</a></li>"
//         "<li><a href=\"views.php\">Mission views</a></li>"
//         "<li><a href=\"communications.php\">Mission log</a>"
         "<li><a href=\"knowledge.php\">Mission library</a></li>"
         "</ul>"
         "<a href=\"/index.php\"><img src=\"/img/icon_home.png\"></a>"
         "<a href=\"javascript:history.go(+1)\"><img src=\"/img/icon_fwd.png\"></a>"
         "<a href=\"javascript:history.go(-1)\"><img src=\"/img/icon_back.png\"></a>"
         "</div><br>");
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
