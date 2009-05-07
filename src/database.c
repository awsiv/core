
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
  char *sp;
 
if (a.database.type && strcmp(a.database.type,"ms_registry") == 0)
   {
#ifdef NT
   Nova_ValidateRegistryPromiser(pp->promiser,a,pp);
#endif
   if (a.database.operation && strcmp(a.database.operation,"create") == 0)
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

   if (a.database.operation && (strcmp(a.database.operation,"delete") == 0 || strcmp(a.database.operation,"drop") == 0))
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
      for (sp = rp->item; *sp != '\0'; sp++)
         {
         if (*sp == ',')
            {
            commas++;
            }         
         }

      if (commas != 2)
         {
         CfOut(cf_error,"","Registry row format should be NAME,REG_SZ,VALUE, not \"%s\"",rp->item);
         retval = false;
         }
      }

   for (rp = a.database.columns; rp != NULL; rp=rp->next)
      {
      for (sp = rp->item; *sp != '\0'; sp++)
         {
         if (*sp == ',')
            {
            commas++;
            }         
         }

      if (commas > 0)
         {
         CfOut(cf_error,"","MS registry column format should be NAME only in deletion");
         retval = false;
         }
      }
   }
else if (a.database.type && strcmp(a.database.type,"sql") == 0)
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
      for (sp = rp->item; *sp != '\0'; sp++)
         {
         if (*sp == ',')
            {
            commas++;
            }         
         }

      if (commas > 2 && commas < 1)
         {
         CfOut(cf_error,"","SQL Column format should be NAME,TYPE[,SIZE]");
         retval = false;
         }
      }

   }

if (a.database.operation && strcmp(a.database.operation,"create") == 0)
   {
   }

if (a.database.operation && (strcmp(a.database.operation,"delete") == 0 || strcmp(a.database.operation,"drop") == 0))
   {
   if (pp->ref == NULL)
      {
      CfOut(cf_error,"","When specifying a delete/drop from an SQL database you must add a comment. Take a backup of the database before making this change. This is a highly destructive operation.");
      retval = false;
      }
   }

return retval;
}

