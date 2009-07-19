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

#include <gd.h>
#include <gdfontl.h>
#include <gdfontg.h>

#undef PACKAGE
#undef AUTOCONF_HOSTNAME

#define CF_SHADES 50
#define CF_TIMESERIESDATA 168
#define CF_MAGDATA 48
#define CF_MAGMARGIN 0


struct CfDataView
   {
   gdImagePtr im;
   int width;
   int height;
   int margin;
   int max_x;  // Window dimensions
   int max_y;
   int origin_x;
   int origin_y;
   double scale_x;
   double scale_y;
   double max;
   double min;
   int over;
   int under;
   int over_dev1;
   int over_dev2;
   int under_dev1;
   int under_dev2;
   double data_E[CF_TIMESERIESDATA];
   double data_q[CF_TIMESERIESDATA];
   double bars[CF_TIMESERIESDATA];
   double error_scale;
   double range;
   int margins;
   char *title;
   };

/* Valid ACL syntax values, from documentation */

// Valid generic permissions
#define CF_VALID_GPERMS "rwx"

// Valid native permission characters
#define CF_VALID_NPERMS_POSIX "rwx"

// Valid operations (first char of mode)
#define CF_VALID_OPS_METHOD_OVERWRITE "=+-"  // op can only be empty or equal when method => overwrite
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
void Nova_SetACLDefaults(char *path, struct CfACL *acl);
int Nova_CheckACLSyntax(char *file,struct CfACL acl,struct Promise *pp);
int Nova_CheckACESyntax(char *ace, char *valid_nperms, char *valid_ops, int deny_support, int mask_support,struct Promise *pp);
int Nova_CheckModeSyntax(char **mode_p, char *valid_nperms, char *valid_ops,struct Promise *pp);
int Nova_CheckPermTypeSyntax(char *permt, int deny_support,struct Promise *pp);
int Nova_CheckDirectoryInherit(char *path, struct CfACL *acl, struct Promise *pp);

#ifdef HAVE_LIBACL
int Nova_CheckPosixLinuxACL(char *file_path, struct CfACL acl, struct Attributes a, struct Promise *pp);
int Nova_CheckPosixLinuxAccessACEs(struct Rlist *aces, enum cf_acl_method method, char *file_path, struct Attributes a, struct Promise *pp);
int Nova_CheckPosixLinuxInheritACEs(struct Rlist *aces, enum cf_acl_method method, enum cf_acl_inherit directory_inherit, char *file_path, struct Attributes a, struct Promise *pp);
int Nova_CheckPosixLinuxACEs(struct Rlist *aces, enum cf_acl_method method, char *file_path, acl_type_t acl_type, struct Attributes a, struct Promise *pp);
int Nova_CheckDefaultEqualsAccessACL(char *file_path, struct Attributes a, struct Promise *pp);
int Nova_CheckDefaultClearACL(char *file_path, struct Attributes a, struct Promise *pp);
int Nova_ParseEntityPosixLinux(char **str, acl_entry_t ace, int *is_mask);
int Nova_ParseModePosixLinux(char *mode, acl_permset_t old_perms);
acl_entry_t Nova_FindACE(acl_t acl, acl_entry_t ace_find);
int Nova_ACLEquals(acl_t first, acl_t second);
int Nova_ACECount(acl_t acl);
int Nova_PermsetEquals(acl_permset_t first, acl_permset_t second);
#endif

/* aggregation.c */

void Nova_WriteSignalData(FILE *fout,char *filename);
void Nova_MagProbe(void);
void Nova_PackNerveBundle(void);
void Nova_UnPackNerveBundle(char *host);
void Nova_LookupAggregateClassName(int n,char *name,char *hostname);

/* cfcore.c */

void Nova_ShowAllGraphs(FILE *fp,char *s,struct Item *el);
void Nova_ShowGraph(FILE *fp, int i,time_t last,enum observables obs);
void Nova_MainPage(char *s,struct Item *el);
void Nova_OtherPages(char *s,struct Item *el);

/* coordinates.c */

int Nova_ViewPortY(struct CfDataView *cfv,double y,double offset);
int Nova_ViewPortX(struct CfDataView *cfv,double x);

/* database.c */

int Nova_CheckDatabaseSanity(struct Attributes a, struct Promise *pp);

/* graphs.c */

void Nova_BuildMeters(struct CfDataView *cfv,char *hostname);
void Nova_BarMeter(struct CfDataView *cfv,int number,double kept,double repaired,char *s);
void Nova_Title(struct CfDataView *cfv,int col);
void Nova_BuildGraphs(struct CfDataView *cfv);
void Nova_MakePalette(struct CfDataView *cfv);
double Nova_GetNowPosition(time_t now);
void Nova_IncludeFile(FILE *fout,char *name);
void Nova_NavBar(FILE *fout);

/* histogram.c */

void Nova_ReadHistogram(struct CfDataView *cfv, char *filename);
void Nova_DrawHistoAxes(struct CfDataView *cfv,int col);
void Nova_PlotHistogram(struct CfDataView *cfv,int *blues,struct Item *spectrum);
void Nova_ViewHisto(struct CfDataView *cfv,char *filename, char *title,enum observables obs);
struct Item *Nova_AnalyseHistogram(struct CfDataView *cfv,char *name,enum observables obs);

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

/* processes.c */

char *Nova_GetProcessOptions(void);

/* promises.c */

void Nova_Version(void);
char * Nova_StrVersion(void);
void Nova_NotePromiseCompliance(struct Promise *pp,double val);
time_t Nova_GetPromiseCompliance(struct Promise *pp,double *value,double *average,double *var,time_t *t);
int Nova_EnterpriseExpiry(char *day,char *month,char *year);
void Nova_CheckAutoBootstrap(void);
void Nova_SetPolicyServer(char *name);
void Nova_CreateFailSafe(char *name);
void Nova_SetDocRoot(char *name);

/* registry.c */

int Nova_ValidateRegistryPromiser(char *s,struct Attributes a,struct Promise *pp);
int Nova_CopyRegistryValue(char *key,char *value,char *buffer);


/* reporting.c */

void Nova_CSV2XML(struct Rlist *list);
void Nova_SummarizePerPromiseCompliance(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeCompliance(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeSetuid(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeFileChanges(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_ReportSoftware(struct CfPackageManager *list);
void Nova_ReportPatches(struct CfPackageManager *list);
void Nova_SummarizeSoftware(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeUpdates(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizeVariables(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_NoteVarUsage(void);

/* server.c */

void  Nova_RegisterLiteralServerData(char *handle,struct Promise *pp);
char *Nova_ReturnLiteralData(char *handle);
char *Nova_GetRemoteScalar(char *handle,char *server,int encrypted);
int Nova_ParseHostname(char *name,char *hostname);
int Nova_RetrieveUnreliableValue(char *caller,char *handle,char *buffer);
void Nova_CacheUnreliableValue(char *caller,char *handle,char *buffer);
pid_t Nova_StartTwin(int argc,char **argv);
void Nova_SignalTwin(void);
void Nova_SignalOther(void);
void Nova_ReviveOther(int argc,char **argv);

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

/* timeseries.c */

int Nova_ReadTimeSeries(struct CfDataView *cfv, char *filename);
void Nova_DrawQAxes(struct CfDataView *cfv,int col);
void Nova_PlotQFile(struct CfDataView *cfv,int col1,int col2,int col3);
int Nova_ViewWeek(struct CfDataView *cfv,char *filename, char *title,enum observables obs);
void Nova_AnalyseWeek(struct CfDataView *cfv,char *name,enum observables obs);

/* magnify.c */

void Nova_ReadMagTimeSeries(struct CfDataView *cfv, char *filename);
void Nova_DrawMagQAxes(struct CfDataView *cfv,int col);
void Nova_PlotMagQFile(struct CfDataView *cfv,int col1,int col2,int col3);
void Nova_ViewLatest(struct CfDataView *cfv,char *filename, char *title,enum observables obs);
void Nova_AnalyseMag(struct CfDataView *cfv,char *name,enum observables obs);

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



