
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



int Nova_GetInstalledPkgsRpath(struct CfPackageItem **pkgList, struct Attributes a, struct Promise *pp)
{
  //  char *rpathFile = "/etc/conary/system-model";
  char *rpathFile = "/home/eystein/tmp/conary-system-model";
  char line[CF_BUFSIZE] = {0};
  FILE *fin;

  Debug("Nova_GetInstalledPkgsRpath()\n");

  if ((fin = cf_popen(a.packages.package_list_command,"r")) == NULL)
    {
      CfOut(cf_error,"cf_popen","Couldn't open the package list with command %s\n",a.packages.package_list_command);
      return false;
    }

  if ((fin = cf_fopen(rpathFile,"r")) == NULL)
    {
    CfOut(cf_error,"cf_fopen","Cannot open the rPath input file -- assuming empty %s",rpathFile);
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

  cf_fclose(fin);
  
  return true;
}


int Nova_ExecPackageCommandRpath(char *command,int verify,int setCmdClasses,struct Attributes a,struct Promise *pp)
{
  // TODO: command string is /cf_internal_rpath_add or
  // /cf_internal_rpath_delete: appendline or deleteline from rpath
  // model file, run conary sync command, return result

  // NOTE: conary sync should only be run once (at last add or remove)

  CfOut(cf_verbose, "", "!! rPath support not yet completed");
  
  return true;

}
