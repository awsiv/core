
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

#include "constraints.h"
#include "files_names.h"
#include "db_common.h"

static void CFDB_SaveBody(EnterpriseDB *dbconn, const Body *body);

/*****************************************************************************/

void CFDB_SaveExpandedPromise(const Promise *pp)
{
    Constraint *cp;
    char rval_buffer[CF_BUFSIZE];
    static int firstCall = true;
    char jStr[32];
    const char *sp;
    char con[CF_MAXVARSIZE];
    EnterpriseDB dbconn = { 0 };
    int j;

// NOTE: Inefficient to Open/Close DB for each promise,
// but call is coming from community which may not have DB

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

    if (firstCall)
    {
        // clear existing data first
        bson b;
        mongo_remove(&dbconn, MONGO_PROMISES_EXP, bson_empty(&b), NULL);
        firstCall = false;
    }

    bson insert_op;

    bson_init(&insert_op);

    bson_append_new_oid(&insert_op, "_id");

    CfDebug("PROMISE: \n");
    CfDebug("\nPromise type is %s, ", pp->agentsubtype);
    CfDebug("class context %s \n", pp->classes);

    bson_append_string(&insert_op, cfp_promisetype, pp->agentsubtype);
    bson_append_string(&insert_op, cfp_classcontext, pp->classes);

    CfDebug("promiser is %s\n", pp->promiser);

    bson_append_string(&insert_op, cfp_promiser_exp, pp->promiser);

    CfDebug("Bundle %s\n", pp->bundle);
    CfDebug("Bundle of type %s\n", pp->bundletype);

    bson_append_string(&insert_op, cfp_bundlename, pp->bundle);
    bson_append_string(&insert_op, cfp_bundletype, pp->bundletype);

    if (pp->audit)
    {
        CfDebug("In file %s near line %zu\n", pp->audit->filename, pp->offset.line);
        bson_append_string(&insert_op, cfp_file, pp->audit->filename);
        bson_append_int(&insert_op, cfp_lineno, pp->offset.line);
    }

// The promise body

    if (pp->promisee.item)
    {
        memset(rval_buffer, 0, sizeof(rval_buffer));
        PrintRval(rval_buffer, CF_BUFSIZE, pp->promisee);
        CfDebug(" -> %s\n", rval_buffer);
        bson_append_string(&insert_op, cfp_promisee_exp, rval_buffer);
    }

    if (pp->ref)
    {
        CfDebug("Comment: %s\n", pp->ref);
        bson_append_string(&insert_op, cfp_comment_exp, pp->ref);
    }

    if ((sp = GetConstraintValue("handle", pp, CF_SCALAR)) || (sp = PromiseID(pp)))
    {
        bson_append_string(&insert_op, cfp_handle_exp, sp);
    }

    {
        bson_append_start_array(&insert_op, cfp_constraints_exp);

        for (cp = pp->conlist, j = 0; cp != NULL; cp = cp->next)
        {
            // comments and handles have their own fields
            if (strcmp(cp->lval, "comment") == 0 || strcmp(cp->lval, "handle") == 0)
            {
                continue;
            }

            memset(rval_buffer, 0, sizeof(rval_buffer));
            PrintRval(rval_buffer, CF_BUFSIZE, cp->rval);
            CfDebug("  %s => %s\n", cp->lval, rval_buffer);
            snprintf(con, sizeof(con), "%s => %s", cp->lval, rval_buffer);
            snprintf(jStr, sizeof(jStr), "%d", j);
            bson_append_string(&insert_op, jStr, con);
            j++;
        }

        bson_append_finish_object(&insert_op);
    }

    bson_finish(&insert_op);

    mongo_insert(&dbconn, MONGO_PROMISES_EXP, &insert_op, NULL);
    bson_destroy(&insert_op);

    CFDB_Close(&dbconn);
}

/*****************************************************************************/

void CFDB_SaveUnExpandedPromises(const Bundle *bundles, const Body *bodies)
{
    Rlist *rp;
    Promise *pp;
    Constraint *cp;
    EnterpriseDB dbconn = { 0 };
    int i, j;
    char iStr[32], jStr[32];
    char rval_buffer[CF_BUFSIZE];
    char con[CF_MAXVARSIZE];
    const char *sp;

    if (!CFDB_Open(&dbconn))
    {
        return;
    }

// remove existing data first
    bson b;
    mongo_remove(&dbconn, MONGO_PROMISES_UNEXP, bson_empty(&b), NULL);

    for (const Bundle *bp = bundles; bp != NULL; bp = bp->next)
    {
        for (const SubType *st = bp->subtypes; st != NULL; st = st->next)
        {
            CfDebug("PROMISE-TYPE: %s\n", st->name);

            for (pp = st->promiselist, i = 0; pp != NULL; pp = pp->next, i++)
            {
                bson insert_op;
                bson_init(&insert_op);
                bson_append_new_oid(&insert_op, "_id");

                CfDebug("\nBundle %s of type %s\n\n", bp->name, bp->type);
                CfDebug("BUNDLE ARGS:\n");

                bson_append_string(&insert_op, cfp_bundlename, bp->name);
                bson_append_string(&insert_op, cfp_bundletype, bp->type);

                {
                    bson_append_start_array(&insert_op, cfp_bundleargs);

                    for (rp = bp->args, i = 0; rp != NULL; rp = rp->next, i++)
                    {
                        CfDebug("   scalar arg %s\n", (char *) rp->item);
                        snprintf(iStr, sizeof(iStr), "%d", i);
                        bson_append_string(&insert_op, iStr, (char *) rp->item);
                    }

                    bson_append_finish_object(&insert_op);
                }

                snprintf(iStr, sizeof(iStr), "%d", i);
                bson_append_string(&insert_op, cfp_promiser, pp->promiser);
                bson_append_string(&insert_op, cfp_promisetype, pp->agentsubtype);
                bson_append_string(&insert_op, cfp_classcontext, pp->classes);

                if (pp->promisee.item)
                {
                    memset(rval_buffer, 0, sizeof(rval_buffer));
                    PrintRval(rval_buffer, CF_BUFSIZE, pp->promisee);

                    bson_append_string(&insert_op, cfp_promisee, rval_buffer);
                }

                // NOTE: We also use audit info to update right expanded promise;
                // can't use handle if there is a variable in it
                if (pp->audit)
                {
                    bson_append_string(&insert_op, cfp_file, pp->audit->filename);
                    bson_append_int(&insert_op, cfp_lineno, pp->offset.line);
                }

                if ((sp = GetConstraintValue("handle", pp, CF_SCALAR)) || (sp = PromiseID(pp)))
                {
                    bson_append_string(&insert_op, cfp_handle, sp);
                }

                if ((sp = GetConstraintValue("comment", pp, CF_SCALAR)))
                {
                    bson_append_string(&insert_op, cfp_comment, sp);
                }

                {
                    bson_append_start_array(&insert_op, cfp_constraints);

                    for (cp = pp->conlist, j = 0; cp != NULL; cp = cp->next)
                    {
                        // comments and handles have their own fields
                        if (strcmp(cp->lval, "comment") == 0 || strcmp(cp->lval, "handle") == 0)
                        {
                            continue;
                        }

                        memset(rval_buffer, 0, sizeof(rval_buffer));
                        PrintRval(rval_buffer, CF_BUFSIZE, cp->rval);
                        CfDebug("  %s => %s\n", cp->lval, rval_buffer);

                        snprintf(con, sizeof(con), "%s => %s", cp->lval, rval_buffer);
                        snprintf(jStr, sizeof(jStr), "%d", j);
                        bson_append_string(&insert_op, jStr, con);
                        j++;
                    }
                    bson_append_finish_object(&insert_op);
                }

                bson_finish(&insert_op);

                mongo_insert(&dbconn, MONGO_PROMISES_UNEXP, &insert_op, NULL);
                bson_destroy(&insert_op);
            }
        }
    }

    /* Now summarize all bodies */
    // clear existing bodies first

    mongo_remove(&dbconn, MONGO_BODIES, bson_empty(&b), NULL);

    for (const Body *bdp = bodies; bdp != NULL; bdp = bdp->next)
    {
        CFDB_SaveBody(&dbconn, bdp);
    }

    CFDB_Close(&dbconn);
}

/*****************************************************************************/

static void CFDB_SaveBody(EnterpriseDB *dbconn, const Body *body)
{
    Rlist *rp;
    Constraint *cp;
    char classContext[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];
    char rval_buffer[CF_BUFSIZE];
    char iStr[32];
    int i;

    bson insert_op;

    bson_init(&insert_op);
    bson_append_new_oid(&insert_op, "_id");

    CfDebug("body %s type %s\n", body->name, body->type);

    bson_append_string(&insert_op, cfb_bodyname, body->name);
    bson_append_string(&insert_op, cfb_bodytype, body->type);

    {
        bson_append_start_array(&insert_op, cfb_bodyargs);

        for (rp = body->args, i = 0; rp != NULL; rp = rp->next, i++)
        {
            CfDebug("%s\n", (char *) rp->item);
            snprintf(iStr, sizeof(iStr), "%d", i);
            bson_append_string(&insert_op, iStr, (char *) rp->item);
        }

        bson_append_finish_object(&insert_op);
    }
    bson_finish(&insert_op);

    mongo_insert(dbconn, MONGO_BODIES, &insert_op, NULL);
    bson_destroy(&insert_op);

// do update with the lval - rval attribs
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfb_bodyname, body->name);
    bson_append_string(&query, cfb_bodytype, body->type);
    bson_finish(&query);

    bson set_op;
    bson_init(&set_op);

    {
        bson_append_start_object(&set_op, "$set");

        for (cp = body->conlist; cp != NULL; cp = cp->next)
        {
            if (cp->classes != NULL)
            {
                // replace illegal key char '.' with  '&'
                ReplaceChar(cp->classes, classContext, sizeof(classContext), '.', '&');
                CfDebug(" if class context %s\n", cp->classes);
            }
            else
            {
                snprintf(classContext, sizeof(classContext), "any");
            }

            memset(rval_buffer, 0, sizeof(rval_buffer));
            PrintRval(rval_buffer, sizeof(rval_buffer), cp->rval);
            CfDebug("  %s => %s\n", cp->lval, rval_buffer);

            snprintf(varName, sizeof(varName), "%s.%s.%s", cfb_classcontext, classContext, cp->lval);
            bson_append_string(&set_op, varName, rval_buffer);
        }

        bson_append_finish_object(&set_op); // $set
    }
    bson_finish(&set_op);

    mongo_update(dbconn, MONGO_BODIES, &query, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&query);
    bson_destroy(&set_op);
}
