
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

/*****************************************************************************/
/*                                                                           */
/* File: sql.c                                                               */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*****************************************************************************/
/* Level                                                                     */
/*****************************************************************************/

int Nova_SizeCfSQLContainer()
{
return sizeof(CfdbConn);
}

/*****************************************************************************/

void Nova_CreateDBQuery(enum cfdbtype type,char *query)

{ 
switch (type)
   {
   case cfd_mysql:
       snprintf(query,CF_MAXVARSIZE-1,"show databases");
       break;
       
   case cfd_postgres:
       /* This gibberish is the simplest thing I can find in postgres */

       snprintf(query,CF_MAXVARSIZE-1,"SELECT pg_database.datname FROM pg_database");       
       break;

   default:
       snprintf(query,CF_MAXVARSIZE,"NULL QUERY");
       break;
   }
}

/*****************************************************************************/

int Nova_ValidateSQLTableName(char *table_path,char *db,char *table)

{ char *sp;
  int dot = false,back = false,fwd = false;

/* Valid separators . / or \ only */ 

if ((sp = cf_strchr(table_path,'/')))
   {
   fwd = true;
   *sp = '.';
   }

if ((sp = cf_strchr(table_path,'\\')))
   {
   back = true;
   *sp = '.';
   }

if ((sp = cf_strchr(table_path,'.')))
   {
   dot = true;
   sp++;
   }

/* Should contain a single separator */

if (dot + back + fwd > true)
   {
   CfOut(cf_error,"","");
   return false;
   }

memset(db,0,CF_MAXVARSIZE);
cf_strncpy(db,table_path,sp-table_path-1);
cf_strncpy(table,sp,CF_MAXVARSIZE-1);
return true;
}

/*****************************************************************************/

void Nova_QueryTableColumns(char *s,char *db,char *table)

{
snprintf(s,CF_MAXVARSIZE-1,"SELECT column_name,data_type,character_maximum_length FROM information_schema.columns WHERE table_name ='%s' AND table_schema = '%s'",table,db); 
}

/*****************************************************************************/

int Nova_NewSQLColumns(char *table,struct Rlist *columns,char ***name_table,char ***type_table,int **size_table,int **done)

{ int i,no_of_cols = RlistLen(columns);
  struct Rlist *cols,*rp;
 
*name_table = (char **)malloc(sizeof(char *)*(no_of_cols+1));
*type_table = (char **)malloc(sizeof(char *)*(no_of_cols+1));
*size_table = (int *)malloc(sizeof(int)*(no_of_cols+1));
*done = (int *)malloc(sizeof(int)*(no_of_cols+1));

for (i = 0,rp = columns; rp != NULL; rp = rp->next,i++)
   {
   (*done)[i] = 0;
   
   cols = SplitStringAsRList((char *)rp->item,',');

   if (!cols)
      {
      CfOut(cf_error,"","No columns promised for table \"%s\" - makes no sense",table);
      return false;
      }
   
   if (cols->item == NULL)
      {
      CfOut(cf_error,"","Malformed column promise for table \"%s\" - found not even a name",table);
      free(*name_table);
      free(*type_table);
      free(*size_table);
      free(*done);
      return false;   
      }

   (*name_table)[i] = cf_strdup((char *)cols->item);
   
   if (cols->next == NULL)
      {
      CfOut(cf_error,"","Malformed column \"%s\" promised for table \"%s\" - missing a type",name_table[i],table);
      free(*name_table);
      free(*type_table);
      free(*size_table);
      free(*done);
      return false;   
      }
   
   (*type_table)[i] = cf_strdup(cols->next->item);

   if (cols->next->next == NULL)
      {
      (*size_table)[i] = 0;
      }
   else
      {
      if (cols->next->next->item)
         {
         (*size_table)[i] = Str2Int(cols->next->next->item);
         }
      else
         {
         (*size_table)[i] = 0;
         }
      }
   
   DeleteRlist(cols);
   }

return true;
}


/*****************************************************************************/

void Nova_DeleteSQLColumns(char **name_table,char **type_table,int *size_table,int *done,int len)

{ int i;

if (name_table == NULL || type_table == NULL || size_table == NULL)
   {
   return;
   }
 
for (i = 0; i < len; i++)
   {
   if (name_table[i] != NULL)
      {
      free(name_table[i]);
      }
   
   if (type_table[i] != NULL)
      {
      free(type_table[i]);
      }
   }
  
free(name_table);
free(type_table);
free(size_table);
free(done);
}

/*****************************************************************************/

int NovaCheckSQLDataType(char *type,char *ref_type,struct Promise *pp)

{ static char *aliases[3][2] =
     {
     {"varchar","character@varying"},
     {"varchar","character varying"},
     {NULL,NULL}
     };

 int i;

for (i = 0; aliases[i][0] != NULL; i++)
   {
   if (cf_strcmp(ref_type,aliases[i][0]) == 0 || cf_strcmp(ref_type,aliases[i][1]) == 0 || cf_strcmp(type,aliases[i][0]) == 0 || cf_strcmp(type,aliases[i][1]) == 0)
      {
      if ((cf_strcmp(type,ref_type) != 0) && (cf_strcmp(aliases[i][0],ref_type) != 0))
         {
         CfOut(cf_verbose,""," !! Promised column in database %s has a non-matching type (%s != %s)",pp->promiser,ref_type,type);
         }      
      }
   else
      {
      if (cf_strcmp(type,ref_type) != 0)
         {
         CfOut(cf_verbose,""," !! Promised column in database %s has a non-matching type (%s != %s)",pp->promiser,ref_type,type);
         }
      }
   }

return true;
}

/*****************************************************************************/

void Nova_ListTables(int type,char *query)

{  
switch (type)
   {
   case cfd_mysql:
       snprintf(query,CF_MAXVARSIZE-1,"show tables");
       break;
       
   case cfd_postgres:
       /* This gibberish is the simplest thing I can find in postgres */

       snprintf(query,CF_MAXVARSIZE-1,"SELECT c.relname as \"Name\" FROM pg_catalog.pg_class c JOIN pg_catalog.pg_roles r ON r.oid = c.relowner LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace WHERE n.nspname = 'public'");       
       break;

   default:
       snprintf(query,CF_MAXVARSIZE,"NULL QUERY");
       break;
   }
}
