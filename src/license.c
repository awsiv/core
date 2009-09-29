
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

if (THIS_AGENT_TYPE == cf_keygen)
   {
   return false;
   }
  
strcpy(u_day,day);
strcpy(u_month,month);
strcpy(u_year,year);
  
// if license file exists, set the date from that, else use the source coded one

snprintf(name,CF_MAXVARSIZE-1,"%s%clicense.dat",CFWORKDIR,FILE_SEPARATOR);

if ((fp = fopen(name,"r")) != NULL)
   {
   CfOut(cf_verbose,""," -> Reading license expiry from %s",name);
   fscanf(fp,"%15s %x %15s %15s %100s",f_day,&number,f_month,f_year,hash);
   fclose(fp);
   
   // This is the simple password hash to obfuscate license fixing

   snprintf(name,CF_MAXVARSIZE-1,"%s-%o.%s Nova %s",f_month,number,f_day,f_year);
   Nova_HashKey(name,digest);

   if (strcmp(HashPrint(cf_md5,digest),hash) == 0)
      {
      strcpy(u_day,f_day);
      strcpy(u_month,f_month);
      strcpy(u_year,f_year);
      LICENSES = number;
      CfOut(cf_verbose,""," -> Verified license file %s",hash);
      }
   else
      {
      CfOut(cf_verbose,"","Failed to verify license file for this host, using fall-back\n");
      LICENSES = 1;
      return true;
      }
   }
else
   {
   CfOut(cf_inform,""," !! No commercial license file found - falling back on internal expiry\n");
   LICENSES = 1;
   }
  
m_now = Month2Int(VMONTH);
d_now = Str2Int(VDAY);

m_expire = Month2Int(u_month);
d_expire = Str2Int(u_day);

Debug("Y. %s > %s\nM. %s > %s\nD: %s > %s = %d\n",VYEAR,year,VMONTH,month,VDAY,day,cf_strcmp(VDAY,day));
Debug("Y. %s > %s\nM. %d > %d\nD: %d > %d = %d\n",VYEAR,year,m_now,m_expire,d_now,d_expire,cf_strcmp(VDAY,day));

snprintf(EXPIRY,31,"%s %s %s",u_day,u_month,u_year);

if ((cf_strcmp(VYEAR,u_year) >= 0) && (m_now >= m_expire) && (d_now > d_expire))
   {
   CfOut(cf_verbose,""," !! %d licenses expired on %s %s %s -- reverting to Community Edition",LICENSES,u_day,u_month,u_year,VDAY,VMONTH,VYEAR);
   
   return false; // return true if we want to stop everything
   }
else
   {
   CfOut(cf_verbose,""," -> Found %d licenses, expiring on %s %s %s",LICENSES,u_day,u_month,u_year);
   return false;
   }
}

/*****************************************************************************/

void Nova_HashKey(char *buffer,unsigned char digest[EVP_MAX_MD_SIZE+1])

{ EVP_MD_CTX context;
  const EVP_MD *md = NULL;
  char *file_buffer;
  FILE *fp;
  int md_len;
  char fbuf[CF_BUFSIZE];

md = EVP_get_digestbyname("md5");       

if (md == NULL)
   {
   CfOut(cf_error,""," !! Unable to compute a valid hash");
   return;
   }

EVP_DigestInit(&context,md);     
EVP_DigestUpdate(&context,(unsigned char*)buffer,strlen(buffer));

if ((fp = fopen(CFPUBKEYFILE,"r")) == NULL)
   {
   CfOut(cf_error,"","No public key file could be read\n");
   exit(1);
   }

fbuf[0] = '\0';

while (!feof(fp))
   {
   fgets(fbuf,CF_BUFSIZE,fp);
   }

fclose(fp);

EVP_DigestUpdate(&context,(unsigned char*)fbuf,strlen(fbuf));
EVP_DigestFinal(&context,digest,&md_len);
}


