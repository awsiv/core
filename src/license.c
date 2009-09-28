
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: license.c                                                           */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

int Nova_EnterpriseExpiry(char *day,char *month,char *year)

/* This function is a convenience to commerical clients during testing */
    
{ struct stat sb;
  char name[CF_MAXVARSIZE],hash[CF_MAXVARSIZE];
  FILE *fp;
  int m_now,m_expire,d_now,d_expire,number = 1;
  char f_day[16],f_month[16],f_year[16];
  char u_day[16],u_month[16],u_year[16];
  unsigned char digest[EVP_MAX_MD_SIZE+1];

strcpy(u_day,day);
strcpy(u_month,month);
strcpy(u_year,year);
  
// if license file exists, set the date from that, else use the source coded one

snprintf(name,CF_MAXVARSIZE-1,"%s/license.dat",CFWORKDIR);

if ((fp = fopen(name,"r")) != NULL)
   {
   CfOut(cf_verbose,""," -> Reading license expiry from %s",name);
   fscanf(fp,"%15s %x %15s %15s %100s",f_day,&number,f_month,f_year,hash);
   fclose(fp);
   
   // This is the simple password hash to obfuscate license fixing

   snprintf(name,CF_MAXVARSIZE-1,"%s-%o.%s Nova %s",f_month,number,f_day,f_year);
   HashString(name,strlen(name),digest,cf_md5);

   if (strcmp(HashPrint(cf_md5,digest),hash) == 0)
      {
      strcpy(u_day,f_day);
      strcpy(u_month,f_month);
      strcpy(u_year,f_year);
      LICENSES = number;
      }
   else
      {
      CfOut(cf_error,"","Failed to verify license file. Please do not tamper with the authorization.\n");
      LICENSES = 0;
      return false;
      }
   }
else
   {
   CfOut(cf_verbose,""," !! No commercial license file found\n");
   }
  
m_now = Month2Int(VMONTH);
d_now = Str2Int(VDAY);

m_expire = Month2Int(u_month);
d_expire = Str2Int(u_day);

Debug("Y. %s > %s\nM. %s > %s\nD: %s > %s = %d\n",VYEAR,year,VMONTH,month,VDAY,day,cf_strcmp(VDAY,day));
Debug("Y. %s > %s\nM. %d > %d\nD: %d > %d = %d\n",VYEAR,year,m_now,m_expire,d_now,d_expire,cf_strcmp(VDAY,day));

snprintf(EXPIRY,31,"%s %s %s",u_day,u_month,u_year);

snprintf(name,CF_MAXVARSIZE-1,"%s/lcs",CFWORKDIR);

if ((cf_strcmp(VYEAR,u_year) >= 0) && (m_now >= m_expire) && (d_now > d_expire))
   {
   if (fp = cf_fopen(name,"w"))
      {
      fprintf(fp,"enable expiry %s %s %s\n",day,month,year);
      cf_fclose(fp);
      }
   return true;
   }

return false;
}

