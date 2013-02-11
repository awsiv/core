/*
  This file is (C) Cfengine AS. See LICENSE for details.
*/

#ifndef CFENGINE_SCORECARDS_H
#define CFENGINE_SCORECARDS_H

#include <bson.h>

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
    HOST_COLOUR_BLUE = 0,
    HOST_COLOUR_GREEN,
    HOST_COLOUR_YELLOW,
    HOST_COLOUR_RED,
    HOST_COLOUR_GREEN_YELLOW_RED,
    HOST_COLOUR_BLACK,
    HOST_COLOUR_NONE
} HostColour;

typedef enum
{
    PROMISE_CONTEXT_MODE_ALL,
    PROMISE_CONTEXT_MODE_USER,
    PROMISE_CONTEXT_MODE_INTERNAL
} PromiseContextMode;

typedef struct
{
    HostRankMethod method;
    HostColour colour;
    time_t blue_time_horizon;
    PromiseContextMode promise_context;
} HostColourFilter;

HostColourFilter *NewHostColourFilter(HostRankMethod method, HostColour colours,
                                      PromiseContextMode promise_context);
const char *Nova_HostColourToString(HostColour colour);
HostColour HostColourFromString(const char *colour);

HostColour HostColourFromScoreForConnectedHost(int score);
HostColour HostColourFromScore(time_t now, time_t last_report, time_t blue_horizon,
                               int score, bool is_black);

int HostComplianceScore(double kept, double repaired);
int Nova_GetComplianceScore(HostRankMethod method, double *k, double *rep,
                            PromiseContextMode promise_context);

/**
  @brief Convert promise context mode string to PromiseContextMode.
  @param mode String context: "user"/"system"/NULL. Return PROMISE_CONTEXT_MODE_ALL on null or not match.
  @return bool
  */
PromiseContextMode PromiseContextModeFromString(const char *mode);

/**
  @brief Check if promise handle match specified context.
  @param handle Promise handle to check.
  @param promise_context Promise context.
  @return true/false.
  */
bool IsHandleWithinPromiseContext(const char *handle, PromiseContextMode promise_context);

/**
  @brief Check is key is in the promise context.
  @param ns Optional namespace of the key / NULL for no namespace.
  @param key Key to match within namespace.
  @return true/false.
  */
bool IsKeyWithinPromiseContext(const char *ns, const char *key,
                               PromiseContextMode promise_context);

// legacy
void ComplianceSummaryGraph(char *hubKeyHash, char *policy, char *buffer, int bufsize);
void Nova_Meter(bson *query, char *db, char *buffer, int bufsize,
                PromiseContextMode promise_context);

#endif
