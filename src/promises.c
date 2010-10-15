
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

void Nova_EnterpriseDiscovery()
{
if (IsDefinedClass("redhat"))
   {
   Nova_SetDocRoot("/var/www/html");
   }

if (IsDefinedClass("SuSE"))
   {
   Nova_SetDocRoot("/srv/www/htdocs");
   }

if (IsDefinedClass("debian"))
   {
   Nova_SetDocRoot("/var/www");
   }
}

/*****************************************************************************/

void Nova_PreSanitizePromise(struct Promise *pp)

{ struct Attributes a = {0};

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
char vStr[CF_SMALLBUF];

if(INFORM || VERBOSE)
  {
  snprintf(vStr,sizeof(vStr),"%s (%s)",VERSION,NOVA_REVISION);
  }
else
  {
  snprintf(vStr,sizeof(vStr),"%s",VERSION);
  }

printf("This core uses commercial Nova extensions at version %s Copyright (C) Cfengine AS 2009-%s\n",vStr,VYEAR);

}

/*****************************************************************************/

char *Nova_GetVersion()

{
return VERSION;
}

/*****************************************************************************/

char *Nova_StrVersion()

{ static char buf[64];

snprintf(buf,63,"Contains Nova extensions at version %s\n",VERSION);
return buf;
}

/***************************************************************/

void Nova_NotePromiseCompliance(struct Promise *pp,double val,enum cf_status status,char *reason)

{ CF_DB *dbp;
  FILE *fp;
  char name[CF_BUFSIZE],date[CF_MAXVARSIZE],id[CF_MAXVARSIZE];
  static char previous[CF_BUFSIZE];
  struct Event e,newe;
  time_t now = time(NULL);
  double lastseen,delta2;
  double vstatus;      /* end with a rough probability */

Debug("Note Promise Compliance\n");

cf_strncpy(id,Nova_PromiseID(pp),CF_MAXVARSIZE);

if (strcmp(previous,id) == 0)
   {
   return;
   }

cf_strncpy(previous,Nova_PromiseID(pp),CF_MAXVARSIZE);

snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_COMPLIANCE);
MapName(name);

switch (status)
   {
   case cfn_kept:
   case cfn_nop:
       vstatus = 1.0;
       break;

   case cfn_repaired:
       vstatus = 0.5;
       break;

   case cfn_notkept:
       vstatus = 0;
       break;
   default:
       return;
   }

if (!OpenDB(name,&dbp))
   {
   return;
   }

/* First record the classes that are in use */

if (ReadDB(dbp,id,&e,sizeof(e)))
   {
   lastseen = now - e.t;
   newe.t = now;
   newe.Q.q = vstatus;
   newe.Q.expect = GAverage(vstatus,e.Q.expect,0.5);
   delta2 = (vstatus - e.Q.expect)*(vstatus - e.Q.expect);
   newe.Q.var = GAverage(delta2,e.Q.var,0.5);
   }
else
   {
   lastseen = 0;
   newe.t = now;
   newe.Q.q = vstatus;
   newe.Q.expect = 0.5*vstatus;  /* With no data it's 50/50 what we can say */
   newe.Q.var = 0.000;
   }

WriteDB(dbp,id,&newe,sizeof(newe));
CloseDB(dbp);

/* Now keep the next log */

switch (status)
   {
   case cfn_repaired:
       snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_REPAIR_LOG);
       break;

   case cfn_notkept:
       snprintf(name,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,CF_NOTKEPT_LOG);
       break;

   default:
       return;
   }

if ((fp = fopen(name,"a")) == NULL)
   {
   CfOut(cf_error,"fopen"," !! Could not open \"%s\"",name);
   return;
   }

fprintf(fp,"%ld,%s,%s\n",(long)now,id,reason);

fclose(fp);
}

/***************************************************************/

time_t Nova_GetPromiseCompliance(struct Promise *pp,double *value,double *average,double *var,time_t *lastseen)

{ CF_DB *dbp;
  char name[CF_MAXVARSIZE];
  struct Event e;
  double lsea = CF_WEEK * 52; /* expire after a year */
  time_t now = time(NULL);

snprintf(name,CF_MAXVARSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_COMPLIANCE);
MapName(name);

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

if (*lastseen > 0 && (now - *lastseen) > lsea)
   {
   CfOut(cf_verbose,"","Promise record \"%s\" expired ... removing\n",name);
   DeleteDB(dbp,name);
   }

CloseDB(dbp);
return *lastseen;
}

/***************************************************************/

void Nova_TrackValue(char *date,double kept,double repaired, double notkept)

{ char month[CF_SMALLBUF],day[CF_SMALLBUF],year[CF_SMALLBUF],key[CF_SMALLBUF],name[CF_BUFSIZE];
  CF_DB *dbp;
  struct promise_value value,new_value;

// Strip out the date resolution so we keep only each day of the year
  
sscanf(date,"%*s %s %s %*s %s",month,day,year);
snprintf(key,CF_SMALLBUF-1,"%s %s %s",day,month,year);

snprintf(name,CF_BUFSIZE-1,"%s/state/%s",CFWORKDIR,NOVA_VALUE);
MapName(name);

if (!OpenDB(name,&dbp))
   {
   return;
   }

if (ReadDB(dbp,key,&value,sizeof(struct promise_value)))
   {
   new_value.kept = value.kept + kept;
   new_value.repaired = value.repaired + repaired;
   new_value.notkept = value.notkept + notkept;
   }
else
   {
   new_value.kept = kept;
   new_value.repaired = repaired;
   new_value.notkept = notkept;
   }

CfOut(cf_verbose,""," -> recording value (%.4lf,%.4lf,%.4lf)",kept,repaired,notkept);
WriteDB(dbp,key,&new_value,sizeof(struct promise_value));
CloseDB(dbp);
}

/*****************************************************************************/

void Nova_LastSawBundle(char *name)

{ char filename[CF_BUFSIZE];
  int lsea = LASTSEENEXPIREAFTER;
  double lastseen,delta2;
  struct QPoint q,newq;
  time_t now = time(NULL);
  CF_DB *dbp;
 
snprintf(filename,CF_BUFSIZE-1,"%s/%s",CFWORKDIR,NOVA_BUNDLE_LOG);
MapName(filename);

if (!OpenDB(filename,&dbp))
   {
   return;
   }

if (ReadDB(dbp,name,&q,sizeof(q)))
   {
   lastseen = (double)now - q.q;
   newq.q = (double)now;                   /* Last seen is now-then */
   newq.expect = GAverage(lastseen,q.expect,0.3);
   delta2 = (lastseen - q.expect)*(lastseen - q.expect);
   newq.var = GAverage(delta2,q.var,0.3);
   }
else
   {
   lastseen = 0.0;
   newq.q = (double)now;
   newq.expect = 0.0;
   newq.var = 0.0;
   }

if (lastseen > (double)lsea)
   {
   CfOut(cf_verbose,""," -> Bundle %s expired\n",name);
   DeleteDB(dbp,name);
   }
else
   {
   CfOut(cf_verbose,""," -> Bundle \"%s\" promise kept at %s\n",name,ctime(&now));
   WriteDB(dbp,name,&newq,sizeof(newq));
   }

CloseDB(dbp);
}

/*****************************************************************************/
/* Bootstrap                                                                 */
/*****************************************************************************/

/*

Bootstrapping is a tricky sequence of fragile events. We need to map shakey/IP
and identify policy hub IP in a special order to bootstrap the license and agents.

During bootstrap:

 - InitGA (generic-agent) loads the public key
 - The verifylicense function sets the policy hub but fails to verify license yet
   as there is no key/IP binding
 - Policy server gets set in workdir/state/am_policy_hub
 - The agents gets run and start this all over again, but this time
   the am_policy_hub is defined and caches the key/IP binding
 - Now the license has a binding, resolves the policy hub's key and succeeds

*/

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

if (!IsPrivileged())
   {
   CfOut(cf_error,""," !! You need root/administrator permissions to boostrap Cfengine");
   FatalError("Insufficient privilege");
   }

if (IsDefinedClass("redhat"))
   {
   Nova_SetDocRoot("/var/www/html");
   }

if (IsDefinedClass("SuSE"))
   {
   Nova_SetDocRoot("/srv/www/htdocs");
   }

if (IsDefinedClass("debian"))
   {
   Nova_SetDocRoot("/var/www");
   }

snprintf(name,CF_BUFSIZE-1,"%s/inputs/failsafe.cf",CFWORKDIR);

if (cfstat(name,&sb) == -1)
   {
   Nova_CreateFailSafe(name);
   repaired = true;
   }

snprintf(name,CF_BUFSIZE-1,"%s/inputs/promises.cf",CFWORKDIR);

if (cfstat(name,&sb) == -1)
   {
   CfOut(cf_cmdout,""," -> No previous policy has been cached on this host");
   }
else
   {
   CfOut(cf_cmdout,""," -> An existing policy was cached on this host in %s/inputs",CFWORKDIR);
   have_policy = true;
   }

if (strlen(POLICY_SERVER) > 0)
   {
   CfOut(cf_cmdout,""," -> Assuming the policy distribution point at: %s:WORKDIR/masterfiles\n",POLICY_SERVER);
   }
else
   {
   if (have_policy)
      {
      CfOut(cf_cmdout,""," -> No policy distribution host was discovered - it might be contained in the existing policy, otherwise this will function autonomously\n");
      }
   else if (repaired)
      {
      CfOut(cf_cmdout,""," -> No policy distribution host was defined - use --policy-server to set one\n",POLICY_SERVER);
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

snprintf(name,sizeof(name),"%s/state/am_policy_hub",CFWORKDIR);
MapName(name);

if (am_appliance)
   {
   NewClass("am_policy_hub");
   printf(" ** This host recognizes itself as a Cfengine Policy Hub, with policy distribution and knowledge base.\n");
   printf(" -> The system is now converging. Full initialisation and self-analysis could take up to 30 minutes\n\n");
   creat(name,0600);
   }
else
   {
   unlink(name);
   printf(" -> Satellite system converging on trajectory\n");
   }

printf("\nNOTICE - this is a commercially licensed version of Cfengine. It is ILLEGAL to install\n");
printf("         the software on systems for which no license has been granted by Cfengine AS.\n");
}

/********************************************************************/

void Nova_SetPolicyServer(char *name)

{ char file[CF_BUFSIZE];
  FILE *fout,*fin;

snprintf(file,CF_BUFSIZE-1,"%s/policy_server.dat",CFWORKDIR);

if (strlen(name) > 0)
   {
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

NewScalar("sys","policy_hub",name,cf_str);
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
"}\n\n"
"body agent control\n"
"{\n"
"skipidentify => \"true\";\n"
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
"   \"$(sys.cf_key)\";\n"
"files:\n"
" !windows::\n"
"  \"$(sys.workdir)/inputs\" \n"
"    handle => \"update_policy\",\n"
"    perms => u_p(\"600\"),\n"
"    copy_from => u_scp(\"$(master_location)\"),\n"
"    depth_search => u_recurse(\"inf\"),\n"
"    action => u_immediate,\n"
"    classes => success(\"got_policy\");\n"
"  \"$(sys.workdir)/bin\" \n"
"    perms => u_p(\"700\"),\n"
"    copy_from => u_scp(\"/usr/local/sbin\"),\n"
"    depth_search => u_recurse(\"inf\"),\n"
"    action => u_immediate;\n"
"\n"
"  \"$(sys.workdir)/bin/cf-twin\"\n"
"         comment => \"Make sure we maintain a clone of the agent for updating\",\n"
"       copy_from => u_cp(\"$(sys.workdir)/bin/cf-agent\"),\n"
"           perms => u_p(\"755\"),\n"
"          action => u_immediate;\n"
"\n"
"  windows::\n"
"  \"$(sys.workdir)\\inputs\" \n"
"    handle => \"windows_update_policy\",\n"
"    copy_from => u_scp(\"/var/cfengine/masterfiles\"),\n"
"    depth_search => u_recurse(\"inf\"),\n"
"    action => u_immediate,\n"
"    classes => success(\"got_policy\");\n\n"

"     \"$(sys.workdir)\\bin-twin\\.\"\n"
"         comment => \"Make sure we maintain a clone of the binaries for updating\",\n"
"       copy_from => u_cp(\"$(sys.workdir)\\bin\\.\"),\n"
"    depth_search => u_recurse(\"1\"),\n"
"          action => u_immediate;\n"

"\n"
"processes:\n"
"!windows::\n"
"\"cf-execd\" restart_class => \"start_exec\";\n"
"policy_host::\n"
"\"cf-serverd\" restart_class => \"start_server\";\n\n"

"commands:\n"
"config.am_policy_hub::\n"
"\"$(sys.cf_promises) -r\";"
"start_exec.!windows::\n"
"\"$(sys.cf_execd)\","
"classes => outcome(\"executor\");\n"
"start_server::\n"
"\"$(sys.cf_serverd)\"\n"
"action => ifwin_bg,\n"
"classes => outcome(\"server\");\n\n"

"services:\n"
"windows::\n"
"\"CfengineNovaExec\"\n"
"   service_policy => \"start\",\n"
"   service_method => bootstart,\n"
"   classes => outcome(\"executor\");\n\n"

"reports:\n"
"  bootstrap_mode.policy_host::\n"
"      \"This host assumes the role of policy distribution host\";\n"
"  bootstrap_mode.!policy_host::\n"
"      \"This autonomous node assumes the role of voluntary client\";\n"
"  got_policy::      \" -> Updated local policy from policy server\";\n"
"  server_ok::      \" -> Started the server - system ready to serve\";\n"
"  executor_ok::      \" -> Started the scheduler - system functional\";\n"
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
"body action u_immediate\n"
"{\n"
"ifelapsed => \"1\";\n"
"}\n"
"############################################\n"
"body action u_background\n"
"{\n"
"background => \"true\";\n"
"}\n"
"############################################\n"
"body depth_search u_recurse(d)\n"
"{\n"
"depth => \"$(d)\";\n"
"}\n"
"############################################\n"
"body service_method bootstart\n"
"{\n"
"service_autostart_policy => \"boot_time\";\n"
"}\n"
"############################################\n"
"body action ifwin_bg\n"
"{\n"
"windows::\n"
"background => \"true\";\n"
"}\n"
"############################################\n"
"body copy_from u_cp(from)\n"
"{\n"
"source      => \"$(from)\";\n"
"compare     => \"digest\";\n"
"}\n"

"\n"
        );

CfOut(cf_cmdout,""," -> No policy failsafe discovered, assume temporary bootstrap vector\n");

fclose(fout);
}

/********************************************************************/
/* Level                                                            */
/********************************************************************/

void Nova_SetDocRoot(char *name)

{ char file[CF_BUFSIZE];
  FILE *fout,*fin;
  struct stat sb;
  enum cfreport level;

if (BOOTSTRAP)
   {
   level = cf_cmdout;
   }
else
   {
   level = cf_verbose;
   }

snprintf(file,CF_BUFSIZE-1,"%s/document_root.dat",CFWORKDIR);

if (cfstat(file,&sb) == -1 && strlen(name) > 0)
   {
   if ((fout = fopen(file,"w")) == NULL)
      {
      CfOut(cf_error,"fopen","Unable to write document root file! (%s)",file);
      return;
      }

   fprintf(fout,"%s",name);
   fclose(fout);
   CfOut(level,""," -> Setting document root for a knowledge base to %s",name);
   strcpy(DOCROOT,name);
   NewScalar("sys","doc_root",DOCROOT,cf_str);
   }
else
   {
   if ((fin = fopen(file,"r")) == NULL)
      {
      }
   else
      {      
      file[0] = 0;
      fscanf(fin,"%255s",file);
      fclose(fin);
      CfOut(level,""," -> Assuming document root for a knowledge base in %s",file);
      strcpy(DOCROOT,name);
      NewScalar("sys","doc_root",DOCROOT,cf_str);
      }
   }
}

