
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
"\n       <div id=\"ftsummary\">\n"
"         <ul class=\"grid_2\">\n"
"             <li><a href=\"index.php\">SUMMARY</a></li>\n"
"             <li><a href=\"hosts.php?type=green\">Green hosts</a></li>\n"
"             <li><a href=\"hosts.php?type=yellow\">Yellow hosts</a></li>\n"
"             <li><a href=\"hosts.php?type=red\">Red hosts</a></li>\n"
"             <li><a href=\"license.php\">License summary</a></li>\n"
"         </ul>\n"
"         <ul class=\"grid_2\">\n"
"             <li><a href=\"helm.php\">PLANNING</a></li>\n"
"             <li><a href=\"services.php\">Enterprises goals</a></li>\n"
"             <li><a href=\"Cfeditor.php\">Develop and work on policy</a></li>\n"
"             <li><a href=\"knowledge.php?topic=security\">Security</a></li>\n"
"             <li><a href=\"search.php?report=Business%20value%20report&manyhosts=1\">Cost and value</a></li>\n"
"             <li><a href=\"knowledge.php?topic=best.*practice\">Standards and practices</a></li>\n"
"             <li><a href=\"knowledge.php?topic=policy.*\">Policy guidance</a></li>\n"
"         </ul>\n"
"         <ul class=\"grid_2\">\n"
"            <li> <a href=\"status.php\">STATUS</a></li>\n"
"            <li><a href=\"topN.php\">50 worst hosts</a></li>\n"
"            <li><a href=\"host.php\">Select a particular host</a></li>\n"
"         </ul>\n"
"         <ul class=\"grid_2\">\n"
"             <li><a href=\"knowledge.php\">KNOWLEDGE</a></li>\n"
"             <li><a href=\"knowledge.php?topic=copernicus knowledge\">Copernicus</a></li>\n"
"             <li><a href=\"knowledge.php?topic=software components\">Cfengine components</a></span></li>\n"
"             <li><a href=\"knowledge.php?topic=example\">Examples</a></li>\n"
"             <li><a href=\"http://demo.cfengine.com/docs/cf3-reference.html\">User manual</a></li>\n"
"             <li><a href=\"knowledge.php?topic=promise theory\">Promise theory</a></li>\n"
"         </ul>\n"
"         <div class=\"grid_4\">\n"
"         <br />\n"
"         <br/>\n"
"         <br />\n"
"         <br />\n"
"         <br />\n"
"         <br />\n"
"         <br/>\n"
"         <br />\n"
"         <br />\n"
"         <br />\n"
"         <span class=\"grid_4 alignright\"><img src=\"images/Cfengine_logo_white_trans.png\" width=\"74px\" height=\"48px\"/></span>\n"
"         </div>\n"
"         <div class=\"clear\"></div>\n"
"       </div>\n"
"       <div id=\"footer\">\n"
"           <div class=\"grid_6 alignleft\">\n"
"              <span class=\"grid_6\">This hub runs Cfengine Nova %s (based on community core %s)</span>\n"
"           </div>\n"
"           <div class=\"grid_6 alignright\">\n"
"            <span class=\"grid_6\">Copyright &copy; 2010 Cfengine - All rights reserved</span>\n"
"           </div>\n"
"       <div class=\"clear\"></div>\n"
"       </div>\n"
"   </div>\n"
"</div>\n"
"</body>\n"
"</html>\n",Nova_GetVersion(),Cf_GetVersion());
}

/*****************************************************************************/

void Nova_TabMenu(char *title,char *buffer, int bufsize)

{ char *class = "class=\"current\"";
  char *css[4];
 
if (strstr(title,"Status")||strstr(title,"Show"))
   {
   css[0] = "";
   css[1] = "";
   css[2] = class;
   css[3] = "";
   }
else if (strstr(title,"Planning"))
   {
   css[0] = "";
   css[1] = class;
   css[2] = "";
   css[3] = "";
   }
else if (strstr(title,"Knowledge"))
   {
   css[0] = "";
   css[1] = "";
   css[2] = "";
   css[3] = class;
   }
else
   {
   css[0] = class;
   css[1] = "";
   css[2] = "";
   css[3] = "";
   }

snprintf(buffer,bufsize,
" <div id=\"nav\">\n"
"   <ul class=\"grid_10\">\n"
"      <li><a href=\"index.php\" %s>SUMMARY</a></li>\n"
"      <li><a href=\"helm.php\" %s>PLANNING</a></li>\n"
"      <li><a href=\"status.php\" %s>STATUS</a></li>\n"
"      <li><a href=\"knowledge.php\" %s>KNOWLEDGE</a></li>\n"
"   </ul>\n"
"   <span id=\"status\" class=\"grid_2 alignright\">\n"
"      %s\n"
"   </span>\n"
"   <div class=\"clearleft\"></div>\n"
         " </div>\n",css[0],css[1],css[2],css[3],title);
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
