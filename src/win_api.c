/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: win_api.c                                                           */
/*                                                                           */
/* Created: Fri Sep 11 13:13:29 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#ifdef MINGW
/* ------ BEGIN TEMP DEFINES ------ */

typedef unsigned long uid_t;
typedef unsigned long gid_t;
typedef unsigned short mode_t;
typedef int	pid_t;

void nw_exper(char *fname, char *msg);
void nw_unimpl(char *fname);

void nw_exper(char *fname, char *msg)
{
  //printf("MINGW: WARNING: Function '%s' is experimental: %s.\n", fname, msg);
  printf("%s(exp)\n", fname);  
}

void nw_unimpl(char *fname)
{
  //printf("MINGW: ERROR: Function '%s' is not implemented.\n", fname);
  printf("%s(uni)\n", fname);
}

/* ------ END TEMP DEFINES ------ */


gid_t getgid(void)
{
  nw_exper("getgid", "always returns 0 (assumes process is run by group root)");
  return 0;
}


uid_t getuid(void)
{
  nw_exper("getuid", "always returns 0 (assumes process is run by root/Administrator)");
  return 0;
}


unsigned int sleep(unsigned int seconds)
{
  // Note: Use SleepEx() if it should be alertable when waiting for IO
  // return values differ when signaled
  nw_exper("sleep", "signals are ignored when sleeping");
  Sleep((seconds)*1000);

  // on windows, the sleep seconds have always elapsed when we return (i.e. no signals are received)
  return 0;
}


/* Change owner of file pointed to by path. If owner and group id are 0,
 * the owner is set to the "BUILTIN\Administrators" group. Otherwise, the function fails.
 * Returns 0 on success, and -1 on failure. */
int chown(const char *path, uid_t owner, gid_t group)
{
  nw_exper("chown", "needs testing");

  PSID sidAdmins = NULL;
  SID_IDENTIFIER_AUTHORITY SIDAuthNT = {SECURITY_NT_AUTHORITY};
  int retv;

  if(owner == 0 && group == 0)
    {
      // change owner to the "BUILTIN\Administrators" group
      if (!AllocateAndInitializeSid(&SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &sidAdmins))
	{
	  CfOut(cf_error,"AllocateAndInitializeSid","Could not allocate sid");
	  return -1;
	}
  
      retv = SetNamedSecurityInfo(path, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, sidAdmins, NULL, NULL, NULL);

      if(retv != ERROR_SUCCESS)
	{
	  CfOut(cf_error,"SetNamedSecurityInfo","Could not change owner to BUILTIN\\Administrators");
	  FreeSid(sidAdmins);
	  return -1;
	}

      if(FreeSid(sidAdmins))
	{
	  CfOut(cf_error,"FreeSid","Could not free sid");
	}
      
      Debug("Owner of %s is set to BUILTIN\\Administrators\n", path);

      return 0;
    }
  else
    {
      CfOut(cf_error,"chown","Owner and group are not both 0: such numerical user or group ids makes no sense on NT");
      return -1;
    }
}


long int random(void)
{
  nw_exper("random", "using rand() from CRT");
  return rand();
}


void srandom(unsigned int seed)
{
  nw_exper("srandom", "using srand() from CRT");
  srand(seed);
}


void setlinebuf(FILE *stream)
{
  nw_exper("setlinebuf", "using NT setvbuf()");

  setvbuf(stream, (char *)NULL, _IOLBF, 0);
}


/* Start up Winsock */
void NovaWin_OpenNetwork(void)
{ 
struct WSAData wsaData;
int nCode;

nCode = WSAStartup(MAKEWORD(2, 2), &wsaData);

if (nCode != 0)
  {
  CfOut(cf_error,"","Winsock could not be initialized: WSAStartup() returned error code %d.\n", nCode);
  }
else
  {
  CfOut(cf_verbose,"","Windows sockets successfully initialized.\n"); 
  }
}

/* TODO: move the functions below to other files ? */

/* Get user name of owner of this process */
int NovaWin_GetCurrentUserName(char *userName, int userNameLen)
{
  DWORD userNameMax = (DWORD)userNameLen;
  
  if(!GetUserName(userName, &userNameMax))
    {
      CfOut(cf_error,"GetUserName","Could not get user name of current process, using \"UNKNOWN\"");

      strncpy(userName, "UNKNOWN", userNameLen);
      userName[userNameLen - 1] = '\0';
      return false;
    }
  
  return true;
}


char *NovaWin_GetErrorStr(void)
{
  static char errbuf[CF_BUFSIZE];
  int len;

  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, errbuf, CF_BUFSIZE, NULL);

  // remove CRLF from end
  len = strlen(errbuf);
  errbuf[len-2] = errbuf[len-1] = '\0';
  
  return errbuf;
}

#endif  /* MINGW */
