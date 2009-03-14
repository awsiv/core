
/*

 This file is (C) Cfengine AS. See LICENSE for details.

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

int Nova_VerifyDatabasePromise(CfdbConn *cfdb,enum cfdbtype dbtype,char *database,struct Attributes a,struct Promise *pp)

{ char query[CF_MAXVARSIZE],name[CF_MAXVARSIZE];
  int found = false;
 
CfOut(cf_verbose,""," -> Verifying promised database");

switch (dbtype)
   {
   case cfd_mysql:
       snprintf(query,CF_MAXVARSIZE-1,"show databases");
       break;
       
   case cfd_postgres:
       snprintf(query,CF_MAXVARSIZE-1,"SELECT pg_database.datname as \"Database\", WHERE pg_database.datdba = pg_user.usesysid");       
       break;
   }

CfNewQueryDB(cfdb,query);
       
if (cfdb->maxcolumns < 1)
   {
   CfOut(cf_error,""," !! The schema did not promise the expected number of fields - got %d expected >= %d\n",cfdb->maxcolumns,1);
   return false;
   }

while(CfFetchRow(cfdb))
   {
   strncpy(name,CfFetchColumn(cfdb,0),CF_MAXVARSIZE-1);

   if (strcmp(name,database) == 0)
      {
      found = true;
      }
   }

if (found)
   {
   CfOut(cf_inform,""," -> Database \"%s\" exists on this connection",database);
   return true;
   }
else
   {
   CfOut(cf_inform,""," !! Database \"%s\" does not seem to exist on this connection",database);
   }

/* Get a list of the columns in the table */

if (a.transaction.action != cfa_warn && !DONTDO)
   {
   CfOut(cf_verbose,""," -> Attempting to create the database %s",database);
   snprintf(query,CF_MAXVARSIZE-1,"create database %s",database); 
   CfVoidQueryDB(cfdb,query);
   return cfdb->result;
   }
else
   {
   CfOut(cf_error,""," !! Need to create the database %s but only a warning was promised\n",database);
   return false;
   }
}

/*****************************************************************************/

int Nova_VerifyTablePromise(CfdbConn *cfdb,enum cfdbtype dbtype,char *table,struct Rlist *columns,struct Attributes a,struct Promise *pp)

{ char name[CF_MAXVARSIZE],type[CF_MAXVARSIZE],query[CF_MAXVARSIZE];
  char **name_table,**type_table;
  int i,count,size,no_of_cols,*size_table,*done;
  struct Rlist *rp, *cols;

CfOut(cf_verbose,""," -> Verifying promised table structure");
  
/* Get a list of the columns in the table */

snprintf(query,CF_MAXVARSIZE-1,"SELECT column_name,data_type,character_maximum_length FROM information_schema.columns WHERE table_name ='%s'",table); 
CfVoidQueryDB(cfdb,query);

if (cfdb->maxcolumns != 3)
   {
   CfOut(cf_error,"","The schema did not promise the expected number of fields - got %d expected %d\n",cfdb->maxcolumns,3);
   return false;
   }

/* Assume that the Rlist has been validated and consists of triplets */

count = 0;
no_of_cols = RlistLen(columns);

name_table = (char **)malloc(sizeof(char *)*no_of_cols);
type_table = (char **)malloc(sizeof(char *)*no_of_cols);
size_table = (int *)malloc(sizeof(int)*no_of_cols);
done = (int *)malloc(sizeof(int)*no_of_cols);

for (i = 0,rp = columns; rp != NULL; rp = rp->next,i++)
   {
   cols = SplitStringAsRList((char *)rp->item,',');
   name_table[i] = (char *)rp->item;
   type_table[i] = rp->next->item;
   size_table[i] = Str2Int(rp->next->next->item);
   done[i] = false;
   DeleteRlist(cols);
   }
   
while(CfFetchRow(cfdb))
   {
   strncpy(name,CfFetchColumn(cfdb,0),CF_MAXVARSIZE-1);
   strncpy(type,ToLowerStr(CfFetchColumn(cfdb,1)),CF_MAXVARSIZE-1);
   size = Str2Int(CfFetchColumn(cfdb,2));

   if (size == CF_NOINT)
      {
      cfPS(cf_error,CF_NOP,"",pp,a,"Integer size of datatype could not be determined - invalid promise.");
      delete(name_table);
      delete(type_table);
      delete(size_table);
      delete(done);
      return false;
      }

   for (i = 0; i < no_of_cols; i++)
      {
      if (done[i])
         {
         continue;
         }
      
      if (strcmp(name,name_table[i]) == 0)
         {
         if (strcmp(type,type_table[i]) != 0)
            {
            cfPS(cf_error,CF_FAIL,"",pp,a,"Column %s in database %s has a non-matching type (%s != %s)",name,pp->promiser,type_table[i],type);           
            }

         if (size != size_table[i])
            {
            cfPS(cf_error,CF_FAIL,"",pp,a,"Column %s in database %s has a non-matching array size (%d != %d)",name,pp->promiser,size,size_table[i]);
            }
         
         count++;
         done[i] = true;
         }
      }

   if (a.transaction.action == cfa_warn)
      {
      cfPS(cf_error,CF_FAIL,"",pp,a,"Column %s in database %s does not exist",name,pp->promiser);
      }
   else
      {
      // Repair
      if (!DONTDO)
         {
         }
      }   
   }

if (count != no_of_cols)
   {
   for (i = 0; i < no_of_cols; i++)
      {
      if (!DONTDO)
         {
         // Add column
         }
      }
   }

CfDeleteQuery(cfdb);
delete(name_table);
delete(type_table);
delete(size_table);
delete(done);
return true;
}
