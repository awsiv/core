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

#ifdef HAVE_GD_H
#include <gd.h>
#include <gdfontl.h>
#include <gdfontg.h>
#endif

#undef PACKAGE
// #undef AUTOCONF_HOSTNAME TODO: Does removal cause troble ?

#define CF_SHADES 25
#define CF_TIMESERIESDATA 168
#define CF_MAGDATA 48
#define CF_LHISTORYDATA 1464
#define CF_MAGMARGIN 0
#define CF_LHISTORYMARGIN 0
#define CF_TRIBE_SIZE 30

/*****************************************************************************/

struct CfDataView
   {
#ifdef HAVE_GD_H
   gdImagePtr im;
#endif
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
   double data_E[CF_LHISTORYDATA];
   double data_q[CF_LHISTORYDATA];
   double bars[CF_LHISTORYDATA];
   double error_scale;
   double range;
   int margins;
   char *title;
   };

struct CfGraphNode
   {
   int real_id;
   int tribe_id;
   char shortname[CF_SMALLBUF];
   char *fullname;
   double potential;
   double x,y;
   double radius;
   double angle;
   int orbit_parent; /* For non-central nodes - who is the star? */
   int distance_from_centre;
   int horizon; // distance to the edge
   };

#define CF_MIN_RADIUS    30.0
#define CF_RADIUS_SCALE  25.0
#define CF_NODEVISIBLE   14
#define CF_CONTAINMENT_FACTOR 1.15

/*****************************************************************************/

struct CfFileLine
   {
   char  *text;
   int    counter;
   struct CfFileLine *next;
   struct CfFileLine *prev;
   };

/*****************************************************************************/

/* Valid ACL syntax values, from documentation */

// Valid generic permissions
#define CF_VALID_GPERMS "rwx"

// Valid native permission characters
#define CF_VALID_NPERMS_NTFS "drtxTwabBpcoD"
#define CF_VALID_NPERMS_POSIX "rwx"

// Valid operations (first char of mode)
#define CF_VALID_OPS_METHOD_OVERWRITE "=+-"
#define CF_VALID_OPS_METHOD_APPEND "=+-"

// Native perms separators in mode
#define CF_NATIVE_PERMS_SEP_START '('
#define CF_NATIVE_PERMS_SEP_END ')'

#ifdef MINGW
#define CF_GENERIC_READ_NT (FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_READ_EA | READ_CONTROL)
#define CF_GENERIC_WRITE_NT (FILE_WRITE_DATA | FILE_APPEND_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA)
#define CF_GENERIC_EXECUTE_NT (FILE_EXECUTE)
#define CF_MINIMUM_PERMS_NT (SYNCHRONIZE)  // access is always denied if synchronize is not set
#endif /* MINGW */

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

/* acl_linux.c */

int Nova_CheckPosixLinuxACL(char *file_path, struct CfACL acl, struct Attributes a, struct Promise *pp);
#ifdef HAVE_LIBACL
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

/* acl_nt.c */

int Nova_CheckNtACL(char *file_path, struct CfACL acl, struct Attributes a, struct Promise *pp);
#ifdef MINGW
int Nova_CheckNtACEs(char *file_path, struct Rlist *aces, inherit_t inherit, enum cf_acl_method method, struct Attributes a, struct Promise *pp);
int Nova_CheckNtInheritACEs(char *file_path, struct Rlist *aces, enum cf_acl_method method, enum cf_acl_inherit directory_inherit, struct Attributes a, struct Promise *pp);
int Nova_CheckNtDefaultClearACL(char *file_path, struct Attributes a, struct Promise *pp);
int Nova_CheckNtDefaultEqualsAccessACL(char *file_path, struct Attributes a, struct Promise *pp);
void Nova_RemoveEasByInheritance(EXPLICIT_ACCESS *eas, int *eaCount, inherit_t inherit);
void Nova_RemoveEmptyEas(EXPLICIT_ACCESS *eas, int *eaCount);
int Nova_ACLEquals(int *aclsEqual, EXPLICIT_ACCESS *firstEas, int eaCount, ACL *acl);
int Nova_AclToExplicitAccess(EXPLICIT_ACCESS *eas, int eaCount, ACL *acl);
int Nova_ParseAcl(char *file_path, struct Rlist *aces, EXPLICIT_ACCESS *eas, int *eaCount, inherit_t inherit);
int Nova_SetEas(char *file_path, EXPLICIT_ACCESS *eas, int eaCount);
EXPLICIT_ACCESS *Nova_FindAceNt(EXPLICIT_ACCESS *eas, EXPLICIT_ACCESS *endEa, SID *findSid, ACCESS_MODE findPermt, inherit_t findInherit);
void Nova_FreeSids(EXPLICIT_ACCESS *eas, int num);
int Nova_EntityToSid(char **acePtr, SID *sid, DWORD sidSz);
int Nova_ParseModeNt(char **modePtr, ACCESS_MASK *perms);
ACCESS_MODE Nova_ParsePermTypeNt(char *ace);
#endif  /* MINGW */

/* aggregation.c */

void Nova_Aggregate(void);
void Nova_WriteSignalData(FILE *fout,char *filename);
void Nova_MagProbe(void);
void Nova_PackNerveBundle(void);
void Nova_UnPackNerveBundle(void);
void Nova_LookupAggregateClassName(int n,char *name, char *desc);

/* cfcore.c */

void Nova_ShowAllGraphs(FILE *fp,char *s,struct Item *el);
void Nova_ShowGraph(FILE *fp, int i,time_t last,enum observables obs);
void Nova_MainPage(char *s,struct Item *el);
void Nova_OtherPages(char *s,struct Item *el);
void Nova_PlotTopicCosmos(int topic,double **adj,char **names,int dim,char *view);
int Nova_GetTribe(int *tribe_id,struct CfGraphNode *tribe_nodes, double **tribe_adj,char **n,int topic,double **full_adj,int dim_full,int *tertiary_boundary);
int Nova_AlreadyInTribe(int node, int *tribe_id);
void Nova_InitVertex(struct CfGraphNode *v,int i);
int Nova_NewVertex(struct CfGraphNode *tribe,int i,char *name,int distance,int real);
void Nova_EigenvectorCentrality(double **A,double *v,int dim);
void Nova_MatrixOperation(double **A,double *v,int dim);

/* coordinates.c */

int Nova_ViewPortY(struct CfDataView *cfv,double y,double offset);
int Nova_ViewPortX(struct CfDataView *cfv,double x);

/* copernicus.c */

void Nova_DrawTribe(char *filename,int *tribe_id,struct CfGraphNode *tribe_node, double **tribe_adj,int tribe_size, double *tribe_evc,char **n,int topic,double **full_adj,int dim_full,int tertiary_boundary);
void Nova_IlluminateTribe(int *tribe_id,struct CfGraphNode *tribe_node, double **tribe_adj,int tribe_size, double *tribe_evc,char **n,int topic,double **full_adj,int dim_full,int tertiary_boundary);
int Nova_GetMaxEvcNode(double *evc,int tribe_size);
int Nova_GetAdjacent(int i,double **adj,int size, struct CfGraphNode *tribe, struct CfGraphNode *neighbours);
int Nova_SplayAdjacent(int i,double **adj,int tribe_size,struct CfGraphNode *tribe,int *trail,struct CfGraphNode *neighbours);
void Nova_CopyNeighbours2(struct CfGraphNode *from,int tribe_size,struct CfGraphNode to[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int index1);
void Nova_CopyNeighbours3(struct CfGraphNode *from,int tribe_size,struct CfGraphNode to[CF_TRIBE_SIZE][CF_TRIBE_SIZE][CF_TRIBE_SIZE],int index1,int index2);
int Nova_GetEvcTops(double **adj,int size, double *evc, int *tops);
void Nova_Annealing(struct CfGraphNode neighbours3[CF_TRIBE_SIZE][CF_TRIBE_SIZE][CF_TRIBE_SIZE],int size1,int size2[CF_TRIBE_SIZE],int size3[CF_TRIBE_SIZE][CF_TRIBE_SIZE],int tribe_size);
void Nova_CentreScale(struct CfDataView *cfv,double min_x,double max_x,double min_y,double max_y);
int Nova_X(struct CfDataView cfv,double x);
int Nova_Y(struct CfDataView cfv,double y);
int Nova_TribeUnion(int *array1,struct CfGraphNode *array2, int size1, int size2);
void Nova_ClearTrail(int *array);
void Nova_AnchorTrail(int *array,int node);
int Nova_InTrail(int *trail,int node);
void Nova_Line(struct CfDataView cfv,double x1,double y1,double x2,double y2,int colour);
void Nova_Disc(struct CfDataView cfv,double x1,double y1,double radius,int colour);
void Nova_Print(struct CfDataView cfv,double x,double y,char *s,int colour);
void Nova_BigPrint(struct CfDataView cfv,double x,double y,char *s,int colour);
double Nova_SignPerturbation(int i);
void Nova_ColdBall(struct CfDataView cfv,double x,double y,double radius,int *shade);
void Nova_HotBall(struct CfDataView cfv,double x,double y,double radius,int *shade);
void Nova_AlignmentCorrection(double *x,double *y,double cx,double cy);
void Nova_MapHorizon(double x,double y,double *min_x,double *min_y,double *max_x,double *max_y);
double Nova_Orbit(struct CfDataView cfv,double radius,double min_x,double max_x,double min_y,double max_y);
void Nova_MapBall(FILE *fp,struct CfDataView cfv,struct CfGraphNode n);

/* database.c */

int Nova_CheckDatabaseSanity(struct Attributes a, struct Promise *pp);

/* graphs.c */

void Nova_BuildMeters(struct CfDataView *cfv,char *hostname);
void Nova_BarMeter(struct CfDataView *cfv,int number,double kept,double repaired,char *s);
void Nova_Title(struct CfDataView *cfv,int col);
void Nova_BuildGraphs(struct CfDataView *cfv);
void Nova_MakePalette(struct CfDataView *cfv);
void Nova_MakeCosmosPalette(struct CfDataView *cfv);
double Nova_GetNowPosition(time_t now);
void Nova_IncludeFile(FILE *fout,char *name);
void Nova_NavBar(FILE *fout);
void Nova_GetLevel(char *id,int *kept,int *repaired);
void Nova_CreateHostPortal(struct Item *list);
void Nova_GetAllLevels(int *kept,int *repaired,struct Item *list,char **names);
void Nova_BuildMainMeter(struct CfDataView *cfv,struct Item *list);
void Nova_GraphLegend(FILE *fp);
void Nova_GraphMagLegend(FILE *fp);

/* histogram.c */

int Nova_ReadHistogram(struct CfDataView *cfv, char *filename);
void Nova_DrawHistoAxes(struct CfDataView *cfv,int col);
void Nova_PlotHistogram(struct CfDataView *cfv,int *blues,struct Item *spectrum);
void Nova_ViewHisto(struct CfDataView *cfv,char *filename, char *title,enum observables obs,char *hist);
struct Item *Nova_AnalyseHistogram(struct CfDataView *cfv,char *name,enum observables obs,char *host);

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

/* license.c */

void Nova_HashKey(char *buffer,unsigned char digest[EVP_MAX_MD_SIZE+1]);
int Nova_EnterpriseExpiry(char *day,char *month,char *year);

/* magnify.c */

int Nova_ViewLatest(struct CfDataView *cfv,char *filename, char *title,enum observables obs,char *host);
int Nova_ReadMagTimeSeries(struct CfDataView *cfv, char *filename);
void Nova_DrawMagQAxes(struct CfDataView *cfv,int col);
void Nova_PlotMagQFile(struct CfDataView *cfv,int col1,int col2,int col3);
void Nova_AnalyseMag(struct CfDataView *cfv,char *name,enum observables obs,char *host);

/* monitoring.c */

void Nova_GetClassName(int i,char *name,char *desc);
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
void Nova_LookupClassName(int n,char *name, char *desc);

/* processes.c */

char *Nova_GetProcessOptions(void);
void Nova_LogFileChange(char *file,int change,struct Attributes a,struct Promise *pp);
void Nova_DoFileDiff(char *file,char *destination,struct stat sb,struct stat dsb);
int Nova_GetFirstChangePosition(char *file,char *destination);
int Nova_FileIsBinary(char *name,int size,int maxsize);
void Nova_ReportFileChange(FILE *fp,char *file,char *destination,int maxsize);
int Nova_LoadFileHunks(char *file,char *destination,struct CfFileLine **list1,struct CfFileLine **list2,int *l1,int *l2,int maxsize);
struct CfFileLine *AppendFileLine(struct CfFileLine **liststart,char *item,int pos);
void DeleteAllFileLines(struct CfFileLine *item);
void DeleteFileLine(struct CfFileLine  **liststart,struct CfFileLine *item);

/* promises.c */

void Nova_Version(void);
char * Nova_StrVersion(void);
void Nova_NotePromiseCompliance(struct Promise *pp,double val,enum cf_status status);
time_t Nova_GetPromiseCompliance(struct Promise *pp,double *value,double *average,double *var,time_t *t);
void Nova_CheckAutoBootstrap(void);
void Nova_SetPolicyServer(char *name);
void Nova_CreateFailSafe(char *name);
void Nova_SetDocRoot(char *name);
void Nova_EnterpriseContext(void);
void Nova_PreSanitizePromise(struct Promise *pp);

/* registry.c */

int Nova_ValidateRegistryPromiser(char *s,struct Attributes a,struct Promise *pp);
int Nova_CopyRegistryValue(char *key,char *value,char *buffer);


/* reporting.c */

void Nova_CSV2XML(struct Rlist *list);
void Nova_SummarizePromiseRepaired(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
void Nova_SummarizePromiseNotKept(int xml,int html,int csv,int embed,char *stylesheet,char *head,char *foot,char *web);
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
void Nova_GrandSummary(void);
void SummarizeComms(void);

/* server.c */

const EVP_CIPHER *Nova_CfengineCipher(char type);
char Nova_CfEnterpriseOptions(void);
int Nova_CfSessionKeySize(char c);
void  Nova_RegisterLiteralServerData(char *handle,struct Promise *pp);
int Nova_ReturnLiteralData(char *handle,char *retval);
char *Nova_GetRemoteScalar(char *handle,char *server,int encrypted,char *rcv);
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
int Nova_ViewWeek(struct CfDataView *cfv,char *filename, char *title,enum observables obs,char *week);
void Nova_AnalyseWeek(struct CfDataView *cfv,char *name,enum observables obs,char *host);

/* trends.c */

int Nova_ViewLongHistory(struct CfDataView *cfv,char *filename, char *title,enum observables obs,char *host);
int Nova_ReadLongHistory(struct CfDataView *cfv, char *name);
void Nova_DrawLongHAxes(struct CfDataView *cfv,int col);
void Nova_PlotLongHFile(struct CfDataView *cfv,int col1,int col2,int col3);
void Nova_AnalyseLongHistory(struct CfDataView *cfv,char *name,enum observables obs,char *host);


#ifdef MINGW
/* win_api.c */

void NovaWin_OpenNetwork(void);
int NovaWin_GetCurrentUserName(char *userName, int userNameLen);
char *NovaWin_GetErrorStr(void);

/* win_file.c */

void NovaWin_CreateEmptyFile(char *name);
FILE *NovaWin_FileHandleToStream(HANDLE fHandle, char *mode);
int NovaWin_IsExecutable(char *file);
int NovaWin_mkdir(const char *path, mode_t mode);
int NovaWin_rename(const char *oldpath, const char *newpath);
int NovaWin_chmod(const char *path, mode_t mode);
int NovaWin_FileExists(const char *fileName);
int NovaWin_IsDir(char *fileName);
int NovaWin_VerifyOwner(char *file,struct Promise *pp,struct Attributes attr);
int NovaWin_TakeFileOwnership(char *path);
int NovaWin_SetFileOwnership(char *path, SID *sid);
int NovaWin_GetOwnerName(char *path, char *owner, int ownerSz);
void NovaWin_VerifyFileAttributes(char *file,struct stat *dstat,struct Attributes attr,struct Promise *pp);
void NovaWin_VerifyCopiedFileAttributes(char *file,struct stat *dstat,struct Attributes attr,struct Promise *pp);

/* win_pipe.c */

FILE *NovaWin_cf_popen(char *command,char *type);
FILE *NovaWin_cf_popen_sh(char *command,char *type);
FILE *NovaWin_cf_popensetuid(char *command,char *type,uid_t uid,gid_t gid,char *chdirv,char *chrootv);
FILE *NovaWin_cf_popen_shsetuid(char *command,char *type,uid_t uid,gid_t gid,char *chdirv,char *chrootv);
int NovaWin_cf_pclose(FILE *pp);
int NovaWin_cf_pclose_def(FILE *pfp, struct Attributes a, struct Promise *pp);

/* win_proc.c */

int NovaWin_DoAllSignals(struct Item *siglist,struct Attributes a,struct Promise *pp);
int NovaWin_GracefulTerminate(pid_t pid);
int NovaWin_ShellCommandReturnsZero(char *comm, int useshell);
int NovaWin_RunCmd(char *comm, int useshell, int inheritHandles, char *startDir, STARTUPINFO *si, HANDLE *procHandle);
int NovaWin_GetCurrentProcessOwner(SID *sid, int sidSz);
int NovaWin_SetTokenPrivilege(HANDLE token, char *privilegeName, int enablePriv);

/* win_ps.c */

int NovaWin_LoadProcessTable(struct Item **procdata,char *psopts);

/* win_user.c */

int NovaWin_UserNameToSid(char *userName, SID *sid, DWORD sidSz);
int NovaWin_GroupNameToSid(char *groupName, SID *sid, DWORD sidSz);
int NovaWin_SidToName(SID* sid, char *name, int nameSz);
int NovaWin_SidToString(SID *sid, char *stringSid, int stringSz);
int NovaWin_StringToSid(char *stringSid, SID *sid, int sidSz);


#endif  /* MINGW */


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

#define CF_REPAIR_LOG    "nova_repair.log"
#define CF_NOTKEPT_LOG   "nova_notkept.log"

