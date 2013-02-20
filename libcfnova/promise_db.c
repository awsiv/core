
/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#include "promise_db.h"
#include "cf.nova.h"

#include "files_names.h"
#include "db_common.h"
#include "bson_lib.h"
#include "string_lib.h"
#include "policy.h"

static void BsonAppendPromisee(bson *b, const Rval *promisee);
static void CFDB_SaveBody(EnterpriseDB *dbconn, const Body *body);


/*****************************************************************************/

void CFDB_SaveExpandedPromise(const Promise *pp)
{
    char rval_buffer[CF_BUFSIZE];
    static int firstCall = true;
    char jStr[32];
    const char *sp;
    char con[CF_MAXVARSIZE];
    EnterpriseDB dbconn = { 0 };
    int j = 0;

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
        MongoRemove(&dbconn, MONGO_PROMISES_EXP, bson_empty(&b), NULL);
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

    if (pp->ns)
    {
        bson_append_string(&insert_op, cfr_bundle_namespace, pp->ns);
    }

    if (pp->audit)
    {
        CfDebug("In file %s near line %zu\n", pp->audit->filename, pp->offset.line);
        bson_append_string(&insert_op, cfp_file, pp->audit->filename);
        bson_append_int(&insert_op, cfp_lineno, pp->offset.line);
    }

    BsonAppendPromisee(&insert_op, &(pp->promisee));

    if (pp->ref)
    {
        CfDebug("Comment: %s\n", pp->ref);
        bson_append_string(&insert_op, cfp_comment_exp, pp->ref);
    }

    if ((sp = ConstraintGetRvalValue("handle", pp, RVAL_TYPE_SCALAR)) || (sp = PromiseID(pp)))
    {
        bson_append_string(&insert_op, cfp_handle_exp, sp);
    }

    {
        bson_append_start_array(&insert_op, cfp_constraints_exp);

        for (size_t i = 0; i < SeqLength(pp->conlist); i++)
        {
            Constraint *cp = SeqAt(pp->conlist, i);

            // comments and handles have their own fields
            if (strcmp(cp->lval, "comment") == 0 || strcmp(cp->lval, "handle") == 0)
            {
                continue;
            }

            memset(rval_buffer, 0, sizeof(rval_buffer));
            RvalPrint(rval_buffer, CF_BUFSIZE, cp->rval);
            CfDebug("  %s => %s\n", cp->lval, rval_buffer);
            snprintf(con, sizeof(con), "%s => %s", cp->lval, rval_buffer);
            snprintf(jStr, sizeof(jStr), "%d", j);
            bson_append_string(&insert_op, jStr, con);
            j++;
        }

        bson_append_finish_object(&insert_op);
    }

    BsonFinish(&insert_op);

    MongoInsert(&dbconn, MONGO_PROMISES_EXP, &insert_op, NULL);
    bson_destroy(&insert_op);

    CFDB_Close(&dbconn);
}

/*****************************************************************************/

void CFDB_SaveUnExpandedPromises(const Seq *bundles, const Seq *bodies)
{
    Rlist *rp;
    EnterpriseDB dbconn = { 0 };
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
    MongoRemove(&dbconn, MONGO_PROMISES_UNEXP, bson_empty(&b), NULL);

    for (size_t i = 0; i < SeqLength(bundles); i++)
    {
        const Bundle *bp = SeqAt(bundles, i);

        for (size_t sti = 0; sti < SeqLength(bp->subtypes); sti++)
        {
            const SubType *st = SeqAt(bp->subtypes, sti);

            CfDebug("PROMISE-TYPE: %s\n", st->name);

            size_t j = 0;
            for (size_t ppi = 0; ppi < SeqLength(st->promises); ppi++)
            {
                Promise *pp = SeqAt(st->promises, ppi);

                bson insert_op;
                bson_init(&insert_op);
                bson_append_new_oid(&insert_op, "_id");

                CfDebug("\nBundle %s of type %s\n\n", bp->name, bp->type);
                CfDebug("BUNDLE ARGS:\n");

                bson_append_string(&insert_op, cfp_bundlename, bp->name);
                bson_append_string(&insert_op, cfp_bundletype, bp->type);

                {
                    bson_append_start_array(&insert_op, cfp_bundleargs);

                    size_t k = 0;
                    for (rp = bp->args; rp != NULL; rp = rp->next, k++)
                    {
                        CfDebug("   scalar arg %s\n", (char *) rp->item);
                        snprintf(iStr, sizeof(iStr), "%ud", (unsigned int)k);
                        bson_append_string(&insert_op, iStr, (char *) rp->item);
                    }

                    bson_append_finish_object(&insert_op);
                }

                snprintf(iStr, sizeof(iStr), "%ud", (unsigned int)j);
                bson_append_string(&insert_op, cfp_promiser, pp->promiser);
                bson_append_string(&insert_op, cfp_promisetype, pp->agentsubtype);
                bson_append_string(&insert_op, cfp_classcontext, pp->classes);

                BsonAppendPromisee(&insert_op, &(pp->promisee));

                // NOTE: We also use audit info to update right expanded promise;
                // can't use handle if there is a variable in it
                if (pp->audit)
                {
                    bson_append_string(&insert_op, cfp_file, pp->audit->filename);
                    bson_append_int(&insert_op, cfp_lineno, pp->offset.line);
                }

                if ((sp = ConstraintGetRvalValue("handle", pp, RVAL_TYPE_SCALAR)) || (sp = PromiseID(pp)))
                {
                    bson_append_string(&insert_op, cfp_handle, sp);
                }

                if ((sp = ConstraintGetRvalValue("comment", pp, RVAL_TYPE_SCALAR)))
                {
                    bson_append_string(&insert_op, cfp_comment, sp);
                }

                if (pp->ns)
                {
                    bson_append_string(&insert_op, cfr_bundle_namespace, pp->ns);
                }

                {
                    bson_append_start_array(&insert_op, cfp_constraints);

                    for (size_t k = 0; k < SeqLength(pp->conlist); k++)
                    {
                        Constraint *cp = SeqAt(pp->conlist, k);

                        // comments and handles have their own fields
                        if (strcmp(cp->lval, "comment") == 0 || strcmp(cp->lval, "handle") == 0)
                        {
                            continue;
                        }

                        memset(rval_buffer, 0, sizeof(rval_buffer));
                        RvalPrint(rval_buffer, CF_BUFSIZE, cp->rval);
                        CfDebug("  %s => %s\n", cp->lval, rval_buffer);

                        snprintf(con, sizeof(con), "%s => %s", cp->lval, rval_buffer);
                        snprintf(jStr, sizeof(jStr), "%ud", (unsigned int)j);
                        bson_append_string(&insert_op, jStr, con);
                    }
                    bson_append_finish_object(&insert_op);
                }

                BsonFinish(&insert_op);

                MongoInsert(&dbconn, MONGO_PROMISES_UNEXP, &insert_op, NULL);
                bson_destroy(&insert_op);
            }
        }
    }

    /* Now summarize all bodies */
    // clear existing bodies first

    MongoRemove(&dbconn, MONGO_BODIES, bson_empty(&b), NULL);

    for (size_t i = 0; i < SeqLength(bodies); i++)
    {
        const Body *bdp = SeqAt(bodies, i);
        CFDB_SaveBody(&dbconn, bdp);
    }

    CFDB_Close(&dbconn);
}

/*****************************************************************************/

static void BsonAppendPromisee(bson *b, const Rval *promisee)
{
    Rlist *promisee_list = NULL;

    if (promisee->item)
    {
        switch(promisee->type)
        {
        case RVAL_TYPE_SCALAR:
            RlistAppend(&promisee_list, (char *)promisee->item, RVAL_TYPE_SCALAR);
            BsonAppendStringArrayRlist(b, cfp_promisee, promisee_list);
            RlistDestroy(promisee_list);
            break;

        case RVAL_TYPE_LIST:
            BsonAppendStringArrayRlist(b, cfp_promisee, (Rlist *) promisee->item);
            break;

        default:
            break;
        } 
    }
}

/*****************************************************************************/

static void CFDB_SaveBody(EnterpriseDB *dbconn, const Body *body)
{
    Rlist *rp;
    char classContext[CF_MAXVARSIZE], varName[CF_MAXVARSIZE];
    char rval_buffer[CF_BUFSIZE];
    char iStr[32];

    bson insert_op;

    bson_init(&insert_op);
    bson_append_new_oid(&insert_op, "_id");

    CfDebug("body %s type %s\n", body->name, body->type);

    bson_append_string(&insert_op, cfb_bodyname, body->name);
    bson_append_string(&insert_op, cfb_bodytype, body->type);

    {
        bson_append_start_array(&insert_op, cfb_bodyargs);

        int i = 0;
        for (rp = body->args; rp != NULL; rp = rp->next, i++)
        {
            CfDebug("%s\n", (char *) rp->item);
            snprintf(iStr, sizeof(iStr), "%d", i);
            bson_append_string(&insert_op, iStr, (char *) rp->item);
        }

        bson_append_finish_object(&insert_op);
    }
    BsonFinish(&insert_op);

    MongoInsert(dbconn, MONGO_BODIES, &insert_op, NULL);
    bson_destroy(&insert_op);

// do update with the lval - rval attribs
    bson query;
    bson_init(&query);
    bson_append_string(&query, cfb_bodyname, body->name);
    bson_append_string(&query, cfb_bodytype, body->type);
    BsonFinish(&query);

    bson set_op;
    bson_init(&set_op);

    {
        bson_append_start_object(&set_op, "$set");

        for (size_t i = 0; i < SeqLength(body->conlist); i++)
        {
            Constraint *cp = SeqAt(body->conlist, i);

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
            RvalPrint(rval_buffer, sizeof(rval_buffer), cp->rval);
            CfDebug("  %s => %s\n", cp->lval, rval_buffer);

            snprintf(varName, sizeof(varName), "%s.%s.%s", cfb_classcontext, classContext, cp->lval);
            bson_append_string(&set_op, varName, rval_buffer);
        }

        bson_append_finish_object(&set_op); // $set
    }
    BsonFinish(&set_op);

    MongoUpdate(dbconn, MONGO_BODIES, &query, &set_op, MONGO_UPDATE_UPSERT, NULL);

    bson_destroy(&query);
    bson_destroy(&set_op);
}
