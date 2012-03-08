/*
  This file is (C) Cfengine AS. See LICENSE for details.
*/

#ifndef CFENGINE_SCORECARDS_H
#define CFENGINE_SCORECARDS_H

#ifdef HAVE_LIBMONGOC

HostColour Nova_HostScoreToColour(int score);
const char *Nova_HostColourToString(HostColour colour);

int Nova_GetComplianceScore(HostRankMethod method, double *k, double *rep);

// gets meters from DB, uses GetComplianceScore. rework.
int Nova_GetHostColour(char *lkeyhash, HostRankMethod method, HostColour *result);

// legacy
void ComplianceSummaryGraph(char *hubKeyHash, char *policy, bool constellation, char *buffer, int bufsize);
void Nova_Meter(bson *query, char *db, char *buffer, int bufsize);
void Nova_BarMeter(int pos, double kept, double rep, char *name, char *buffer, int bufsize);

#endif

#endif
