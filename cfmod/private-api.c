#include "private-api.h"

#include "common.h"
#include "rlist.h"
#include "cf.nova.web_api.h"
#include "web_rbac.h"


#define cfr_software     "sw"
#define cfr_patch_avail  "pa"
#define cfr_patch_installed "pi"

static time_t DeltaHrsConvert(long hrsAgo);
char **String2StringArray(char *str, char separator);
void FreeStringArray(char **strs);
static bool ParseVitalsArgs(int argc, char **retHostKey, char **retVitalId);
static JsonElement *ParseRolesToJson(HubQuery *hq);


/******************************************************************************/
/* API                                                                        */
/******************************************************************************/

PHP_FUNCTION(cfpr_ldap_authenticate)
{
char *host,*dn,*passwd;
int h_len, d_len, p_len;
const char *errmsg;
zend_bool starttls = false;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|b",&host,&h_len,&dn,&d_len,&passwd,&p_len,&starttls) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

if (Nova2PHP_LDAPAuthenticate(host, dn, passwd, starttls, &errmsg) == true)
   {
   RETURN_LONG(true);
   }
else
   {
   if (errmsg == NULL)
      {
      /* Authentication failed */
      RETURN_LONG(false);
      }
   else
      {
      zend_throw_exception(cfmod_ldap_exception_ce, (char *)errmsg, 0 TSRMLS_CC);
      }
   }
}

/******************************************************************************/

PHP_FUNCTION(cfpr_ldap_get_single_attribute_list)


/* cfpr_ldap_get_single_attribute_list(
                "ldap://10.0.0.100",
                "uid=sudhir",
                "cn=users,dc=cf022osx,dc=cfengine,dc=com",
                "(uid=sudhir)",
                "givenName",
                "subtree",
                "sasl",
                "q1w2e3r4t5",
                FALSE,
                1,
                100) == 0)
                {
                printf("JSON: %s\n",buffer);
                }
*/

{
char *uri,*basedn,*filter,*name,*scopes,*sec,*passwd,*user;
int u_len, d_len, f_len,n_len,s_len,ss_len,p_len,page,linesperpage,uu_len;
char buffer[1000000];
const char *errmsg;
zend_bool starttls = false;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssssssll|b",
                          &uri,&u_len,
                          &user,&uu_len,
                          &basedn,&d_len,
                          &filter,&f_len,
                          &name,&n_len,
                          &scopes,&s_len,
                          &sec,&ss_len,
                          &passwd,&p_len,
                          &page,
                          &linesperpage,
                          &starttls
                          ) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

buffer[0]='\0';

if (Nova2PHP_LDAPGetSingleAttributeList(uri,user,basedn,filter,name,scopes,sec,passwd,starttls,page,linesperpage,buffer,1000000,&errmsg) == 0)
   {
   RETURN_STRING(buffer,1);
   }
else
   {
   zend_throw_exception(cfmod_ldap_exception_ce, (char *)errmsg, 0 TSRMLS_CC);
   }
}

/******************************************************************************/

PHP_FUNCTION(cfpr_ldap_get_several_attributes)

/*

cfpr_ldap_get_several_attributes(
               "ldap://10.0.0.152",
               "uid=sudhir",
               "ou=people,dc=cfengine,dc=com",
               "(|(objectClass=organizationalPerson)(objectClass=inetOrgPerson))" ,
               "uid,mail,sn,altSecurityIdentities", // search ids
               "subtree",
               "sasl",
               "password")

*/

{
Rlist *names = NULL;
char *uri,*basedn,*filter,*name,*scopes,*sec,*passwd,*user;
int ret,u_len, d_len, f_len,n_len,s_len,ss_len,p_len,page,linesperpage,uu_len;
char buffer[1000000];
const char *errstr;
zend_bool starttls;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssssssll|b",
                          &uri,&u_len,
                          &user,&uu_len,
                          &basedn,&d_len,
                          &filter,&f_len,
                          &name,&n_len,
                          &scopes,&s_len,
                          &sec,&ss_len,
                          &passwd,&p_len,
                          &page,
                          &linesperpage,
                          &starttls
                          ) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

names = SplitStringAsRList(name,',');

buffer[0]='\0';
ret = Nova2PHP_LDAPGetSeveralAttributes(uri,user,basedn,filter,names,scopes,sec,passwd,starttls,page,linesperpage,buffer,1000000, &errstr);

DeleteRlist(names);

if (ret == 0)
   {
   RETURN_STRING(buffer,1);
   }
else
   {
   zend_throw_exception(cfmod_ldap_exception_ce, (char *)errstr, 0 TSRMLS_CC);
   }
}

/******************************************************************************/

PHP_FUNCTION(cfpr_enterprise_version)

{ const int bufsize = 256;
 char buf[bufsize];

 buf[0] = '\0';

 Nova2PHP_enterprise_version(buf, bufsize);

 RETURN_STRING(buf,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_community_version)

{
 const int bufsize = 256;
 char buf[bufsize];

 buf[0] = '\0';
 Nova2PHP_community_version(buf,bufsize);
 RETURN_STRING(buf,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_getlicense_summary)

{ char s[4096] = {0};

 s[0] = '\0';

 CFDB_GetValue("license_report",s,sizeof(s),MONGO_SCRATCH);

 if (strlen(s) < 2)
    {
    snprintf(s,sizeof(s),"{\"error\":\"Too early to retrieve license details, please try again later\"}");
    }

 RETURN_STRING(s,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_hub_key)

{ const int bufsize = 4096;
 char buffer[bufsize];

 buffer[0] = '\0';
 if( Nova2PHP_GetHubKey(buffer,bufsize))
    {
      RETURN_STRING(buffer,1);
    }

 RETURN_STRING("Cannot find Hub hostkey!",1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_get_hub_master)

{ const int bufsize = 4096;
 char buffer[bufsize];

 buffer[0] = '\0';

 if( Nova2PHP_GetHubMaster(buffer,bufsize))
    {
      RETURN_STRING(buffer,1);
    }

 RETURN_STRING("Unknown Hub Master!",1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_getlicense_expiry)

{ char s[4096];

 s[0] = '\0';

 CFDB_GetValue("license_expires",s,sizeof(s),MONGO_SCRATCH);

 if (strlen(s) < 2)
    {
    snprintf(s,sizeof(s),"no license");
    }

 RETURN_STRING(s,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_getlicense_installtime)

{ char s[4096];

 s[0] = '\0';

 CFDB_GetValue("license_installtime",s,sizeof(s),MONGO_SCRATCH);

 if (strlen(s) < 2)
    {
    snprintf(s,sizeof(s),"no license");
    }

 RETURN_STRING(s,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_getlicense_owner)

{ char s[4096];

 s[0] = '\0';
 CFDB_GetValue("license_owner",s,sizeof(s),MONGO_SCRATCH);

 if (strlen(s) < 3)
    {
    snprintf(s,sizeof(s),"unknown licensee");
    }

 RETURN_STRING(s,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_getlicenses_promised)

{ char s[4096];
 int i = -1;

 s[0] = '\0';
 CFDB_GetValue("licenses_promised",s,sizeof(s),MONGO_SCRATCH);

 sscanf(s,"%d",&i);
 if (i == -1)
    {
    snprintf(s,sizeof(s),"No licenses promised");
    }

 RETURN_STRING(s,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_getlicenses_granted)

{ char s[4096];
 int i = -1;

 CFDB_GetValue("licenses_granted",s,sizeof(s),MONGO_SCRATCH);
 sscanf(s,"%d",&i);
 if (i == -1)
    {
    snprintf(s,sizeof(s),"No license granted!");
    }

 RETURN_STRING(s,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hostname)

{ char s1[4096],s2[4096];
 char *hostkey;
 int hk_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 s1[0]='\0';
 s2[0]='\0';
 Nova2PHP_hostinfo(hostkey,s1,s2,sizeof(s1));

 RETURN_STRING(s1,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_ipaddr)

{ char s1[4096],s2[4096];
 char *hostkey;
 int hk_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 s1[0]='\0';
 s2[0]='\0';
 Nova2PHP_hostinfo(hostkey,s1,s2,sizeof(s1));
 RETURN_STRING(s2,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summary_meter)

{ const int bufsize = 512*1024;
 char buffer[bufsize];

 buffer[0]='\0';
 Nova2PHP_summary_meter(buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_host_meter)

{ char *hostkey,*fhostkey;
 int hk_len;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;

 buffer[0]='\0';
 Nova2PHP_meter(fhostkey,buffer,sizeof(buffer));
 RETURN_STRING(buffer,1);
}

/*****************************************************************************/
/* Vitals functions                                                          */
/*****************************************************************************/

PHP_FUNCTION(cfpr_vitals_list)
{
 char *hostkey;
 int hk_len;
 char buffer[4096];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(hk_len);

 buffer[0]='\0';
 Nova2PHP_vitals_list(hostkey, buffer, sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_magnified)

{ char *hostkey, *vitalId;
 int hk_len, vi_len;
 char buffer[1024*1024];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 // only support one host and vital at this time
 ARGUMENT_CHECK_CONTENTS(hk_len && vi_len);

 buffer[0]='\0';
 Nova2PHP_vitals_view_magnified(hostkey, vitalId, buffer, sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_week)

{ char *hostkey, *vitalId;
 int hk_len, vi_len;
 char buffer[1024*1024];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 // only support one host and vital at this time
 ARGUMENT_CHECK_CONTENTS(hk_len && vi_len);

 buffer[0]='\0';
 Nova2PHP_vitals_view_week(hostkey, vitalId, buffer, sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_year)

{ char *hostkey, *vitalId;
 int hk_len, vi_len;
 char buffer[1024*1024];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 // only support one host and vital at this time
 ARGUMENT_CHECK_CONTENTS(hk_len && vi_len);
 

 buffer[0]='\0';
 Nova2PHP_vitals_view_year(hostkey, vitalId, buffer, sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_histogram)

{ char *hostkey, *vitalId;
 int hk_len, vi_len;
 char buffer[1024*1024];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 // only support one host and vital at this time
 ARGUMENT_CHECK_CONTENTS(hk_len && vi_len);
 
 buffer[0]='\0';
 Nova2PHP_vitals_view_histogram(hostkey, vitalId, buffer, sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_magnified)
{
 char *hostkey, *vitalId;
 char buffer[4096];

 if(!ParseVitalsArgs(ZEND_NUM_ARGS() TSRMLS_CC,&hostkey, &vitalId))
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_analyse_magnified(hostkey, vitalId, buffer, sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_week)

{ char *hostkey, *vitalId;

 const int bufsize = 4096;
 char buffer[bufsize];

 if(!ParseVitalsArgs(ZEND_NUM_ARGS() TSRMLS_CC,&hostkey, &vitalId))
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_analyse_week(hostkey, vitalId, buffer, bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_year)

{ char *hostkey, *vitalId;

 const int bufsize = 4096;
 char buffer[bufsize];

 if(!ParseVitalsArgs(ZEND_NUM_ARGS() TSRMLS_CC,&hostkey, &vitalId))
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_analyse_year(hostkey, vitalId, buffer, bufsize);

 RETURN_STRING(buffer,1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_histogram)

{ char *hostkey, *vitalId;

 const int bufsize = 4096;
 char buffer[bufsize];

 if(!ParseVitalsArgs(ZEND_NUM_ARGS() TSRMLS_CC,&hostkey, &vitalId))
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_analyse_histogram(hostkey, vitalId, buffer, bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

static bool ParseVitalsArgs(int argc, char **retHostKey, char **retVitalId)
{
 char *hostkey,*fhostkey, *vitalId, *fvitalId;
 int hk_len, vi_len;

 if (zend_parse_parameters(argc, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    return false;
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fvitalId =  (vi_len == 0) ? NULL : vitalId;

 // only support one host and vital at this time
 if(!fhostkey)
    {
    php_printf("ParseVitalsParams: error: hostkey is undefined\n");
    return false;
    }

  if(!fvitalId)
    {
    php_printf("ParseVitalsParams: error: vitalid is undefined\n");
    return false;
    }

  *retHostKey = fhostkey;
  *retVitalId = fvitalId;

  return true;
}

/******************************************************************************/
/* Returning tabular html data                                                */
/******************************************************************************/

PHP_FUNCTION(cfpr_report_software_in)
{
 char *userName, *hostkey,*name,*version,*arch,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int user_len, hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 int use_reg;
 char buffer[CF_WEBBUFFER];
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssbssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &name, &n_len,
                           &version, &v_len,
                           &arch, &a_len,
                           &regex,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);
 
 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_software_report(fhostkey, fname, fversion, farch, use_reg, cfr_software, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_patch_in)
{
 char *userName, *hostkey,*name,*version,*arch,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int user_len, hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 char buffer[CF_WEBBUFFER];
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssbssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &name, &n_len,
                           &version, &v_len,
                           &arch, &a_len,
                           &regex,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg= (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);

 Nova2PHP_software_report(fhostkey, fname, fversion, farch, use_reg, cfr_patch_installed, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_patch_avail)
{
 char *userName, *hostkey,*name,*version,*arch,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int user_len, hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 char buffer[CF_WEBBUFFER];
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssbssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &name, &n_len,
                           &version, &v_len,
                           &arch, &a_len,
                           &regex,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_software_report(fhostkey, fname, fversion, farch, use_reg, cfr_patch_avail, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_classes)
{
 char *userName, *hostkey,*name,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int user_len, hk_len, n_len,cr_len;
 long regex;
 char buffer[CF_WEBBUFFER];
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &name, &n_len,
                           &regex,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }
 
 ARGUMENT_CHECK_CONTENTS(user_len);

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';
 
 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_classes_report(fhostkey, fname, regex, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_list_time_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,cr_len;
 long regex;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_time(fhostkey,fname,regex,fclassreg,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_list_host_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,cr_len;
 long regex;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_host(fhostkey,fname,regex,fclassreg,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_list_all_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,cr_len;
 long regex;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_all(fhostkey,fname,regex,fclassreg,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_list_soft_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,cr_len;
 long regex;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_soft(fhostkey,fname,regex,fclassreg,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_list_ip_classes)
//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,cr_len;
 long regex;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_ip(fhostkey,fname,regex,fclassreg,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_class_cloud)

{ char*classes;
 char *fclasses;
 int cl_len;
 const int bufsize = 512*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&classes,&cl_len) == FAILURE)
    {
    RETURN_NULL();
    }

 fclasses =  (cl_len == 0) ? NULL : classes;

 char **classArray = String2StringArray(fclasses, ',');

 buffer[0]='\0';
 Nova2PHP_classes_summary(classArray, buffer, sizeof(buffer));

 FreeStringArray(classArray);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_vars)
{
 char *userName, *hostkey,*scope,*lval,*rval,*type,*classreg;
 char *fhostkey,*fscope,*flval,*frval,*ftype,*fclassreg;
 int user_len, hk_len,s_len,l_len,r_len,t_len,use_reg,cr_len;
 zend_bool regex;
 char buffer[CF_WEBBUFFER];
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssssbssbll",
                           &userName, &user_len,
                           &hostkey,&hk_len,
                           &scope,&s_len,
                           &lval,&l_len,
                           &rval,&r_len,
                           &type,&t_len,
                           &regex,
                           &classreg,&cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fscope =  (s_len == 0) ? NULL : scope;
 flval = (l_len == 0) ? NULL : lval;
 frval = (r_len == 0) ? NULL : rval;
 ftype = (t_len == 0) ? NULL : type;
 fclassreg = (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_vars_report(fhostkey, fscope, flval, frval, ftype, use_reg, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_compliance_summary_graph)

{ const int bufsize = 512*1024;
 char buffer[bufsize];
 char *policy, *fpolicy;
 int pol_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&policy,&pol_len) == FAILURE)
    {
    RETURN_NULL();
    }

// "any" is keyword for all policies

 fpolicy =  (pol_len == 0) ? "any" : policy;

 buffer[0]='\0';
 Nova2PHP_ComplianceSummaryGraph(fpolicy,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_compliance_summary)
{
 char *userName, *hostkey,*version,*cmp,*classreg;
 char *fhostkey,*fversion;
 int user_len, hk_len,v_len,cmp_len,cr_len;
 long k,nk,r,t;
 char buffer[CF_WEBBUFFER];
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllllsssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &version, &v_len,
                           &t,
                           &k,
                           &nk,
                           &r,
                           &cmp, &cmp_len,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fversion = (v_len == 0) ? NULL : version;
 char *fclassreg = (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_compliance_report(fhostkey, fversion, (time_t)t, (int)k, (int)nk, (int)r, cmp, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_compliance_promises)
{
 char *userName, *hostkey,*handle,*status,*classreg;
 char *fhostkey,*fhandle,*fstatus,*fclassreg;
 int hk_len,h_len,s_len,cr_len;
 char buffer[CF_WEBBUFFER];
 zend_bool regex;
 int user_len, use_reg;
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &handle, &h_len,
                           &status, &s_len,
                           &regex,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fstatus =  (s_len == 0) ? NULL : status;
 fclassreg =  (s_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);
 
 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_compliance_promises(fhostkey, fhandle, fstatus, use_reg, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_overall_summary)
{
 char *hostkey,*handle,*status,*classreg;
 char *fhostkey,*fhandle,*fstatus,*fclassreg;
 int hk_len,h_len,s_len,cr_len;
 char buffer[CF_WEBBUFFER];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbs",
                           &hostkey,&hk_len,&handle,&h_len,&status,&s_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fstatus =  (s_len == 0) ? NULL : status;
 fclassreg =  (s_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_summary_report(fhostkey,fhandle,fstatus,use_reg,fclassreg,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_lastseen)
{
 char *userName, *hostkey,*host,*address,*hash,*classreg;
 char *fhostkey,*fhost,*faddress,*fhash,*fclassreg;
 int user_len, hk_len,h_len,a_len,h2_len,cr_len;
 char buffer[CF_WEBBUFFER];
 long ago;
 time_t tago;
 zend_bool regex;
 int use_reg;
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssslbssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &hash, &h2_len,
                           &host, &h_len,
                           &address, &a_len,
                           &ago,
                           &regex,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 tago = (time_t)ago;
 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhash =  (h2_len == 0) ? NULL : hash;
 fhost =  (h_len == 0) ? NULL : host;
 faddress =  (a_len == 0) ? NULL : address;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_lastseen_report(fhostkey, fhash, fhost, faddress, tago,use_reg, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);

}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_performance)
{
 char *userName, *hostkey,*job,*classreg;
 char *fhostkey,*fjob,*fclassreg;
 int user_len, hk_len,j_len,cr_len;
 char buffer[CF_WEBBUFFER];
 zend_bool regex;
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &job, &j_len,
                           &regex,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fjob =  (j_len == 0) ? NULL : job;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_performance_report(fhostkey, fjob, regex, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_setuid)
{
 char *userName, *hostkey,*file,*classreg;
 char *fhostkey,*ffile,*fclassreg;
 int user_len, hk_len,j_len,cr_len;
 char buffer[CF_WEBBUFFER];
 zend_bool regex;
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &file, &j_len,
                           &regex,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (j_len == 0) ? NULL : file;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_setuid_report(fhostkey, ffile, regex, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filechanges)
{
 char *userName, *hostkey,*file,*cmp,*classreg;
 char *fhostkey,*ffile,*fclassreg;
 int user_len, hk_len,f_len,c_len,cr_len;
 char buffer[CF_WEBBUFFER];
 zend_bool regex;
 long t;
 time_t then;
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssblsssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &file, &f_len,
                           &regex,
                           &t,
                           &cmp, &c_len,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);
 
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_filechanges_report(fhostkey, ffile, regex, then, cmp, filter, &page, false, buffer,sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filediffs)
{
 char *userName, *hostkey,*file,*cmp,*diff,*classreg;
 char *fhostkey,*ffile,*fdiff,*fclassreg;
 int user_len, hk_len,f_len,c_len,d_len,cr_len;
 char buffer[CF_WEBBUFFER];
 zend_bool regex;
 int use_reg;
 long t;
 time_t then;
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssblsssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &file, &f_len,
                           &diff, &d_len,
                           &regex,
                           &t,
                           &cmp, &c_len,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 use_reg = (int)regex;
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fdiff =  (d_len == 0) ? NULL : diff;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_filediffs_report(fhostkey, ffile, fdiff, use_reg, then, cmp, filter, &page, false, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filechanges_longterm)
{
 char *userName, *hostkey,*file,*cmp,*classreg;
 char *fhostkey,*ffile,*fclassreg;
 int user_len, hk_len,f_len,c_len,cr_len;
 char buffer[CF_WEBBUFFER];
 zend_bool regex;
 long t;
 time_t then;
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssblsssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &file, &f_len,
                           &regex,
                           &t,
                           &cmp, &c_len,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending, 
                           &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_filechanges_report(fhostkey, ffile, regex, then, cmp, filter, &page, true, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filediffs_longterm)
{
 char *userName, *hostkey,*file,*cmp,*diff,*classreg;
 char *fhostkey,*ffile,*fdiff,*fclassreg;
 int user_len, hk_len,f_len,c_len,d_len,cr_len;
 char buffer[CF_WEBBUFFER];
 zend_bool regex;
 int use_reg;
 long t;
 time_t then;
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssblsssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &file, &f_len,
                           &diff, &d_len,
                           &regex,
                           &t,
                           &cmp, &c_len,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 use_reg = (int)regex;
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fdiff =  (d_len == 0) ? NULL : diff;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_filediffs_report(fhostkey, ffile, fdiff, use_reg, then, cmp, filter, &page, true, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_details)
{
 char *userName, *handle;
 int user_len, handle_len;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                           &userName, &user_len,
                           &handle, &handle_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len && handle_len);
 
 
 HubQuery *hqPromiseFilter = CFBD_PromiseFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

 PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);
 PromiseFilterAddPromiseBody(filter, handle, NULL);

 buffer[0]='\0';
 Nova2PHP_promise_details(filter, buffer, sizeof(buffer));

 DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_bundlesseen)
{
 char *userName, *hostkey,*bundle,*classreg;
 char *fhostkey,*fbundle,*fclassreg;
 int user_len, hk_len,j_len,cr_len;
 char buffer[CF_WEBBUFFER];
 zend_bool regex;
 int use_reg;
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &bundle, &j_len,
                           &regex,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fbundle =  (j_len == 0) ? NULL : bundle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';
 
 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_bundle_report(fhostkey, fbundle, use_reg, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_value)
{
 char *userName, *hostkey,*day,*month,*year,*classreg;
 char *fhostkey,*fmonth,*fday,*fyear,*fclassreg;
 int user_len, hk_len,d_len,m_len,y_len,cr_len;
 char buffer[CF_WEBBUFFER];
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &day, &d_len,
                           &month, &m_len,
                           &year, &y_len,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fday =  (d_len == 0) ? NULL : day;
 fmonth =  (m_len == 0) ? NULL : month;
 fyear =  (y_len == 0) ? NULL : year;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_value_report(fhostkey, fday, fmonth, fyear, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_value_graph)

{ char *hostkey,*day,*month,*year,*classreg;
 char *fhostkey,*fmonth,*fday,*fyear,*fclassreg;
 int hk_len,d_len,m_len,y_len,cr_len;
 const int bufsize = 10000;
 char buffer[bufsize];
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssss",
                           &hostkey,&hk_len,&day,&d_len,&month,&m_len,&year,&y_len,&classreg,&cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fday =  (d_len == 0) ? NULL : day;
 fmonth =  (m_len == 0) ? NULL : month;
 fyear =  (y_len == 0) ? NULL : year;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_get_value_graph(fhostkey,fday,fmonth,fyear,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_notkept)
{
 char *userName, *hostkey, *handle, *classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int user_len, hk_len, h_len, cr_len;
 char buffer[CF_WEBBUFFER];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &handle, &h_len,
                           &hours_deltafrom,
                           &hours_deltato,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage), &(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 // convert delta hours to absolute time (deltato is oldest)
 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);
 
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_promiselog(fhostkey, fhandle, PROMISE_LOG_STATE_NOTKEPT, from, to, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_repaired)

{ char *userName, *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int user_len, hk_len,h_len,cr_len;
 char buffer[CF_WEBBUFFER];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &handle, &h_len,
                           &hours_deltafrom, &hours_deltato,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

// convert delta hours to absolute time (deltato is oldest)

 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_promiselog(fhostkey,fhandle,PROMISE_LOG_STATE_REPAIRED,from,to,filter,&page,buffer,sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summarize_notkept)
{
 char *userName, *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int user_len,hk_len,h_len,cr_len;
 char buffer[CF_WEBBUFFER];
 time_t from,to;
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &handle, &h_len,
                           &from, &to,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_promiselog_summary(fhostkey, fhandle, PROMISE_LOG_STATE_NOTKEPT, from, to, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summarize_repaired)
{
 char *userName, *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int user_len, hk_len,h_len,cr_len;
 char buffer[CF_WEBBUFFER];
 time_t from,to;
 PageInfo page = {0};
 char *sortColumnName;
 char *fsortColumnName;
 int sc_len;
 bool sortDescending;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllssbll",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &handle, &h_len,
                           &from, &to,
                           &classreg, &cr_len,
			   &sortColumnName, &sc_len, &sortDescending,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
 fsortColumnName =  (sc_len == 0) ? NULL : sortColumnName;

 buffer[0]='\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_promiselog_summary(fhostkey, fhandle, PROMISE_LOG_STATE_REPAIRED, from, to, filter, &page, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
 
 RETURN_STRING(buffer,1);
}


/******************************************************************************/
/* Returning hosts                                                            */
/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_software_in)
{
 char *userName, *hostkey,*name,*version,*arch,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int user_len, hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 char buffer[512*1024];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssbs",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &name, &n_len,
                           &version, &v_len,
                           &arch, &a_len,
                           &regex,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_software_hosts(fhostkey, fname, fversion, farch, use_reg, cfr_software, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_value)
{
 char *userName, *hostkey,*day,*month,*year,*classreg;
 char *fhostkey,*fmonth,*fday,*fyear,*fclassreg;
 int user_len, hk_len,d_len,m_len,y_len,cr_len;
 const int bufsize = 512*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssss",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &day, &d_len,
                           &month, &m_len,
                           &year, &y_len,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fday =  (d_len == 0) ? NULL : day;
 fmonth =  (m_len == 0) ? NULL : month;
 fyear =  (y_len == 0) ? NULL : year;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_value_hosts(fhostkey,fday,fmonth,fyear,filter,buffer,bufsize);
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_patch_in)
{
 char *userName, *hostkey,*name,*version,*arch,*classreg;
 char *fclassreg;
 int user_len, hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 char buffer[512*1024];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssbs",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &name, &n_len,
                           &version, &v_len,
                           &arch, &a_len,
                           &regex,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 char *fhostkey = (hk_len == 0) ? NULL : hostkey;
 char *fname = (n_len == 0) ? NULL : name;
 char *fversion = (v_len == 0) ? NULL : version;
 char *farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_software_hosts(fhostkey, fname, fversion, farch, regex, cfr_patch_installed, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_patch_avail)
{
 char *userName, *hostkey,*name,*version,*arch,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int user_len, hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 char buffer[512*1024];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssbs",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &name, &n_len,
                           &version, &v_len,
                           &arch, &a_len,
                           &regex,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_software_hosts(fhostkey, fname, fversion, farch, use_reg, cfr_patch_avail, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_classes)
{
 char *userName, *hostkey,*name,*classreg;
 int user_len, hk_len, n_len,cr_len;
 long regex;
 char buffer[512*1024];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbs",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &name, &n_len,
                           &regex,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 char *fhostkey =  (hk_len == 0) ? NULL : hostkey;
 char *fname =  (n_len == 0) ? NULL : name;
 char *fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);
 
 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_classes_hosts(fhostkey, fname, regex, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_repaired)

//$ret = cfpr_hosts_with_repaired($hostkey,$name,$hours_deltafrom,$hours_deltato,$class_regex);

{ char *userName, *hostkey,*handle,*classreg;
 int user_len, hk_len, h_len,cr_len;
 char buffer[512*1024];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssslls",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &handle, &h_len,
                           &hours_deltafrom, &hours_deltato,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

// convert delta hours to absolute time (deltato is oldest)
 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);

 char *fhostkey = (hk_len == 0) ? NULL : hostkey;
 char *fhandle = (h_len == 0) ? NULL : handle;
 char *fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_promiselog_hosts(fhostkey, fhandle, PROMISE_LOG_STATE_REPAIRED, from, to, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_notkept)
{
 char *userName, *hostkey,*handle,*classreg;
 int user_len, hk_len, h_len,cr_len;
 char buffer[512*1024];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssslls",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &handle, &h_len,
                           &hours_deltafrom,
                           &hours_deltato,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

// convert delta hours to absolute time (deltato is oldest)

 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);

 char *fhostkey = (hk_len == 0) ? NULL : hostkey;
 char *fhandle = (h_len == 0) ? NULL : handle;
 char *fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);
 
 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_promiselog_hosts(fhostkey, fhandle, PROMISE_LOG_STATE_NOTKEPT, from, to, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_vars)
{
 char *userName, *hostkey,*scope,*lval,*rval,*type,*classreg;
 char *fhostkey,*fscope,*flval,*frval,*ftype,*fclassreg;
 int user_len, hk_len,s_len,l_len,r_len,t_len,use_reg,cr_len;
 zend_bool regex;
 char buffer[512*1024];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssssbs",
                           &userName, &user_len,
                           &hostkey,&hk_len,
                           &scope,&s_len,
                           &lval,&l_len,
                           &rval,&r_len,
                           &type,&t_len,
                           &regex,
                           &classreg,&cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fscope =  (s_len == 0) ? NULL : scope;
 flval = (l_len == 0) ? NULL : lval;
 frval = (r_len == 0) ? NULL : rval;
 ftype = (t_len == 0) ? NULL : type;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_vars_hosts(fhostkey, fscope, flval, frval, ftype, use_reg, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_compliance_summary)
{
 char *userName, *hostkey,*version,*cmp,*classreg;
 int user_len, hk_len,v_len,cmp_len,cr_len;
 long k,nk,r,t;
 char buffer[512*1024];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllllss",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &version, &v_len,
                           &t,
                           &k,
                           &nk,
                           &r,
                           &cmp, &cmp_len,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 char *fhostkey = (hk_len == 0) ? NULL : hostkey;
 char *fversion = (v_len == 0) ? NULL : version;
 char *fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_compliance_hosts(fhostkey, fversion, (int)t, (int)k, (int)nk, (int)r, cmp, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_compliance_promises)
{
 char *userName, *hostkey,*handle,*status,*classreg;
 char *fhostkey,*fhandle,*fstatus,*fclassreg;
 int user_len, hk_len, h_len, s_len, cr_len;
 char buffer[512*1024];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbs",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &handle, &h_len,
                           &status, &s_len,
                           &regex,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fstatus =  (s_len == 0) ? NULL : status;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 
 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);
 
 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_promise_hosts(fhostkey, fhandle, fstatus, use_reg, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_lastseen)
{
 char *userName, *hostkey,*host,*address,*hash,*classreg;
 char *fhostkey,*fhost,*faddress,*fhash,*fclassreg;
 int user_len, hk_len,h_len,a_len,h2_len,cr_len;
 char buffer[512*1024];
 long ago;
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssslbs",
                           &userName, &user_len,                           
                           &hostkey, &hk_len,
                           &hash, &h2_len,
                           &host, &h_len,
                           &address, &a_len,
                           &ago,
                           &regex,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhash =  (h2_len == 0) ? NULL : hash;
 fhost =  (h_len == 0) ? NULL : host;
 faddress =  (a_len == 0) ? NULL : address;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_lastseen_hosts(fhostkey, fhash, fhost, faddress, ago, use_reg, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_performance)
{
 char *userName, *hostkey,*job,*classreg;
 char *fhostkey,*fjob,*fclassreg;
 int user_len, hk_len,j_len,cr_len;
 char buffer[512*1024];
 zend_bool regex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbs",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &job, &j_len,
                           &regex,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);
 
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fjob =  (j_len == 0) ? NULL : job;
 fclassreg =  (cr_len == 0) ? NULL : job;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_performance_hosts(fhostkey, fjob, regex, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_setuid)
{
 char *userName, *hostkey,*file, *classreg;
 char *fhostkey,*ffile;
 int user_len, hk_len,j_len,cr_len;
 char buffer[512*1024];
 zend_bool regex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbs",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &file, &j_len,
                           &regex,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (j_len == 0) ? NULL : file;
 char *fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_setuid_hosts(fhostkey, ffile, regex, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_filechanges)
{
 char *userName, *hostkey,*file,*cmp,*classreg;
 char *fhostkey,*ffile,*fcmp,*fclassreg;
 int user_len, hk_len,j_len,c_len,cr_len;
 char buffer[512*1024];
 zend_bool regex;
 long t;
 time_t then;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssblss",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &file, &j_len,
                           &regex,
                           &t,
                           &cmp, &c_len,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (j_len == 0) ? NULL : file;
 fcmp =  (c_len == 0) ? NULL : cmp;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);

 Nova2PHP_filechanges_hosts(fhostkey, ffile, regex, then, fcmp, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_filediffs)
{
 char *userName, *hostkey,*file,*cmp,*diff;
 char *fhostkey,*ffile,*fcmp,*fclassreg,*classreg;
 int user_len, hk_len,j_len,c_len,d_len,cr_len;
 char buffer[512*1024];
 zend_bool regex;
 long t;
 time_t then;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssblss",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &file, &j_len,
                           &diff, &d_len,
                           &regex,
                           &t,
                           &cmp, &c_len,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);
 
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (j_len == 0) ? NULL : file;
 fcmp =  (c_len == 0) ? NULL : cmp;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);
 
 Nova2PHP_filediffs_hosts(fhostkey, ffile, diff, regex, then, fcmp, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_bundlesseen)
{
 char *userName, *hostkey,*bundle,*classreg;
 char *fhostkey,*fbundle,*fclassreg;
 int user_len, hk_len,j_len,cr_len;
 char buffer[512*1024];
 zend_bool regex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbs",
                           &userName, &user_len,
                           &hostkey, &hk_len,
                           &bundle, &j_len,
                           &regex,
                           &classreg, &cr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fbundle =  (j_len == 0) ? NULL : bundle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 
 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 HostClassFilterAddClasses(filter, fclassreg, NULL);

 Nova2PHP_bundle_hosts(fhostkey, fbundle, regex, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
/* Topic Map                                                                  */
/******************************************************************************/

PHP_FUNCTION(cfpr_get_pid_for_topic)

{ char *type,*topic,typed_topic[1024];
 int o_len,y_len;
 long pid;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&type,&y_len,&topic,&o_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 if (y_len > 0)
    {
    snprintf(typed_topic,1023,"%s::%s",type,topic);
    pid = (long) Nova_GetTopicIdForTopic(typed_topic);
    }
 else
    {
    pid = (long) Nova_GetTopicIdForTopic(topic);
    }

 RETURN_LONG(pid);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_search_topics)

//$ret = cfpr_search_topics("leads","string",regex);

{ char *search;
 char *fsearch;
 int s_len;
 const int bufsize = 100000;
 char buffer[bufsize];
 zend_bool regex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sb",&search,&s_len,&regex) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 fsearch =  (s_len == 0) ? NULL : search;

 buffer[0] = '\0';
 Nova2PHP_search_topics(fsearch,regex,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_topic)

//$ret = cfpr_search_topics("leads","string",regex);

{ const int bufsize = 100000;
 char buffer[bufsize];
 long id;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&id) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_show_topic((int)id,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_topic_leads)

{ const int bufsize = 100000;
 char buffer[bufsize];
 long id;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&id) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_show_topic_leads((int)id,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_topic_hits)

{ const int bufsize = 100000;
 char buffer[bufsize];
 long id;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&id) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_show_topic_hits((int)id,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_topic_category)

{ const int bufsize = 10000;
 char buffer[bufsize];
 long id;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&id) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_show_topic_category((int)id,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
/* GRAPHS                                                                     */
/******************************************************************************/

PHP_FUNCTION(cfpr_top_n_hosts)

{ char *policy;
 int hk_len;
 long n;
 const int bufsize = 100000;
 char buffer[bufsize];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "slll",&policy,&hk_len,&n,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_show_topN(policy,(int)n,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_count_all_hosts)

{
 long val = Nova2PHP_count_hosts();

 RETURN_LONG(val);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_count_red_hosts)
{
 long val = Nova2PHP_count_red_hosts();

 RETURN_LONG(val);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_count_yellow_hosts)
{
 long val = Nova2PHP_count_yellow_hosts();

 RETURN_LONG(val);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_count_green_hosts)
{
 long val = Nova2PHP_count_green_hosts();

 RETURN_LONG(val);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_count_blue_hosts)
{
 long val = Nova2PHP_count_blue_hosts();

 RETURN_LONG(val);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_select_hosts)

{ char buffer[CF_WEBBUFFER];
 char *policy,*select;
 int hk_len,s_len;
 long n;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl",&select,&s_len,&policy,&hk_len,&n) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_select_hosts(policy,select,(int)n,buffer,sizeof(buffer));
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_select_reports)

{ const int bufsize = 1000000;
  char buffer[bufsize];
  char *report_id;
  int r_len;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &report_id, &r_len) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

buffer[0] = '\0';
Nova2PHP_select_reports(report_id, buffer, bufsize);
RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_red_hosts)

{ char buffer[CF_WEBBUFFER];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll",&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_show_col_hosts("red",200,&page,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_yellow_hosts)

{ char buffer[CF_WEBBUFFER];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll",&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }
 
 buffer[0] = '\0';
 Nova2PHP_show_col_hosts("yellow",200,&page,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_green_hosts)

{ char buffer[CF_WEBBUFFER];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll",&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }
 buffer[0] = '\0';
 Nova2PHP_show_col_hosts("green",200,&page,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_blue_hosts)

{ char buffer[CF_WEBBUFFER];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll",&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_show_col_hosts("blue",200,&page,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_hosts_ip)

{ char buffer[CF_WEBBUFFER];
 char *ipRegex, *fipRegex, *classRegex, *fclassRegex;
 int ip_len,cl_len;
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll",&ipRegex,&ip_len,&classRegex,&cl_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    RETURN_NULL();
    }

 fipRegex = (ip_len == 0) ? NULL : ipRegex;
 fclassRegex = (cl_len == 0) ? NULL : classRegex;

 buffer[0] = '\0';
 Nova2PHP_show_hosts(NULL,fipRegex,fclassRegex,&page,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_hosts_name)

{ char buffer[CF_WEBBUFFER];
 char *hostNameRegex, *fhostNameRegex, *classRegex, *fclassRegex;
 int ip_len,cl_len;
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll",&hostNameRegex,&ip_len,&classRegex,&cl_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    RETURN_NULL();
    }

 fhostNameRegex = (ip_len == 0) ? NULL : hostNameRegex;
 fclassRegex = (cl_len == 0) ? NULL : classRegex;

 buffer[0] = '\0';
 Nova2PHP_show_hosts(fhostNameRegex,NULL,fclassRegex,&page,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_getlastupdate)

{ char *hostkey;
 int hk_len;
 const int bufsize = 100;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_getlastupdate(hostkey,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_host_colour)

{ char *hostkey;
 int hk_len;
 const int bufsize = 100;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_host_colour(hostkey,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_list_by_handle_rx)
{
 char *userName, *handle;
 int user_len, h_len;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                           &userName, &user_len,
                           &handle, &h_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 char *fhandle = (h_len == 0) ? NULL : handle;

 buffer[0] = '\0';

 HubQuery *hqPromiseFilter = CFBD_PromiseFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

 PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);
 PromiseFilterAddPromiseBodyRx(filter, fhandle, NULL);
 
 Nova2PHP_promise_list(filter,buffer,sizeof(buffer));

 DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_list_by_promiser)
{
 char *userName, *promiser;
 int user_len, pr_len;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                           &userName, &user_len,
                           &promiser, &pr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);
 
 char *fpromiser = (pr_len == 0) ? NULL : promiser;

 buffer[0] = '\0';

 HubQuery *hqPromiseFilter = CFBD_PromiseFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

 PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);
 PromiseFilterAddPromiseBody(filter, NULL, fpromiser);

 Nova2PHP_promise_list(filter, buffer, sizeof(buffer));

 DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_list_by_promiser_rx)
{
 char *userName, *promiser;
 int user_len, pr_len;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                           &userName, &user_len,
                           &promiser, &pr_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);
 
 char *fpromiser = (pr_len == 0) ? NULL : promiser;

 buffer[0] = '\0';

 HubQuery *hqPromiseFilter = CFBD_PromiseFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

 PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);
 PromiseFilterAddPromiseBodyRx(filter, NULL, fpromiser);

 Nova2PHP_promise_list(filter, buffer, sizeof(buffer));

 DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_list_by_promise_type)
{
 char *userName, *promiseType;
 int user_len, pt_len;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                           &userName, &user_len,
                           &promiseType, &pt_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len && pt_len);
 
 buffer[0] = '\0';

 HubQuery *hqPromiseFilter = CFBD_PromiseFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

 PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);
 PromiseFilterAddPromiseType(filter, promiseType);

 Nova2PHP_promise_list(filter, buffer, sizeof(buffer));

 DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_list_by_bundle)
{
 char *userName, *bundleType, *bundleName;
 int user_len, btype_len, bname_len;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",
                           &userName, &user_len,
                           &bundleType, &btype_len,
                           &bundleName, &bname_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len && btype_len && bname_len);

 HubQuery *hqPromiseFilter = CFBD_PromiseFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

 PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);
 PromiseFilterAddBundleType(filter, bundleType);
 PromiseFilterAddBundles(filter, bundleName, NULL);

 buffer[0] = '\0';
 Nova2PHP_promise_list(filter, buffer, sizeof(buffer));

 DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_promise_list_by_bundle_rx)
{
 char *userName, *bundle;
 int user_len, b_len;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                           &userName, &user_len,
                           &bundle, &b_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 char *fbundle = (b_len == 0) ? NULL : bundle;
 
 buffer[0] = '\0';

 HubQuery *hqPromiseFilter = CFBD_PromiseFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

 PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);
 PromiseFilterAddBundlesRx(filter, fbundle, NULL);
 
 Nova2PHP_promise_list(filter, buffer, sizeof(buffer));

 DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_bundle_by_promise_handle)

{
 char *userName;
 char *promiseHandle;
 int user_len, ph_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                           &userName, &user_len,
                           &promiseHandle, &ph_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }
 
 ARGUMENT_CHECK_CONTENTS(user_len && ph_len);

 HubQuery *hqPromiseFilter = CFBD_PromiseFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

 PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);
 PromiseFilterAddPromiseBody(filter, promiseHandle, NULL);

 mongo_connection conn;
 DATABASE_OPEN(&conn);
 
 HubQuery *hqBundle = CFDB_QueryPromiseBundles(&conn, filter);
 DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);
 
 DATABASE_CLOSE(&conn);

 HubPromiseBundle *bundle = HubQueryGetFirstRecord(hqBundle);

 char *returnBuf = NULL;

 if(bundle && bundle->bundleName)
    {
    returnBuf = estrdup(bundle->bundleName);
    }

 DeleteHubQuery(hqBundle, DeleteHubPromiseBundle);

 if(returnBuf)
    {
    RETURN_STRING(returnBuf, 0);
    }
 else
    {
    RETURN_NULL();
    }
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_variable)

{ char *key,*scope,*lval;
 int hk_len,s_len,l_len;
 char buffer[2048];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",&key,&hk_len,&scope,&s_len,&lval,&l_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_variable(key,scope,lval,buffer,2048);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_bundle_classes_used)
{
 char *userName, *bundleName, *bundleType;
 int user_len, btype_len, bname_len;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",
                           &userName, &user_len,
                           &bundleType, &btype_len,
                           &bundleName, &bname_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len && btype_len && bname_len);

 HubQuery *hqPromiseFilter = CFBD_PromiseFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

 PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter); 
 PromiseFilterAddBundleType(filter, bundleType);
 PromiseFilterAddBundles(filter, bundleName, NULL);

 buffer[0] = '\0';
 Nova2PHP_bundle_classes_used(filter, buffer, sizeof(buffer));

 DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_bundle_arguments)
{
 char *userName;
 char *bundleName;
 char *bundleType;
 int user_len, bname_len, btype_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",
                           &userName, &user_len,
                           &bundleType, &btype_len,
                           &bundleName, &bname_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len && btype_len && bname_len);

 HubQuery *hqPromiseFilter = CFBD_PromiseFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

 PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);
 PromiseFilterAddBundleType(filter, bundleType);
 PromiseFilterAddBundles(filter, bundleName, NULL);

 mongo_connection conn;
 DATABASE_OPEN(&conn);
 
 HubQuery *hqBundle = CFDB_QueryPromiseBundles(&conn, filter);
 DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);
 
 DATABASE_CLOSE(&conn);

 JsonElement *output = JsonArrayCreate(100);

 HubPromiseBundle *bundle = HubQueryGetFirstRecord(hqBundle);
 
 if(bundle)
    {
    for (Rlist *rp = bundle->bundleArgs; rp != NULL; rp = rp->next)
       {
       JsonArrayAppendString(output, ScalarValue(rp));
       }
    }

 DeleteHubQuery(hqBundle, DeleteHubPromiseBundle);

 RETURN_JSON(output);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_bundle_list_all)
{
 char *userName;
 int user_len;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                           &userName, &user_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }
 
 ARGUMENT_CHECK_CONTENTS(user_len);

 HubQuery *hqPromiseFilter = CFBD_PromiseFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

 PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);
 
 mongo_connection conn;
 DATABASE_OPEN(&conn);
 
 HubQuery *hqBundles = CFDB_QueryPromiseBundles(&conn, filter);
 DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);
 
 DATABASE_CLOSE(&conn);
 
 HubQuerySortPromiseBundles(hqBundles);

 JsonElement *output = JsonArrayCreate(100);
 
 for(Rlist *rp = hqBundles->records; rp != NULL; rp = rp->next)
    {
    HubPromiseBundle *bundle = rp->item;
    
    JsonElement *bundleArray = JsonArrayCreate(2);
    
    JsonArrayAppendString(bundleArray, bundle->bundleType);
    JsonArrayAppendString(bundleArray, bundle->bundleName);
    
    JsonArrayAppendArray(output, bundleArray);
    }

 DeleteHubQuery(hqBundles, DeleteHubPromiseBundle);
 
 RETURN_JSON(output);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_bundle_agent_goals)
{
 char buffer[CF_WEBBUFFER];
 char *userName;
 int user_len;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                           &userName, &user_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }
 
 ARGUMENT_CHECK_CONTENTS(user_len);

 HubQuery *hqPromiseFilter = CFBD_PromiseFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);
 
 PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);
 PromiseFilterAddBundleType(filter, "agent");
 
 buffer[0] = '\0';
 Nova2PHP_bundle_agent_goals(filter,buffer,sizeof(buffer));

 DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_bundle_type)
// FIXME: this function inherently wrong - a bundle is not unique by its name -
//        e.g. there may be a "bundle monitor test" and a "bundle agent test"
{
 char *bundleName;
 int bname_len;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                           &bundleName, &bname_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(bname_len);

 PromiseFilter *filter = NewPromiseFilter();
 PromiseFilterAddBundles(filter, bundleName, NULL);

 mongo_connection conn;
 DATABASE_OPEN(&conn);
 
 HubQuery *hqBundle = CFDB_QueryPromiseBundles(&conn, filter);
 
 DeletePromiseFilter(filter);
 DATABASE_CLOSE(&conn);

 HubPromiseBundle *bundle = HubQueryGetFirstRecord(hqBundle);

 char *returnBuf = NULL;
 
 if(bundle && bundle->bundleType)
    {
    returnBuf = estrdup(bundle->bundleType);
    }

 DeleteHubQuery(hqBundle, DeleteHubPromiseBundle);

 if(returnBuf)
    {
    RETURN_STRING(returnBuf, 0);
    }
 else
    {
    RETURN_NULL();
    }
}

/******************************************************************************/

PHP_FUNCTION(cfpr_bundle_list_by_bundle_usage)
{
 char *userName, *bundleName;
 int user_len, bname_len;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                           &userName, &user_len,
                           &bundleName, &bname_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len && bname_len);

 HubQuery *hqPromiseFilter = CFBD_PromiseFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqPromiseFilter, DeletePromiseFilter);

 PromiseFilter *filter = HubQueryGetFirstRecord(hqPromiseFilter);

 buffer[0] = '\0';
 Nova2PHP_bundle_list_by_bundle_usage(filter, bundleName, buffer, sizeof(buffer));

 DeleteHubQuery(hqPromiseFilter, DeletePromiseFilter);
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_list_business_goals)

{
 const int bufsize = 1000000;
 char buffer[bufsize];

 buffer[0] = '\0';

 Nova2PHP_list_all_goals(buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_class_frequency)

{
 char *userName, *hkey,*fhkey,*pattern,*fpattern;
 int user_len, p_len, h_len,count;
 char buffer[1000000];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",
                           &userName, &user_len,
                           &hkey, &h_len,
                           &pattern, &p_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 fpattern = (p_len == 0) ? NULL : pattern;
 fhkey =  (h_len == 0) ? NULL : hkey;

 buffer[0] = '\0';

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);

 count = Nova2PHP_countclasses(fhkey, fpattern, 1, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_LONG((long)count);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_class_frequency)
{
 char *userName, *hkey,*fhkey,*pattern,*fpattern;
 int user_len, p_len, h_len;
 char buffer[CF_WEBBUFFER];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",
                           &userName, &user_len,
                           &hkey, &h_len,
                           &pattern, &p_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(user_len);

 buffer[0] = '\0';

 fpattern = (p_len == 0) ? NULL : pattern;
 fhkey =  (h_len == 0) ? NULL : hkey;

 HubQuery *hqHostClassFilter = CFBD_HostClassFilterFromUserRBAC(userName);
 ERRID_RBAC_CHECK(hqHostClassFilter, DeleteHostClassFilter);

 HostClassFilter *filter = (HostClassFilter *)HubQueryGetFirstRecord(hqHostClassFilter);
 
 Nova2PHP_countclasses(fhkey, fpattern, 1, filter, buffer, sizeof(buffer));
 DeleteHubQuery(hqHostClassFilter, DeleteHostClassFilter);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_body_details)

{
 char *bodyType, *bodyName;
 int btype_len, bname_len;
 char buffer[1000000];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                           &bodyType, &btype_len,
                           &bodyName, &bname_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(btype_len && bname_len);

 buffer[0] = '\0';
 Nova2PHP_GetPromiseBody(bodyName, bodyType, buffer, sizeof(buffer));
 
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_body_list)
{
 char *bodyType, *bodyName;
 int bname_len, btype_len;
 char buffer[1000000];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                           &bodyType, &btype_len,
                           &bodyName, &bname_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';

 Nova2PHP_body_list(bodyName, bodyType, buffer, sizeof(buffer));
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_network_rate)

{ char *name;
 int n_len;
 const int bufsize = 1000000;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&name,&n_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_network_speed(name,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_report_description)

{ char *reportName;
 int reportNameSz;
 const int bufSize = 5*1024;
 char buf[bufSize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&reportName,&reportNameSz) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buf[0] = '\0';
 Nova2PHP_report_description(reportName,buf,sizeof(buf));

 RETURN_STRING(buf,1);
}



/******************************************************************************/
/*                    Content-Driven Policy Reports                           */
/******************************************************************************/


PHP_FUNCTION(cfpr_cdp_reportnames)

{ char buf[1024]={0};

 Nova2PHP_cdp_reportnames(buf,sizeof(buf));

 RETURN_STRING(buf,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_cdp_report)

{char buffer[CF_WEBBUFFER];  // one row is ~300 bytes
 char *hostkey,*reportName;
 int hk_len,rn_len;
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll",&hostkey,&hk_len,&reportName,&rn_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_cdp_report(hostkey,reportName,&page,buffer,sizeof(buffer));

 RETURN_STRING(buffer,1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_validate_policy)

{ char *file;
 int f_len, ret;
 const int bufsize = 100000;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&file,&f_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0]='\0';
 ret = Nova2PHP_validate_policy(file,buffer,bufsize);
 switch(ret)
    {
    case 0:
        RETURN_STRING("SUCCESS",1);
        break;
    case -1:  // error
    case 1:
    default:
        break;
    }
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_delete_host)
/* Delete the record of a certain host from web-supporting DB  */
{ char *hostkey;
 int hk_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 if(hk_len == 0)
    {
    return;
    }
 Nova2PHP_delete_host(hostkey);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_environments_list)
{
 EnvironmentsList *el, *i;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 if (!Nova2PHP_environments_list(&el))
    {
    zend_throw_exception(cfmod_exception_generic, "Unable to query list of environments", 0 TSRMLS_CC);     
    RETURN_NULL();
    }

 array_init(return_value);

 for (i = el; i != NULL; i = i->next)
    {
    add_next_index_string(return_value, i->name, 1);
    }

 FreeEnvironmentsList(el);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_environment_contents)
{
 HostsList *hl, *i;
 char *name;
 int name_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 if (!Nova2PHP_environment_contents(name, &hl))
    {
    zend_throw_exception(cfmod_exception_generic, "Unable to query environment", 0 TSRMLS_CC);     
    RETURN_NULL();
    }

 array_init(return_value);

 for (i = hl; i != NULL; i = i->next)
    {
    add_next_index_string(return_value, i->keyhash, 1);
    }

 FreeHostsList(hl);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_get_host_environment)
{
 char *hostkey;
 int hostkey_len;
 char *environment;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &hostkey, &hostkey_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 environment = Nova2PHP_get_host_environment(hostkey);

 if (!environment)
    {
    RETURN_NULL();
    }
 else
    {
    char *envcopy = estrdup(environment);
    free(environment);
    RETURN_STRING(envcopy, 0);
    }
}


/******************************************************************************/
/* COPERNICUS KNOWLEDGE MAP                                                   */
/******************************************************************************/

PHP_FUNCTION(cfpr_get_knowledge_view)

{ char *view;
 int v_len;
 const int bufsize = 1000000;
 char buffer[bufsize];
 long pid;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls",&pid,&view,&v_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_knowledge_view(pid,view,buffer,100000);

 RETURN_STRING(buffer,1);
}



/***** Local helpers ******/

static time_t DeltaHrsConvert(long hrsAgo)
{
 time_t now, absTime;

 if(hrsAgo == 0)
    {
    return 0;
    }

 now = time(NULL);

 absTime = now - (hrsAgo * 3600);

 return absTime;

}



/******************************************************************************/
/*
 * commenting
 */
/******************************************************************************/
PHP_FUNCTION(cfpr_add_note)

{ char *user,*note,*nid;
 char *fuser,*fnote;
 int u_len,n_len, nid_len;
 time_t datetime;
 const int bufsize = 1000;
 char returnval[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssls",&nid,&nid_len,&user,&u_len,&datetime,&note,&n_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(nid_len);

 fuser =  (u_len == 0) ? NULL : user;
 fnote =  (n_len == 0) ? NULL : note;

 returnval[0]='\0';
 Nova2PHP_add_note(nid,fuser,datetime,fnote,returnval,bufsize);
 RETURN_STRING(returnval,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_new_note)
{
 char *hostkey,*user,*note, *repid;
 char *fuser,*fnote;
 int hk_len,u_len,n_len,rid_len;
 int report_type;
 time_t datetime;
 char returnval[1000];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslsls",
                           &hostkey, &hk_len,
                           &repid, &rid_len,
                           &report_type,
                           &user, &u_len,
                           &datetime,
                           &note, &n_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(hk_len && rid_len);

 fuser =  (u_len == 0) ? NULL : user;
 fnote =  (n_len == 0) ? NULL : note;

 returnval[0]='\0';
 Nova2PHP_add_new_note(hostkey, repid, report_type, fuser, datetime, fnote, returnval, sizeof(returnval));
 RETURN_STRING(returnval,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_query_note)

{ char *hostkey = {0},*user, *nid;
 char *fhostkey,*fuser, *fnid;
 int hk_len,u_len,nid_len;
 const int bufsize = 1000000;
 long from,to;
 PageInfo page = {0};

 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssllll",&hostkey,&hk_len,&nid,&nid_len,&user, &u_len, &from, &to,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fuser =  (u_len == 0) ? NULL : user;
 fnid =  (nid_len == 0) ? NULL : nid;

 buffer[0] = '\0';
 Nova2PHP_get_notes(fhostkey,fnid,fuser, from, to, &page, buffer, bufsize);
 RETURN_STRING(buffer,1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_get_host_noteid)

{ char s1[4096];
 char *hostkey;
 int hk_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 s1[0]='\0';
 Nova2PHP_get_host_noteid(hostkey,s1,sizeof(s1));

 RETURN_STRING(s1,1);
}

/******************************************************************************/

// TODO: get policiy names, get compliance by policy name

PHP_FUNCTION(cfpr_replica_status)
/* MongoDB hub replication */
{
 const int bufsize = 4096;
 char buf[bufsize];

 buf[0] = '\0';
 Nova2PHP_replica_status(buf, sizeof(buf));
 RETURN_STRING(buf,1);
}

/******************************************************************************/
/* Library                                                                    */
/******************************************************************************/

PHP_FUNCTION(cfpr_list_documents)

{ const int bufsize = 1000000;
  char buffer[bufsize];
  char *filePath,*fFilePath;
  int fpLen;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&filePath, &fpLen) == FAILURE)
   {
   RETURN_NULL();
   }

fFilePath = (fpLen == 0) ? NULL : filePath;

buffer[0]='\0';
Nova2PHP_GetLibraryDocuments(fFilePath,buffer,bufsize);
RETURN_STRING(buffer,1);
}


/******************************************************************************/
/* RBAC                                                                       */
/******************************************************************************/

PHP_FUNCTION(cfpr_user_authenticate)
{
char *username = NULL,
     *password = NULL;
int username_len = -1,
    password_len;

if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                          &username, &username_len,
                          &password, &password_len) == FAILURE)
   {
   zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
   RETURN_NULL();
   }

switch (CFDB_UserAuthenticate(username, password, password_len))
   {
   case ERRID_DBCONNECT:
      zend_throw_exception(cfmod_exception_db, "Error opening database", 0 TSRMLS_CC);
      RETURN_NULL();

   case ERRID_DBCLOSE:
      zend_throw_exception(cfmod_exception_db, "Error closing database", 0 TSRMLS_CC);
      RETURN_NULL();

   case ERRID_SUCCESS:
      RETURN_BOOL(true);

   default:
      RETURN_BOOL(false);
   }
}

/******************************************************************************/


PHP_FUNCTION(cfpr_role_create)
{
 char *creatingUserName, *roleName, *description, *includeClassRx, *excludeClassRx, *includeBundleRx, *excludeBundleRx;
 int creatingUserNameLen, roleNameLen, descLen, icrxLen, ecrxLen, ibrxLen, ebrxLen;
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssss",
                            &creatingUserName, &creatingUserNameLen,                            
                            &roleName, &roleNameLen,
                            &description, &descLen,
                            &includeClassRx, &icrxLen,
                            &excludeClassRx, &ecrxLen,
                            &includeBundleRx, &ibrxLen,
                            &excludeBundleRx, &ebrxLen) == FAILURE)
     {
     zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
     RETURN_NULL();
     }

  ARGUMENT_CHECK_CONTENTS(creatingUserNameLen && roleNameLen && descLen);

  char *fIncludeClassRx = (icrxLen == 0) ? NULL : includeClassRx;
  char *fExcludeClassRx = (ecrxLen == 0) ? NULL : excludeClassRx;
  char *fIncludeBundleRx = (ibrxLen == 0) ? NULL : includeBundleRx;
  char *fExcludeBundleRx = (ebrxLen == 0) ? NULL : excludeBundleRx;

  cfapi_errid errid = CFDB_CreateRole(creatingUserName, roleName, description,
                                      fIncludeClassRx, fExcludeClassRx, fIncludeBundleRx, fExcludeBundleRx);
  
  if(errid != ERRID_SUCCESS)
     {
     zend_throw_exception(cfmod_exception_generic, (char *)GetErrorDescription(errid), 0 TSRMLS_CC);     
     }
  
  RETURN_LONG(true);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_role_delete)
{
 char *userName, *roleName;
 int userNameLen, roleNameLen;
  
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &userName, &userNameLen ,&roleName, &roleNameLen) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(userNameLen && roleNameLen);

 cfapi_errid errid = CFDB_DeleteRole(userName, roleName, true);
 
 if(errid != ERRID_SUCCESS)
    {
     zend_throw_exception(cfmod_exception_generic, (char *)GetErrorDescription(errid), 0 TSRMLS_CC);
    }
 
 RETURN_LONG(true);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_role_update)
{
 char *updatingUserName, *roleName, *description, *includeClassRx, *excludeClassRx, *includeBundleRx, *excludeBundleRx;
 int updatingUserNameLen, roleNameLen, descLen, icrxLen, ecrxLen, ibrxLen, ebrxLen;
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssss",
                            &updatingUserName, &updatingUserNameLen,                            
                            &roleName, &roleNameLen,
                            &description, &descLen,
                            &includeClassRx, &icrxLen,
                            &excludeClassRx, &ecrxLen,
                            &includeBundleRx, &ibrxLen,
                            &excludeBundleRx, &ebrxLen) == FAILURE)
     {
     zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
     RETURN_NULL();
     }

  ARGUMENT_CHECK_CONTENTS(updatingUserNameLen && roleNameLen && descLen);

  char *fIncludeClassRx = (icrxLen == 0) ? NULL : includeClassRx;
  char *fExcludeClassRx = (ecrxLen == 0) ? NULL : excludeClassRx;
  char *fIncludeBundleRx = (ibrxLen == 0) ? NULL : includeBundleRx;
  char *fExcludeBundleRx = (ebrxLen == 0) ? NULL : excludeBundleRx;

  cfapi_errid errid = CFDB_UpdateRole(updatingUserName, roleName, description,
                                      fIncludeClassRx, fExcludeClassRx, fIncludeBundleRx, fExcludeBundleRx);
  
  if(errid != ERRID_SUCCESS)
     {
     zend_throw_exception(cfmod_exception_generic, (char *)GetErrorDescription(errid), 0 TSRMLS_CC);     
     }
  
  RETURN_LONG(true);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_role_list_all)
{
 char *userName;
 int userNameLen;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&userName, &userNameLen) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(userNameLen);
 
 HubQuery *hq = CFDB_GetAllRolesAuth(userName);

 if(hq->errid != ERRID_SUCCESS)
    {
    DeleteHubQuery(hq, DeleteHubRole);
    zend_throw_exception(cfmod_exception_generic, (char *)GetErrorDescription(hq->errid), 0 TSRMLS_CC);
    RETURN_NULL();
    }

 JsonElement *roles = ParseRolesToJson(hq);

 DeleteHubQuery(hq, DeleteHubRole);
   
 RETURN_JSON(roles);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_role_list_by_name)
{

 char *userName, *roleName;
 int userNameLen, roleNameLen;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&userName, &userNameLen, &roleName, &roleNameLen) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, LABEL_ERROR_ARGS, 0 TSRMLS_CC);
    RETURN_NULL();
    }

 ARGUMENT_CHECK_CONTENTS(userNameLen && roleNameLen);

 HubQuery *hq = CFDB_GetRoleByNameAuth(userName, roleName);

 if(hq->errid != ERRID_SUCCESS)
    {
    DeleteHubQuery(hq, DeleteHubRole);
    zend_throw_exception(cfmod_exception_generic, (char *)GetErrorDescription(hq->errid), 0 TSRMLS_CC);
    RETURN_NULL();
    }

 JsonElement *roles = ParseRolesToJson(hq);

 DeleteHubQuery(hq, DeleteHubRole);

 RETURN_JSON(roles);
}

/******************************************************************************/

static JsonElement *ParseRolesToJson(HubQuery *hq)
{
#define LABEL_ROLE_NAME "name"
#define LABEL_ROLE_DESCRIPTION "description"
#define LABEL_ROLE_CLASSRX_INCLUDE "classrxinclude"
#define LABEL_ROLE_CLASSRX_EXCLUDE "classrxexclude"
#define LABEL_ROLE_BUNDLERX_INCLUDE "bundlerxinlcude"
#define LABEL_ROLE_BUNDLERX_EXCLUDE "bundlerxexclude"

 JsonElement *roles = JsonArrayCreate(100);
 
 for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
    HubRole *record = (HubRole *)rp->item;
    JsonElement *role_entry = JsonObjectCreate(10);
      
    JsonObjectAppendString(role_entry, LABEL_ROLE_NAME, record->name);
    JsonObjectAppendString(role_entry, LABEL_ROLE_DESCRIPTION, record->description);
    JsonObjectAppendString(role_entry, LABEL_ROLE_CLASSRX_INCLUDE, record->classRxInclude);
    JsonObjectAppendString(role_entry, LABEL_ROLE_CLASSRX_EXCLUDE, record->classRxExclude);
    JsonObjectAppendString(role_entry, LABEL_ROLE_BUNDLERX_INCLUDE, record->bundleRxInclude);
    JsonObjectAppendString(role_entry, LABEL_ROLE_BUNDLERX_EXCLUDE, record->bundleRxExclude);
    
    JsonArrayAppendObject(roles, role_entry);
    }
 
 return roles;
}
