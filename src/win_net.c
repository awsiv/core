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

#define WORKING_BUFFER_SIZE 15000
#define MAX_ALLOCTRIES 3

void NovaWin_GetInterfaceInfo()
/**
 * Creates classes from a host's IP addresses, e.g.  192_168_2_1,
 * ipv4_192_168_2_1, ipv4_192_168_2, ipv4_192_168, ipv4_192 for all
 * the addresses, also variables sys.ipv4[IFNAME] = ipaddr, etc.
 */    
{
 DWORD retVal = 0;
 PIP_ADAPTER_ADDRESSES pAddresses = NULL;
 ULONG outBufLen = 0;
 int allocTries = 0;
 PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
 PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
 char addrBuf[CF_SMALLBUF];
 char buf[CF_SMALLBUF];
 char bufVal[CF_SMALLBUF];
 char ifNameBuf[CF_SMALLBUF];
 int firstIfaceIp4 = true;
 int firstIfaceIp6 = true;
 int stored_ipv4, stored_ipv6;
 char *ifType;
 int tup, j;


 if(!BOOTSTRAP && (THIS_AGENT_TYPE == cf_agent) && !Nova_CheckLicenseWin("NovaWin_GetInterfaceInfo"))
    {
    return;
    }


 outBufLen = WORKING_BUFFER_SIZE;
 
 do
    {
    pAddresses = (IP_ADAPTER_ADDRESSES *)cf_malloc(outBufLen, "NovaWin_GetInterfaceInfo3");

    // get both ipv4 and ipv6 addresses
    retVal = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen);

    if(retVal == ERROR_BUFFER_OVERFLOW)
       {
       free(pAddresses);
       pAddresses = NULL;
       }
    else
       {
       break;
       }

    allocTries++;

    }while((retVal == ERROR_BUFFER_OVERFLOW) && (allocTries < MAX_ALLOCTRIES));


 if(retVal != NO_ERROR)
    {

    if(retVal == ERROR_NO_DATA)
       {
       CfOut(cf_verbose, "", "No network interface information found");
       }
    else
       {
       CfOut(cf_error, "GetAdaptersAddresses", "!! Could not get network interface information");
       }
    
    if (pAddresses)
       {
       free(pAddresses);
       }
    
    return;
    }
 
 pCurrAddresses = pAddresses;
    
 while (pCurrAddresses)
    {
       
    // we only care about ethernet interfaces that are up
    if(pCurrAddresses->IfType == IF_TYPE_ETHERNET_CSMACD &&
       pCurrAddresses->OperStatus == IfOperStatusUp)
      {
          
       stored_ipv4 = false;
       stored_ipv6 = false;
          
       pUnicast = pCurrAddresses->FirstUnicastAddress;
       if (pUnicast == NULL)
          {
          Debug("No unicast address found for the current interface - skipping\n");
          }

       // get at most one unicast address for each ipv4 and ipv6
       for(;(pUnicast != NULL) && !(stored_ipv4 && stored_ipv6); pUnicast = pUnicast->Next)
          {
          
          memset(ifNameBuf, 0, sizeof(ifNameBuf));
          wcstombs(ifNameBuf, pCurrAddresses->FriendlyName, sizeof(ifNameBuf) - 1);

          if(getnameinfo(pUnicast->Address.lpSockaddr, pUnicast->Address.iSockaddrLength,
                         addrBuf, sizeof(addrBuf), NULL, 0, NI_NUMERICHOST) != 0)
             {
             CfOut(cf_error, "getnameinfo", "!! Could not convert ip address to string");
             break;
             }

	  // keep only one address of each type (ipv4/ipv6)
	  if(IsIPV4Address(addrBuf))
	    {
	    if(stored_ipv4)
	      {
   	      continue;
	      }

            ifType = "ipv4";
	    stored_ipv4 = true;
	    }
	  else
	    {
	    if(stored_ipv6)
	      {
   	      continue;
	      }

            ifType = "ipv6";
	    stored_ipv6 = true;
	    }

	  
	  // set the interface name and ipv4 & ipv6 address of first
	  // interface only
          if(firstIfaceIp4 && firstIfaceIp6)
             {
             NewScalar("sys", "interface", CanonifyName(ifNameBuf), cf_str);
             }

	  if((strcmp(ifType, "ipv4") == 0) && firstIfaceIp4)
	    {
	    NewScalar("sys", "ipv4", addrBuf, cf_str);
            firstIfaceIp4 = false;
	    }

	  if((strcmp(ifType, "ipv6") == 0) && firstIfaceIp6)
	    {
	    NewScalar("sys", "ipv6", addrBuf, cf_str);
            firstIfaceIp6 = false;
	    }
	  
	  AppendItem(&IPADDRESSES,addrBuf,"");

          // e.g. sys.ipv4[Local_Area_Connection] = 192.168.2.5
          snprintf(buf, sizeof(buf), "%s[%s]", ifType, CanonifyName(ifNameBuf));
          NewScalar("sys", buf, addrBuf, cf_str);	  

          // class e.g. ipv4_192_168_2_5
          snprintf(buf, sizeof(buf), "%s_%s", ifType, addrBuf);

          NewClass(buf);
             
          // add address part-clasess and vars, if ipv4
          if(strcmp(ifType, "ipv4") == 0)
             {
             tup = 1;

	     for(j = 0; addrBuf[j] != '\0'; j++)
	       {
		 if(addrBuf[j] == '.')
		   {
		   strcpy(bufVal, addrBuf);
		   bufVal[j] = '\0';

		   snprintf(buf, sizeof(buf), "ipv4_%s", bufVal);
                   NewClass(buf);
		   
                   snprintf(buf, sizeof(buf), "ipv4_%d[%s]", tup, CanonifyName(ifNameBuf));
                   NewScalar("sys", buf, bufVal, cf_str);

		   tup++;
		   }
	       }
                
             }
          }
       
        }

    pCurrAddresses = pCurrAddresses->Next;
    }

 if (pAddresses)
    {
    free(pAddresses);
    } 
}

int NovaWin_TryConnect(struct cfagent_connection *conn, struct timeval *tvp, struct sockaddr_in *cinp, int cinpSz)
/** 
 * Tries a nonblocking connect and then restores blocking if
 * successful. Returns true on success, false otherwise.
 **/
{
  int res;
  long arg;
  struct sockaddr_in emptyCin = {0};
  struct timeval tvRecv = {0};
  u_long nonBlock;

  if(!cinp)
    {
      cinp = &emptyCin;
      cinpSz = sizeof(emptyCin);
    }
  

   /* set non-blocking socket */

   nonBlock= true;
   if(ioctlsocket(conn->sd,FIONBIO,&nonBlock) != 0)
     {
     CfOut(cf_error,"ioctlsocket","!! Could not disable socket blocking mode");
     }

   
   res = connect(conn->sd,cinp,cinpSz);

   if (res == SOCKET_ERROR)
      {
	fd_set myset;
	int valopt;
	socklen_t lon = sizeof(int);
         
	FD_ZERO(&myset);
	FD_SET(conn->sd,&myset);

	/* now wait for connect, but no more than tvp.sec */
	res = select(conn->sd + 1, NULL, &myset, NULL, tvp);
	if(getsockopt(conn->sd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) != 0)
	  {
	    CfOut(cf_error,"getsockopt","!! Could not check connection status");
	    return false;
	  }
	
	if (valopt || res <= 0)
	  {
            CfOut(cf_inform,"connect"," !! Error connecting to server (timeout)");
            return false;
	  }
      }


   /* connection is succeed; return to blocking mode */

   nonBlock = false;

   if(ioctlsocket(conn->sd,FIONBIO,&nonBlock) != 0)
     {
     CfOut(cf_error,"ioctlsocket","!! Could not enable socket blocking mode");
     }


   tvRecv.tv_sec = RECVTIMEOUT;
   tvRecv.tv_usec = 0;

   if (setsockopt(conn->sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tvRecv, sizeof(tvRecv)))
      {
      CfOut(cf_inform,"setsockopt","!! Couldn't set socket timeout");
      }
  
  return true;
}



#endif  /* MINGW */
