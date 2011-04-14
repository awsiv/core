
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/*                                                                           */
/* File: cfnova.c                                                            */
/*                                                                           */
/*****************************************************************************/

void Nova_Initialize()

{
#ifdef HAVE_FIPS_MODE_SET
if (FIPS_MODE && !FIPS_mode_set(1))
   {
   FatalError("Unable to initialize validated FIPS mode");
   }
else
   {
   CfOut(cf_verbose,""," ** Cfengine Nova Operating if FIPS 140-2 validated mode");
   }
#else
if (FIPS_MODE)
   {
   FatalError("This software version has not been built with FIPS mode support");
   }
#endif
}

/*****************************************************************************/

void Nova_SaveDocumentRoot(void)
{
#if defined(HAVE_LIBMONGOC)
if (IsDefinedClass("redhat"))
   {
   CfOut(cf_verbose,""," -> Recording default document root /var/www/html");
   CFDB_PutValue("document_root","/var/www/html");
   }

if (IsDefinedClass("SuSE"))
   {
   CfOut(cf_verbose,""," -> Recording default document root /srv/www/htdocs");
   CFDB_PutValue("document_root","/srv/www/htdocs");
   }

if (IsDefinedClass("debian"))
   {
   CfOut(cf_verbose,""," -> Recording default document root /var/www");
   CFDB_PutValue("document_root","/var/www");
   }
#endif
}

/*****************************************************************************/

void WebCache(char *s, char *t)
{
#if defined(HAVE_LIBMONGOC)
CFDB_PutValue(s,t);
#endif
}

/*****************************************************************************/

void Nova_CheckInstalledLibraries(void)
{
#ifndef HAVE_LIBLDAP
printf("\t->LIBLDAP not found!!\n");
#endif

#ifndef HAVE_FIPS_MODE_SET
printf("\t->FIPS OpenSSL canister not found!!\n");
#endif

#ifndef HAVE_LIBVIRT
printf("\t->LIBVIRT not found!!\n");
#endif
}
