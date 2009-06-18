
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: promises.c                                                          */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

char POLICY_SERVER[CF_BUFSIZE];

/*****************************************************************************/

int Nova_EnterpriseExpiry(char *day,char *month,char *year)

/* This function is a convenience to commerical clients during testing */
    
{ struct stat sb;
  char name[CF_MAXVARSIZE];
  FILE *fp;
  
snprintf(name,CF_MAXVARSIZE,"%s/NEE",CFWORKDIR);
  
if (stat(name,&sb) == -1)
   {
   int m_now,m_expire,d_now,d_expire;
   
   m_now = Month2Int(VMONTH);
   d_now = Str2Int(VDAY);

   m_expire = Month2Int(month);
   d_expire = Str2Int(day);

   Debug("Y. %s > %s\nM. %s > %s\nD: %s > %s = %d\n",VYEAR,year,VMONTH,month,VDAY,day,cf_strcmp(VDAY,day));
   Debug("Y. %s > %s\nM. %d > %d\nD: %d > %d = %d\n",VYEAR,year,m_now,m_expire,d_now,d_expire,cf_strcmp(VDAY,day));

   if ((cf_strcmp(VYEAR,year) >= 0) && (m_now >= m_expire) && (d_now > d_expire))
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
else
   {
   return true;
   }
}

/*****************************************************************************/

void Nova_Version()

{
 printf("Contains Nova extensions at version %s (C) Cfengine AS 2009-%s\n",VERSION,VYEAR);
}

/*****************************************************************************/

char *Nova_StrVersion()

{ static char buf[64];

snprintf(buf,63,"Contains Nova extensions at version %s\n",VERSION);
return buf;
}

/***************************************************************/

void Nova_NotePromiseCompliance(struct Promise *pp,double val)

{ DB *dbp;
  DBC *dbcp;
  char name[CF_BUFSIZE];
  struct Event e,newe;
  time_t now = time(NULL);
  double lastseen,delta2;
  double vtrue = 1.0;      /* end with a rough probability */

Debug("Note Promise Compliance\n");

snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,"promise_compliance.db");

if (!OpenDB(name,&dbp))
   {
   return;
   }

/* First record the classes that are in use */

cf_strncpy(name,Nova_PromiseID(pp),CF_MAXVARSIZE);

if (ReadDB(dbp,name,&e,sizeof(e)))
   {
   lastseen = now - e.t;
   newe.t = now;
   newe.Q.q = vtrue;
   newe.Q.expect = GAverage(vtrue,e.Q.expect,0.5);
   delta2 = (vtrue - e.Q.expect)*(vtrue - e.Q.expect);
   newe.Q.var = GAverage(delta2,e.Q.var,0.5);
   }
else
   {
   lastseen = 0;
   newe.t = now;
   newe.Q.q = 0.5*vtrue;
   newe.Q.expect = 0.5*vtrue;  /* With no data it's 50/50 what we can say */
   newe.Q.var = 0.000;
   }

WriteDB(dbp,name,&newe,sizeof(newe));

dbp->close(dbp,0);
}

/***************************************************************/

time_t Nova_GetPromiseCompliance(struct Promise *pp,double *value,double *average,double *var,time_t *lastseen)

{ DB *dbp;
  DBC *dbcp;
  char name[CF_MAXVARSIZE];
  struct Event e;
  double lsea = CF_WEEK * 52; /* expire after a year */
  
snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,"promise_compliance.db");

if (!OpenDB(name,&dbp))
   {
   return (time_t)0;
   }

cf_strncpy(name,Nova_PromiseID(pp),CF_MAXVARSIZE);

if (ReadDB(dbp,name,&e,sizeof(e)))
   {
   *lastseen = e.t;
   *value = e.Q.q;
   *average = e.Q.expect;
   *var = e.Q.var;
   }
else
   {
   *lastseen = 0;
   *value = 0;
   *average = 0;
   *var = 0;
   }

if (*lastseen > lsea)
   {
   Debug("Promise record %s expired\n",name);
   DeleteDB(dbp,name);
   }

dbp->close(dbp,0);
return *lastseen;
}

/***************************************************************/

void Nova_CheckAutoBootstrap()

{ struct stat sb;
  char name[CF_BUFSIZE];
  FILE *pp;
  int repaired = false;

printf("\nCfengine enterprise level bootstrap mode\n");
printf("--------------------------------------------------------\n");
Version("cfengine");
  
snprintf(name,CF_BUFSIZE-1,"%s/inputs/failsafe.cf",CFWORKDIR);

if (stat(name,&sb) == -1)
   {
   Nova_CreateFailSafe(name);
   repaired = true;
   }

snprintf(name,CF_BUFSIZE-1,"%s/masterfiles/promises.cf",CFWORKDIR);

if (stat(name,&sb) == -1)
   {
   CfOut(cf_error,""," -> This host has not been installed to run as a policy server");
   }
else
   {
   CfOut(cf_error,""," -> A master policy is located on this host in %s/mastefiles",CFWORKDIR);
   }

if (strlen(POLICY_SERVER) > 0)
   {
   CfOut(cf_error,""," -> Assuming the policy distribution point at: %s:/var/cfengine/masterfiles\n",POLICY_SERVER);
   }
else
   {
   if (repaired)
      {
      CfOut(cf_error,""," -> No policy distribution host was set - use --policy-server to set one\n",POLICY_SERVER);      }
   else
      {
      CfOut(cf_error,""," -> No policy distribution host was discovered - assuming it is defined in the existing failsafe\n");
      }
   }

snprintf(name,CF_BUFSIZE-1,"%s/inputs/promises.cf",CFWORKDIR);

if (stat(name,&sb) == -1)
   {
   CfOut(cf_error,""," -> No previous policy has been cached on this host");
   }
else
   {
   CfOut(cf_error,""," -> An existing policy was cached on this host in %s/inputs",CFWORKDIR);
   }

CfOut(cf_error,""," -> Level 2 self-diagnostic available as cf-promises --diagnostic");
CfOut(cf_error,""," -> Accepting default policy trajectory for this host");
CfOut(cf_error,""," -> Attemping to start promised services.");
}

/********************************************************************/

void Nova_SetPolicyServer(char *name)

{ char file[CF_BUFSIZE];
  FILE *fout,*fin;

snprintf(file,CF_BUFSIZE-1,"%s/policy_server.dat",CFWORKDIR);

if (strlen(name) > 0)
   {  
   strncpy(POLICY_SERVER,name,CF_BUFSIZE-1);
   
   if ((fout = fopen(file,"w")) == NULL)
      {
      CfOut(cf_error,"fopen","Unable to write policy server file! (%s)",file);
      return;
      }
   
   fprintf(fout,"%s",name);
   fclose(fout);
   CfOut(cf_error,""," -> Setting the policy distribution point at: %s:/var/cfengine/masterfiles\n",POLICY_SERVER);
   }
else
   {
   if ((fin = fopen(file,"r")) == NULL)
      {
      }
   else
      {
      fscanf(fin,"%s",name);
      fclose(fin);
      }
   }
}

/********************************************************************/

void Nova_CreateFailSafe(char *name)

{ FILE *fout;
 
if ((fout = fopen(name,"w")) == NULL)
   {
   CfOut(cf_error,"fopen","Unable to write failsafe file! (%s)",name);
   }

fprintf(fout,
        "body common control\n"
        "{\n"
        "bundlesequence => { \"update\" };\n"
        "}\n"


        
        "bundle agent update\n"
        "{\n"
        "vars:\n"
        
        " \"master_location\" string => \"$(sys.workdir)/masterfiles\";\n"
        
        " \"policy_server\"   string => readfile(\"$(sys.workdir)/policy_server.dat\",40),\n"
        "                      comment => \"IP address to locate your policy host.\";\n"
        
        "classes:\n"
        
        "  \"policy_host\" or => { \n"
        "                        classmatch(canonify(\"ipv4_$(policy_server)\")),\n"
        "                        classmatch(canonify(\"$(policy_server)\"))\n"
        "                        },\n"
        
        "                   comment => \"Define the ip identity of the policy source host\";\n"
        
        "  \"have_ppkeys\" expression => fileexists(\"$(sys.workdir)/ppkeys/localhost.pub\");\n"
        
        "  \"gotfile\" expression => fileexists(\"$(sys.workdir)/policy_server.dat\");\n"
        
        "commands:\n"
        
        " !have_ppkeys::\n"
        
        "   \"/usr/local/sbin/cf-key\";\n"
        
        "files:\n"
        
        "  \"/var/cfengine/inputs\" \n"
        
        "    handle => \"update_policy\",\n"
        "    perms => u_p(\"600\"),\n"
        "    copy_from => u_scp(\"$(master_location)\"),\n"
        "    depth_search => u_recurse(\"inf\"),\n"
        "    action => immediate;\n"
        
        "  \"/var/cfengine/bin\" \n"
        
        "    perms => u_p(\"700\"),\n"
        "    copy_from => u_scp(\"/usr/local/sbin\"),\n"
        "    depth_search => u_recurse(\"inf\"),\n"
        "    action => immediate;\n"

        "processes:\n"

        "any::\n"
        
        "\"cf-execd\" restart_class => \"start_exec\";\n"
        
        "policy_host::\n"
        
        "\"cf-serverd\" restart_class => \"start_server\";\n"
        
        "commands:\n"
        
        "start_exec::\n"
        "\"$(sys.workdir)/bin/cf-execd\",\n"
        "action => logme(\"executor\"),\n"
        "classes => outcome(\"executor\");\n"
        
        "start_server::\n"
        "\"$(sys.workdir)/bin/cf-serverd\",\n"
        "action => logme(\"server\"),\n"
        "classes => outcome(\"server\");\n"
        
        "reports:\n"
        
        "  bootstrap_mode.policy_host::\n"
        
        "      \"I am the policy host - i.e. with ipv4 address $(policy_server)\";\n"

        "  server_failed::"
        "      \" !! Failed to start the server\";\n"
        "  executor_failed::"
        "      \" !! Failed to start the scheduler cf-execd - please consult an engineer\";\n"
        
        "}\n"

        "############################################\n"

        "body action logme(x)\n"
        "{\n"
        "log_repaired => \"stdout\";\n"
        "log_string => \" -> Attempted to start the $(x)\";\n"
        "}\n"

        "############################################\n"

        "body classes outcome(x)\n"
        "{\n"
        "repair_failed => {\"$(x)_failed\"};\n"
        "}\n"

        "############################################\n"
        
        "body perms u_p(p)\n"
        
        "{\n"
        "mode  => \"$(p)\";\n"
        "}\n"
        
        "#############################################\n"
        
        "body copy_from u_scp(from)\n"
        
        "{\n"
        "source      => \"$(from)\";\n"
        "compare     => \"digest\";\n"
        "trustkey    => \"true\";\n"
        
        "!policy_host.gotfile::\n"
        
        "servers => { \"$(policy_server)\" };\n"
        "}\n"
        
        "############################################\n"
        
        "body action immediate\n"
        "{\n"
        "ifelapsed => \"1\";\n"
        "}\n"
        
        "############################################\n"
        
        "body depth_search u_recurse(d)\n"
        
        "{\n"
        "depth => \"$(d)\";\n"
        "}\n"
        );

CfOut(cf_error,""," -> No failsafe present, attempting a temporary workaround\n");

fclose(fout);
}
