/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: win_net.c                                                           */
/*                                                                           */
/* Created: Tue Nov 17 14:09:11 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef MINGW

void NovaWin_GetInterfaceInfo3()
/**
 * Creates classes from a host's IP addresses, e.g.  192_168_2_1,
 * ipv4_192_168_2_1, ipv4_192_168_2, ipv4_192_168, ipv4_192 for all
 * the addresses.
 */
{
  char *ip;
  struct hostent *hostinfo;
  char nameBuf[MAXHOSTNAMELEN];
  char ipBuf[MAXIP4CHARLEN + 5];
  int i, j;

  if(gethostname(nameBuf, MAXHOSTNAMELEN) == 0)
    {
      if((hostinfo = gethostbyname(nameBuf)) != NULL)
	{
	  i = 0;
	  while(hostinfo->h_addr_list[i] != NULL)
	    {
	      ip = inet_ntoa(*(struct in_addr *)hostinfo->h_addr_list[i]);
	      AppendItem(&IPADDRESSES,ip,"");
	      NewClass(CanonifyName(ip));

	      snprintf(ipBuf, sizeof(ipBuf), "ipv4_%s", ip);
	      NewClass(CanonifyName(ipBuf));

	      for(j = sizeof(ipBuf) - 1; j > 0; j--)
		{
		  if(ipBuf[j] == '.')
		    {
		      snprintf(ipBuf, j + 1, "ipv4_%s", ip);
		      NewClass(CanonifyName(ipBuf));
		    }
		}

	      i++;
	    }
	}
      else
	{
	  CfOut(cf_error, "gethostbyname", "!! Could not get host entry for local host (%s)", nameBuf);
	}
    }
  else
    {
      CfOut(cf_error, "gethostname", "!! Could not get current hostname");
    }
}


#endif  /* MINGW */
