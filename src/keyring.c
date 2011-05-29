/*****************************************************************************/
/*                                                                           */
/* File: keyring.c<2>                                                        */
/*                                                                           */
/* Created: Sun May 29 13:38:26 2011                                         */
/*                                                                           */
/*****************************************************************************/

#ifdef NOT_DEFINED

// These functions are deprecated either temporarily or permanently

/********************************************************************/

RSA *Nova_SelectKeyRing(char *name)

{ struct Rlist *rp;
  struct CfKeyBinding *kp;
  
CfOut(cf_verbose,""," -> Looking for key for \"%s\" in cache",name);
 
for (rp = SERVER_KEYRING; rp !=  NULL; rp=rp->next)
   {
   kp = (struct CfKeyBinding *) rp->item;

   if (strcmp(kp->name,name) == 0)
      {
      CfOut(cf_verbose,""," -> Retrieving key for \"%s\" from cache with address",name,kp->address);
      ThreadLock(cft_system);
      RSA_up_ref(kp->key);
      ThreadUnlock(cft_system);
      return kp->key;
      }
   }

return NULL;
}

/********************************************************************/

void Nova_IdempAddToKeyRing(char *name,char *ipaddress,RSA *key)

{ struct Rlist *rp;
  struct CfKeyBinding *kp;
  
for (rp = SERVER_KEYRING; rp !=  NULL; rp=rp->next)
   {
   kp = (struct CfKeyBinding *) rp->item;

   if (strcmp(kp->name,name) == 0)
      {
      return;
      }
   }

CfOut(cf_verbose,""," -> Caching key for %s in the ring",name);

ThreadLock(cft_system);
kp = (struct CfKeyBinding *)malloc((sizeof(struct CfKeyBinding)));
ThreadUnlock(cft_system);

if (kp == NULL)
   {
   return;
   }

ThreadLock(cft_system);
if ((kp->name = strdup(name)) == NULL)
   {
   free(kp);
   ThreadUnlock(cft_system);
   return;
   }
ThreadUnlock(cft_system);

rp = PrependRlist(&SERVER_KEYRING,"nothing",CF_SCALAR);

ThreadLock(cft_system);
free(rp->item);
rp->item = kp;
RSA_up_ref(key);
ThreadUnlock(cft_system);
kp->address = strdup(ipaddress);
kp->key = key;
kp->timestamp = time(NULL);
}

/********************************************************************/

void Nova_PurgeKeyRing()

{ struct Rlist *rp,*rpp = NULL;
  struct CfKeyBinding *kp;
  time_t now = time(NULL);
  static time_t then;
  
if (now < then + 3600 && then > 0 && then <= now + 3600)
   {
   // Rate limiter
   return;
   }

then = now;
rp = SERVER_KEYRING;

while (rp !=  NULL)
   {
   kp = (struct CfKeyBinding *) rp->item;

   CfOut(cf_verbose,""," -> Holding key for %s (%s)",kp->name,kp->address);
   
   if (now > kp->timestamp + KEYTTL*3600)
      {
      ThreadLock(cft_system);

      if (rpp == NULL)
         {
         SERVER_KEYRING = rp->next;
         }
      else
         {
         rpp->next = rp->next;
         }

      RSA_free(kp->key);
      
      if (kp->address)
         {
         free(kp->address);
         }

      free(kp->name);
      free(kp);
      free(rp);      
      ThreadUnlock(cft_system);

      if (rpp == NULL)
         {
         rp = SERVER_KEYRING;
         }
      else
         {
         rp = rpp->next;
         }
      }
   else
      {
      rpp = rp;
      rp = rp->next;
      }
   }

CfOut(cf_verbose,"","----");
}


#endif
