/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf.nova.h"
#include "stories.h"

#include "env_context.h"
#include "constraints.h"
#include "files_names.h"
#include "item_lib.h"
#include "sort.h"
#include "matching.h"
#include "unix.h"
#include "cfstream.h"
#include "communication.h"
#include "ontology.h"
#include "vars.h"
#include "generic_agent.h" // ControlBodyConstraints

#include "bson_lib.h"
#include "db_query.h"
#include "topicmap.h"
#include "install.h"

static int Nova_GetReportedScalar(const char *hostkey, const char *ns, const char *bundle, const char *lval, char *returnval, int bufsize);
static int Nova_GetReportedList(const char *hostkey, const char *ns, const char *bundle, const char *lval, Rlist **list);

/***************************************************************************

 INFERENCES are currently hard wired until we understand their use ******

There are plenty of semantic heuristics here that are used, changing these
can have a large impact on the sense of the results. We try to keep these
as far as possible to the guidelines of promise theory and limit the number
of direct inferences, else the stories become trivial and lose their explanatory power.

There are 2 and 3 inferences. To select special kinds of stories, we search in general
and look for certain classes of inference. These are either

1. causal
2. connective
3. part (part of)
4. security (has access to)

Each kind of inference should end in one of the approved types to bring
about quick convergence of the reasoning.

v****************************************************************************/

#define CF_NARRATIVE_THRESHOLD 30

char *CF_2_CAUSE_INFERENCES_F[CF_INFSIZE][2] =
   {
   { KM_GENERALIZES_F, KM_AFFECTS_UNCERT_F },
   { KM_GENERALIZES_B, KM_AFFECTS_UNCERT_F },
   { NOVA_SEEALSO, KM_AFFECTS_UNCERT_F },
   { NOVA_STAKEHOLDER_INV, KM_AFFECTS_UNCERT_F },
   { NOVA_GOAL, KM_AFFECTS_UNCERT_F },
   { NOVA_GIVES, KM_AFFECTS_UNCERT_F },
   { NOVA_HANDLE_INV,KM_AFFECTS_UNCERT_F },
   { "might provide data for", KM_AFFECTS_UNCERT_F }, // Nova hard reln
   { "provides data for", KM_AFFECTS_CERT_F }, // Nova hard reln
   { KM_USES_CERT_B, KM_AFFECTS_CERT_F },
   { KM_AFFECTS_CERT_F, KM_AFFECTS_UNCERT_F },
   { KM_AFFECTS_POSS_F, KM_AFFECTS_UNCERT_F },
   { KM_AFFECTS_UNCERT_F, KM_AFFECTS_UNCERT_F },
   { KM_CAUSE_CERT_F, KM_AFFECTS_UNCERT_F },
   { KM_CAUSE_POSS_F, KM_AFFECTS_UNCERT_F },
   { KM_CAUSE_UNCERT_F, KM_AFFECTS_UNCERT_F },
   { NOVA_GIVES, KM_AFFECTS_UNCERT_F },
   { NOVA_ACTIVATES, KM_AFFECTS_UNCERT_F },
   { NOVA_IMPACTS, KM_AFFECTS_UNCERT_F },
   { "helps", KM_AFFECTS_UNCERT_F },
   { KM_REPAIR_CERT_F, KM_AFFECTS_UNCERT_F },
   { KM_REPAIR_POSS_F, KM_AFFECTS_UNCERT_F },
   { KM_REPAIR_UNCERT_F, KM_AFFECTS_UNCERT_F },
   { "improves", KM_AFFECTS_UNCERT_F },
   { NULL, NULL },
   };

char *CF_2_CAUSE_INFERENCES_B[CF_INFSIZE][2] =
   {
   { KM_GENERALIZES_F, KM_AFFECTS_UNCERT_B },
   { KM_GENERALIZES_B, KM_AFFECTS_UNCERT_B },
   { NOVA_STAKEHOLDER, KM_AFFECTS_UNCERT_B },
   { NOVA_SEEALSO, KM_AFFECTS_UNCERT_B },
   { NOVA_USES, KM_AFFECTS_CERT_B },
   { NOVA_HANDLE, KM_AFFECTS_UNCERT_B },
   { NOVA_GOAL_INV, KM_AFFECTS_UNCERT_B },
   { "might use data from", KM_AFFECTS_UNCERT_B }, // Nova hard reln
   { "uses data from", KM_AFFECTS_CERT_B }, // Nova hard reln
   { KM_USES_CERT_F, KM_AFFECTS_CERT_B },
   { KM_AFFECTS_CERT_B, KM_AFFECTS_UNCERT_B },
   { KM_AFFECTS_POSS_B, KM_AFFECTS_UNCERT_B },
   { KM_AFFECTS_UNCERT_B, KM_AFFECTS_UNCERT_B },
   { KM_CAUSE_CERT_B, KM_AFFECTS_UNCERT_B },
   { KM_CAUSE_POSS_B, KM_AFFECTS_UNCERT_B },
   { KM_CAUSE_UNCERT_B, KM_AFFECTS_UNCERT_B },
   { NOVA_USES, KM_AFFECTS_UNCERT_B },
   { NOVA_ACTIVATED, KM_AFFECTS_UNCERT_B },
   { NOVA_ISIMPACTED, KM_AFFECTS_UNCERT_B },
   { NOVA_BUNDLE_DATA_INV_B, KM_AFFECTS_UNCERT_B },
   { NOVA_BUNDLE_DATA_INV_P, KM_AFFECTS_UNCERT_B },
   { NOVA_GEN, KM_AFFECTS_UNCERT_B },
   { "is helped by", KM_AFFECTS_UNCERT_B },
   { NOVA_BUNDLE_DATA, KM_AFFECTS_UNCERT_B },
   { KM_USES_UNCERT_F, KM_AFFECTS_UNCERT_B },
   { KM_REPAIR_CERT_B, KM_AFFECTS_UNCERT_B },
   { KM_REPAIR_POSS_B, KM_AFFECTS_UNCERT_B },
   { KM_REPAIR_UNCERT_B, KM_AFFECTS_UNCERT_B },
   { "is improved by", KM_AFFECTS_UNCERT_B },
   { NULL, NULL },
   };

char *CF_2_CONNECT_INFERENCES_F[CF_INFSIZE][2] =
   {
   { KM_GENERALIZES_F, KM_CONNECTS_UNCERT_F },
   { KM_GENERALIZES_B, KM_CONNECTS_UNCERT_F },
   { KM_LOCATED_CERT_F, KM_CONNECTS_UNCERT_F },
   { KM_LOCATED_UNCERT_F, KM_CONNECTS_UNCERT_F },
   { KM_LOCATED_POSS_F, KM_CONNECTS_UNCERT_F },
   { NULL, NULL },
   };

char *CF_2_CONNECT_INFERENCES_B[CF_INFSIZE][2] =
   {
   { KM_GENERALIZES_F, KM_CONNECTS_UNCERT_F },
   { KM_GENERALIZES_B, KM_CONNECTS_UNCERT_F },
   { NOVA_SEEALSO, KM_CONNECTS_UNCERT_F },
   { KM_LOCATED_CERT_F, KM_CONNECTS_UNCERT_F },
   { KM_LOCATED_UNCERT_F, KM_CONNECTS_UNCERT_F },
   { KM_LOCATED_POSS_F, KM_CONNECTS_UNCERT_F },
   { NULL, NULL },
   };

char *CF_2_PART_INFERENCES_F[CF_INFSIZE][2] =
   {
   { KM_GENERALIZES_B, KM_PARTOF_UNCERT_F },
   { NOVA_SEEALSO, KM_PARTOF_UNCERT_F },
   { NOVA_GOAL, KM_PARTOF_CERT_B },
   { NOVA_GOAL_INV, KM_PARTOF_CERT_F },
   { KM_LOCATED_CERT_F, KM_PARTOF_CERT_F },
   { KM_LOCATED_UNCERT_F, KM_PARTOF_UNCERT_F },
   { KM_LOCATED_POSS_F, KM_PARTOF_UNCERT_F },
   { KM_MENTIONS_F, KM_PARTOF_CERT_F},
   { NULL, NULL },
   };

char *CF_2_PART_INFERENCES_B[CF_INFSIZE][2] =
   {
   { KM_GENERALIZES_F, KM_PARTOF_UNCERT_B },
   { NOVA_SEEALSO, KM_PARTOF_UNCERT_F },
   { KM_MENTIONS_B, KM_PARTOF_CERT_B},
   { NULL, NULL },
   };

char *CF_2_ACCESS_INFERENCES_F[CF_INFSIZE][2] =
   {
   {KM_PROVIDES_UNCERT_F,KM_ACCESS_UNCERT_F},
   {KM_PROVIDES_CERT_F,KM_ACCESS_UNCERT_F},
   {KM_PROVIDES_POSS_F,KM_ACCESS_UNCERT_F},
   { NULL, NULL },
   };

char *CF_2_ACCESS_INFERENCES_B[CF_INFSIZE][2] =
   {
   {KM_PROVIDES_UNCERT_B,KM_ACCESS_UNCERT_B},
   {KM_PROVIDES_CERT_B,KM_ACCESS_UNCERT_B},
   {KM_PROVIDES_POSS_B,KM_ACCESS_UNCERT_B},
   { NULL, NULL },
   };

/*****************************************************************************/

// These TRIADIC inferences should only refer to DYADIC INFERENCES

char *CF_3_CAUSE_INFERENCES_F[CF_INFSIZE][3] =
   {
   { KM_PARTOF_CERT_F, KM_AFFECTS_CERT_F, KM_AFFECTS_UNCERT_F },
   { KM_PARTOF_POSS_F, KM_AFFECTS_CERT_F, KM_AFFECTS_UNCERT_F },
   { KM_PARTOF_UNCERT_F, KM_AFFECTS_CERT_F, KM_AFFECTS_UNCERT_F },
//   { KM_PARTOF_CERT_B, KM_AFFECTS_CERT_F, KM_AFFECTS_CERT_F },
//   { KM_PARTOF_POSS_B, KM_AFFECTS_CERT_F, KM_AFFECTS_CERT_F },
   { KM_PARTOF_UNCERT_B, KM_AFFECTS_CERT_F, KM_AFFECTS_CERT_F },
   { KM_AFFECTS_UNCERT_F, KM_AFFECTS_CERT_F, KM_AFFECTS_UNCERT_F },
   { KM_AFFECTS_CERT_F, KM_AFFECTS_UNCERT_F, KM_AFFECTS_UNCERT_F },
   { KM_AFFECTS_POSS_F, KM_AFFECTS_POSS_F, KM_AFFECTS_POSS_F },
   { NULL, NULL, NULL },
   };

char *CF_3_CAUSE_INFERENCES_B[CF_INFSIZE][3] =
   {
   { KM_DETERMINES_CERT_B, KM_PARTOF_CERT_F, KM_DETERMINES_UNCERT_B },
   { KM_AFFECTS_UNCERT_B, KM_AFFECTS_CERT_B, KM_AFFECTS_UNCERT_B },
   { KM_AFFECTS_CERT_B, KM_AFFECTS_UNCERT_B, KM_AFFECTS_UNCERT_B },
   { KM_AFFECTS_POSS_B, KM_AFFECTS_POSS_B, KM_AFFECTS_POSS_B },
   { NULL, NULL, NULL },
   };

char *CF_3_CONNECT_INFERENCES_F[CF_INFSIZE][3] =
   {
   { NULL, NULL, NULL },
   };

char *CF_3_CONNECT_INFERENCES_B[CF_INFSIZE][3] =
   {
   { NULL, NULL, NULL },
   };

char *CF_3_PART_INFERENCES_F[CF_INFSIZE][3] =
   {
   { KM_PARTOF_CERT_F, KM_PARTOF_CERT_F, KM_PARTOF_CERT_F },
   { KM_DETERMINES_CERT_F, KM_PARTOF_CERT_F, KM_PARTOF_UNCERT_F },
   { NULL, NULL, NULL },
   };

char *CF_3_PART_INFERENCES_B[CF_INFSIZE][3] =
   {
   { NULL, NULL, NULL },
   };

char *CF_3_ACCESS_INFERENCES_F[CF_INFSIZE][3] =
   {
   { KM_PARTOF_CERT_F, KM_PARTOF_CERT_F, KM_PARTOF_CERT_F },
   { KM_DETERMINES_CERT_F, KM_PARTOF_CERT_F, KM_PARTOF_UNCERT_F },
   { NULL, NULL, NULL },
   };

char *CF_3_ACCESS_INFERENCES_B[CF_INFSIZE][3] =
   {
   { NULL, NULL, NULL },
   };

/*****************************************************************************/
/* Chapter discovery                                                           */
/*****************************************************************************/

void Nova_ScanAccessRelationships(FILE *fp, Promise *pp,char *promise_id)

// Try to infer from the promises made what the network structure might be
    
{ Rlist *rp;
 char host[CF_MAXVARSIZE] = {0},ip[CF_MAXVARSIZE] = {0},context[CF_MAXVARSIZE];

// Server copy_from bodies

fprintf(fp,"topics:\n");

    if (strcmp(pp->agentsubtype,"files") == 0)
    {
        for (size_t i = 0; i < SeqLength(pp->conlist); i++)
        {
            Constraint *cp = SeqAt(pp->conlist, i);

            if (strcmp("servers",cp->lval) == 0)
            {
                for (rp = cp->rval.item; rp != NULL; rp=rp->next)
                {
                    fprintf(fp,"hosts:: \"%s\" comment => \"Host listed as a server in a remote copy promise\";\n", (const char *)rp->item);
                    fprintf(fp,"hosts:: \"%s\" association => a(\"is a remote files server in context\",\"%s\",\"contains files server\");\n", (const char *)rp->item,pp->classes);
                }
            }
        }
    }
 
// Access promises - see if we can figure out which hub we belong to

if (strcmp(pp->agentsubtype,"access") == 0)
   {

    for (size_t i = 0; i < SeqLength(pp->conlist); i++)
    {
        Constraint *cp = SeqAt(pp->conlist, i);

      if (strcmp("admit",cp->lval) == 0)
         {
         for (rp = cp->rval.item; rp != NULL; rp=rp->next)
            {
            if (IsRegex(rp->item))
               {
               strncpy(host,rp->item,sizeof(host));
               strncpy(ip,rp->item,sizeof(ip));
               }
            else
               {
               strncpy(host,Hostname2IPString(rp->item),CF_MAXVARSIZE-1);
               strncpy(ip,IPString2Hostname(rp->item),CF_MAXVARSIZE-1);
               }
            
            if (strcmp(host,ip) != 0)
               {
               strcpy(context,"hosts");
               }
            else
               {
               strcpy(context,"network");
               }
            
            if (strcmp(pp->promiser,"relay") == 0)
               {
               fprintf(fp,"%s:: \"%s\" association => a(\"%s\",\"constellation hub\",\"%s\");\n",context, (const char *)rp->item,KM_PROVIDES_UNCERT_F,KM_PROVIDES_UNCERT_B);
               fprintf(fp,"%s:: \"%s\" association => a(\"%s\",\"constellation hub\",\"%s\");\n",context, (const char *)rp->item,KM_MONITOR_UNCERT_F,KM_MONITOR_UNCERT_B);
               fprintf(fp,"%s:: \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",context, (const char *)rp->item,KM_MONITOR_UNCERT_F,pp->classes,KM_MONITOR_UNCERT_B);
               }
            else if (strcmp(pp->promiser,"full") == 0)
               {
               fprintf(fp,"%s:: \"%s\" association => a(\"%s\",\"full hub query\",\"%s\");\n",context, (const char *)rp->item,KM_ACCESS_UNCERT_B,KM_ACCESS_UNCERT_F);

               if (IsIPV4Address(rp->item)||IsIPV6Address(rp->item))
                  {
                  fprintf(fp,"%s:: \"%s\" association => a(\"might contain\",\"reporting hub\",\"might be contained by\");\n",context, (const char *)rp->item);
                  }
               else
                  {
                  fprintf(fp,"%s:: \"%s\" association => a(\"reports to hub\",\"%s\",\"collects reports from\");\n",context, (const char *)rp->item, VFQNAME);
                  }
               }
            else if (strcmp(pp->promiser,"delta") == 0)
               {
               fprintf(fp,"%s:: \"%s\" association => a(\"%s\",\"delta hub query\",\"%s\");\n",context, (const char *)rp->item, KM_ACCESS_UNCERT_B,KM_ACCESS_UNCERT_F);

               if (IsIPV4Address(rp->item)||IsIPV6Address(rp->item))
                  {
                  fprintf(fp,"%s:: \"%s\" association => a(\"might contain\",\"reporting hub\",\"might be contained by\");\n",context, (const char *)rp->item);
                  }
               else
                  {
                  fprintf(fp,"%s:: \"%s\" association => a(\"reports to hub\",\"%s\",\"collects reports from\");\n",context, (const char *)rp->item,VFQNAME);
                  }
               }
            else if (*(pp->promiser) != '/')
               {
               fprintf(fp,"%s:: \"%s\"  association => a(\"%s\",\"remote_scalars::%s\",\"%s\");\n",context, (const char *)rp->item, KM_ACCESS_CERT_B,pp->promiser,KM_ACCESS_CERT_F);
               }
            else
               {
               fprintf(fp,"%s:: \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",context, (const char *)rp->item, KM_ACCESS_CERT_B,pp->promiser,KM_ACCESS_CERT_F);
               fprintf(fp,"%s:: \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",context, (const char *)rp->item, KM_CONNECTS_UNCERT_F,pp->classes,KM_CONNECTS_UNCERT_B);

               fprintf(fp,"contexts:: \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",pp->classes,KM_PROVIDES_CERT_F,pp->promiser,KM_PROVIDES_CERT_B);
               }
            }
         }
      }
   }


if (strcmp(pp->agentsubtype,"access") == 0)
   {

   for (size_t i = 0; i < SeqLength(pp->conlist); i++)
   {
       Constraint *cp = SeqAt(pp->conlist, i);

      if (strcmp("deny",cp->lval) == 0)
         {
         for (rp = cp->rval.item; rp != NULL; rp=rp->next)
            {
            strncpy(host,Hostname2IPString(rp->item),CF_MAXVARSIZE-1);
            strncpy(ip,IPString2Hostname(rp->item),CF_MAXVARSIZE-1);

            if (strcmp(host,ip) != 0)
               {
               strcpy(context,"hosts");
               }
            else
               {
               strcpy(context,"network");
               }
            
            if (strcmp(pp->promiser,"relay") == 0)
               {
               fprintf(fp,"%s:: \"%s\" association => a(\"%s\",\"reporting hub\",\"%s\");\n",context, (const char *)rp->item, KM_SHIELD_CERT_F,KM_SHIELD_CERT_B);
               }
            else if (strcmp(pp->promiser,"full") == 0)
               {
               fprintf(fp,"%s:: \"%s\" association => a(\"%s\",\"full hub query\",\"%s\");\n",context, (const char *)rp->item, KM_SHIELD_CERT_B,KM_SHIELD_CERT_F);
               }
            else if (strcmp(pp->promiser,"delta") == 0)
               {
               fprintf(fp,"%s:: \"%s\" association => a(\"%s\",\"delta hub query\",\"%s\");\n",context, (const char *)rp->item, KM_SHIELD_CERT_F,KM_SHIELD_CERT_B);
               }
            else if (*(pp->promiser) != '/')
               {
               fprintf(fp,"%s:: \"%s\"  association => a(\"%s\",\"remote_scalars::%s\",\"%s\");\n",context, (const char *)rp->item, pp->promiser,KM_SHIELD_CERT_B,KM_SHIELD_CERT_F);
               }
            else
               {
               fprintf(fp,"%s:: \"%s\" association => a(\"%s\",\"%s\",\"%s\");\n",context, (const char *)rp->item, KM_SHIELD_CERT_B,pp->promiser,KM_SHIELD_CERT_F);
               }
            }
         }
      }
   }

}

/*****************************************************************************/
/* Host StoryLine                                                              */
/*****************************************************************************/

void Nova_HostStory(const Policy *policy, char *hostkey,char *buffer,int bufsize)

// e.g. cf-know -z SHA=e5a14c9705066357324895426566b558ebe5989b64f9df4505c457eef19d1407
    
{ Item *low = NULL, *high = NULL, *anomaly = NULL, *unfocused = NULL, *focused = NULL, *ldt = NULL;
  Item *ip;
  int high_count = ListLen(high);
  char uqhost[CF_MAXVARSIZE],fqhost[CF_MAXVARSIZE],ipv4[CF_MAXVARSIZE],flavour[CF_SMALLBUF];
  char ostype[CF_SMALLBUF],os[CF_SMALLBUF],cpus[CF_SMALLBUF],loadavg[CF_SMALLBUF],devloadavg[CF_SMALLBUF],cpu[CF_SMALLBUF];
  char version[CF_MAXVARSIZE];
  Rlist *interfaces = NULL, *ip_addresses = NULL, *mac_addresses = NULL,*locations = NULL;
  Rlist *udp4 = NULL, *udp6 = NULL, *tcp4 = NULL, *tcp6 = NULL;
  Rlist *rp;
  
buffer[0] = '\0';
uqhost[0] = '\0';

// Don't use the buffer until we have the content ok

Nova_GetReportedScalar(hostkey, NULL, "sys","uqhost",uqhost,CF_MAXVARSIZE);
Nova_GetReportedScalar(hostkey, NULL, "sys","fqhost",fqhost,CF_MAXVARSIZE);
Nova_GetReportedScalar(hostkey, NULL, "sys","ipv4",ipv4,CF_MAXVARSIZE);
Nova_GetReportedScalar(hostkey, NULL, "sys","flavour",flavour,CF_SMALLBUF);
Nova_GetReportedScalar(hostkey, NULL, "sys","os",os,CF_SMALLBUF);
Nova_GetReportedScalar(hostkey, NULL, "sys","ostype",ostype,CF_SMALLBUF);
Nova_GetReportedScalar(hostkey, NULL, "mon","value_loadavg",loadavg,CF_SMALLBUF);
Nova_GetReportedScalar(hostkey, NULL, "mon","value_cpu",cpu,CF_SMALLBUF);
Nova_GetReportedScalar(hostkey, NULL, "mon","dev_loadavg",devloadavg,CF_SMALLBUF);
Nova_GetReportedScalar(hostkey, NULL, "sys","cpus",cpus,CF_SMALLBUF);
Nova_GetReportedScalar(hostkey, NULL, "sys","constellation_version",version,CF_SMALLBUF);

Nova_GetReportedList(hostkey, NULL, "sys","ip_addresses",&ip_addresses);
Nova_GetReportedList(hostkey, NULL, "sys","interfaces",&interfaces);
Nova_GetReportedList(hostkey, NULL, "sys","hardware_addresses",&mac_addresses);
Nova_GetReportedList(hostkey, NULL, "mon","listening_udp4_ports",&udp4);
Nova_GetReportedList(hostkey, NULL, "mon","listening_tcp4_ports",&tcp4);
Nova_GetReportedList(hostkey, NULL, "mon","listening_udp6_ports",&udp6);
Nova_GetReportedList(hostkey, NULL, "mon","listening_tcp6_ports",&tcp6);

// Report who am I?

if (strlen(uqhost) > 0)
   {
   printf("\nThe ID you provided is the keyhash for a host attached to this hub\n\n");
   printf("It reports its own name as \"%s\", (full name %s)\n",uqhost,fqhost);
   }
else
   {
   printf("You entered something looking like a keyhash, but no host with this ID has yet reported.\n");
   printf("Perhaps try again later?\n");
   return;
   }

if (strcmp(ipv4,IPString2Hostname(ipv4)) == 0)
   {
   printf("The name service reports its name to be %s, which probably means it is not registered in DNS or any directory service.\n",IPString2Hostname(ipv4));
   }
else
   {
   printf("The name service reports its name to be %s, to the network.\n",IPString2Hostname(ipv4));
   }

if (RlistLen(interfaces) > 1)
   {
   printf("This host has multiple interfaces: ");
   for (rp = interfaces; rp != NULL; rp=rp->next)
      {
      if (rp->next && rp->next->next == NULL)
         {
         printf("%s, and ",(char *)rp->item);
         }
      else
         {
         printf("%s,",(char *)rp->item);
         }
      }      

   printf(" with MAC addresses ");
   
   for (rp = mac_addresses; rp != NULL; rp=rp->next)
      {
      if (rp->next && rp->next->next == NULL)
         {
         printf("%s, and ",(char *)rp->item);
         }
      else
         {
         printf("%s,",(char *)rp->item);
         }
      }      

   printf(" and IP addresses ");
   
   for (rp = ip_addresses; rp != NULL; rp=rp->next)
      {
      if (rp->next && rp->next->next == NULL)
         {
         printf("%s, and ",(char *)rp->item);
         }
      else
         {
         printf("%s,",(char *)rp->item);
         }         
      }
   
   printf(" respectively.\n");
   }
else
   {
   if (interfaces)
      {
      printf("Its last known address was at %s via interface %s\n\n",ipv4,(char *)interfaces->item);
      }
   }

printf("It has %s cpu cores running at about %s%% combined utilization.\n",cpus,cpu);

// Try to infer a location - can we use Google maps somehow?

Nova_GetLocations(policy, hostkey,&locations);

for (rp = locations; rp != NULL; rp = rp->next)
   {
   printf("This host seems to have been located in: %s\n",(char *)rp->item);
   // Could tell a story about the location here Nova_GenerateStoriesCmdLine(rp->item,cfi_connect);
   }

// What flavour am I?

printf("It is currently running %s, i.e. a kind of %s, specifically %s\n",flavour,os,ostype);

// Look at health /weather

printf("The system load average is %s (plus or minus about %s) shared between %s cores.\n",loadavg,devloadavg,cpus);

if (atof(loadavg) >= atof(cpu))
   {
   printf("The load is approaching a high level.\n");
   }
else if (atof(loadavg) > atof(cpu)*0.5)
   {
   printf("The load is moderate for hardware of this capacity\n");
   }
else
   {
   printf("The load is negligeable for hardware of this capacity\n");
   }

Nova_GetWeatherReport(hostkey,&low,&high,&anomaly,&focused,&unfocused,&ldt);

if (low)
   {
   printf("The system is quiet, running low: ");
   for (ip = low; ip != NULL; ip=ip->next)
      {
      printf("%s ",ip->name);
      }
   printf("\n");
   }

if (high)
   {
   printf("These vitals are running high: ");
   for (ip = high; ip != NULL; ip=ip->next)
      {
      printf("%s ",ip->name);
      }
   printf("\n");
   }

if (anomaly)
   {
   printf("There has been some notable activity, running abnormally high, in: ");
   for (ip = anomaly; ip != NULL; ip=ip->next)
      {
      printf("%s ",ip->name);
      Nova_GenerateStoriesCmdLine(ip->name,cfi_cause);
      }
   printf("\n");
   }

if (low == NULL && high == NULL && anomaly == NULL)
   {
   printf("All the vital signs look normal, nothing to report.\n");
   }

if (high_count > 2) // If more than one measure is anomalously high, indicates stress
   {
   printf("System activity is running hot right now: ");
   for (ip = high; ip != NULL; ip=ip->next)
      {
      printf("%s ",ip->name);
      }

   printf("\n");
   }
else
   {
   if (high)
      {
      printf("There was an anomalous value, which could be %s\n",high->name);
      Nova_GenerateStoriesCmdLine(high->name,cfi_cause);
      }
   }

for (rp = tcp4; rp != NULL; rp=rp->next)
   {
       printf("Open tcp4 port: %s\n", RlistScalarValue(rp));
   }

DeleteItemList(low);
DeleteItemList(high);
DeleteItemList(ldt);
DeleteItemList(focused);
DeleteItemList(unfocused);
RlistDestroy(ip_addresses);
RlistDestroy(mac_addresses);
RlistDestroy(interfaces);
RlistDestroy(udp4);
RlistDestroy(udp6);
RlistDestroy(tcp4);
RlistDestroy(tcp6);

// Try some stories

//Nova_GenerateStoryLineCmdLine(ipv4,cfi_cause);

printf("CFEngine version Enterprise %s is tending to this host.\n",version);

}

/*****************************************************************************/

void Nova_GetWeatherReport(char *hostkey,Item **low, Item **high, Item **anomaly, Item **focused, Item **unfocused, Item **ldt)

{ HubClass *hc;
  HubQuery *hq;
  Rlist *rp;
  EnterpriseDB dbconn;

/* BEGIN query the current classes from this host */
 
if (!CFDB_Open(&dbconn))
   {
   return;
   }

time_t now = time(NULL);
hq = CFDB_QueryClasses(&dbconn, hostkey, NULL, now - 10000, now, NULL,
                       PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_DISABLE_ALL);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hc = (HubClass *)rp->item;

   // Look for _high_ and _low_ vitals and trust their integrity to estimate state

   if (strstr(hc->class,"high_ldt"))
      {
      PrependItem(ldt,hc->class,NULL);
      continue;
      }

   if (strstr(hc->class,"entropy_") && strstr(hc->class,"_high"))
      {
      PrependItem(unfocused,hc->class,NULL);
      continue;
      }

   if (strstr(hc->class,"entropy_") && strstr(hc->class,"_low"))
      {
      PrependItem(focused,hc->class,NULL);
      continue;
      }

   if (strstr(hc->class,"_high_anomaly"))
      {
      PrependItem(anomaly,hc->class,NULL);
      continue;
      }

   if (strstr(hc->class,"_high_dev2"))
      {
      PrependItem(high,hc->class,NULL);
      continue;
      }

   if (strstr(hc->class,"_low"))
      {
      PrependItem(low,hc->class,NULL);
      continue;
      }
   }
 
DeleteHubQuery(hq,DeleteHubClass);
CFDB_Close(&dbconn);
}

/*****************************************************************************/

void Nova_GetLocations(const Policy *policy, char *hostkey, Rlist **locations)

{
    HubClass *hc;
    HubQuery *hq;
    Rlist *rp;
    EnterpriseDB dbconn;
    Rval retval;

    /* BEGIN query the current classes from this host */

    // Check for location descriptors - we need this categorization for hierarchy view / spanning tree

    Seq *constraints = ControlBodyConstraints(policy, AGENT_TYPE_COMMON);
    if (constraints)
    {
        for (size_t i = 0; i < SeqLength(constraints); i++)
        {
            Constraint *cp = SeqAt(constraints, i);

            if (IsExcluded(cp->classes, NULL))
            {
                continue;
            }

            if (GetVariable("control_common", CFG_CONTROLBODY[cfg_site_classes].lval, &retval) == DATA_TYPE_NONE)
            {
                // retval.item is a list of classes that can be considered locations, if defined
                return;
            }
        }
    }


if (!CFDB_Open(&dbconn))
   {
   return;
   }

time_t now = time(NULL);
hq = CFDB_QueryClasses(&dbconn, hostkey, NULL, now - 10000, now, NULL,
                       PROMISE_CONTEXT_MODE_ALL, NULL, QUERY_FLAG_DISABLE_ALL);

for (rp = hq->records; rp != NULL; rp=rp->next)
   {
   hc = (HubClass *)rp->item;

   if (RlistIsStringIn(retval.item,hc->class))
      {
      RlistPrependScalar(locations,hc->class,RVAL_TYPE_SCALAR);
      break;
      }
   }
 
DeleteHubQuery(hq,DeleteHubClass);
CFDB_Close(&dbconn);
}


/*****************************************************************************/
/* Topic Stories                                                             */
/*****************************************************************************/

void Nova_GenerateStoriesCmdLine(char *typed_topic,enum storytype type)

{ int topic_id;
  char topic[CF_BUFSIZE],context[CF_BUFSIZE];
  Chapter *episode;
  StoryLine *allstories = NULL, *root = NULL;
  bson query,field;
  mongo_cursor *cursor;
  bson_iterator it1;
  EnterpriseDB conn;
  char topic_name[CF_BUFSIZE];
  char topic_context[CF_BUFSIZE];
  Item *ip,*list = NULL;

if (!CFDB_Open(&conn))
   {
   return;
   }

/* BEGIN query document */

if (!NULL_OR_EMPTY(typed_topic))
   {
   DeClassifyTopic(typed_topic,topic,context);

   bson_init(&query);
   bson_append_regex(&query,cfk_topicname,topic,"");
   bson_append_regex(&query,cfk_topiccontext,context,"");
   BsonFinish(&query);
   }
else
   {
   bson_empty(&query);
   }

/* BEGIN RESULT DOCUMENT */

BsonSelectReportFields(&field, 3,
                       cfk_topicname,
                       cfk_topicid,
                       cfk_topiccontext);

/* BEGIN SEARCH for matching topics */

cursor = MongoFind(&conn,MONGO_KM_TOPICS,&query,&field,0,0,CF_MONGO_SLAVE_OK);

bson_destroy(&query);
bson_destroy(&field);

while (MongoCursorNext(cursor))  // loops over documents
   {
   bson_iterator_init(&it1, mongo_cursor_bson(cursor));
   
   topic_name[0] = '\0';
   topic_context[0] = '\0';
   topic_id = 0;

   while (BsonIsTypeValid(bson_iterator_next(&it1)) > 0)
      {
      /* Query episodeecific search/marshalling */
      
      if (strcmp(bson_iterator_key(&it1),cfk_topicname) == 0)
         {
         strncpy(topic_name,bson_iterator_string(&it1),CF_BUFSIZE-1);
         }   

      if (strcmp(bson_iterator_key(&it1),cfk_topiccontext) == 0)
         {
         strncpy(topic_context,bson_iterator_string(&it1),CF_BUFSIZE-1);
         }
      
      if (strcmp(bson_iterator_key(&it1),cfk_topicid) == 0)
         {
         topic_id = (int)bson_iterator_int(&it1);
         }

      PrependFullItem(&list,topic_name,topic_context,topic_id,0);
      }
   }

mongo_cursor_destroy(cursor);
CFDB_Close(&conn);

list = SortItemListCounters(list); // Sort these to assist later aggregation

// Now try the narratives, set up a story for each matching topic

for (ip = list; ip != NULL; ip=ip->next)
   {
   strcpy(topic,ip->name);
   strcpy(context,ip->classes);
   topic_id = ip->counter;

   // Add each starting topic to a root list
   
   episode = NewChapter(topic,context,topic_id);
   PrependStoryLine(&root,episode);

   // Now make a recursive story, priming the tree with this reference

   if ((episode = AppendToStory(&episode,topic,context,"begins with",topic_id)))
      {
      GenerateStoriesAbout(&episode,type,&allstories,0,cfi_forwards);
      GenerateStoriesAbout(&episode,type,&allstories,0,cfi_backwards);
      }
   }

Nova_ShowStoryLine(allstories,type);

if (list == NULL || ListLen(list) < 2)
   {
   printf("   Didn't find anything interesting about this.\n");
   }
  
DeleteStoryLine(root);
//DeleteStoryLine(allstories); - the refs should not be freed, already done
DeleteItemList(list);
}

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

void GenerateStoriesAbout(Chapter **story_topic,enum storytype type,StoryLine **allstories,int depth,enum cf_direction direction)

{ Item *ip,*nn;
  char topic[CF_BUFSIZE],context[CF_BUFSIZE],*assoc;
  Chapter *episode;
  int new_topic_id;

if (depth >= CF_MAX_STORY_DEPTH-1)
   {
   PrependStoryLine(allstories,*story_topic);
   return;
   }

if (*(story_topic) == NULL)
   {
   return;
   }

nn = Nova_ScanLeadsAssociations((*story_topic)->topic_id,NULL);

if (nn == NULL)
   {
   // Add the finished story to &allstories so we know how to find it quickly
   PrependStoryLine(allstories,*story_topic);
   return;
   }

nn = SortItemListCounters(nn); // Sort these to assist later aggregation

for (ip = nn; ip != NULL; ip=ip->next)
   {
   new_topic_id = ip->counter;

//   printf("   %d NN gave (%s,%s,%d)\n",depth,ip->name,ip->classes,ip->counter);
   
   DeClassifyTopic(ip->classes,topic,context);
   assoc = ip->name;

   if (AlreadySaidThis((*story_topic)->history,new_topic_id))
      {
      continue;
      }

   // Check for any inferred weak transitivity in the associations
   
   if (!PossibleInference(assoc,type,direction,*story_topic,topic,context))
      {
      // This is usually how stories end
      PrependStoryLine(allstories,*story_topic);
      continue;
      }
   
   if ((episode = AppendToStory(story_topic,topic,context,assoc,new_topic_id)))
      {
      GenerateStoriesAbout(&episode,type,allstories,depth+1,direction);
      }
   }
}

/*****************************************************************************/
/* Level 1                                                                   */
/*****************************************************************************/

StoryLine *PrependStoryLine(StoryLine **start,Chapter *story)

{ StoryLine *new;
  int i;
  long checksum = 0;
  StoryLine *episode;
  
// Make a checksum to compare stories quickly and cheaply

if (story->history)
   {
   for (i = 0; story->history[i] != NULL; i++)
      {
      checksum += (long)story->history[i];
      }
   }

for (episode = *start; episode != NULL; episode=episode->next)
   {
   if (checksum == episode->checksum)
      {
      return *start;
      }
   }

new = xmalloc(sizeof(StoryLine));
new->story = story;
new->next = *start;
new->checksum = checksum;
new->done = false;
*start = new;

return *start;
}

/*****************************************************************************/

Chapter *NewChapter(char *topic,char *context,int topic_id)

{ Chapter *new;
  int i; 

new = xmalloc(sizeof(Chapter));

for (i = 0; i < CF_MAX_STORY_DEPTH; i++)
   {
   new->history[i] = NULL;
   }

// Now add the new story topic-data
new->assoc = NULL;
new->topic = xstrdup(topic);
new->context = xstrdup(context);
new->topic_id = topic_id;

new->indirect_inferences = NULL;
return new;
}

/*****************************************************************************/

void DeleteStoryLine(StoryLine *start)
{
if (start)
   {
   DeleteStoryLine(start->next);
   DeleteStory(start->story);
   }
}

/*****************************************************************************/

Chapter *AppendToStory(Chapter **last,char *topic,char *context,char *assoc,int topic_id)

{ Chapter *new;
  int i; 

new = NewChapter(topic,context,topic_id);

new->assoc = xstrdup(assoc);

for (i = 0; i < CF_MAX_STORY_DEPTH; i++)
   {
   new->history[i] = NULL;
   }

// Append ourselves to the running history, like BGP

for (i = 0; (*last)->history[i] != NULL; i++)
   {
   new->history[i] = (*last)->history[i];
   }

new->history[i] = new;
new->indirect_inferences = NULL;
return new;
}

/*****************************************************************************/

void DeleteStory(Chapter *this)

{ 
free(this->assoc);
free(this->topic);
DeleteItemList(this->indirect_inferences);
free(this);
}

/*****************************************************************************/

int AlreadySaidThis(Chapter *history[CF_MAX_STORY_DEPTH],int topic_id)

{ int i;
 
for (i = 0; history[i] != NULL; i++)
   {
   if (history[i] && history[i]->topic_id == topic_id)
      {
      return true;
      }
   }

return false;
}

/*****************************************************************************/

int PossibleInference(char *assoc,enum storytype type,enum cf_direction direction,Chapter *this,char *topic,char *context)

{ void *inf_2F,*inf_2B,*inf_3F;

// If we are already at the most general level, don't elaborate, and risk multiple equiv storiessibel

if (strcmp(this->context,"any") == 0 && strcmp(assoc,KM_GENERALIZES_F) == 0)
   {
   return false;
   }

if (strcmp(context,"any") == 0 && strcmp(assoc,KM_GENERALIZES_B) == 0)
   {
   return false;
   }



switch (type)
   {
   case cfi_cause:
       switch (direction) // Dual path inferences
          {
          case cfi_forwards:
              inf_2F = CF_2_CAUSE_INFERENCES_F;
              inf_2B = CF_2_CAUSE_INFERENCES_B;
              inf_3F = CF_3_CAUSE_INFERENCES_F;
              break;
          default:
              inf_2F = CF_2_CAUSE_INFERENCES_B;
              inf_2B = CF_2_CAUSE_INFERENCES_F;
              inf_3F = CF_3_CAUSE_INFERENCES_B;
              break;
          }
       break;

   case cfi_connect:
       switch (direction)
          {
          case cfi_forwards:
              inf_2F = CF_2_CONNECT_INFERENCES_F;
              inf_2B = CF_2_CONNECT_INFERENCES_B;
              inf_3F = CF_3_CONNECT_INFERENCES_F;
              break;
          default:
              inf_2F = CF_2_CONNECT_INFERENCES_B;
              inf_2B = CF_2_CONNECT_INFERENCES_F;
              inf_3F = CF_3_CONNECT_INFERENCES_B;
              break;
          }       
       break;
 
   case cfi_part:
       switch (direction)
          {
          case cfi_forwards:
              inf_2F = CF_2_PART_INFERENCES_F;
              inf_2B = CF_2_PART_INFERENCES_B;
              inf_3F = CF_3_PART_INFERENCES_F;
              break;
          default:
              inf_2F = CF_2_PART_INFERENCES_B;
              inf_2B = CF_2_PART_INFERENCES_F;
              inf_3F = CF_3_PART_INFERENCES_B;
              break;
              
          }
       break;
       
   case cfi_access:
       switch (direction)
          {
          case cfi_forwards:
              inf_2F = CF_2_ACCESS_INFERENCES_F;
              inf_2B = CF_2_ACCESS_INFERENCES_B;
              inf_3F = CF_3_ACCESS_INFERENCES_F;
              break;
          default:
              inf_2F = CF_2_ACCESS_INFERENCES_B;
              inf_2B = CF_2_ACCESS_INFERENCES_F;
              inf_3F = CF_3_ACCESS_INFERENCES_B;              
              break;
          }
       break;
   default:
       return true;
       
   }

// Check to see if our association could be part of one of these inferences

switch (direction)
   {
   case cfi_forwards:
       if (Nova_Relevant2Inference(assoc,inf_2F))
          {
          return true;
          }
              
       if (Nova_Possible3Inference(assoc,inf_3F))
          {
          return true;
          }

       // Take notes on what we see of indirect references
       
       if (strcmp(assoc,KM_SYNONYM) == 0)
          {
          IdempPrependItem(&(this->indirect_inferences),assoc,topic);
          }
       
       if (Nova_Relevant2Inference(assoc,inf_2B))
          {
          IdempPrependItem(&(this->indirect_inferences),assoc,topic);
          }
       
   break;

   default:
       if (Nova_Relevant2Inference(assoc,inf_2B))
          {
          return true;
          }              
       break;
   }

return false;
}

/*****************************************************************************/

void Nova_ShowStoryLine(StoryLine *list,enum storytype type)

{ int pos,len = 0,i,count = 0;
  Chapter *tp;
  char *topic = NULL,*context = NULL,*assoc,previous[CF_MAXVARSIZE];
  StoryLine *ptr,*ptr2;
  Item *check = NULL;

for (ptr = list; ptr != NULL; ptr=ptr->next)
   {
   len = 0;
   strcpy(previous,"this");

   if (ptr->done)
      {
      continue;
      }

   for (i = 0; ptr->story->history[i] != NULL; i++)
      {
      len++;
      }
   
   if (len < 2)
      {
      continue;
      }

   printf("Found something about %s::%s\n\n",ptr->story->history[0]->context,ptr->story->history[0]->topic);

   for (i = 0; ptr->story->history[i] != NULL; i++)
      {
      tp = ptr->story->history[i];
      topic = tp->topic;
      context = tp->context;
      assoc = tp->assoc;
      
      if (count == 0)
         {
         printf(" %s \"%s\" (in %s),\n",assoc,topic,context);
         }
      else if (count == 1)
         {
         printf("   %s \"%s\" (in %s),\n",assoc,topic,context);
         }
      else if (ptr->story->history[i+1] == NULL)
         {
         printf("   moreover this (%s) %s \"%s\" (in %s)\n",previous,assoc,topic,context);
         }
      else 
         {
         printf("   which %s \"%s\" (in %s),\n",assoc,topic,context);
         }
      
      strncpy(previous,topic,CF_MAXVARSIZE);
      count++;
      PrependItem(&check,topic,NULL);
      }

   ptr->done = true;

   // Aggregate similar outcomes - perhaps this could be improved later (difficult)
   
   for (ptr2 = ptr->next; ptr2 != NULL; ptr2=ptr2->next)
      {
      if ((pos = SameStoryDifferentOutcome(ptr->story,ptr2->story)))
         {
         topic = ptr2->story->history[pos]->topic;
         context = ptr2->story->history[pos]->context;
         assoc =ptr2->story->history[pos]->assoc;
         ptr2->done = true;

         if (!IsItemIn(check,topic))
            {
            printf("     and %s \"%s\" (in %s),\n",assoc,topic,context);
            PrependItem(&check,topic,NULL);
            }
         }
      else
         {
         // Because of the pre-ordering we can assume no more possibilities
         break;
         }
      }

   DeleteItemList(check);
   check = NULL;
   
// Addendum - what else could affect the story outcome, coming in from the side?
   
   strcpy(previous,"this");
   
   for (i = 0; ptr->story->history[i] != NULL; i++)
      {
      if (i > 1 && ptr->story->history[i]->indirect_inferences)
         {
         Item *ip;
         
         printf("\n  Note, incidentally, that:\n ");
         
         for (ip = ptr->story->history[i]->indirect_inferences; ip != NULL; ip=ip->next)
            {
            printf("    - %s %s \"%s\", which might also influence the result\n",ptr->story->topic,ip->name,ip->classes);
            }
         }
      }
   
   printf("\nEnd of story\n\n");
   }
}


/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int SameStoryDifferentOutcome(Chapter *s1,Chapter *s2)

{ int i;

for (i = 0; s1->history[i] != NULL && s2->history[i] != NULL; i++)
   {
   if (s1->history[i] != s2->history[i])
      {
      if (s1->history[i+1] == NULL && s2->history[i+1] == NULL)
         {
         return i;
         }
      else
         {
         return false;
         }
      }
   }

return false;
}

/*****************************************************************************/
/* LEVEL                                                                     */
/*****************************************************************************/

char *Nova_Relevant2Inference(char *this,char *table[CF_INFSIZE][2])

{ int i;

for (i = 0; table[i][0] != NULL; i++)
   {
   if (strcmp(this,table[i][0]) == 0)
      {
      return table[i][1];
      }
   }

return NULL;
}

/*****************************************************************************/

char *Nova_Possible3Inference(char *this,char *table[CF_INFSIZE][3])

{ int i;

for (i = 0; table[i][0] != NULL; i++)
   {
   if (strcmp(this,table[i][0]) == 0)
      {
      return table[i][1];
      }
   }

return NULL;
}

/*****************************************************************************/

char *Nova_Relevant3Inference(char *prev,char *this,char *table[CF_INFSIZE][3])
    
{ int i;

if (prev == NULL || this == NULL)
   {
   return NULL;
   }
 
for (i = 0; table[i][0] != NULL; i++)
   {
   if (strcmp(prev,table[i][0]) == 0 && strcmp(this,table[i][1]) == 0)
      {
      return table[i][2];
      }
   }

return NULL;
}

/*****************************************************************************/
/* */
/*****************************************************************************/

int Nova_GenerateStories_by_name_JSON(char *typed_topic,enum storytype type,char *buffer,int bufsize)

{ char topic[CF_BUFSIZE],context[CF_BUFSIZE];
  int topic_id;
  StoryLine *allstories = NULL;
  Chapter *episode;
  
DeClassifyTopic(typed_topic,topic,context);
topic_id = Nova_GetTopicIdForTopic(typed_topic);

if (!topic_id)
   {
   return false;
   }

// Add each starting topic to a root list

episode = NewChapter(topic,context,topic_id);

// Now make a recursive story, priming the tree with this reference

if ((episode = AppendToStory(&episode,topic,context,"begins",topic_id)))
   {
   GenerateStoriesAbout(&episode,type,&allstories,0,cfi_forwards);
   GenerateStoriesAbout(&episode,type,&allstories,0,cfi_backwards);
   }

Nova_ShowStoryLine_JSON(allstories,type,buffer,bufsize);

//DeleteStoryLine(allstories);
// Need to clean up memory here
return true;
}

/*****************************************************************************/

int Nova_GenerateStories_by_id_JSON(int topic_id,enum storytype type,char *buffer,int bufsize)

{ char topic[CF_BUFSIZE],context[CF_BUFSIZE],topid[CF_BUFSIZE];
  Chapter *episode;
  StoryLine *allstories = NULL;
 
Nova_GetTopicByTopicId(topic_id,topic,topid,context,NULL);
  
if (strlen(topic) == 0)
   {
   return false;
   }

// Add each starting topic to a root list

episode = NewChapter(topic,context,topic_id);

// Now make a recursive story, priming the tree with this reference

if ((episode = AppendToStory(&episode,topic,context,"begins",topic_id)))
   {
   GenerateStoriesAbout(&episode,type,&allstories,0,cfi_forwards);
   GenerateStoriesAbout(&episode,type,&allstories,0,cfi_backwards);
   }

Nova_ShowStoryLine_JSON(allstories,type,buffer,bufsize);

//DeleteStoryLine(allstories);
// Need to clean up memory here
return true;
}

/*****************************************************************************/

void Nova_ShowStoryLine_JSON(StoryLine *list,enum storytype type,char *buffer,int bufsize)

{ int pos,len = 0,i,count = 0;
  Chapter *tp;
  char *topic = NULL,*context = NULL,*assoc,previous[CF_MAXVARSIZE];
  StoryLine *ptr,*ptr2;
  int topic_id;
  JsonElement *json_array_out = JsonArrayCreate(100);

// A list of narrative elements = (story = (intro,assoc,topic,id),notes)
  
for (ptr = list; ptr != NULL; ptr=ptr->next)
   {
   len = 0;
   strcpy(previous,"this");

   if (ptr->done)
      {
      continue;
      }

   for (i = 0; ptr->story->history[i] != NULL; i++)
      {
      len++;
      }
   
   if (len < 2)
      {
      continue;
      }

   // STORY
   
   JsonElement *json_narrative_obj = JsonObjectCreate(2);
   JsonElement *json_story_arr = JsonArrayCreate(5);

   for (i = 0; ptr->story->history[i] != NULL; i++)
      {
      tp = ptr->story->history[i];
      topic = tp->topic;
      context = tp->context;
      assoc = tp->assoc;
      topic_id = tp->topic_id;

      // THIS NEEDS TO POINT TO topic_id to link up in the mission portal

      JsonElement *json_obj = JsonObjectCreate(5);

      if (count == 0)
         {
         JsonObjectAppendString(json_obj, "intro", "");       
         }
      else if (count == 1)
         {
         JsonObjectAppendString(json_obj, "intro", "");
         }
      else if (ptr->story->history[i+1] == NULL)
         {
         JsonObjectAppendString(json_obj, "intro", "moreover this");
         }
      else 
         {
         JsonObjectAppendString(json_obj, "intro", "which");
         }

      JsonObjectAppendString(json_obj, "association", assoc);
      JsonObjectAppendString(json_obj, "topic", topic);         
      JsonObjectAppendInteger(json_obj, "topic_id", topic_id);
      JsonObjectAppendString(json_obj, "context", context);

      JsonArrayAppendObject(json_story_arr, json_obj);
      strncpy(previous,topic,CF_MAXVARSIZE);
      count++;
      }

   ptr->done = true;

   // Aggregate similar outcomes - perhaps this could be improved later (difficult)

   for (ptr2 = ptr->next; ptr2 != NULL; ptr2=ptr2->next)
      {
      if ((pos = SameStoryDifferentOutcome(ptr->story,ptr2->story)))
         {
         topic = ptr2->story->history[pos]->topic;
         context = ptr2->story->history[pos]->context;
         assoc = ptr2->story->history[pos]->assoc;
         topic_id = ptr2->story->history[pos]->topic_id;
         ptr2->done = true;

         JsonElement *json_obj = JsonObjectCreate(5);
         JsonObjectAppendString(json_obj, "intro", "and");
         JsonObjectAppendString(json_obj, "association", assoc);
         JsonObjectAppendString(json_obj, "topic", topic);
         JsonObjectAppendInteger(json_obj, "topic_id", topic_id);
         JsonObjectAppendString(json_obj, "context", context);

         JsonArrayAppendObject(json_story_arr, json_obj);
         }
      else
         {
         // Because of the pre-ordering we can assume no more possibilities
         break;
         }
      }

   JsonObjectAppendArray(json_narrative_obj, "story", json_story_arr);

   // Addendum - what else could affect the story outcome, coming in from the side?
   // NOTES
   
   JsonElement *json_notes_arr = JsonArrayCreate(4);
   
   strcpy(previous,"this");

   for (i = 0; ptr->story->history[i] != NULL; i++)
      {
      if (i > 1 && ptr->story->history[i]->indirect_inferences)
         {
         Item *ip;
         
         for (ip = ptr->story->history[i]->indirect_inferences; ip != NULL; ip=ip->next)
            {
            JsonElement *json_obj = JsonObjectCreate(5);
            JsonObjectAppendString(json_obj, "assoc", ip->name);
            JsonObjectAppendString(json_obj, "topic", ptr->story->topic);
            JsonObjectAppendInteger(json_obj, "topic_id", ip->counter);
            JsonObjectAppendString(json_obj, "context", ip->classes);
            JsonArrayAppendObject(json_notes_arr, json_obj);   
            }
         }
      }

   JsonObjectAppendArray(json_narrative_obj, "notes", json_notes_arr);
   JsonArrayAppendObject(json_array_out, json_narrative_obj);   
   }

    Writer *writer = StringWriter();
    JsonElementPrint(writer, json_array_out, 1);
    JsonElementDestroy(json_array_out);
    snprintf(buffer,CF_BUFSIZE-1,"%s", StringWriterData(writer));
    WriterClose(writer);

}

/*****************************************************************************/

static int Nova_GetReportedScalar(const char *hostkey, const char *ns, const char *bundle, const char *lval, char *returnval, int bufsize)
{
    char buffer[CF_BUFSIZE] = { 0 };
    Rlist *rp;
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryVariables(&dbconn, hostkey, ns, bundle, lval, NULL,
                                       NULL, 0, time(NULL), NULL, PROMISE_CONTEXT_MODE_ALL, NULL,
                                       QUERY_FLAG_DISABLE_ALL);

    returnval[0] = '\0';

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        const HubVariable *hv = (HubVariable *) rp->item;

        if (strlen(hv->dtype) > 1)      // list
        {
            char b[CF_BUFSIZE];

            b[0] = '\0';
            RlistPrint(b, CF_BUFSIZE, hv->rval.item);
            snprintf(returnval, bufsize - 1, "%s", b);
        }
        else
        {
            snprintf(returnval, bufsize - 1, "%s", (char *) hv->rval.item);
        }
    }

    if (hq->records == NULL)
    {
        snprintf(buffer, sizeof(buffer), "Unknown value");
    }

    DeleteHubQuery(hq, DeleteHubVariable);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    return true;
}

/*****************************************************************************/

static int Nova_GetReportedList(const char *hostkey, const char *ns, const char *bundle, const char *lval, Rlist **list)
/* This function allocates memory which needs to be deleted afterwards */
{
    char buffer[CF_BUFSIZE] = { 0 };
    Rlist *rp;
    EnterpriseDB dbconn;

    if (!CFDB_Open(&dbconn))
    {
        return false;
    }

    HubQuery *hq = CFDB_QueryVariables(&dbconn, hostkey, ns, bundle, lval, NULL,
                                       NULL, 0, time(NULL), NULL, PROMISE_CONTEXT_MODE_ALL, NULL,
                                       QUERY_FLAG_DISABLE_ALL);

    for (rp = hq->records; rp != NULL; rp = rp->next)
    {
        const HubVariable *hv = rp->item;

        if (strlen(hv->dtype) > 1)      // list
        {
            *list = RvalCopy((Rval) {hv->rval.item, RVAL_TYPE_LIST}).item;
        }
        else
        {
            // Shoud not get here
        }
    }

    if (hq->records == NULL)
    {
        snprintf(buffer, sizeof(buffer), "Unknown value");
    }

    DeleteHubQuery(hq, DeleteHubVariable);

    if (!CFDB_Close(&dbconn))
    {
        CfOut(OUTPUT_LEVEL_VERBOSE, "", "!! Could not close connection to report database");
    }

    return true;
}


