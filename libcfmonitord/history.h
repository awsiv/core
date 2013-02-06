/*

 This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_HISTORY_H
#define CFENGINE_HISTORY_H

#include "cf3.defs.h"

void HistoryUpdate(Averages newvals);

void VerifyMeasurement(double *this, Attributes a, Promise *pp);

#endif
