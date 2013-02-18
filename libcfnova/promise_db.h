#ifndef PROMISE_DB_H
#define PROMISE_DB_H

#include "cf3.defs.h"

#include "sequence.h"

void CFDB_SaveExpandedPromise(const Promise *pp);
void CFDB_SaveUnExpandedPromises(const Seq *bundles, const Seq *bodies);

#endif // PROMISE_DB_H
