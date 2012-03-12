/*
  This file is (C) Cfengine AS. See LICENSE for details.
*/

#ifndef CFENGINE_SCORECARDS_H
#define CFENGINE_SCORECARDS_H

typedef enum
{
    HOST_RANK_METHOD_COMPLIANCE   = 0x00000001,
    HOST_RANK_METHOD_ANOMALY      = 0x00000010,
    HOST_RANK_METHOD_PERFORMANCE  = 0x00000100,
    HOST_RANK_METHOD_LASTSEEN     = 0x00001000,
    HOST_RANK_METHOD_MIXED        = 0x00010000
} HostRankMethod;

typedef enum
{
    HOST_COLOUR_GREEN,
    HOST_COLOUR_YELLOW,
    HOST_COLOUR_RED,
    HOST_COLOUR_BLUE,
    HOST_COLOUR_GREEN_YELLOW_RED
} HostColour;

typedef struct
{
    HostRankMethod method;
    HostColour colour;
    time_t blue_time_horizon;
} HostColourFilter;

HostColourFilter *NewHostColourFilter(HostRankMethod method, HostColour colours);

#ifdef HAVE_LIBMONGOC

HostColour Nova_HostScoreToColour(int score);

const char *Nova_HostColourToString(HostColour colour);
HostColour HostColourFromString(const char *colour);

int Nova_GetComplianceScore(HostRankMethod method, double *k, double *rep);

// gets meters from DB, uses GetComplianceScore. rework.
int Nova_GetHostColour(char *lkeyhash, HostRankMethod method, HostColour *result);

// legacy
void ComplianceSummaryGraph(char *hubKeyHash, char *policy, bool constellation, char *buffer, int bufsize);
void Nova_Meter(bson *query, char *db, char *buffer, int bufsize);
void Nova_BarMeter(int pos, double kept, double rep, char *name, char *buffer, int bufsize);

#endif

#endif
