
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#if defined(HAVE_LIBMONGOC)
#include "db_save.h"
#endif

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

        // Nova2PHP_choose_hosts("123",2,buffer,1000);
        Nova2PHP_get_host_colour(NULL, buffer, 3);
        Nova2PHP_meter(NULL, NULL, 0);

        /* svn helper */
        Nova2PHP_validate_policy(NULL, NULL, 10000);
        /*
         * commenting
         */
        Nova2PHP_add_note(NULL, NULL, NULL, -1, NULL, NULL, 100);
        Nova2PHP_get_notes(NULL, NULL, NULL, NULL, -1, -1, NULL, NULL, 10000);
        Nova2PHP_get_host_noteid(NULL, NULL, 4096);

        Nova2PHP_get_knowledge_view(0, NULL);

        CfLDAPAuthenticate(NULL, NULL, NULL, NULL, false);
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
