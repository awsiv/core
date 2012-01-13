#include "private-api.h"

#include "json.h"

#define cfr_software     "sw"
#define cfr_patch_avail  "pa"
#define cfr_patch_installed "pi"

static time_t DeltaHrsConvert(long hrsAgo);
char **String2StringArray(char *str, char separator);
void FreeStringArray(char **strs);
static bool ParseVitalsArgs(int argc, char **retHostKey, char **retVitalId);
static JsonArray *ParseRolesToJson(HubQuery *hq);


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
   php_printf("Error is cfpr_ldap_authenticate args");
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
   php_printf("Error is cfpr_ldap_get_single_attribute_list args");
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
   php_printf("Error is cfpr_ldap_get_several_attributes args");
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

 CFDB_GetValue("license_report",s,sizeof(s));

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

 CFDB_GetValue("license_expires",s,sizeof(s));

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

 CFDB_GetValue("license_installtime",s,sizeof(s));

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
 CFDB_GetValue("license_owner",s,sizeof(s));

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
 CFDB_GetValue("licenses_promised",s,sizeof(s));

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

 CFDB_GetValue("licenses_granted",s,sizeof(s));
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
    php_printf("Error is cfpr_hostname function args");
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
    php_printf("Error is cfpr_ipaddr function args");
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
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    php_printf("Error in cfpr_host_meter function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;

 buffer[0]='\0';
 Nova2PHP_meter(fhostkey,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/*****************************************************************************/
/* Vitals functions                                                          */
/*****************************************************************************/

PHP_FUNCTION(cfpr_vitals_list)

{ char *hostkey,*fhostkey;
 int hk_len;
 const int bufsize = 4096;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&hostkey,&hk_len) == FAILURE)
    {
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;

 if(!fhostkey)
    {
    php_printf("cfpr_vitals_list: error: hostkey is undefined\n");
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_list(fhostkey, buffer, bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_magnified)

{ char *hostkey,*fhostkey, *vitalId, *fvitalId;
 int hk_len, vi_len;
 const int bufsize = 1024*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fvitalId =  (vi_len == 0) ? NULL : vitalId;

 // only support one host and vital at this time
 if(!fhostkey)
    {
    php_printf("cfpr_vitals_view_magnified: error: hostkey is undefined\n");
    RETURN_NULL();
    }

  if(!fvitalId)
    {
    php_printf("cfpr_vitals_view_magnified: error: vitalid is undefined\n");
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_view_magnified(fhostkey, fvitalId, buffer, bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_week)

{ char *hostkey,*fhostkey, *vitalId, *fvitalId;
 int hk_len, vi_len;
 const int bufsize = 1024*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fvitalId =  (vi_len == 0) ? NULL : vitalId;

 // only support one host and vital at this time
 if(!fhostkey)
    {
    php_printf("cfpr_vitals_view_week: error: hostkey is undefined\n");
    RETURN_NULL();
    }

  if(!fvitalId)
    {
    php_printf("cfpr_vitals_view_week: error: vitalid is undefined\n");
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_view_week(fhostkey, fvitalId, buffer, bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_year)

{ char *hostkey,*fhostkey, *vitalId, *fvitalId;
 int hk_len, vi_len;
 const int bufsize = 1024*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fvitalId =  (vi_len == 0) ? NULL : vitalId;

 // only support one host and vital at this time
 if(!fhostkey)
    {
    php_printf("cfpr_vitals_view_year: error: hostkey is undefined\n");
    RETURN_NULL();
    }

  if(!fvitalId)
    {
    php_printf("cfpr_vitals_view_year: error: vitalid is undefined\n");
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_view_year(fhostkey, fvitalId, buffer, bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_view_histogram)

{ char *hostkey,*fhostkey, *vitalId, *fvitalId;
 int hk_len, vi_len;
 const int bufsize = 1024*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hostkey,&hk_len,&vitalId,&vi_len) == FAILURE)
    {
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fvitalId =  (vi_len == 0) ? NULL : vitalId;

 // only support one host and vital at this time
 if(!fhostkey)
    {
    php_printf("cfpr_vitals_view_histogram: error: hostkey is undefined\n");
    RETURN_NULL();
    }

  if(!fvitalId)
    {
    php_printf("cfpr_vitals_view_histogram: error: vitalid is undefined\n");
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_view_histogram(fhostkey, fvitalId, buffer, bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_magnified)

{ char *hostkey, *vitalId;

 const int bufsize = 4096;
 char buffer[bufsize];

 if(!ParseVitalsArgs(ZEND_NUM_ARGS() TSRMLS_CC,&hostkey, &vitalId))
    {
    php_printf("cfpr_vitals_analyse_magnified: Error while parsing arguments\n");
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_vitals_analyse_magnified(hostkey, vitalId, buffer, bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_vitals_analyse_week)

{ char *hostkey, *vitalId;

 const int bufsize = 4096;
 char buffer[bufsize];

 if(!ParseVitalsArgs(ZEND_NUM_ARGS() TSRMLS_CC,&hostkey, &vitalId))
    {
    php_printf("cfpr_vitals_analyse_week: Error while parsing arguments\n");
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
    php_printf("cfpr_vitals_analyse_year: Error while parsing arguments\n");
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
    php_printf("cfpr_vitals_analyse_histogram: Error while parsing arguments\n");
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

//$ret = cfpr_report_software_in($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 int use_reg;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbsll",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_software_in function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_software_report(fhostkey,fname,fversion,farch,use_reg,cfr_software,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_patch_in)

// $ret = cfpr_report_patch_in($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbsll",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_patch_in function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg= (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_software_report(fhostkey,fname,fversion,farch,use_reg,cfr_patch_installed,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_patch_avail)

// $ret = cfpr_report_patch_avail($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbsll",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_patch_avail function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_software_report(fhostkey,fname,fversion,farch,use_reg,cfr_patch_avail,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbsll",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_classes_in function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_classes_report(fhostkey,fname,regex,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_list_time_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_time_classes function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_time(fhostkey,fname,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_list_host_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_host_classes function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_host(fhostkey,fname,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_list_all_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_all_classes function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_all(fhostkey,fname,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_list_soft_classes)

//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_soft_classes function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_soft(fhostkey,fname,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_list_ip_classes)
//$ret = cfpr_report_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*classreg;
 char *fname,*fhostkey,*fclassreg;
 int hk_len, n_len,cr_len;
 long regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_ip_classes function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_listclasses_ip(fhostkey,fname,regex,fclassreg,buffer,bufsize);

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

//$ret = cfpr_report_vars($hostkey,$scope,$lval,$rval,$type,$regex);

{ char *hostkey,*scope,*lval,*rval,*type,*classreg;
 char *fhostkey,*fscope,*flval,*frval,*ftype,*fclassreg;
 int hk_len,s_len,l_len,r_len,t_len,use_reg,cr_len;
 zend_bool regex;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssbsll",
                           &hostkey,&hk_len,
                           &scope,&s_len,
                           &lval,&l_len,
                           &rval,&r_len,
                           &type,&t_len,
                           &regex,
                           &classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_vars function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fscope =  (s_len == 0) ? NULL : scope;
 flval = (l_len == 0) ? NULL : lval;
 frval = (r_len == 0) ? NULL : rval;
 ftype = (t_len == 0) ? NULL : type;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_vars_report(fhostkey,fscope,flval,frval,ftype,use_reg,fclassreg,&page,buffer,bufsize);

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

//$ret = cfpr_report_compliance_summary($hostkey,$version,$time,$kept,$notkept,$repaired,">");

{ char *hostkey,*version,*cmp,*classreg;
 char *fhostkey,*fversion;
 int hk_len,v_len,cmp_len,cr_len;
 long k,nk,r,t;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllllssll",
                           &hostkey,&hk_len,&version,&v_len,&t,&k,&nk,&r,&cmp,&cmp_len,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_compliance function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fversion = (v_len == 0) ? NULL : version;

 buffer[0]='\0';
 Nova2PHP_compliance_report(fhostkey,fversion,(time_t)t,(int)k,(int)nk,(int)r,cmp,classreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_compliance_promises)

//$ret = cfpr_report_compliance_promises($hostkey,$handle,$status,$regex);

{ char *hostkey,*handle,*status,*classreg;
 char *fhostkey,*fhandle,*fstatus,*fclassreg;
 int hk_len,h_len,s_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbsll",
                           &hostkey,&hk_len,&handle,&h_len,&status,&s_len,&regex,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_compliance_promises function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fstatus =  (s_len == 0) ? NULL : status;
 fclassreg =  (s_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_compliance_promises(fhostkey,fhandle,fstatus,use_reg,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_report_overall_summary)
//$ret = cfpr_report_overall_summary($hostkey,$handle,$status,$regex);

{ char *hostkey,*handle,*status,*classreg;
  char *fhostkey,*fhandle,*fstatus,*fclassreg;
  int hk_len,h_len,s_len,cr_len;

 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbs",
                           &hostkey,&hk_len,&handle,&h_len,&status,&s_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_report_overall_summary function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fstatus =  (s_len == 0) ? NULL : status;
 fclassreg =  (s_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_summary_report(fhostkey,fhandle,fstatus,use_reg,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_lastseen)


{ char *hostkey,*host,*address,*hash,*classreg;
 char *fhostkey,*fhost,*faddress,*fhash,*fclassreg;
 int hk_len,h_len,a_len,h2_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 long ago;
 time_t tago;
 zend_bool regex;
 int use_reg;
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssslbsll",
                           &hostkey,&hk_len,&hash,&h2_len,&host,&h_len,&address,&a_len,&ago,&regex,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_lastseem function args");
    RETURN_NULL();
    }

 tago = (time_t)ago;
 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhash =  (h2_len == 0) ? NULL : hash;
 fhost =  (h_len == 0) ? NULL : host;
 faddress =  (a_len == 0) ? NULL : address;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_lastseen_report(fhostkey,fhash,fhost,faddress,tago,use_reg,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);

}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_performance)

//$ret = cfpr_report_performance($hostkey,$job,$regex);

{ char *hostkey,*job,*classreg;
 char *fhostkey,*fjob,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbsll",
                           &hostkey,&hk_len,&job,&j_len,&regex,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_performance function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fjob =  (j_len == 0) ? NULL : job;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_performance_report(fhostkey,fjob,regex,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_setuid)

//$ret = cfpr_report_setuid($hostkey,$file,$regex);

{ char *hostkey,*file,*classreg;
 char *fhostkey,*ffile,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbsll",
                           &hostkey,&hk_len,&file,&j_len,&regex,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_setuid function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (j_len == 0) ? NULL : file;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_setuid_report(fhostkey,ffile,regex,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filechanges)

//$ret = cfpr_report_filechanges($hostkey,$name,$regex,$time,">");

{ char *hostkey,*file,*cmp,*classreg;
 char *fhostkey,*ffile,*fclassreg;
 int hk_len,f_len,c_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 long t;
 time_t then;
 PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssblssll",
                           &hostkey,&hk_len,&file,&f_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_filechanges function args");
    RETURN_NULL();
    }

 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0]='\0';
 Nova2PHP_filechanges_report(fhostkey,ffile,regex,then,cmp,fclassreg,&page,false,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filediffs)

//$ret = cfpr_report_filediffs($hostkey,$name,$diff,$regex,$time,">");

{ char *hostkey,*file,*cmp,*diff,*classreg;
 char *fhostkey,*ffile,*fdiff,*fclassreg;
 int hk_len,f_len,c_len,d_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 long t;
 time_t then;
 PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssblssll",
                           &hostkey,&hk_len,&file,&f_len,&diff,&d_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_filediffs function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fdiff =  (d_len == 0) ? NULL : diff;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0]='\0';
 Nova2PHP_filediffs_report(fhostkey,ffile,fdiff,use_reg,then,cmp,fclassreg,&page,false,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filechanges_longterm)

//$ret = cfpr_report_filechanges($hostkey,$name,$regex,$time,">");

{ char *hostkey,*file,*cmp,*classreg;
 char *fhostkey,*ffile,*fclassreg;
 int hk_len,f_len,c_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 long t;
 time_t then;
 PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssblssll",
                           &hostkey,&hk_len,&file,&f_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_filechanges function args");
    RETURN_NULL();
    }

 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0]='\0';
 Nova2PHP_filechanges_report(fhostkey,ffile,regex,then,cmp,fclassreg,&page,true,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_filediffs_longterm)

//$ret = cfpr_report_filediffs($hostkey,$name,$diff,$regex,$time,">");

{ char *hostkey,*file,*cmp,*diff,*classreg;
 char *fhostkey,*ffile,*fdiff,*fclassreg;
 int hk_len,f_len,c_len,d_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 long t;
 time_t then;
 PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssblssll",
                           &hostkey,&hk_len,&file,&f_len,&diff,&d_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_filediffs function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (f_len == 0) ? NULL : file;
 fdiff =  (d_len == 0) ? NULL : diff;
 fclassreg =  (cr_len == 0) ? NULL : classreg;
// NOT THIS fcmp =  (c_len == 0) ? NULL : cmp;

 buffer[0]='\0';
 Nova2PHP_filediffs_report(fhostkey,ffile,fdiff,use_reg,then,cmp,fclassreg,&page,true,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summarize_promise)

//$ret = cfpr_summarize_promise($handle);

{
 char *handle;
 int handleLen;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&handle,&handleLen) == FAILURE)
    {
    php_printf("Error is cfpr_summarize_promise function args");
    RETURN_NULL();
    }

 buffer[0]='\0';
 Nova2PHP_summarize_promise(handle, buffer, bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_bundlesseen)

//$ret = cfpr_report_bundlesseen($hostkey,$name,$isNameRegex,$classRegexStr);

{ char *hostkey,*bundle,*classreg;
 char *fhostkey,*fbundle,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;
 PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbsll",
                           &hostkey,&hk_len,&bundle,&j_len,&regex,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_report_bundlesseen function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fbundle =  (j_len == 0) ? NULL : bundle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_bundle_report(fhostkey,fbundle,use_reg,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_value)

{ char *hostkey,*day,*month,*year,*classreg;
 char *fhostkey,*fmonth,*fday,*fyear,*fclassreg;
 int hk_len,d_len,m_len,y_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 PageInfo page = {0};
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssll",
                           &hostkey,&hk_len,&day,&d_len,&month,&m_len,&year,&y_len,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_report_value function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fday =  (d_len == 0) ? NULL : day;
 fmonth =  (m_len == 0) ? NULL : month;
 fyear =  (y_len == 0) ? NULL : year;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_value_report(fhostkey,fday,fmonth,fyear,fclassreg,&page,buffer,bufsize);

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
    php_printf("Error in cfpr_get_value_graph function args");
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

{ char *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int hk_len,h_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllsll",
                           &hostkey,&hk_len,&handle,&h_len,&hours_deltafrom,&hours_deltato,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_report_notkept function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

// convert delta hours to absolute time (deltato is oldest)

 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);

 buffer[0]='\0';
 Nova2PHP_promiselog(fhostkey,fhandle,plog_notkept,from,to,fclassreg,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_repaired)

{ char *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int hk_len,h_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllsll",
                           &hostkey,&hk_len,&handle,&h_len,&hours_deltafrom,&hours_deltato,&classreg,&cr_len,
                           &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_report_repaired function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

// convert delta hours to absolute time (deltato is oldest)

 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);

 buffer[0]='\0';
 Nova2PHP_promiselog(fhostkey,fhandle,plog_repaired,from,to,fclassreg,&page,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summarize_notkept)

{ char *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int hk_len,h_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 time_t from,to;
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllsll",&hostkey,&hk_len,&handle,&h_len,&from,&to,&classreg,&cr_len, &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_summarize_notkept function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_promiselog_summary(fhostkey,fhandle,plog_notkept,from,to,fclassreg,&page,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_summarize_repaired)

//$ret = cfpr_summarize_notkept(hostkey,handle,from,to,classRegex);

{ char *hostkey,*handle,*classreg;
 char *fhostkey,*fhandle,*fclassreg;
 int hk_len,h_len,cr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 time_t from,to;
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllsll",&hostkey,&hk_len,&handle,&h_len,&from,&to,&classreg,&cr_len, &(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_summarize_repaired function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0]='\0';
 Nova2PHP_promiselog_summary(fhostkey,fhandle,plog_repaired,from,to,fclassreg,&page,buffer,bufsize);
 RETURN_STRING(buffer,1);
}


/******************************************************************************/
/* Returning hosts                                                            */
/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_software_in)

//$ret = cfpr_hosts_with_software_in($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 const int bufsize = 512*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbs",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_software_in function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_software_hosts(fhostkey,fname,fversion,farch,use_reg,cfr_software,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_value)

{ char *hostkey,*day,*month,*year,*classreg;
 char *fhostkey,*fmonth,*fday,*fyear,*fclassreg;
 int hk_len,d_len,m_len,y_len,cr_len;
 const int bufsize = 512*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssss",
                           &hostkey,&hk_len,&day,&d_len,&month,&m_len,&year,&y_len,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error in cfpr_hosts_with_value function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fday =  (d_len == 0) ? NULL : day;
 fmonth =  (m_len == 0) ? NULL : month;
 fyear =  (y_len == 0) ? NULL : year;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_value_hosts(fhostkey,fday,fmonth,fyear,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_patch_in)

// $ret = cfpr_hosts_with_patch_in($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*classreg;
 char *fclassreg;
 int hk_len, n_len,v_len,a_len,cr_len;
 long regex;
 const int bufsize = 512*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbs",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_patch_in function args");
    RETURN_NULL();
    }

 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_software_hosts(hostkey,name,version,arch,regex,cfr_patch_installed,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_patch_avail)

// $ret = cfpr_hosts_with_patch_avail($hostkey,$name,$version,$arch,$regex);

{ char *hostkey,*name,*version,*arch,*classreg;
 char *fhostkey,*fname,*fversion,*farch,*fclassreg;
 int hk_len, n_len,v_len,a_len,use_reg,cr_len;
 long regex;
 const int bufsize = 512*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssbs",&hostkey,&hk_len,&name,&n_len,&version,&v_len,&arch,&a_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_patch_avail function args");
    RETURN_NULL();
    }


 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fname =  (n_len == 0) ? NULL : name;
 fversion = (v_len == 0) ? NULL : version;
 farch = (a_len == 0) ? NULL : arch;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_software_hosts(fhostkey,fname,fversion,farch,use_reg,cfr_patch_avail,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_classes)

//$ret = cfpr_hosts_with_classes($hostkey,$name,$regex);

{ char *hostkey,*name,*classreg;
 int hk_len, n_len,cr_len;
 long regex;
 const int bufsize = 512*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&name,&n_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_classes_in function args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_classes_hosts(hostkey,name,regex,classreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_repaired)

//$ret = cfpr_hosts_with_repaired($hostkey,$name,$hours_deltafrom,$hours_deltato,$class_regex);

{ char *hostkey,*handle,*classreg;
 int hk_len, h_len,cr_len;
 const int bufsize = 512*1024;
 char buffer[bufsize];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslls",&hostkey,&hk_len,&handle,&h_len,&hours_deltafrom,&hours_deltato,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_repaired function args");
    RETURN_NULL();
    }

// convert delta hours to absolute time (deltato is oldest)

 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);

 buffer[0] = '\0';
 Nova2PHP_promiselog_hosts(hostkey,handle,plog_repaired,from,to,classreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_notkept)

//$ret = cfpr_hosts_with_notkept($hostkey,$name,$hours_deltafrom,$hours_deltato,$class_regex);

{ char *hostkey,*handle,*classreg;
 int hk_len, h_len,cr_len;
 const int bufsize = 512*1024;
 char buffer[bufsize];
 long hours_deltafrom, hours_deltato;
 time_t from = 0, to = 0;


 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslls",&hostkey,&hk_len,&handle,&h_len,&hours_deltafrom,&hours_deltato,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_notkept function args");
    RETURN_NULL();
    }

// convert delta hours to absolute time (deltato is oldest)

 from = DeltaHrsConvert(hours_deltato);
 to = DeltaHrsConvert(hours_deltafrom);

 buffer[0] = '\0';
 Nova2PHP_promiselog_hosts(hostkey,handle,plog_notkept,from,to,classreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}


/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_vars)

//$ret = cfpr_hosts_with_vars($hostkey,$scope,$lval,$rval,$type,$regex);

{ char *hostkey,*scope,*lval,*rval,*type,*classreg;
 char *fhostkey,*fscope,*flval,*frval,*ftype,*fclassreg;
 int hk_len,s_len,l_len,r_len,t_len,use_reg,cr_len;
 zend_bool regex;
 const int bufsize = 512*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssssbs",
                           &hostkey,&hk_len,
                           &scope,&s_len,
                           &lval,&l_len,
                           &rval,&r_len,
                           &type,&t_len,
                           &regex,
                           &classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_vars function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fscope =  (s_len == 0) ? NULL : scope;
 flval = (l_len == 0) ? NULL : lval;
 frval = (r_len == 0) ? NULL : rval;
 ftype = (t_len == 0) ? NULL : type;
 fclassreg = (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_vars_hosts(fhostkey,fscope,flval,frval,ftype,use_reg,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_compliance_summary)

//$ret = cfpr_hosts_with_compliance_summary($hostkey,$version,$time,$kept,$notkept,$repaired,">");

{ char *hostkey,*version,*cmp,*classreg;
 int hk_len,v_len,cmp_len,cr_len;
 long k,nk,r,t;
 const int bufsize = 512*1024;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssllllss",&hostkey,&hk_len,&version,&v_len,&t,&k,&nk,&r,&cmp,&cmp_len,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_compliance function args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_compliance_hosts(hostkey,version,(int)t,(int)k,(int)nk,(int)r,cmp,classreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_compliance_promises)

//$ret = cfpr_hosts_with_compliance_promises($hostkey,$handle,$status,$regex);

{ char *hostkey,*handle,*status,*classreg;
 char *fhostkey,*fhandle,*fstatus,*fclassreg;
 int hk_len,h_len,s_len,cr_len;
 const int bufsize = 512 * 1024;
 char buffer[bufsize];
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssbs",&hostkey,&hk_len,&handle,&h_len,&status,&s_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_compliance_promises function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhandle =  (h_len == 0) ? NULL : handle;
 fstatus =  (s_len == 0) ? NULL : status;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_promise_hosts(fhostkey,fhandle,fstatus,use_reg,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_lastseen)

//$ret = cfpr_hosts_with_lastseen($hostkey,$host,$addr,$tago,$regex);

{ char *hostkey,*host,*address,*hash,*classreg;
 char *fhostkey,*fhost,*faddress,*fhash,*fclassreg;
 int hk_len,h_len,a_len,h2_len,cr_len;
 const int bufsize = 512*1024;
 char buffer[bufsize];
 long ago;
 zend_bool regex;
 int use_reg;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssslbs",&hostkey,&hk_len,&hash,&h2_len,&host,&h_len,&address,&a_len,&ago,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_lastseen function args");
    RETURN_NULL();
    }

 use_reg = (int)regex;

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fhash =  (h2_len == 0) ? NULL : hash;
 fhost =  (h_len == 0) ? NULL : host;
 faddress =  (a_len == 0) ? NULL : address;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_lastseen_hosts(fhostkey,fhash,fhost,faddress,ago,use_reg,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);

}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_performance)

//$ret = cfpr_hosts_with_performance($hostkey,$job,$regex);

{ char *hostkey,*job,*classreg;
 char *fhostkey,*fjob,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = 512*1024;
 char buffer[bufsize];
 zend_bool regex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&job,&j_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_performance function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fjob =  (j_len == 0) ? NULL : job;
 fclassreg =  (cr_len == 0) ? NULL : job;

 buffer[0] = '\0';
 Nova2PHP_performance_hosts(fhostkey,fjob,regex,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_setuid)

//$ret = cfpr_hosts_with_setuid($hostkey,$file,$regex);

{ char *hostkey,*file, *classreg;
 char *fhostkey,*ffile;
 int hk_len,j_len,cr_len;
 const int bufsize = 512*1024;
 char buffer[bufsize];
 zend_bool regex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&file,&j_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_setuid function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (j_len == 0) ? NULL : file;

 buffer[0] = '\0';
 Nova2PHP_setuid_hosts(fhostkey,ffile,regex,fhostkey,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_filechanges)

//$ret = cfpr_hosts_with_filechanges($hostkey,$name,$regex,$time,">");

{ char *hostkey,*file,*cmp,*classreg;
 char *fhostkey,*ffile,*fcmp,*fclassreg;
 int hk_len,j_len,c_len,cr_len;
 const int bufsize = 512*1024;
 char buffer[bufsize];
 zend_bool regex;
 long t;
 time_t then;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssblss",&hostkey,&hk_len,&file,&j_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_filechanges function args");
    RETURN_NULL();
    }

 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (j_len == 0) ? NULL : file;
 fcmp =  (c_len == 0) ? NULL : cmp;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_filechanges_hosts(fhostkey,ffile,regex,then,fcmp,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_filediffs)

//$ret = cfpr_hosts_with_filediffs($hostkey,$name,$diff,$regex,$time,">");

{ char *hostkey,*file,*cmp,*diff;
 char *fhostkey,*ffile,*fcmp,*fclassreg,*classreg;
 int hk_len,j_len,c_len,d_len,cr_len;
 const int bufsize = 512*1024;
 char buffer[bufsize];
 zend_bool regex;
 long t;
 time_t then;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssblss",&hostkey,&hk_len,&file,&j_len,&diff,&d_len,&regex,&t,&cmp,&c_len,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_filediffs function args");
    RETURN_NULL();
    }

 then = (time_t)t;
 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 ffile =  (j_len == 0) ? NULL : file;
 fcmp =  (c_len == 0) ? NULL : cmp;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_filediffs_hosts(fhostkey,ffile,diff,regex,then,fcmp,fclassreg,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_hosts_with_bundlesseen)

//$ret = cfpr_hosts_with_bundlesseen($hostkey,$name,$isNameRegex,$classreg);

{ char *hostkey,*bundle,*classreg;
 char *fhostkey,*fbundle,*fclassreg;
 int hk_len,j_len,cr_len;
 const int bufsize = 512*1024;
 char buffer[bufsize];
 zend_bool regex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssbs",&hostkey,&hk_len,&bundle,&j_len,&regex,&classreg,&cr_len) == FAILURE)
    {
    php_printf("Error is cfpr_hosts_with_bundlesseen function args");
    RETURN_NULL();
    }

 fhostkey =  (hk_len == 0) ? NULL : hostkey;
 fbundle =  (j_len == 0) ? NULL : bundle;
 fclassreg =  (cr_len == 0) ? NULL : classreg;

 buffer[0] = '\0';
 Nova2PHP_bundle_hosts(fhostkey,fbundle,regex,fclassreg,buffer,bufsize);

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
    php_printf("Error is cfpr_get_pid_for_topic function args");
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
    php_printf("Error is cfpr_search_topics function args");
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
    php_printf("Error is cfpr_show_topic function args");
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
    php_printf("Error is cfpr_show_topic_leads function args");
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
    php_printf("Error is cfpr_show_topic_hits function args");
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
    php_printf("Error is cfpr_show_topic_category function args");
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
    php_printf("Error in cfpr_show_top_n_hosts needs a policy");
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

{  const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 char *policy,*select;
 int hk_len,s_len;
 long n;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl",&select,&s_len,&policy,&hk_len,&n) == FAILURE)
    {
    php_printf("Error in cfpr_select_hosts args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_select_hosts(policy,select,(int)n,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_select_reports)

{ const int bufsize = 1000000;
  char buffer[bufsize];

// buffer[0] = '\0';
Nova2PHP_select_reports(buffer,bufsize);
RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_red_hosts)

{ const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll",&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_show_red_hosts args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_show_col_hosts("red",200,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_yellow_hosts)

{  const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll",&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_show_yellow_hosts args");
    RETURN_NULL();
    }
 buffer[0] = '\0';
 Nova2PHP_show_col_hosts("yellow",200,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_green_hosts)

{  const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll",&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_show_green_hosts args");
    RETURN_NULL();
    }
 buffer[0] = '\0';
 Nova2PHP_show_col_hosts("green",200,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_blue_hosts)

{  const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll",&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error in cfpr_show_blue_hosts args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_show_col_hosts("blue",200,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_hosts_ip)

{  const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
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
 Nova2PHP_show_hosts(NULL,fipRegex,fclassRegex,&page,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_show_hosts_name)

{  const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
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
 Nova2PHP_show_hosts(fhostNameRegex,NULL,fclassRegex,&page,buffer,bufsize);

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
    php_printf("Error in cfpr_getlastupdate needs a host key");
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
    php_printf("Error in cfpr_get_host_colour needs a host key");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_host_colour(hostkey,buffer,bufsize);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_list_handles)

{ char *promiser;
 char *ptype;
 int pr_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssb",&promiser,&pr_len,&ptype,&p_len,&regex) == FAILURE)
    {
    php_printf("Error in cfpr_list_handles args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_list_promise_handles(promiser,ptype,"","",(int)regex,buffer,100000);

 RETURN_STRING(buffer,1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_policy_finder_by_handle)

{ char *handle;
 int h_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool escRegex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sb",&handle,&h_len,&escRegex) == FAILURE)
    {
    php_printf("Error in cfpr_policy_finder_by_handle args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_list_handles_policy_finder(handle,"","",escRegex,buffer,100000);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_policy_finder_by_promiser)

{ char *promiser;
 int pr_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool escRegex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sb",&promiser,&pr_len,&escRegex) == FAILURE)
    {
    php_printf("Error in cfpr_policy_finder_by_promiser args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_list_handles_policy_finder("",promiser,"",escRegex,buffer,100000);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_policy_finder_by_bundle)

{ char *bundle;
 int r_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool escRegex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sb",&bundle,&r_len,&escRegex) == FAILURE)
    {
    php_printf("Error in cfpr_policy_finder_by_bundle args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_list_handles_policy_finder("","",bundle,escRegex,buffer,100000);

 RETURN_STRING(buffer,1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_get_handles_for_bundle_with_comments)

{ char *bundle;
 char *btype;
 int r_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&bundle,&r_len,&btype,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_handles_for_bundle_with_comments args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 Nova2PHP_list_promise_handles_with_comments(bundle,btype,buffer,100000);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/
PHP_FUNCTION(cfpr_list_handles_for_bundle)

{ char *bundle;
 char *btype;
 int r_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];
 zend_bool regex;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssb",&bundle,&r_len,&btype,&p_len,&regex) == FAILURE)
    {
    php_printf("Error in cfpr_list_handles args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_list_promise_handles("","",bundle,btype,(int)regex,buffer,100000);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_promise_comment)

{ char *handle;
 int hk_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&handle,&hk_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_promise_comment args");
    RETURN_NULL();
    }

 RETURN_STRING(Nova2PHP_GetPromiseComment(handle),1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_promise_bundle)

{ char *handle;
 int hk_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&handle,&hk_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_promise_bundle args");
    RETURN_NULL();
    }

 RETURN_STRING(Nova2PHP_GetPromiseBundle(handle),1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_promise_type)

{ char *handle;
 int hk_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&handle,&hk_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_promise_type args");
    RETURN_NULL();
    }

 RETURN_STRING(Nova2PHP_GetPromiseType(handle),1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_promiser)

{ char *handle;
 int hk_len;

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&handle,&hk_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_promiser args");
    RETURN_NULL();
    }

 RETURN_STRING(Nova2PHP_GetPromiser(handle),1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_variable)

{ char *key,*scope,*lval;
 int hk_len,s_len,l_len;
 char buffer[2048];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",&key,&hk_len,&scope,&s_len,&lval,&l_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_variable args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_variable(key,scope,lval,buffer,2048);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_classes_for_bundle)

{ char *bundle;
 char *btype;
 int r_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&bundle,&r_len,&btype,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_classes_for_bundle args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_classes_for_bundle(bundle,btype,buffer,100000);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_args_for_bundle)

{ char *bundle;
 char *btype;
 int r_len, p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&bundle,&r_len,&btype,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_handles args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 Nova2PHP_get_args_for_bundle(bundle,btype,buffer,100000);

 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_list_all_bundles)

{
 char *btype,*fbtype;
 int p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&btype,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_all_bundles args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 fbtype =  (p_len == 0) ? NULL : btype;

 Nova2PHP_list_all_bundles(fbtype,buffer,bufsize);
 RETURN_STRING(buffer,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_get_bundle_type)

{
 char *bname,*fbname;
 int n_len;
 const int bufsize = 10000;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&bname,&n_len) == FAILURE)
    {
    php_printf("Error in cfpr_get_bundle_type args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 fbname =  (n_len == 0) ? NULL : bname;

 Nova2PHP_get_bundle_type(fbname,buffer,bufsize);
 RETURN_STRING(buffer,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_list_bundles_using)

{ char *bundle,*fbundle;
 int p_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&bundle,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_bundles_using args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 fbundle =  (p_len == 0) ? NULL : bundle;

 Nova2PHP_list_bundles_using(fbundle,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_bundle_count)

{
 const int bufsize = 32;
 char buffer[bufsize];

 buffer[0] = '\0';

 Nova2PHP_get_bundle_count(buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_promise_count)

{
 const int bufsize = 32;
 char buffer[bufsize];

 buffer[0] = '\0';

 Nova2PHP_get_promise_count(buffer,bufsize);
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

{ char *hkey,*fhkey,*pattern,*fpattern;
 int p_len, h_len,count;
 const int bufsize = 1000000;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hkey,&h_len,&pattern,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_count_classes args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 fpattern = (p_len == 0) ? NULL : pattern;
 fhkey =  (h_len == 0) ? NULL : hkey;

 count = Nova2PHP_countclasses(fhkey,fpattern,1,buffer,bufsize);
 RETURN_LONG((long)count);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_report_class_frequency)

{ char *hkey,*fhkey,*pattern,*fpattern;
 int p_len, h_len;
 const int bufsize = CF_WEBBUFFER;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&hkey,&h_len,&pattern,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_count_classes args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 fpattern = (p_len == 0) ? NULL : pattern;
 fhkey =  (h_len == 0) ? NULL : hkey;

 Nova2PHP_countclasses(fhkey,fpattern,1,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_docroot)

{ const int bufsize = 1000000;
 char buffer[bufsize];

 buffer[0] = '\0';
 Nova2PHP_docroot(buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_get_promise_body)

{ char *type,*ftype,*name,*fname;
 int p_len, t_len;
 const int bufsize = 1000000;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&name,&p_len,&type,&t_len) == FAILURE)
    {
    php_printf("Error in cfpr_show-body args");
    RETURN_NULL();
    }

 buffer[0] = '\0';
 fname = (p_len == 0) ? NULL : name;
 ftype = (t_len == 0) ? NULL : type;

 Nova2PHP_GetPromiseBody(fname,ftype,buffer,bufsize);
 RETURN_STRING(buffer,1);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_list_bodies)

{ char *name;
 char *btype;
 int pr_len, p_len;
 const int bufsize = 1000000;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",&name,&pr_len,&btype,&p_len) == FAILURE)
    {
    php_printf("Error in cfpr_list_bodies args");
    RETURN_NULL();
    }

 buffer[0] = '\0';

 Nova2PHP_list_bodies(name,btype,buffer,100000);
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
    php_printf("Error in cfpr_get_network_rate args");
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
    php_printf("Error is cfpr_report_description function args");
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

{char buf[CF_WEBBUFFER];  // one row is ~300 bytes
 char *hostkey,*reportName;
 int hk_len,rn_len;
 PageInfo page = {0};

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll",&hostkey,&hk_len,&reportName,&rn_len,&(page.resultsPerPage),&(page.pageNum)) == FAILURE)
    {
    php_printf("Error is cfpr_cdp_report_acl function args");
    RETURN_NULL();
    }

 buf[0]='\0';
 Nova2PHP_cdp_report(hostkey,reportName,&page,buf,sizeof(buf));

 RETURN_STRING(buf,1);
}
/******************************************************************************/

PHP_FUNCTION(cfpr_validate_policy)

{ char *file;
 int f_len, ret;
 const int bufsize = 100000;
 char buffer[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&file,&f_len) == FAILURE)
    {
    php_printf("Error in cfpr_validate_policy args");
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
    php_printf("Error is cfpr_delete_host function args");
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
    php_printf("Wrong number of arguments in cfpr_environments_list");
    RETURN_NULL();
    }

 if (!Nova2PHP_environments_list(&el))
    {
    php_printf("Unable to query list of environments in cfpr_environments_list");
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
    php_printf("Wrong arguments in cfpr_environment_contents");
    RETURN_NULL();
    }

 if (!Nova2PHP_environment_contents(name, &hl))
    {
    php_printf("Unable to query environment in cfpr_environment_contents");
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
    php_printf("Wrong arguments in cfpr_get_host_contents");
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
    php_printf("Error in cfpr_get_knowledge_view args");
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
 char *fuser,*fnote,*fnid;
 int u_len,n_len, nid_len;
 time_t datetime;
 const int bufsize = 1000;
 char returnval[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssls",&nid,&nid_len,&user,&u_len,&datetime,&note,&n_len) == FAILURE)
    {
    php_printf("Error in cfpr_add_note function args");
    RETURN_NULL();
    }

 if(nid_len == 0)
    {
    php_printf("Note ID mut be given!!\n");
    RETURN_NULL();
    }

 fnid = nid;
 fuser =  (u_len == 0) ? NULL : user;
 fnote =  (n_len == 0) ? NULL : note;

 returnval[0]='\0';
 Nova2PHP_add_note(fnid,fuser,datetime,fnote,returnval,bufsize);
 RETURN_STRING(returnval,1);
}
/******************************************************************************/
PHP_FUNCTION(cfpr_new_note)

{ char *hostkey,*user,*note, *repid;
 char *fhostkey,*fuser,*fnote, *frepid;
 int hk_len,u_len,n_len,rid_len;
 int report_type;
 time_t datetime;
 const int bufsize = 1000;
 char returnval[bufsize];

 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sslsls",&hostkey,&hk_len,&repid,&rid_len,&report_type,&user,&u_len,&datetime,&note,&n_len) == FAILURE)
    {
    php_printf("Error is cfpr_add_note function args");
    RETURN_NULL();
    }

 /* none of these fields can be null (due to unique key constraint)!!*/
 if(hk_len == 0 && rid_len == 0)
    {
    php_printf("Host Key and report id must be given!!\n");
    RETURN_NULL();
    }

 fhostkey = hostkey;
 frepid = repid;

 fuser =  (u_len == 0) ? NULL : user;
 fnote =  (n_len == 0) ? NULL : note;

 returnval[0]='\0';
 Nova2PHP_add_new_note(fhostkey,frepid,report_type,fuser,datetime,fnote,returnval,bufsize);
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
    php_printf("Error is cfpr_query_note function args");
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
    php_printf("Error is cfpr_hostname function args");
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
   zend_throw_exception(cfmod_exception_args, "Incorrect argument count or types", 0 TSRMLS_CC);
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
  char *name, *description, *includeClassRx, *excludeClassRx, *includeBundleRx;
  int nameLen, descLen, icrxLen, ecrxLen, ibrxLen;
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssss",
                            &name, &nameLen,
                            &description, &descLen,
                            &includeClassRx, &icrxLen,
                            &excludeClassRx, &ecrxLen,
                            &includeBundleRx, &ibrxLen) == FAILURE)
     {
     zend_throw_exception(cfmod_exception_args, "Incorrect argument count or types", 0 TSRMLS_CC);
     RETURN_NULL();
     }

  if(!(nameLen|descLen|icrxLen|ecrxLen|ibrxLen))
     {
     // NOTE: important security check to not fool access control check algorithm
     // FIXME: this is too strict, support empty input (but be careful)
     zend_throw_exception(cfmod_exception_args, "Missing argument contents", 0 TSRMLS_CC);
     RETURN_NULL();
     }

  cfapi_errid errid = CFDB_CreateRole(name, description, includeClassRx, excludeClassRx, includeBundleRx);
  
  if(errid != ERRID_SUCCESS)
     {
     zend_throw_exception(cfmod_exception_generic, (char *)GetErrorDescription(errid), 0 TSRMLS_CC);     
     }
}

/******************************************************************************/

PHP_FUNCTION(cfpr_role_delete)
{
 char *name;
 int nameLen;
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&name, &nameLen) == FAILURE)
     {
     zend_throw_exception(cfmod_exception_args, "Incorrect argument count or types", 0 TSRMLS_CC);
     RETURN_NULL();
     }

  if(!nameLen)
     {
     zend_throw_exception(cfmod_exception_args, "Missing argument contents", 0 TSRMLS_CC);
     RETURN_NULL();
     }

  cfapi_errid errid = CFDB_DeleteRole(name);
  
  if(errid != ERRID_SUCCESS)
     {
     zend_throw_exception(cfmod_exception_generic, (char *)GetErrorDescription(errid), 0 TSRMLS_CC);
     }
}

/******************************************************************************/

PHP_FUNCTION(cfpr_role_list_all)
{
#define LABEL_ROLE_NAME "name"
#define LABEL_ROLE_DESCRIPTION "description"
#define LABEL_ROLE_CLASSRX_INCLUDE "classrxinclude"
#define LABEL_ROLE_CLASSRX_EXCLUDE "classrxexclude"
#define LABEL_ROLE_BUNDLERX_INCLUDE "bundlerxinlcude"
 
 if (ZEND_NUM_ARGS() != 0)
    {
    zend_throw_exception(cfmod_exception_args, "Incorrect argument count or types", 0 TSRMLS_CC);
    RETURN_NULL();
    }

 HubQuery *hq = CFDB_GetAllRoles();

 if(hq->errid != ERRID_SUCCESS)
    {
    DeleteHubQuery(hq, DeleteHubRole);
    zend_throw_exception(cfmod_exception_generic, (char *)GetErrorDescription(hq->errid), 0 TSRMLS_CC);
    RETURN_NULL();
    }

 JsonArray *roles = ParseRolesToJson(hq);

 DeleteHubQuery(hq, DeleteHubRole);
   
 RETURN_JSON(roles);
}

/******************************************************************************/

PHP_FUNCTION(cfpr_role_list_by_name)
{

 char *name;
 int nameLen;
 
 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&name, &nameLen) == FAILURE)
    {
    zend_throw_exception(cfmod_exception_args, "Incorrect argument count or types", 0 TSRMLS_CC);
    RETURN_NULL();
    }

 if(nameLen == 0)
    {
    zend_throw_exception(cfmod_exception_args, "Missing argument contents", 0 TSRMLS_CC);
    RETURN_NULL();
    }

 HubQuery *hq = CFDB_GetRoleByName(name);

 if(hq->errid != ERRID_SUCCESS)
    {
    DeleteHubQuery(hq, DeleteHubRole);
    zend_throw_exception(cfmod_exception_generic, (char *)GetErrorDescription(hq->errid), 0 TSRMLS_CC);
    RETURN_NULL();
    }

 JsonArray *roles = ParseRolesToJson(hq);

 DeleteHubQuery(hq, DeleteHubRole);

 RETURN_JSON(roles);
}

/******************************************************************************/

static JsonArray *ParseRolesToJson(HubQuery *hq)
{
#define LABEL_ROLE_NAME "name"
#define LABEL_ROLE_DESCRIPTION "description"
#define LABEL_ROLE_CLASSRX_INCLUDE "classrxinclude"
#define LABEL_ROLE_CLASSRX_EXCLUDE "classrxexclude"
#define LABEL_ROLE_BUNDLERX_INCLUDE "bundlerxinlcude"

 JsonArray *roles = NULL;
 
 for (Rlist *rp = hq->records; rp != NULL; rp = rp->next)
    {
    HubRole *record = (HubRole *)rp->item;
    JsonObject *role_entry = NULL;
      
    JsonObjectAppendString(&role_entry, LABEL_ROLE_NAME, record->name);
    JsonObjectAppendString(&role_entry, LABEL_ROLE_DESCRIPTION, record->description);
    JsonObjectAppendString(&role_entry, LABEL_ROLE_CLASSRX_INCLUDE, record->classRxInclude);
    JsonObjectAppendString(&role_entry, LABEL_ROLE_CLASSRX_EXCLUDE, record->classRxExclude);
    JsonObjectAppendString(&role_entry, LABEL_ROLE_BUNDLERX_INCLUDE, record->bundleRxInclude);
    
    JsonArrayAppendObject(&roles, role_entry);
    }
 
 return roles;
}
