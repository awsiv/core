/*

This file is (C) Cfengine AS. See COSL LICENSE for details.

*/

#ifndef CFENGINE_BENCHMARK_H
#define CFENGINE_BENCHMARK_H

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"


#ifdef HAVE_LIBMONGOC

int Nova_BenchmarkReportStorage(char *reportFilePath, int iterationsToRun);

#endif  /* HAVE_LIBMONGOC */


#endif  /* NOT CFENGINE_BENCHMARK_H */

    
