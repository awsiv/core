
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

"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"   \"http://www.w3.org/TR/html4/loose.dtd\"><html>\n"
"  <head>\n"
"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n"
"    %s"
"    <title>%s</title>\n"
"    <link rel=\"stylesheet\" href=\"%s\" type=\"text/css\" media=\"screen\">\n"
"  </head>\n"
"  <body>\n"
"    <div id=\"top\">\n"
"      <div id=\"search\">\n"
"        <form method=\"post\" action=\"%s\">\n"
"          <p><input class=\"searchfield\" type=\"text\" name=\"search_string\">&nbsp;Search</p>\n"
"        </form>\n"
"      </div> \n"
"        <h2>CFENGINE MISSION PORTAL <span id=\"subheading\"><b>%s</b></span></h2>\n"
         "    </div>\n",reload,title,css,CF_KMAP,title);
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
         "<li><a href=\"index.php\"><b>Mission summary</b><br></a></li>"
         "<li><a href=\"helm.php\"><b>Mission planning</b></a></li>"
         "<li><a href=\"status.php\"><b>Mission status</b></a></li>"
//         "<li><a href=\"views.php\"><b>Mission views</b></a></li>"
//         "<li><a href=\"communications.php\"><b>Mission log</b></a>"
         "<li><a href=\"knowledge.php\"><b>Mission library</b></a></li>"
         "</ul>"
         "<a href=\"#\" onClick=\"history.go(-1)\"><img src=\"/img/icon_back.png\"></a>"
         "<a href=\"/index.php\"><img src=\"/img/icon_home.png\"></a>"
         "<a href=\"#\" onClick=\"history.go(+1)\"><img src=\"/img/icon_fwd.png\"></a>"
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
