
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"
#include "db_common.h"
#include "db_save.h"

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
        CfOut(cf_verbose, "", " ** CFEngine Nova is operating in FIPS 140-2 validated mode");
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
        CfOut(cf_verbose, "", " -> Recording default document root /var/www/html");
        CFDB_PutValue("document_root", "/var/www/html", MONGO_SCRATCH);
    }

    if (IsDefinedClass("SuSE"))
    {
        CfOut(cf_verbose, "", " -> Recording default document root /srv/www/htdocs");
        CFDB_PutValue("document_root", "/srv/www/htdocs", MONGO_SCRATCH);
    }

    if (IsDefinedClass("debian"))
    {
        CfOut(cf_verbose, "", " -> Recording default document root /var/www");
        CFDB_PutValue("document_root", "/var/www", MONGO_SCRATCH);
    }
#endif
}

/*****************************************************************************/

void WebCache(char *s, char *t)
{
#if defined(HAVE_LIBMONGOC)
    CFDB_PutValue(s, t, MONGO_SCRATCH);
#endif
}

/*****************************************************************************/

#if defined(__MINGW32__)

bool BootstrapAllowed(void)
{
    return true;
}

#endif

/*****************************************************************************/

void EnterpriseModuleTrick()
{
#ifdef HAVE_LIBMONGOC
    char buffer[CF_MAXVARSIZE];

// Shared library linking requires these functions to appear in something linked to libpromises
// use this for diagnostics? This is linked through enterprise_stubs.c

    if (false)
    {
        //   Nova2PHP_software_report(0,0,0,0,0,cfr_software,NULL,buffer,20);
        //   Nova2PHP_classes_report(NULL,NULL,0,NULL,buffer,1000);
        //   Nova2PHP_vars_report(NULL,NULL,NULL,NULL,NULL,0,NULL,buffer,1000);
        //   Nova2PHP_compliance_promises(NULL,NULL,"x",0,NULL,buffer,10000);
        //   Nova2PHP_lastseen_report(NULL,NULL,NULL,NULL,-1,0,NULL,buffer,10000);
        //   Nova2PHP_bundles_report(NULL,NULL,0,NULL,buffer,10000);
        //   Nova2PHP_performance_report(NULL,NULL,0,NULL,buffer,10000);
        //   Nova2PHP_setuid_report(NULL,NULL,0,NULL,buffer,10000);
        //   Nova2PHP_filechanges_report(NULL,NULL,false,-1,">",NULL,buffer,10000);
        //   Nova2PHP_filediffs_report(NULL,NULL,NULL,false,-1,">",NULL,buffer,10000);
        //   Nova2PHP_value_report(NULL,NULL,NULL,NULL,NULL,buffer,1000);
        //   Nova2PHP_promiselog(NULL,NULL,1,0,0,NULL,buffer,1000);
        //   Nova2PHP_promises(NULL, NULL, NULL, 0);
        //   Nova2PHP_getlastupdate(NULL,buffer,10);

        CFDB_PutValue("one_two", "three", MONGO_SCRATCH);
        CFDB_GetValue("newvar", buffer, 120, MONGO_SCRATCH);
        Nova_Header("123", "123", buffer, 1000);
        Nova_Footer(buffer, 1000);
        // Nova2PHP_choose_hosts("123",2,buffer,1000);
        Nova2PHP_get_host_colour(NULL, buffer, 3);
        Nova_Header("sd", "we", buffer, 3);
        Nova2PHP_meter(NULL, NULL, 0);

        /* svn helper */
        Nova2PHP_validate_policy(NULL, NULL, 10000);
        /*
         * commenting
         */
        Nova2PHP_add_new_note(NULL, NULL, -1, NULL, -1, NULL, NULL, 100);
        Nova2PHP_add_note(NULL, NULL, -1, NULL, NULL, 100);
        Nova2PHP_get_notes(NULL, NULL, NULL, -1, -1, NULL, NULL, 10000);
        Nova2PHP_get_host_noteid(NULL, NULL, 4096);

        Nova2PHP_get_knowledge_view(0, NULL, NULL, 999);

        CfLDAPAuthenticate(NULL, NULL, NULL, NULL, false);
        Nova2PHP_cdp_report(NULL, NULL, NULL, NULL, 10000);
    }
#endif

#if defined(HAVE_CONSTELLATION)
    Constellation_EnterpriseModuleTrick();
#endif
}

/*****************************************************************************/

const char *GetConsolePrefix(void)
{
#if defined(HAVE_CONSTELLATION)
    return "constellation";
#else
    return "nova";
#endif
}

/*****************************************************************************/

const char *MailSubject(void)
{
    if (LICENSES)
    {
        return "Nova";
    }
    else
    {
        return "NO LICENSE";
    }
}
