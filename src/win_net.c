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

void NovaWin_GetInterfaceInfo3()
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
 int firstIface = true;
 char *ifType;
 int tup, j;

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
          
       // NOTE: May be both ipv4 and ipv6 interface, handle this ?
          
       pUnicast = pCurrAddresses->FirstUnicastAddress;
       if (pUnicast == NULL)
          {
          Debug("No unicast address found for the current interface - skipping\n");
          }
       else  // contains a unicast address
          {
          
          if(pCurrAddresses->Ipv6IfIndex == 0)  // ipv4 interface
             {
             ifType = "ipv4";
             }
          else // ipv6 interface
             {
             ifType = "ipv6";
             }

          memset(ifNameBuf, 0, sizeof(ifNameBuf));
          wcstombs(ifNameBuf, pCurrAddresses->FriendlyName, sizeof(ifNameBuf) - 1);

          if(getnameinfo(pUnicast->Address.lpSockaddr, pUnicast->Address.iSockaddrLength,
                         addrBuf, sizeof(addrBuf), NULL, 0, NI_NUMERICHOST) != 0)
             {
             CfOut(cf_error, "getnameinfo", "!! Could not convert ip address to string");
             break;
             }
          
          if(firstIface)
             {
             NewScalar("sys", "interface", CanonifyName(ifNameBuf), cf_str);
             NewScalar("sys", ifType, CanonifyName(addrBuf), cf_str);
             firstIface = false;
             }


          // e.g. sys.ipv4[Local_Area_Connection] = 192_168_2_5
          snprintf(buf, sizeof(buf), "%s[%s]", ifType, CanonifyName(ifNameBuf));
          NewScalar("sys", buf, CanonifyName(addrBuf), cf_str);

          // class e.g. ipv4_192_168_2_5
          snprintf(buf, sizeof(buf), "%s_%s", ifType, addrBuf);
          NewClass(CanonifyName(buf));
             
          // add address part-clasess and vars, if ipv4
          if(strcmp(ifType, "ipv4") == 0)
             {
             tup = 3;
                
             for(j = strlen(addrBuf); j > 0; j--)
                {
                if(addrBuf[j] == '.')
                   {
                   snprintf(bufVal, j + 1, "%s", addrBuf);
                      
                   snprintf(buf, sizeof(buf), "ipv4_%s", bufVal);
                   NewClass(CanonifyName(buf));
                                            
                   snprintf(buf, sizeof(buf), "ipv4_%d[%s]", tup, CanonifyName(ifNameBuf));
                   NewScalar("sys", buf, CanonifyName(bufVal), cf_str);

                   tup--;
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


#endif  /* MINGW */
