
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: environments.c                                                      */
/*                                                                           */
/* Created: Mon May 17 10:15:20 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef HAVE_LIBVIRT
extern virConnectPtr CFVC;
#endif

#define CF_MAX_CONCURRENT_ENVIRONMENTS 256

int CF_RUNNING[CF_MAX_CONCURRENT_ENVIRONMENTS];
char *CF_SUSPENDED[CF_MAX_CONCURRENT_ENVIRONMENTS];

/*****************************************************************************/

void Nova_VerifyEnvironmentsPromise(struct Promise *pp)

{ struct Attributes a;
  struct CfLock thislock;

#ifdef HAVE_LIBVIRT
a = GetEnvironmentsAttributes(pp);

if (Nova_EnvironmentsSanityChecks(a,pp))
   {
   thislock = AcquireLock("virtual",VUQNAME,CFSTARTTIME,a,pp);
   
   if (thislock.lock == NULL)
      {
      return;
      }

   CF_NODES++;

   Nova_VerifyEnvironments(a,pp);
   }

YieldCurrentLock(thislock);
#endif
}

/*****************************************************************************/

#ifdef HAVE_LIBVIRT

int Nova_EnvironmentsSanityChecks(struct Attributes a,struct Promise *pp)

{
if (a.env.specfile)
   {
   if (a.env.cpus || a.env.memory || a.env.disk)
      {
      CfOut(cf_error,""," !! Conflicting promise of both a spec-file and cpu/memory/disk resources");
      return false;
      }
   }

if (a.env.host == NULL)
   {
   CfOut(cf_error,""," !! No environment_host defined for environment promise");
   return false;
   }

return true;
}

/*****************************************************************************/

void Nova_VerifyEnvironments(struct Attributes a,struct Promise *pp)

{ char hyper_uri[CF_MAXVARSIZE];

switch (Str2Hypervisors(a.env.type))
   {
   case cfv_virt_xen:
       snprintf(hyper_uri,CF_MAXVARSIZE-1,"xen:///");
       break;
       
   case cfv_virt_kvm:
       snprintf(hyper_uri,CF_MAXVARSIZE-1,"qemu:///session");

   case cfv_virt_esx:
       snprintf(hyper_uri,CF_MAXVARSIZE-1,"esx://127.0.0.1");
       break;

   case cfv_virt_test:
       snprintf(hyper_uri,CF_MAXVARSIZE-1,"test:///default");
       break;

   case cfv_zone:
       snprintf(hyper_uri,CF_MAXVARSIZE-1,"solaris_zone");
       break;

   default:
       break;
   }

CfOut(cf_verbose,""," -> Selecting environment type \"%s\" -> \"%s\"",a.env.type,hyper_uri);

if (strcmp(a.env.host,VFQNAME) != 0 || strcmp(a.env.host,VUQNAME) != 0)
   {
   switch (a.env.state)
      {
      case cfvs_create:
      case cfvs_running:
          CfOut(cf_verbose,""," -> This is not the promised host for the environment, so setting its intended state to \"down\"");
          a.env.state = cfvs_down;
          break;
      default:
          CfOut(cf_verbose,""," -> This is not the promised host for the environment, but it does not promise a run state, so take promise as valid");
      }
   }

switch (VSYSTEMHARDCLASS)
   {
   case linuxx:

       switch (Str2Hypervisors(a.env.type))
          {
          case cfv_virt_xen:
          case cfv_virt_kvm:
          case cfv_virt_esx:
          case cfv_virt_test:
              Nova_VerifyVirtDomain(hyper_uri,a,pp);
              break;
          case cfv_virt_network:
              break;
          case cfv_ec2:
              break;       
          case cfv_eucalyptus:
              break;
          default:
              break;
          }
       break;
       
   case solaris:

       switch (Str2Hypervisors(a.env.type))
          {
          case cfv_zone:
              VerifyZone(a,pp);
              break;
          default:
              break;
          }

       break;
       
   default:
       CfOut(cf_verbose,""," -> Unable to resolve an environment supervisor/monitor for this platform, aborting");
       break;
   }
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

void Nova_VerifyVirtDomain(char *uri,struct Attributes a,struct Promise *pp)

{ int num,i;
  virDomainPtr dom;
  const char *name;
 
/* set up the library error handler */
virSetErrorFunc(NULL,Nova_EnvironmentErrorHandler);

/* set up the signals handlers to catch disconnections */
//vshSetupSignals();

if (CFVC == NULL)
   {
   if ((CFVC = virConnectOpenAuth(uri,virConnectAuthPtrDefault,0)) == NULL)
      {
      CfOut(cf_error,""," !! Failed to connect to virtualization monitor \"%s\"",uri);
      return;
      }

   strncpy(WEBDRIVER,a.env.type,CF_MAXVARSIZE);
   }
else
   {
   if (strcmp(a.env.type,WEBDRIVER) != 0)
      {
      CfOut(cf_error,""," !! Conflicting environment type \"%s\" promised -- this seems to be a \"%s\" environment",a.env.type,WEBDRIVER);
      return;
      }
   }

for (i = 0; i < CF_MAX_CONCURRENT_ENVIRONMENTS; i++)
   {
   CF_RUNNING[i] = -1;
   CF_SUSPENDED[i] = NULL;
   }

num = virConnectListDomains(CFVC,CF_RUNNING,CF_MAX_CONCURRENT_ENVIRONMENTS);
CfOut(cf_verbose,""," -> Found %d running virtual domain environments on this host",num);
Nova_ShowRunList(CFVC);
num = virConnectListDefinedDomains(CFVC,CF_SUSPENDED,CF_MAX_CONCURRENT_ENVIRONMENTS);
CfOut(cf_verbose,""," -> Found %d dormant virtual domain environments on this host",num);
Nova_ShowDormant(CFVC);

switch(a.env.state)
   {
   case cfvs_create:
       Nova_CreateVirtDom(CFVC,uri,a,pp);
       break;
   case cfvs_delete:
       Nova_DeleteVirt(CFVC,uri,a,pp);
       break;
   case cfvs_running:
       Nova_RunningVirt(CFVC,uri,a,pp);
       break;
   case cfvs_suspended:
       Nova_SuspendedVirt(CFVC,uri,a,pp);
       break;
   case cfvs_down:
       Nova_DownVirt(CFVC,uri,a,pp);
       break;
   default:
       CfOut(cf_inform,""," !! No state specified for this environment");
       break;
   }
}  

/*****************************************************************************/

int VerifyZone(struct Attributes a,struct Promise *pp)
{
return true;
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int Nova_CreateVirtDom(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp)

{ int alloc_file = false;
  char *xml_file;
  const char *name;
  char defaultxml[CF_MAXVARSIZE];
  virDomainPtr dom;
  int i;

snprintf(defaultxml,CF_MAXVARSIZE-1,
         "<domain type='test'>"
         "  <name>%s</name>"
         "  <memory>8388608</memory>"
         "  <currentMemory>2097152</currentMemory>"
         "  <vcpu>2</vcpu>"
         "  <os>"
         "    <type>hvm</type>"
         "  </os>"
         "</domain>",pp->promiser
         );
         
for (i = 0; CF_RUNNING[i] > 0; i++)
   {
   dom = virDomainLookupByID(CFVC,CF_RUNNING[i]);
   name = virDomainGetName(dom);

   if (strcmp(name,pp->promiser) == 0)
      {
      cfPS(cf_verbose,CF_NOP,"",pp,a," -> Found a running environment called \"%s\" - promise kept\n",name);
      return true;
      }
   
   virDomainFree(dom);
   }

for (i = 0; CF_SUSPENDED[i] != NULL; i++)
   {
   if (strcmp(CF_SUSPENDED[i],pp->promiser) == 0)
      {
      CfOut(cf_inform,""," -> Found an existing, but suspended, environment id = %d, called \"%s\"\n",CF_SUSPENDED[i],CF_SUSPENDED[i]);
      }
   }

if (a.env.specfile)
   {
   if (virFileReadAll(a.env.specfile,CF_BUFSIZE,&xml_file) < 0)
      {
      cfPS(cf_verbose,CF_FAIL,"",pp,a," !! Unable to read environment specfile \"%s\"\n",a.env.specfile);
      return false;
      }

   alloc_file = true;
   }
else
   {
   xml_file = defaultxml;
   }

if (dom = virDomainCreateXML(vc,xml_file,0))
   {
   cfPS(cf_verbose,CF_CHG,"",pp,a," -> Created a virtual domain \"%s\"\n",pp->promiser);   

   if (a.env.cpus != CF_NOINT)
      {
      int maxcpus;
      
      if ((maxcpus = virConnectGetMaxVcpus(vc,virConnectGetType(vc))) == -1)
         {
         CfOut(cf_verbose,""," !! Can't determine the available CPU resources");
         }
      else
         {
         if (a.env.cpus > maxcpus)
            {
            CfOut(cf_inform,""," !! The promise to allocate %d CPUs in domain \"%s\" cannot be kept - only %d exist on the host",a.env.cpus,pp->promiser,maxcpus);
            }
         else if (virDomainSetVcpus(dom,(unsigned int)a.env.cpus) == -1)
            {
            CfOut(cf_inform,""," -> Unable to adjust CPU count to %d",a.env.cpus);
            }
         else
            {
            CfOut(cf_inform,""," -> Verified that environment CPU count is now %d",a.env.cpus);
            }
         }
      }
   
   if (a.env.memory != CF_NOINT)
      {
      unsigned long maxmem;

      if ((maxmem = virDomainGetMaxMemory(dom)) == -1)
         {
         CfOut(cf_verbose,""," !! Can't determine the available CPU resources");
         }
      else
         {
         if (virDomainSetMaxMemory(dom,(unsigned long)a.env.memory) == -1)
            {
            CfOut(cf_inform,""," !!! Unable to set the memory limit to %d",a.env.memory);
            }
         else
            {
            CfOut(cf_inform,""," -> Setting the memory limit to %ld",a.env.memory);
            }
         
         if (virDomainSetMemory(dom,(unsigned long)a.env.memory) == -1)
            {
            CfOut(cf_inform,""," !!! Unable to set the current memory to %ld",a.env.memory);
            }
         }
      }
   
   if (a.env.disk != CF_NOINT)
      {
      CfOut(cf_verbose,""," -> Info: env_disk parameter is not currently supported on this platform");
      }

   virDomainFree(dom);
   }
else
   {
   cfPS(cf_verbose,CF_FAIL,"",pp,a," !! Failed to create a virtual domain \"%s\"\n",pp->promiser);   
   }

if (alloc_file)
   {
   free(xml_file);
   }

return true;
}

/*****************************************************************************/

int Nova_DeleteVirt(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp)

{ virDomainPtr dom;
  int ret = true;

dom = virDomainLookupByName(vc,pp->promiser);

if (dom)
   {
   if (virDomainDestroy(dom) == -1)
      {
      cfPS(cf_verbose,CF_FAIL,"",pp,a," !! Failed to delete virtual domain \"%s\"\n",pp->promiser);   
      ret = false;
      }
   else
      {
      cfPS(cf_verbose,CF_CHG,"",pp,a," -> Deleted virtual domain \"%s\"\n",pp->promiser);   
      }

   virDomainFree(dom);
   }
else
   {
   cfPS(cf_verbose,CF_NOP,"",pp,a," -> No such virtual domain called \"%s\" - promise kept\n",pp->promiser);
   }

return ret;
}

/*****************************************************************************/

int Nova_RunningVirt(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp)

{ virDomainPtr dom;
  virDomainInfo info;
 
dom = virDomainLookupByName(vc,pp->promiser);

if (dom)
   {
   if (virDomainGetInfo(dom,&info) == -1)
      {
      cfPS(cf_inform,CF_FAIL,"",pp,a," !! Unable to probe virtual domain \"%s\"",pp->promiser);
      virDomainFree(dom);
      return false;
      }

   switch (info.state)
      {
      case VIR_DOMAIN_RUNNING:
          cfPS(cf_verbose,CF_NOP,"",pp,a," -> Virtual domain \"%s\" running - promise kept\n",pp->promiser);
          break;

      case VIR_DOMAIN_BLOCKED:
          cfPS(cf_verbose,CF_NOP,"",pp,a," -> Virtual domain \"%s\" running but waiting for a resource - promise kept as far as possible\n",pp->promiser);
          break;
          
      case VIR_DOMAIN_SHUTDOWN:
          cfPS(cf_verbose,CF_INTERPT,"",pp,a," -> Virtual domain \"%s\" is shutting down\n",pp->promiser);
          CfOut(cf_verbose,""," -> It is currently impossible to know whether it will reboot or not - deferring promise check until it has completed its shutdown");          
          break;
          
      case VIR_DOMAIN_PAUSED:
          
          if (virDomainResume(dom) == -1)
             {
             cfPS(cf_verbose,CF_INTERPT,"",pp,a," -> Virtual domain \"%s\" failed to resume after suspension\n",pp->promiser);
             virDomainFree(dom);
             return false;
             }

          cfPS(cf_verbose,CF_CHG,"",pp,a," -> Virtual domain \"%s\" was suspended, resuming\n",pp->promiser);
          break;
          
      case VIR_DOMAIN_SHUTOFF:
          
          if (virDomainCreate(dom) == -1)
             {
             cfPS(cf_verbose,CF_INTERPT,"",pp,a," -> Virtual domain \"%s\" failed to resume after halting\n",pp->promiser);
             virDomainFree(dom);
             return false;
             }

          cfPS(cf_verbose,CF_CHG,"",pp,a," -> Virtual domain \"%s\" was inactive, booting...\n",pp->promiser);
          break;
          
      case VIR_DOMAIN_CRASHED:

          if (virDomainReboot(dom,0) == -1)
             {
             cfPS(cf_verbose,CF_INTERPT,"",pp,a," -> Virtual domain \"%s\" has crashed and rebooting failed\n",pp->promiser);
             virDomainFree(dom);
             return false;
             }

          cfPS(cf_verbose,CF_CHG,"",pp,a," -> Virtual domain \"%s\" has crashed, rebooting...\n",pp->promiser);
          break;
          
      default:
          CfOut(cf_verbose,""," !! Virtual domain \"%s\" is reported as having no state, whatever that means",pp->promiser);
          break;
      }

   if (a.env.cpus > 0)
      {
      if (virDomainSetVcpus(dom,a.env.cpus) == -1)
         {
         CfOut(cf_inform,""," !!! Unable to set the number of cpus to %d",a.env.cpus);
         }
      else
         {
         CfOut(cf_inform,""," -> Setting the number of virtual cpus to %d",a.env.cpus);
         }
      }
   
   if (a.env.memory != CF_NOINT)
      {
      if (virDomainSetMaxMemory(dom,(unsigned long)a.env.memory) == -1)
         {
         CfOut(cf_inform,""," !!! Unable to set the memory limit to %d",a.env.memory);
         }
      else
         {
         CfOut(cf_inform,""," -> Setting the memory limit to %ld",a.env.memory);
         }

      if (virDomainSetMemory(dom,(unsigned long)a.env.memory) == -1)
         {
         CfOut(cf_inform,""," !!! Unable to set the current memory to %ld",a.env.memory);
         }
      }
   
   if (a.env.disk != CF_NOINT)
      {
      CfOut(cf_verbose,""," -> Info: env_disk parameter is not currently supported on this platform");
      }
   
   virDomainFree(dom);
   }
else
   {
   CfOut(cf_verbose,""," -> Virtual domain \"%s\" cannot be located, attempting to recreate",pp->promiser);
   Nova_CreateVirtDom(vc,uri,a,pp);
   }

return true;
}

/*****************************************************************************/

int Nova_SuspendedVirt(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp)

{ virDomainPtr dom;
  virDomainInfo info;

dom = virDomainLookupByName(vc,pp->promiser);

if (dom)
   {
   if (virDomainGetInfo(dom,&info) == -1)
      {
      cfPS(cf_inform,CF_FAIL,"",pp,a," !! Unable to probe virtual domain \"%s\"",pp->promiser);
      virDomainFree(dom);
      return false;
      }

   switch (info.state)
      {
      case VIR_DOMAIN_BLOCKED:
      case VIR_DOMAIN_RUNNING:
          if (virDomainSuspend(dom) == -1)
             {
             cfPS(cf_verbose,CF_INTERPT,"",pp,a," -> Virtual domain \"%s\" failed to suspend!\n",pp->promiser);
             virDomainFree(dom);
             return false;
             }

          cfPS(cf_verbose,CF_CHG,"",pp,a," -> Virtual domain \"%s\" running, suspending\n",pp->promiser);
          break;

      case VIR_DOMAIN_SHUTDOWN:
          cfPS(cf_verbose,CF_INTERPT,"",pp,a," -> Virtual domain \"%s\" is shutting down\n",pp->promiser);
          CfOut(cf_verbose,""," -> It is currently impossible to know whether it will reboot or not - deferring promise check until it has completed its shutdown");          
          break;
          
      case VIR_DOMAIN_PAUSED:
          
          cfPS(cf_verbose,CF_NOP,"",pp,a," -> Virtual domain \"%s\" is suspended - promise kept\n",pp->promiser);
          break;
          
      case VIR_DOMAIN_SHUTOFF:

          cfPS(cf_verbose,CF_NOP,"",pp,a," -> Virtual domain \"%s\" is down - promise kept\n",pp->promiser);
          break;
          
      case VIR_DOMAIN_CRASHED:

          if (virDomainSuspend(dom) == -1)
             {
             cfPS(cf_verbose,CF_INTERPT,"",pp,a," -> Virtual domain \"%s\" is crashed has failed to suspend!\n",pp->promiser);
             virDomainFree(dom);
             return false;
             }
          
          cfPS(cf_verbose,CF_CHG,"",pp,a," -> Virtual domain \"%s\" is in a crashed state, suspending\n",pp->promiser);
          break;
          
      default:
          CfOut(cf_verbose,""," !! Virtual domain \"%s\" is reported as having no state, whatever that means",pp->promiser);
          break;
      }

   virDomainFree(dom);
   }
else
   {
   cfPS(cf_verbose,CF_NOP,"",pp,a," -> Virtual domain \"%s\" cannot be found - take promise as kept\n",pp->promiser);
   }

return true;
}

/*****************************************************************************/

int Nova_DownVirt(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp)

{ virDomainPtr dom;
  virDomainInfo info;

dom = virDomainLookupByName(vc,pp->promiser);

if (dom)
   {
   if (virDomainGetInfo(dom,&info) == -1)
      {
      cfPS(cf_inform,CF_FAIL,"",pp,a," !! Unable to probe virtual domain \"%s\"",pp->promiser);
      virDomainFree(dom);
      return false;
      }

   switch (info.state)
      {
      case VIR_DOMAIN_BLOCKED:
      case VIR_DOMAIN_RUNNING:
          if (virDomainShutdown(dom) == -1)
             {
             cfPS(cf_verbose,CF_INTERPT,"",pp,a," -> Virtual domain \"%s\" failed to shutdown!\n",pp->promiser);
             virDomainFree(dom);
             return false;
             }

          cfPS(cf_verbose,CF_CHG,"",pp,a," -> Virtual domain \"%s\" running, terminating\n",pp->promiser);
          break;
          
      case VIR_DOMAIN_SHUTOFF:
      case VIR_DOMAIN_SHUTDOWN:
          cfPS(cf_verbose,CF_NOP,"",pp,a," -> Virtual domain \"%s\" is down - promise kept\n",pp->promiser);
          break;
          
      case VIR_DOMAIN_PAUSED:          
          cfPS(cf_verbose,CF_INTERPT,"",pp,a," -> Virtual domain \"%s\" is suspended - ignoring promise\n",pp->promiser);
          break;
          
      case VIR_DOMAIN_CRASHED:

          if (virDomainSuspend(dom) == -1)
             {
             cfPS(cf_verbose,CF_INTERPT,"",pp,a," -> Virtual domain \"%s\" is crashed and failed to shutdown\n",pp->promiser);
             virDomainFree(dom);
             return false;
             }
          
          cfPS(cf_verbose,CF_CHG,"",pp,a," -> Virtual domain \"%s\" is in a crashed state, terminating\n",pp->promiser);
          break;
          
      default:
          CfOut(cf_verbose,""," !! Virtual domain \"%s\" is reported as having no state, whatever that means",pp->promiser);
          break;
      }

   virDomainFree(dom);
   }
else
   {
   cfPS(cf_verbose,CF_NOP,"",pp,a," -> Virtual domain \"%s\" cannot be found - take promise as kept\n",pp->promiser);
   }

return true;
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

void Nova_EnvironmentErrorHandler()
{
}

/*****************************************************************************/

void Nova_ShowRunList(virConnectPtr vc)

{ int i;
  virDomainPtr dom;
  const char *name;
  
for (i = 0; CF_RUNNING[i] > 0; i++)
   {
   dom = virDomainLookupByID(CFVC,CF_RUNNING[i]);
   name = virDomainGetName(dom);
   CfOut(cf_verbose,""," ---> Found a running virtual domain called \"%s\"\n",name);
   virDomainFree(dom);
   }
}

/*****************************************************************************/

void Nova_ShowDormant(virConnectPtr vc)

{ int i;
  virDomainPtr dom;
 
for (i = 0; CF_SUSPENDED[i] != NULL; i++)
   {
   CfOut(cf_verbose,""," ---> Found a suspended, environment id = %d, called \"%s\"\n",CF_SUSPENDED[i],CF_SUSPENDED[i]);
   }
}
#endif
