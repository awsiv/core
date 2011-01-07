
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


  if(BEGINSWITH(command,"/cf_internal_rpath_add"))
    {
    packName = command + 23;  // pack name comes after command
    snprintf(line,sizeof(line),"install %s",packName);
    result = AppendIfNoSuchLine(RPATH_FILE,line);
    }
  else if(BEGINSWITH(command,"/cf_internal_rpath_delete"))
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


