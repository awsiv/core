
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

/*****************************************************************************/

void Nova_VerifyEnvironmentsPromise(struct Promise *pp)

{ struct Attributes a;

#ifdef HAVE_LIBVIRT
a = GetEnvironmentsAttributes(pp);

if (Nova_EnvironmentsSanityChecks(a,pp))
   {
   Nova_VerifyEnvironments(a,pp);
   }
#endif
}

/*****************************************************************************/

#ifdef HAVE_LIBVIRT

int Nova_EnvironmentsSanityChecks(struct Attributes a,struct Promise *pp)

{
 if (a.env.specfile)
    {
    }
 
return true;
}

/*****************************************************************************/

void Nova_VerifyEnvironments(struct Attributes a,struct Promise *pp)

{ char hyper_uri[CF_MAXVARSIZE];

switch (Str2Hypervisors(a.env.type))
   {
   case cfv_virt_xen:
       snprintf(hyper_uri,CF_MAXVARSIZE-1,"%s:///",a.env.type);
       break;
       
   case cfv_virt_kvm:
       snprintf(hyper_uri,CF_MAXVARSIZE-1,"qemu:///session",a.env.type);

   case cfv_virt_esx:
       snprintf(hyper_uri,CF_MAXVARSIZE-1,"%esx://127.0.0.1",a.env.type);
       break;

   case cfv_virt_test:
       snprintf(hyper_uri,CF_MAXVARSIZE-1,"%s:///default",a.env.type);
       break;

   case cfv_zone:
       snprintf(hyper_uri,CF_MAXVARSIZE-1,"solaris_zone",a.env.type);
       break;

   default:
       break;
   }

CfOut(cf_verbose,""," -> Selecting environment type \"%s\" -> \"%s\"",a.env.type,hyper_uri);

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

{ virConnectPtr vc;

/* set up the library error handler */
//virSetErrorFunc(NULL, virshErrorHandler);

/* set up the signals handlers to catch disconnections */
//vshSetupSignals();
  
if (vc = virConnectOpenAuth(uri,virConnectAuthPtrDefault,0))
   {
   CfOut(cf_error,""," !! Failed to connect to virtualization monitor \"%s\"",uri);
   }

// Also 
    
/* Discovery *********************************************

// Hypervisor type

virConnectGetType();
virConnectGetMaxVcpus();

// Get INactive

virConnectListDefinedNetworks(vconn);
virConnectListDefinedInterfaces();
virConnectListDefinedDomains();

// Get ACTIVE
virConnectListNetworks();
virConnectListDomains();
virConnectListInterfaces();

bool virDomainIsActive();
virDomainReboot();
***********************************************************/

switch(a.env.state)
   {
   case cfvs_create:
       Nova_CreateVirtDom(vc,uri,a,pp);
       break;
   case cfvs_delete:
       Nova_DeleteVirt(vc,uri,a,pp);
       break;
   case cfvs_running:
       Nova_RunningVirt(vc,uri,a,pp);
       break;
   case cfvs_suspended:
       Nova_SuspendedVirt(vc,uri,a,pp);
       break;
   case cfvs_down:
       Nova_DownVirt(vc,uri,a,pp);
       break;
   default:
       break;
   }

virConnectClose(vc);
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

{ char *xml_file;
  virDomainPtr dom;

if (virFileReadAll(a.env.specfile,CF_BUFSIZE,&xml_file) < 0)
   {
   return false;
   }

// virDomainDefineXML();

dom = virDomainCreateXML(vc,xml_file,0);

free(xml_file);

if (dom != NULL)
   {
   // DO SOMETHING
   //virDomainGetName(dom), from);
     
   virDomainFree(dom);
   }

return true;
}

/*****************************************************************************/

int Nova_DeleteVirt(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp)

{ virDomainPtr dom;

//dom = virDomainLookupByName(conn,localname);
//dom = virDomainLookupByID(conn,int id);
// virDomainDestroy();
// virDomainUndefine();

return true;
}

/*****************************************************************************/

int Nova_RunningVirt(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp)

{ virDomainPtr dom;
 
/*
struct virDomainInfo
{
unsigned char	state	: the running state, one of virDomainState
unsigned long	maxMem	: the maximum memory in KBytes allowed
unsigned long	memory	: the memory in KBytes used by the domain
unsigned short	nrVirtCpu	: the number of virtual CPUs for the domain
unsigned long long	cpuTime	: the CPU time used in nanoseconds
}

enum virDomainState
{
VIR_DOMAIN_NOSTATE	= 	0	: no state
VIR_DOMAIN_RUNNING	= 	1	: the domain is running
VIR_DOMAIN_BLOCKED	= 	2	: the domain is blocked on resource
VIR_DOMAIN_PAUSED	= 	3	: the domain is paused by user
VIR_DOMAIN_SHUTDOWN	= 	4	: the domain is being shut down
VIR_DOMAIN_SHUTOFF	= 	5	: the domain is shut off
VIR_DOMAIN_CRASHED	= 	6	: the domain is crashed

}
*/

//int virDomainGetInfo(virDomainPtr domain, virDomainInfoPtr info)
// virConnectOpenReadOnly();
// virDomainResume(); // if only suspended
// virDomainCreate(); // if not started

return true;
}

/*****************************************************************************/

int Nova_SuspendedVirt(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp)

{ virDomainPtr dom;
// virSuspendDomain();

return true;
}

/*****************************************************************************/

int Nova_DownVirt(virConnectPtr vc,char *uri,struct Attributes a,struct Promise *pp)

{ virDomainPtr dom;
// virDomainShutdown();
return true;
}


#endif
