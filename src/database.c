
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: database.c                                                          */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/

int Nova_CheckDatabaseSanity(struct Attributes a, struct Promise *pp)

{ struct Rlist *rp;
  int retval = true,commas = 0;
 
if (a.database.type && cf_strcmp(a.database.type,"ms_registry") == 0)
   {
#ifdef NT
   Nova_ValidateRegistryPromiser(pp->promiser,a,pp);
#endif
   if (a.database.operation && cf_strcmp(a.database.operation,"create") == 0)
      {
      if (a.database.rows == NULL)
         {
         CfOut(cf_inform,"","No row values promised for the MS registry database");
         }

      if (a.database.columns != NULL)
         {
         CfOut(cf_error,"","Columns are only used to delete promised values for the MS registry database");
         }
      }

   if (a.database.operation && (cf_strcmp(a.database.operation,"delete") == 0 || cf_strcmp(a.database.operation,"drop") == 0))
      {
      if (a.database.columns == NULL)
         {
         CfOut(cf_inform,"","No columns were promised deleted in the MS registry database");
         }

      if (a.database.rows != NULL)
         {
         CfOut(cf_error,"","Rows cannot be deleted in the MS registry database, only entire columns");
         }
      }
   
   for (rp = a.database.rows; rp != NULL; rp=rp->next)
      {
      commas = 0;

      if (CountChar(rp->item,',') != 2)
         {
         CfOut(cf_error,"","Registry row format should be NAME,REG_SZ,VALUE, not \"%s\"",rp->item);
         retval = false;
         }
      }

   for (rp = a.database.columns; rp != NULL; rp=rp->next)
      {
      if (CountChar(rp->item,',') > 0)
         {
         CfOut(cf_error,"","MS registry column format should be NAME only in deletion");
         retval = false;
         }
      }
   }
else if (a.database.type && cf_strcmp(a.database.type,"sql") == 0)
   {
   if (strchr(pp->promiser,'.') == NULL && strchr(pp->promiser,'/') == NULL && strchr(pp->promiser,'\\') == NULL)
      {
      if (a.database.columns)
         {
         CfOut(cf_error,"","Row values promised for an SQL table, but only the root database was promised");
         retval = false;
         }

      if (a.database.rows)
         {
         CfOut(cf_error,"","Columns promised for an SQL table, but only the root database was promised");
         retval = false;
         }
      }
   
   if (a.database.db_server_host == NULL)
      {
      CfOut(cf_error,"","No server host is promised for connecting to the SQL server");
      retval = false;
      }

   if (a.database.db_server_owner == NULL)
      {
      CfOut(cf_error,"","No database login user is promised for connecting to the SQL server");
      retval = false;
      }

   if (a.database.db_server_password == NULL)
      {
      CfOut(cf_error,"","No database authentication password is promised for connecting to the SQL server");
      retval = false;
      }

   for (rp = a.database.columns; rp != NULL; rp=rp->next)
      {
      commas = CountChar(rp->item,',');
      
      if (commas > 2 && commas < 1)
         {
         CfOut(cf_error,"","SQL Column format should be NAME,TYPE[,SIZE]");
         retval = false;
         }
      }

   }

if (a.database.operation && cf_strcmp(a.database.operation,"create") == 0)
   {
   }

if (a.database.operation && (cf_strcmp(a.database.operation,"delete") == 0 || cf_strcmp(a.database.operation,"drop") == 0))
   {
   if (pp->ref == NULL)
      {
      CfOut(cf_error,"","When specifying a delete/drop from an SQL database you must add a comment. Take a backup of the database before making this change. This is a highly destructive operation.");
      retval = false;
      }
   }

return retval;
}

/*****************************************************************************/
/* Linker troubles require this code to be here in the main body             */
/*****************************************************************************/

int Nova_VerifyTablePromise(CfdbConn *cfdb,char *table_path,struct Rlist *columns,struct Attributes a,struct Promise *pp)

{ char name[CF_MAXVARSIZE],type[CF_MAXVARSIZE],query[CF_MAXVARSIZE],table[CF_MAXVARSIZE],db[CF_MAXVARSIZE];
  int i,count,size,no_of_cols,*size_table,*done,identified,retval = true;
  char **name_table,**type_table;

CfOut(cf_verbose,""," -> Verifying promised table structure for \"%s\"",table_path);

if (!Nova_ValidateSQLTableName(table_path,db,table))
   {
   CfOut(cf_error,""," !! The structure of the promiser did not match that for an SQL table, i.e. \"database.table\"\n",table_path);
   return false;
   }
else
   {
   CfOut(cf_verbose,""," -> Assuming database \"%s\" with table \"%s\"",db,table);
   }

/* Verify the existence of the tables within the database */

if (!Nova_TableExists(cfdb,table))
   {
   CfOut(cf_error,""," !! The database did not contain the promised table \"%s\"\n",table_path);

   if (a.database.operation && strcmp(a.database.operation,"create") == 0)
      {
      if (!DONTDO && a.transaction.action != cfa_warn)
         {
         cfPS(cf_error,CF_CHG,"",pp,a," -> Database.table %s doesn't seem to exist, creating\n",table_path);
         return Nova_CreateTableColumns(cfdb,table,columns,a,pp);
         }
      else
         {
         CfOut(cf_error,""," -> Database.table %s doesn't seem to exist, but only a warning was promised\n",table_path);
         }
      }
   
   return false;
   }

/* Get a list of the columns in the table */

Nova_QueryTableColumns(query,db,table);
CfNewQueryDB(cfdb,query);

if (cfdb->maxcolumns != 3)
   {
   cfPS(cf_error,CF_FAIL,"",pp,a,"Could not make sense of the columns");
   CfDeleteQuery(cfdb);
   return false;
   }

/* Assume that the Rlist has been validated and consists of a,b,c */

count = 0;
no_of_cols = RlistLen(columns);

if (!Nova_NewSQLColumns(table,columns,&name_table,&type_table,&size_table,&done))
   {
   cfPS(cf_error,CF_FAIL,"",pp,a,"Could not make sense of the columns");
   return false;
   }

/* Obtain columns from the named table - if any */

while(CfFetchRow(cfdb))
   {
   char *sizestr;
   name[0] = '\0';
   type[0] = '\0';
   size = CF_NOINT;

   strncpy(name,CfFetchColumn(cfdb,0),CF_MAXVARSIZE-1);
   strncpy(type,ToLowerStr(CfFetchColumn(cfdb,1)),CF_MAXVARSIZE-1);
   sizestr = CfFetchColumn(cfdb,2);

   if (sizestr)
      {
      size = Str2Int(sizestr);
      }
   
   CfOut(cf_verbose,"","    ... discovered column (%s,%s,%d)",name,type,size);
   
   if (sizestr && size == CF_NOINT)
      {
      cfPS(cf_verbose,CF_NOP,"",pp,a," !! Integer size of SQL datatype could not be determined or was not specified - invalid promise.");
      Nova_DeleteSQLColumns(name_table,type_table,size_table,done,no_of_cols);
      CfDeleteQuery(cfdb);
      return false;
      }

   identified = false;
   
   for (i = 0; i < no_of_cols; i++)
      {
      if (done[i])
         {
         continue;
         }

      if (strcmp(name,name_table[i]) == 0)
         {
         NovaCheckSQLDataType(type,type_table[i],pp);

         if (size != size_table[i])
            {
            cfPS(cf_error,CF_FAIL,"",pp,a," !! Promised column \"%s\" in database.table \"%s\" has a non-matching array size (%d != %d)",name,table_path,size,size_table[i]);
            }
         else
            {
            CfOut(cf_verbose,""," -> Promised column \"%s\" in database.table \"%s\" is as promised",name,table_path);
            }
         
         count++;
         done[i] = true;
         identified = true;
         break;
         }
      }

   if (!identified)
      {
      cfPS(cf_error,CF_FAIL,"",pp,a,"Column \"%s\" found in database.table \"%s\" is not part of its promise.",name,table_path);

      if (a.database.operation && strcmp(a.database.operation,"drop") == 0)
         {
         cfPS(cf_error,CF_FAIL,"",pp,a,"Cfengine will not promise to repair this, as the operation is potentially too destructive.");
         // Future allow deletion?
         }
      
      retval = false;
      }
   }

CfDeleteQuery(cfdb);

/* Now look for deviations - only if we have promised to create missing */

if (a.database.operation && strcmp(a.database.operation,"drop") == 0)
   {
   return retval;
   }

if (count != no_of_cols)
   {
   for (i = 0; i < no_of_cols; i++)
      {
      if (!done[i])
         {
         CfOut(cf_error,""," !! Promised column \"%s\" missing from database table %s",name_table[i],pp->promiser);
         
         if (!DONTDO && a.transaction.action != cfa_warn)
            {
            if (size_table[i] > 0)
               {
               snprintf(query,CF_MAXVARSIZE-1,"ALTER TABLE %s ADD %s %s(%d)",table,name_table[i],type_table[i],size_table[i]);
               }
            else
               {
               snprintf(query,CF_MAXVARSIZE-1,"ALTER TABLE %s ADD %s %s",table,name_table[i],type_table[i]);
               }
            
            CfVoidQueryDB(cfdb,query);
            cfPS(cf_error,CF_CHG,"",pp,a," !! Adding promised column \"%s\" to database table %s",name_table[i],table);
            retval = true;
            }
         else
            {
            cfPS(cf_error,CF_WARN,"",pp,a," !! Promised column \"%s\" missing from database table %s but only a warning was promised",name_table[i],table);
            retval = false;
            }
         }
      }
   }

Nova_DeleteSQLColumns(name_table,type_table,size_table,done,no_of_cols);

return retval;
}

/*****************************************************************************/

int Nova_TableExists(CfdbConn *cfdb,char *name)

{ struct Rlist *rp,*list = NULL;
  int match = false;

list = Nova_GetSQLTables(cfdb);
 
for (rp = list; rp != NULL; rp=rp->next)
   {
   if (strcmp(name,rp->item) == 0)
      {
      match = true;
      }
   }

DeleteRlist(list);

return match;
}

/*****************************************************************************/

int Nova_CreateTableColumns(CfdbConn *cfdb,char *table,struct Rlist *columns,struct Attributes a,struct Promise *pp)

{ char entry[CF_MAXVARSIZE],query[CF_BUFSIZE];
  int i,*size_table,*done;
  char **name_table,**type_table;
  int no_of_cols = RlistLen(columns);

CfOut(cf_error,""," -> Trying to create table %s\n",table);
  
if (!Nova_NewSQLColumns(table,columns,&name_table,&type_table,&size_table,&done))
   {
   return false;
   }

if (no_of_cols > 0)
   {
   snprintf(query,CF_BUFSIZE-1,"create table %s(",table);
   
   for (i = 0; i < no_of_cols; i++)
      {
      CfOut(cf_verbose,""," -> Forming column template %s %s %d\n",name_table[i],type_table[i],size_table[i]);;
      
      if (size_table[i] > 0)
         {
         snprintf(entry,CF_MAXVARSIZE-1,"%s %s(%d)",name_table[i],type_table[i],size_table[i]);
         }
      else
         {
         snprintf(entry,CF_MAXVARSIZE-1,"%s %s",name_table[i],type_table[i]);
         }

      strcat(query,entry);

      if (i < no_of_cols -1)
         {
         strcat(query,",");
         }
      }

   strcat(query,")");
   }

CfVoidQueryDB(cfdb,query);
Nova_DeleteSQLColumns(name_table,type_table,size_table,done,no_of_cols);
return true;
}


/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

struct Rlist *Nova_GetSQLTables(CfdbConn *cfdb)

{ struct Rlist *list = NULL;
  char query[CF_MAXVARSIZE];

Nova_ListTables(cfdb->type,query);

CfNewQueryDB(cfdb,query);

if (cfdb->maxcolumns != 1)
   {
   CfOut(cf_error,"","Could not make sense of the columns");
   CfDeleteQuery(cfdb);
   return NULL;
   }

while(CfFetchRow(cfdb))
   {
   PrependRScalar(&list,CfFetchColumn(cfdb,0),CF_SCALAR);
   }

CfDeleteQuery(cfdb);

return list;
}


