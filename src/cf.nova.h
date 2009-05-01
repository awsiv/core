/*****************************************************************************/
/*                                                                           */
/* File: cf.nova.h                                                           */
/*                                                                           */
/*****************************************************************************/

#define NOVA 1

# ifdef HAVE_LDAP_H
#  include <ldap.h>
# endif

#ifdef HAVE_ACL_H
# include <acl.h>
#endif

#ifdef HAVE_SYS_ACL_H
# include <sys/acl.h>
#endif

#ifdef HAVE_ACL_LIBACL_H
# include <acl/libacl.h>
#endif

/* Valid ACL syntax values, from documentation */

// Valid generic permissions
#define CF_VALID_GPERMS "rwx"

// Valid native permission characters
#define CF_VALID_NPERMS_POSIX "rwx"

// Valid operations (first char of mode)
#define CF_VALID_OPS_METHOD_OVERWRITE "="  // op can only be empty or equal when method => overwrite
#define CF_VALID_OPS_METHOD_APPEND "=+-"

// Native perms separators in mode
#define CF_NATIVE_PERMS_SEP_START '('
#define CF_NATIVE_PERMS_SEP_END ')'

// Cfengine standard model permissions (non-overlapping bitmasks)
#define CF_PERM_CF_NONE 0x0
#define CF_PERM_CF_EXECUTE 0x1
#define CF_PERM_CF_WRITE 0x2
#define CF_PERM_CF_READ 0x4

/************************************************************************************/
/* Prototypes                                                                       */
/************************************************************************************/

/* acl.c */

void Nova_VerifyACL(char *file,struct Attributes a, struct Promise *pp);
void Nova_SetACLDefaults(struct CfACL *acl);
int Nova_CheckACLSyntax(struct CfACL acl,struct Promise *pp);
int Nova_CheckACESyntax(char *ace, char *valid_nperms, char *valid_ops, int deny_support,struct Promise *pp);
int Nova_CheckModeSyntax(char **mode_p, char *valid_nperms, char *valid_ops,struct Promise *pp);
int Nova_CheckPermTypeSyntax(char *permt, int deny_support,struct Promise *pp);
int Nova_CheckDirectoryInherit(struct CfACL *acl, struct Promise *pp);

#ifdef HAVE_LIBACL
int Nova_CheckPosixLinuxACL(char *file_path, struct CfACL acl);
int Nova_CheckPosixLinuxAccessACEs(struct Rlist *aces, enum cf_acl_method method, char *file_path);
int Nova_CheckPosixLinuxInheritACEs(struct Rlist *aces, enum cf_acl_method method, enum cf_acl_inherit directory_inherit, char *file_path);
int Nova_CheckPosixLinuxACEs(struct Rlist *aces, enum cf_acl_method method, char *file_path, acl_type_t acl_type);
int Nova_CheckDefaultEqualsAccessACL(char *file_path);
int Nova_CheckDefaultClearACL(char *file_path);
int Nova_ParseEntityPosixLinux(char **str, acl_entry_t ace, int *is_mask);
int Nova_ParseModePosixLinux(char *mode, acl_permset_t old_perms);
acl_entry_t Nova_FindACE(acl_t acl, acl_entry_t ace_find);
int Nova_ACLEquals(acl_t first, acl_t second);
int Nova_ACECount(acl_t acl);
int Nova_PermsetEquals(acl_permset_t first, acl_permset_t second);
#endif

/* database.c */

int Nova_CheckDatabaseSanity(struct Attributes a, struct Promise *pp);

/* html.c */

void NovaHtmlHeader(FILE *fp,char *title,char *css,char *webdriver,char *header);
void NovaHtmlFooter(FILE *fp,char *footer);

/* knowledge.c */

void Nova_SyntaxCompletion(char *s);
void Nova_ListAgents(void);
void Nova_ListFunctions(void);
void Nova_ListFunction(struct FnCallType f);
void Nova_ListPromiseTypes(void);

void Nova_MapPromiseToTopic(FILE *fp,struct Promise *pp,char *version);
void Nova_ShowTopicRepresentation(FILE *fp);
void Nova_BundleReference(FILE *fp,char *bundle);
void Nova_TypeNode(FILE *fp,char *type);
void Nova_PromiseNode(FILE *fp,struct Promise *pp,int calltype);
void Nova_TypeNode(FILE *fp,char *type);
void Nova_BundleNode(FILE *fp,char *bundle);
void Nova_BodyNode(FILE *fp,char *body,int calltype);
char *Nova_PromiseID(struct Promise *pp);
void Nova_DependencyGraph(struct Topic *map);
void Nova_PlotTopicDependencies(int topic,double **adj,char **names,int dim);
void Nova_MapClassParameterAssociations(FILE *fp, struct Promise *pp,char *promise_id);
double NovaShiftAverage(double new,double old);
double NovaExtractValueFromStream(char *handle,struct Item *stream,struct Attributes a,struct Promise *pp);
void NovaLogSymbolicValue(char *handle,struct Item *stream,struct Attributes a,struct Promise *pp);
void Nova_ShowBundleDependence(FILE *fp);
void Nova_RegisterBundleDepedence(char *name,struct Promise *pp);
char *NovaEscape(char *s);
void NovaShowValues(FILE *fp,struct BodySyntax bs);

/* ldap.c */

#ifdef HAVE_LDAP_H
void *Nova_LDAPValue(char *uri,char *basedn,char *filter,char *name,char *scope,char *sec);
void *Nova_LDAPList(char *uri,char *dn,char *filter,char *name,char *scope,char *sec);
void *Nova_LDAPArray(char *array,char *uri,char *dn,char *filter,char *scope,char *sec);
void *Nova_RegLDAP(char *uri,char *dn,char *filter,char *name,char *scope,char *regex,char *sec);
LDAP *NovaQueryLDAP(char *uri,char *sec);
int NovaStr2Scope(char *scope);
#endif

/* monitoring.c */

void Nova_GetClassName(int i,char *name);
void NovaInitMeasurements(void);
void Nova_HistoryUpdate(char *key,struct Averages newvals);
void Nova_UpdateShiftAverage(struct Averages *shift_value,struct Averages *newvals);
void Nova_ResetShiftAverage(struct Averages *shift_value);
double ShiftAverage(double new,double old);
void Nova_VerifyMeasurement(double *this,struct Attributes a,struct Promise *pp);
void Nova_LongHaul(char *day,char *month,char* lifecycle,char *shift);
void NovaOpenNewLifeCycle(int age,FILE **fp);
void NovaCloseLifeCycle(int age,FILE **fp);
void NovaIncrementShift(char *day,char *month,char* lifecycle,char *shift);
int NovaLifeCyclePassesGo(char *d,char *m,char *l,char *s,char *day,char *month,char* lifecycle,char *shift);
int NovaGetNextDay(int day,char *month,int year);
int NovaGetSlotHash(char *name);
struct Item *NovaGetMeasurementStream(struct Attributes a,struct Promise *pp);
struct Item *NovaReSample(int slot,struct Attributes a,struct Promise *pp);
void NovaNamedEvent(char *eventname,double value,struct Attributes a,struct Promise *pp);
void Nova_SetMeasurementPromises(struct Item **classlist);
void Nova_LoadSlowlyVaryingObservations(void);
void Nova_DumpSlowlyVaryingObservations(void);
void Nova_LookupClassName(int n,char *name);

/* promises.c */

void Nova_Version(void);
char * Nova_StrVersion(void);
void Nova_NotePromiseCompliance(struct Promise *pp,double val);
time_t Nova_GetPromiseCompliance(struct Promise *pp,double *value,double *average,double *var,time_t *t);

/* registry.c */

int Nova_ValidateRegistryPromiser(char *s,struct Attributes a,struct Promise *pp);

/* reporting.c */

void Nova_SummarizeCompliance(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeSetuid(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeFileChanges(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_ReportSoftware(struct CfPackageManager *list);
void Nova_SummarizeSoftware(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeUpdates(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);

/* server.c */

void  Nova_RegisterLiteralServerData(char *handle,struct Promise *pp);
char *Nova_ReturnLiteralData(char *handle);
char *Nova_GetRemoteScalar(char *handle,char *server,int encrypted);
int Nova_ParseHostname(char *name,char *hostname);

/* sql.c */

int Nova_ValidateSQLTableName(char *table_path,char *db,char *table);
int Nova_VerifyTablePromise(CfdbConn *cfdb,char *table_path,struct Rlist *columns,struct Attributes a,struct Promise *pp);
int Nova_ValidateSQLTableName(char *table_path,char *db,char *table);
void Nova_QueryTableColumns(char *s,char *db,char *table);
int Nova_NewSQLColumns(char *table,struct Rlist *columns,char ***name_table,char ***type_table,int **size_table,int **done);
void Nova_DeleteSQLColumns(char **name_table,char **type_table,int *size_table,int *done,int len);
void Nova_CreateDBQuery(enum cfdbtype type,char *query);
int Nova_CreateTableColumns(CfdbConn *cfdb,char *table,struct Rlist *columns,struct Attributes a,struct Promise *pp);
int NovaCheckSQLDataType(char *type,char *ref_type,struct Promise *pp);
int Nova_TableExists(CfdbConn *cfdb,char *name);
struct Rlist *Nova_GetSQLTables(CfdbConn *cfdb);
void Nova_ListTables(int type,char *query);

/***************************************************************************/

#define NOVA_USES "relies on promise from"
#define NOVA_GIVES "makes promise to"

#define NOVA_ACTIVATED "is activated by class context"
#define NOVA_ACTIVATES "activates promises"

#define NOVA_IMPACTS "has direct influence on"
#define NOVA_ISIMPACTED "is directly influenced by"
#define NOVA_BUNDLE_DATA "uses variables defined in"
#define NOVA_BUNDLE_DATA_INV_B "promises variables used by bundle"
#define NOVA_BUNDLE_DATA_INV_P "promises variables used by promise"

#define NOVA_HISTORYDB "history.db"


struct month_days
   {
   char *m;
   int d;
   };
    
