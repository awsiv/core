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
{
    Rlist *rp;
    KeyBinding *kp;

    CfOut(cf_verbose, "", " -> Looking for key for \"%s\" in cache", name);

    for (rp = SERVER_KEYRING; rp != NULL; rp = rp->next)
    {
        kp = (KeyBinding *) rp->item;

        if (strcmp(kp->name, name) == 0)
        {
            CfOut(cf_verbose, "", " -> Retrieving key for \"%s\" from cache with address", name, kp->address);
            ThreadLock(cft_system);
            RSA_up_ref(kp->key);
            ThreadUnlock(cft_system);
            return kp->key;
        }
    }

    return NULL;
}

/********************************************************************/

void Nova_IdempAddToKeyRing(char *name, char *ipaddress, RSA *key)
{
    Rlist *rp;
    KeyBinding *kp;

    for (rp = SERVER_KEYRING; rp != NULL; rp = rp->next)
    {
        kp = (KeyBinding *) rp->item;

        if (strcmp(kp->name, name) == 0)
        {
            return;
        }
    }

    CfOut(cf_verbose, "", " -> Caching key for %s in the ring", name);

    ThreadLock(cft_system);
    kp = xmalloc(sizeof(KeyBinding));
    ThreadUnlock(cft_system);

    ThreadLock(cft_system);
    kp->name = xstrdup(name);
    ThreadUnlock(cft_system);

    rp = PrependRlist(&SERVER_KEYRING, "nothing", CF_SCALAR);

    ThreadLock(cft_system);
    free(rp->item);
    rp->item = kp;
    RSA_up_ref(key);
    ThreadUnlock(cft_system);
    kp->address = xstrdup(ipaddress);
    kp->key = key;
    kp->timestamp = time(NULL);
}

#endif
