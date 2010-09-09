
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

int Nova_EnterpriseModuleExpiry(char *day,char *month,char *year)
{
return false;
}

/*****************************************************************************/

int Nova_EnterpriseExpiry(char *day,char *month,char *year, char *setcompany)

/* This function is a convenience to commerical clients during testing */
    
{ struct stat sb;
  char name[CF_MAXVARSIZE],hash[CF_MAXVARSIZE],serverkey[CF_MAXVARSIZE],policy_server[CF_MAXVARSIZE];
  char company[CF_BUFSIZE],snumber[CF_SMALLBUF];
  int m_now,m_expire,d_now,d_expire,number = 1,am_policy_server = false;
  char f_day[16],f_month[16],f_year[16];
  char u_day[16],u_month[16],u_year[16];
  unsigned char digest[EVP_MAX_MD_SIZE+1] = {0},serverdig[CF_MAXVARSIZE] = {0};
  FILE *fp;
  RSA * serverrsa;
   
if (THIS_AGENT_TYPE == cf_keygen)
   {
   return false;
   }
  
strcpy(u_day,day);
strcpy(u_month,month);
strcpy(u_year,year);
policy_server[0] = '\0';
company[0] = '\0';

// Verify first whether this host has been bootstrapped

snprintf(name,sizeof(name),"%s%cpolicy_server.dat",CFWORKDIR,FILE_SEPARATOR);

if ((fp = fopen(name,"r")) != NULL)
   {
   fscanf(fp,"%s",policy_server);
   fclose(fp);
   }

if (strlen(policy_server) == 0)
   {
   if (!BOOTSTRAP)
      {
      CfOut(cf_inform,""," !! This host has not been bootstrapped, so a license cannot be verified (file \"%s\" is empty)", name);
      }
   
   LICENSES = 0;
   return false;
   }

// if license file exists, set the date from that, else use the source coded one

snprintf(name,sizeof(name),"%s/inputs/license.dat",CFWORKDIR);
MapName(name);

if (stat(name,&sb) == -1)
   {
   snprintf(name,CF_MAXVARSIZE-1,"%s/masterfiles/license.dat",CFWORKDIR);
   MapName(name);
   }

if ((fp = fopen(name,"r")) != NULL)
   {
   CfOut(cf_verbose,""," -> Reading license expiry from %s",name);
   fscanf(fp,"%15s %x %15s %15s %100s %[^\n]",f_day,&number,f_month,f_year,hash,company);
   fclose(fp);
   
   // This is the simple password hash to obfuscate license fixing
   // Nothing top security here - this is a helper file to track licenses

   if (strlen(company) > 0)
      {
      snprintf(name,sizeof(name),"%s-%o.%s Nova %s %s",f_month,number,f_day,f_year,company);
      }
   else
      {
      snprintf(name,sizeof(name),"%s-%o.%s Nova %s",f_month,number,f_day,f_year);
      }

   // This next step requires a pre-existing binding
   
   IPString2KeyDigest(policy_server,serverdig);

   snprintf(serverkey,sizeof(name),"%s/ppkeys/%s-%s.pub",CFWORKDIR,"root",serverdig);
   CfOut(cf_verbose,""," -> Look for server %s's key file\n",policy_server);

   if (serverrsa = HavePublicKey("root",policy_server,serverdig))
      {
      RSA_free(serverrsa);
      }
   else
      {
      CfOut(cf_verbose,"","Failed to verify license file for this host (%s) as we don't know the policy hub's public ID\n",hash);
      LICENSES = 0;
      return false;
      }

   memset(digest,0,sizeof(digest));
   
   if (Nova_HashKey(CFPUBKEYFILE,name,digest,hash))
      {
      strcpy(u_day,f_day);
      strcpy(u_month,f_month);
      strcpy(u_year,f_year);
      LICENSES = number;
      CfOut(cf_verbose,""," -> Verified license file %s - this is a policy server (%s)",hash,company);
      am_policy_server = true;
      NewClass("am_policy_hub");
      }
   else if (memset(digest,0,sizeof(digest)) && Nova_HashKey(serverkey,name,digest,hash))
      {
      strcpy(u_day,f_day);
      strcpy(u_month,f_month);
      strcpy(u_year,f_year);
      LICENSES = number;
      CfOut(cf_verbose,""," -> Verified license file %s - as a satellite of %s (%s)",hash,policy_server,company);
      }
   else
      {
      CfOut(cf_verbose,"","Failed to verify license file for this host (%s)\n",hash);
      LICENSES = 0;
      return false; // Want to be able to bootstrap
      }
   }
else
   {
   CfOut(cf_inform,""," !! No commercial license file found - falling back on internal expiry\n");
   LICENSES = 1;
   snprintf(company,sizeof(company),"%s",setcompany);
   snprintf(name,sizeof(name),"%s/state/am_policy_hub",CFWORKDIR);
   MapName(name);
   
   if (stat(name,&sb) != -1)
      {
      CfOut(cf_verbose,""," -> This is a policy server %s of %s",POLICY_SERVER,company);
      am_policy_server = true;
      NewClass("am_policy_hub");
      }
   else
      {
      CfOut(cf_verbose,""," -> This system is a satellite of (%s)",POLICY_SERVER);
      am_policy_server = false;
      }
   }

m_now = Month2Int(VMONTH);
d_now = Str2Int(VDAY);

m_expire = Month2Int(u_month);
d_expire = Str2Int(u_day);

Debug("Y. %s > %s\nM. %s > %s\nD: %s > %s = %d\n",VYEAR,year,VMONTH,month,VDAY,day,cf_strcmp(VDAY,day));
Debug("Y. %s > %s\nM. %d > %d\nD: %d > %d = %d\n",VYEAR,year,m_now,m_expire,d_now,d_expire,cf_strcmp(VDAY,day));

snprintf(EXPIRY,sizeof(EXPIRY),"%s %s %s",u_day,u_month,u_year);
strncpy(LICENSE_COMPANY,company,CF_SMALLBUF-1);

Nova_LogLicenseStatus();

NewScalar("sys","license_owner",company,cf_str);
snprintf(snumber,CF_SMALLBUF,"%d",LICENSES);
NewScalar("sys","licenses_granted",snumber,cf_int);

#ifdef HAVE_LIBMONGOC
if (am_policy_server && THIS_AGENT_TYPE == cf_agent)
   {
   CFDB_PutValue("license_owner",company);
   CFDB_PutValue("licenses_granted",snumber);
   CFDB_PutValue("license_expires",EXPIRY);
   }
#endif

if ((cf_strcmp(VYEAR,u_year) > 0) || ((cf_strcmp(VYEAR,u_year) == 0) && (m_now > m_expire))
    || ((cf_strcmp(VYEAR,u_year) == 0) && (m_now == m_expire) && (d_now > d_expire)))
   {
   CfOut(cf_error,""," !! %d licenses expired on %s %s %s -- reverting to Community Edition",LICENSES,u_day,u_month,u_year,VDAY,VMONTH,VYEAR);
   LICENSES = 0;
   return false; // return true if we want to stop everything
   }
else
   {
   CfOut(cf_verbose,""," -> Found %d licenses, expiring on %s %s %s for %s",LICENSES,u_day,u_month,u_year,company);
   return false;
   }
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int Nova_HashKey(char *filename,char *buffer,unsigned char digest[EVP_MAX_MD_SIZE+1],char *hash)

{ EVP_MD_CTX context;
  const EVP_MD *md = NULL;
  char *file_buffer;
  FILE *fp;
  int md_len,result = false;
  char fbuf[CF_BUFSIZE];

md = EVP_get_digestbyname("sha256");       

if (md == NULL)
   {
   CfOut(cf_error,""," !! Unable to compute a valid hash");
   return false;
   }

EVP_DigestInit(&context,md);     
EVP_DigestUpdate(&context,(unsigned char*)buffer,strlen(buffer));

if ((fp = fopen(filename,"r")) == NULL)
   {
   return false;
   }
else
   {
   fbuf[0] = '\0';
   
   while (!feof(fp))
      {
      fgets(fbuf,CF_BUFSIZE,fp);
      EVP_DigestUpdate(&context,(unsigned char*)fbuf,strlen(fbuf));
      }
   
   fclose(fp);

   EVP_DigestFinal(&context,digest,&md_len);
   
// Compare this to the assertions
   
   if (strcmp(HashPrint(cf_sha256,digest),hash) == 0)
      {   
      return true;
      }
   
//   CfOut(cf_verbose,""," Key file %s was not authorized as policy server by sha256\n",filename);
   result = false;
   }

// Support legacy

md = EVP_get_digestbyname("md5");       

if (md == NULL)
   {
   CfOut(cf_error,""," !! Unable to compute a valid hash");
   return false;
   }

EVP_DigestInit(&context,md);     
EVP_DigestUpdate(&context,(unsigned char*)buffer,strlen(buffer));

if ((fp = fopen(filename,"r")) == NULL)
   {
   return false;
   }

fbuf[0] = '\0';

while (!feof(fp))
   {
   fgets(fbuf,CF_BUFSIZE,fp);
   EVP_DigestUpdate(&context,(unsigned char*)fbuf,strlen(fbuf));
   }

fclose(fp);

EVP_DigestFinal(&context,digest,&md_len);

// Compare this to the assertions

if (strcmp(HashPrint(cf_md5,digest),hash) == 0)
   {   
   return true;
   }

//CfOut(cf_verbose,""," Key file %s was not authorized as policy server by md5\n",filename);

return result;
}

/*****************************************************************************/

void Nova_CheckLicensePromise()

{ int licenses = 0;
  char *retval,rettype;

if (GetVariable("control_common",CFG_CONTROLBODY[cfg_licenses].lval,(void *)&retval,&rettype) != cf_notype)
   {   
   licenses = Str2Int(retval);
   CfOut(cf_verbose,""," -> %d paid licenses have been purchased (this is a promise by you)",licenses);
   NewScalar("sys","licenses_promised",retval,cf_int);
#ifdef HAVE_LIBMONGOC
   if (THIS_AGENT_TYPE == cf_agent)
      {
      CFDB_PutValue("licenses_promised",retval);
      }
#endif
   }

if (licenses == 0)
   {
   if (!BOOTSTRAP && getuid() == 0 && (THIS_AGENT_TYPE != cf_know) && (THIS_AGENT_TYPE != cf_keygen))
      {
      CfOut(cf_error,""," !! Your configuration promises no host_licenses_paid in common control");
      CfOut(cf_error,""," !! By doing this, you confirm the terms of contract already legally binding");
      }
   }
else if (licenses > LICENSES && THIS_AGENT_TYPE != cf_know)
   {
   CfOut(cf_inform,""," !! You have promised that %d licenses have been paid for, but Cfengine has only promised to honour %d in the agreement. ",licenses,LICENSES);
   CfOut(cf_inform,""," !! You could be in violation of contract.");
   }
else if (licenses < LICENSES)
   {
   CfOut(cf_inform,""," -> According to you only %d licenses have been paid for. Cfengine has promised to honour %d in the agreement.",licenses,LICENSES);
   }
}

/*****************************************************************************/

void Nova_LogLicenseStatus()

{ CF_DB *dbp;
  CF_DBC *dbcp;
  char rettype,datestr[CF_MAXVARSIZE],data[CF_MAXVARSIZE],name[CF_BUFSIZE];
  char buffer[CF_BUFSIZE] = {0},work[CF_BUFSIZE] = {0};
  void *retval;
  int licenses = 0,count = 0;
  struct Promise *pp = NewPromise("track_license","License tracker");
  struct Attributes dummyattr;
  struct CfLock thislock;
  struct QPoint entry;
  struct Rlist *counter = NULL;
  int min = 9999999,max = -1,lic1,lic2,i = 0;
  time_t now = time(NULL),dt,then;
  double average,granted,sum_t = 0,ex_t = 0,lic_t = 0;
  int ksize,vsize;
  char *key;
  void *value;
  long ltime;
  
dummyattr.transaction.ifelapsed = 10080; // 1 week
dummyattr.transaction.expireafter = 10180; // 1 week

if (!IsDefinedClass("am_policy_hub"))
   {
   return;
   }

thislock = AcquireLock("license_track",VUQNAME,CFSTARTTIME,dummyattr,pp);

if (thislock.lock == NULL)
   {
   DeletePromise(pp);
   return;
   }

if (GetVariable("control_common",CFG_CONTROLBODY[cfg_licenses].lval,(void *)&retval,&rettype) != cf_notype)
   {   
   licenses = Str2Int(retval);
   }

snprintf(name,CF_MAXVARSIZE-1,"%s%c%s",CFWORKDIR,FILE_SEPARATOR,CF_LASTDB_FILE);

if (OpenDB(name,&dbp))
   {
   memset(&entry,0,sizeof(entry)); 
      
   if (NewDBCursor(dbp,&dbcp))
      {
      while(NextDB(dbp,dbcp,&key,&ksize,&value,&vsize))
         {
         if (value == NULL)
            {
            continue;
            }

         IdempPrependRScalar(&counter,key+1,CF_SCALAR);
         }

      DeleteDBCursor(dbp,dbcp);
      }

   CloseDB(dbp);
   }

count = RlistLen(counter);
DeleteRlist(counter);

if (count == 0)
   {
   count = 1;
   }

CfOut(cf_verbose,""," -> Detected current number of used licenses at approximately %d/%d\n",count,LICENSES);

snprintf(name,CF_MAXVARSIZE-1,"%s%cstate%c%s",CFWORKDIR,FILE_SEPARATOR,FILE_SEPARATOR,NOVA_LICENSE);
MapName(name);

if (!OpenDB(name,&dbp))
   {
   DeletePromise(pp);
   YieldCurrentLock(thislock);
   return;
   }

snprintf(datestr,CF_MAXVARSIZE-1,"%s",cf_ctime(&now));
snprintf(data,CF_MAXVARSIZE-1,"%d,%d,%d,%ld",count,LICENSES,licenses,(long)now);

Chop(datestr);
WriteDB(dbp,datestr,data,sizeof(data));

/* Cache the license report */

// Calculate utilization in each dt of the record

then = time(NULL); // Set this to now for first round

if (NewDBCursor(dbp,&dbcp))
   {
   while(NextDB(dbp,dbcp,&key,&ksize,&value,&vsize))
      {
      if (value == NULL)
         {
         continue;
         }
      
      count = lic1 = lic2 = 0;
      ltime = 0L;
      
      sscanf(value,"%d,%d,%d,%ld",&count,&lic1,&lic2,&ltime);
      
      i++;
      now = (time_t)ltime;
      
      if (count > max)
         {
         max = count;
         }
      
      if (count < min)
         {
         min = count;
         }
      
      dt = now - then;
      
      if (dt > 0)
         {
         ex_t += (double)dt*count;
         lic_t += (double)dt*lic1;
         sum_t += (double)dt;
         }
      
      if (then > 0)
         {
         then = now;
         }
      else
         {
         then = time(NULL);
         }
      }
   
   DeleteDBCursor(dbp,dbcp);
   }

CloseDB(dbp);
   
snprintf(buffer,sizeof(buffer),"<div id=\"license\">");
snprintf(work,sizeof(work),"<h4>Last measured on %s based on %d samples</h4>",ctime(&now),i);
Join(buffer,work,sizeof(buffer));
snprintf(work,sizeof(work),"<table class=\"border\">\n");
Join(buffer,work,sizeof(buffer));

if (sum_t > 0)
   {
   average = ex_t/sum_t;
   granted = lic_t/sum_t;
   snprintf(work,sizeof(work),"<tr><td>Minimum observed level</td><td> &ge; %d</td><tr>\n",min);
   Join(buffer,work,sizeof(buffer));
   snprintf(work,sizeof(work),"<tr><td>Maximum observed level</td><td> &ge; %d hosts</td><tr>\n",max);
   Join(buffer,work,sizeof(buffer));
   snprintf(work,sizeof(work),"<tr><td>Mean actual usage</td><td> &ge; %lf</td></tr>\n",average);
   Join(buffer,work,sizeof(buffer));
   snprintf(work,sizeof(work),"<tr><td>Mean expected usage</td><td> &le; %lf</td><tr>\n",granted);
   Join(buffer,work,sizeof(buffer));
   snprintf(work,sizeof(work),"<tr><td>Mean utilization</td><td> &le; %lf %%</td><tr>\n",average/granted*100.0);
   Join(buffer,work,sizeof(buffer));
   }
else
   {
   snprintf(work,sizeof(work),"<tr><td>Minimum observed level</td><td> &ge; %d</td><tr>\n",min);
   Join(buffer,work,sizeof(buffer));
   snprintf(work,sizeof(work),"<tr><td>Maximum observed level</td><td> &ge; %d hosts</td><tr>\n",max);
   Join(buffer,work,sizeof(buffer));
   snprintf(work,sizeof(work),"<tr><td>Mean usage</td><td> unknown</td><tr>\n");
   Join(buffer,work,sizeof(buffer));
   }

snprintf(work,sizeof(work),"</table></div>\n");
Join(buffer,work,sizeof(buffer));

#ifdef HAVE_LIBMONGOC
CFDB_PutValue("license_report",buffer);
#endif
YieldCurrentLock(thislock);
DeletePromise(pp);
}

/*****************************************************************************/

int Nova_CheckLicenseWin(char *pos)

{
if (LICENSES == 0)
   {
   CfOut(cf_error, "", " !! Invalid Enterprise license limits functionality (%s requires a license, agent %s)", pos, THIS_AGENT);
   }

return true;
}
