
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: packages.c                                                          */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

//  char *rpathFile = "/etc/conary/system-model";
char *RPATH_FILE = "/home/eystein/tmp/conary-system-model";


int AppendIfNoSuchLine(char *filename, char *line);


int Nova_GetInstalledPkgsRpath(struct CfPackageItem **pkgList, struct Attributes a, struct Promise *pp)
{
  char line[CF_BUFSIZE] = {0};
  FILE *fin;

  Debug("Nova_GetInstalledPkgsRpath()\n");

  if ((fin = cf_popen(a.packages.package_list_command,"r")) == NULL)
    {
      CfOut(cf_error,"cf_popen","Couldn't open the package list with command %s\n",a.packages.package_list_command);
      return false;
    }

  if ((fin = fopen(RPATH_FILE,"r")) == NULL)
    {
    CfOut(cf_error,"fopen","Cannot open the rPath input file -- assuming empty %s",RPATH_FILE);
    return true;
    }
  
  while (!feof(fin))
    {
      line[0] = '\0';
      CfReadLine(line,sizeof(line),fin);
      
      if(FullTextMatch(a.packages.package_installed_regex,line))
	{
	Debug("READPKGINST(rpath)=%s\n",line);
	PrependListPackageItem(pkgList,line,a,pp);
	}
    }

  fclose(fin);
  
  return true;
}


int Nova_ExecPackageCommandRpath(char *command,int verify,int setCmdClasses,struct Attributes a,struct Promise *pp)
{
  // TODO: command string is /cf_internal_rpath_add or
  // /cf_internal_rpath_delete: appendline or deleteline from rpath
  // model file, run conary sync command, return result, params

  // NOTE: conary sync should only be run once (at last add or remove)
  // Already found to be out of compliance, can just add/remove
  
  int result = false;
  char *packName;
  char line[CF_MAXVARSIZE];


  if(strcmp(command,"/cf_internal_rpath_add") == 0)
    {
    packName = command + 23;
    snprintf(line,sizeof(line),"install %s",packName);
    result = AppendIfNoSuchLine(RPATH_FILE,line);
    }
  else if(strcmp(command,"/cf_internal_rpath_delete") == 0)
    {
    
    result = true;
    }
  else
    {
    cfPS(cf_error,CF_FAIL,"",pp,a,"!! The package command \"%s\" is not recognised as a Cfengine internal rPath command",command);
    result = false;
    }
  
  return result;

}


int AppendIfNoSuchLine(char *filename, char *line)
/* Appends line to the file with path filename if it is not already
   there. line should not contain newline.
   Returns true if the line is there on exit, false on error. */
{
  FILE *fread,*fappend;
  char lineCp[CF_MAXVARSIZE], lineBuf[CF_MAXVARSIZE];
  int lineExists = false;
  int result = false;
  size_t written = 0;
  
  if ((fread = fopen(filename,"rw")) == NULL)
   {
   CfOut(cf_error,"fopen","!! Cannot open the file \"%s\" for read", filename);
   return false;
   }
  
  while(CfReadLine(lineBuf,sizeof(lineBuf),fread))  // strips newlines automatically
    {
    if(strcmp(line,lineBuf) == 0)
      {
      lineExists = true;
      result = true;
      break;
      }
    }

  fclose(fread);


  if(!lineExists)
    // we are at EOF and line does not exist already
    {
      if((fappend = fopen(filename,"a")) == NULL)
	{
	CfOut(cf_error,"fopen","!! Cannot open the file \"%s\" for append", filename);
	return false;
	}
      
      if(line[strlen(line) - 1] == '\n')
	{
	snprintf(lineCp,sizeof(lineCp),"%s",line);
	}
      else
	{
	snprintf(lineCp,sizeof(lineCp),"%s\n",line);
	}

      written = fwrite(lineCp, sizeof(char), strlen(lineCp), fappend);
	
      if(written == strlen(lineCp))
	{
	result = true;
	}
      else
	{
	CfOut(cf_error, "fwrite", "!! Could not write %d characters to \"%s\" (wrote %d)", strlen(lineCp), filename, written);
	result = false;
	}
      
      fclose(fappend);
    }
  

  return result;
}
