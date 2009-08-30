
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

#ifdef HAVE_ZONE_H
# include <zone.h>
#endif

char POLICY_SERVER[CF_BUFSIZE];

/*****************************************************************************/

int Nova_EnterpriseExpiry(char *day,char *month,char *year)

/* This function is a convenience to commerical clients during testing */
    
{ struct stat sb;
  char name[CF_MAXVARSIZE];
  FILE *fp;
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

/*****************************************************************************/

void Nova_EnterpriseContext()

{ char vbuff[CF_BUFSIZE];
#ifdef HAVE_GETZONEID
 zoneid_t zid;
 char zone[ZONENAME_MAX];
 
zid = getzoneid();
getzonenamebyid(zid,zone,ZONENAME_MAX);
CfOut(cf_verbose,""," -> Cfengine seems to be running inside a solaris zone of name \"%s\"",zone);

NewScalar("sys","zone",zone,cf_str);
snprintf(vbuff,CF_BUFSIZE-1,"zone_%s",zone);
NewClass(vbuff);
#endif
}

/*****************************************************************************/

void Nova_PreSanitizePromise(struct Promise *pp)

{ struct Attributes a;

 /* Attempt run-time-like analysis here before execution if possible */

if (strcmp("processes",pp->agentsubtype) == 0)
   {
   return;
   }

if (strcmp("storage",pp->agentsubtype) == 0)
   {
   return;
   }

if (strcmp("packages",pp->agentsubtype) == 0)
   {
   return;
   }

if (strcmp("files",pp->agentsubtype) == 0)
   {
   a = GetFilesAttributes(pp);   
   FileSanityChecks(pp->promiser,a,pp);
   return;
   }
  
if (strcmp("commands",pp->agentsubtype) == 0)
   {
   return;
   }

if (strcmp("databases",pp->agentsubtype) == 0)
   {
   return;
   }

if (strcmp("methods",pp->agentsubtype) == 0)
   {
   return;
   }

if (strcmp("reports",pp->agentsubtype) == 0)
   {
   return;
   }
}


/*****************************************************************************/

void Nova_Version()

{
printf("This core uses commercial Nova extensions at version %s (C) Cfengine AS 2009-%s\n",VERSION,VYEAR);
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
  
snprintf(name,CF_MAXVARSIZE-1,"%s/state/%s",CFWORKDIR,"promise_compliance.db");

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
  int repaired = false, have_policy = false, am_appliance = false;

printf("\n ** Initiated the cfengine enterprise diagnostic bootstrap probe\n");
printf(" ** This is a Nova automation extension (self-healing installer)\n\n");

Version("cfengine");
printf("\n");

printf(" -> This host is: %s\n",VSYSNAME.nodename);
printf(" -> Operating System Type is %s\n",VSYSNAME.sysname);
printf(" -> Operating System Release is %s\n",VSYSNAME.release);
printf(" -> Architecture = %s\n",VSYSNAME.machine);
printf(" -> Internal soft-class is %s\n",CLASSTEXT[VSYSTEMHARDCLASS]);

if (IsDefinedClass("redhat"))
   {
   Nova_SetDocRoot("/var/www/html");
   }

if (IsDefinedClass("SuSE"))
   {
   Nova_SetDocRoot("/srv/www/htdocs");
   }

snprintf(name,CF_BUFSIZE-1,"%s/inputs/failsafe.cf",CFWORKDIR);

if (stat(name,&sb) == -1)
   {
   Nova_CreateFailSafe(name);
   repaired = true;
   }

snprintf(name,CF_BUFSIZE-1,"%s/inputs/promises.cf",CFWORKDIR);

if (stat(name,&sb) == -1)
   {
   CfOut(cf_error,""," -> No previous policy has been cached on this host");
   }
else
   {
   CfOut(cf_error,""," -> An existing policy was cached on this host in %s/inputs",CFWORKDIR);
   have_policy = true;
   }

if (strlen(POLICY_SERVER) > 0)
   {
   CfOut(cf_error,""," -> Assuming the policy distribution point at: %s:WORKDIR/masterfiles\n",POLICY_SERVER);
   }
else
   {
   if (have_policy)
      {
      CfOut(cf_error,""," -> No policy distribution host was discovered - it might be contained in the existing policy, otherwise this will function autonomously\n");
      }
   else if (repaired)
      {
      CfOut(cf_error,""," -> No policy distribution host was defined - use --policy-server to set one\n",POLICY_SERVER);
      }
   }

printf(" -> Policy trajectory accepted\n");
printf(" -> Attempting to initiate promised autonomous services\n\n");


am_appliance = IsDefinedClass(CanonifyName(POLICY_SERVER));
snprintf(name,CF_MAXVARSIZE,"ipv4_%s",CanonifyName(POLICY_SERVER));
am_appliance |= IsDefinedClass(name);

if (strlen(POLICY_SERVER) == 0)
   {
   am_appliance = false;
   }

if (am_appliance)
   {
   printf(" ** This host recognizes itself as a Cfengine Appliance, with policy distribution and knowledge base.\n");
   printf(" -> The system is now converging. Full initialisation of self-analysis could take up to 30 minutes\n\n");
   }
else
   {
   printf(" -> System converging on trajectory\n");
   }

printf("WARNING- this is a commercially licensed version of Cfengine. It is ILLEGAL\n");
printf("         to install this only systems for which no license has been purchased.\n");
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
        
        "  \"$(sys.workdir)/inputs\" \n"
        
        "    handle => \"update_policy\",\n"
        "    perms => u_p(\"600\"),\n"
        "    copy_from => u_scp(\"$(master_location)\"),\n"
        "    depth_search => u_recurse(\"inf\"),\n"
        "    action => immediate,\n"
        "    classes => success(\"config\");\n"
        
        "  \"$(sys.workdir)/bin\" \n"
        
        "    perms => u_p(\"700\"),\n"
        "    copy_from => u_scp(\"/usr/local/sbin\"),\n"
        "    depth_search => u_recurse(\"inf\"),\n"
        "    action => immediate;\n"

        "processes:\n"

        "config::\n"
        
        "\"cf-execd\" restart_class => \"start_exec\";\n"
        
        "config.policy_host::\n"
        
        "\"cf-serverd\" restart_class => \"start_server\";\n"
        
        "commands:\n"
        
        "start_exec::\n"
        "\"$(sys.workdir)/bin/cf-execd\",\n"
        "classes => outcome(\"executor\");\n"
        
        "start_server::\n"
        "\"$(sys.workdir)/bin/cf-serverd\",\n"
        "classes => outcome(\"server\");\n"
        
        "reports:\n"
        
        "  bootstrap_mode.policy_host::\n"
        
        "      \"This host assumes the role of policy distribution host\";\n"

        "  bootstrap_mode.!policy_host::\n"
        
        "      \"This autonomous node assumes the role of voluntary client\";\n"

        
        "  server_ok::"
        "      \" -> Started the server - system ready to serve\";\n"
        "  executor_ok::"
        "      \" -> Started the scheduler - system functional\";\n"
        
        "}\n"

        "############################################\n"

        "body classes outcome(x)\n"
        "{\n"
        "promise_repaired => {\"$(x)_ok\"};\n"
        "}\n"

        "############################################\n"

        "body classes success(x)\n"
        "{\n"
        "promise_repaired => {\"$(x)\"};\n"
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

CfOut(cf_error,""," -> No policy failsafe discovered, assume temporary bootstrap vector\n");

fclose(fout);
}


/********************************************************************/
/* Level                                                            */
/********************************************************************/

void Nova_SetDocRoot(char *name)

{ char file[CF_BUFSIZE];
  FILE *fout,*fin;

snprintf(file,CF_BUFSIZE-1,"%s/document_root.dat",CFWORKDIR);

if (strlen(name) > 0)
   {  
   if ((fout = fopen(file,"w")) == NULL)
      {
      CfOut(cf_error,"fopen","Unable to write document root file! (%s)",file);
      return;
      }
   
   fprintf(fout,"%s",name);
   fclose(fout);
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

CfOut(cf_error,""," -> Assuming document root for a knowledge base in %s",name);
}

