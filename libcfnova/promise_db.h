#ifndef PROMISE_DB_H
#define PROMISE_DB_H

#include "cf3.defs.h"

void CFDB_SaveExpandedPromise(const Promise *pp);
void CFDB_SaveUnExpandedPromises(const Bundle *bundles, const Body *bodies);

#endif // PROMISE_DB_H
