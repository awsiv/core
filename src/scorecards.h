/*
  This file is (C) Cfengine AS. See LICENSE for details.
*/

#ifndef CFENGINE_SCORECARDS_H
#define CFENGINE_SCORECARDS_H

typedef enum
   {
   HOST_COLOUR_BLUE,
   HOST_COLOUR_RED,
   HOST_COLOUR_YELLOW,
   HOST_COLOUR_GREEN
   } HostColour;

#ifdef HAVE_LIBMONGOC

HostColour Nova_HostScoreToColour(int score);
const char *Nova_HostColourToString(HostColour colour);

int Nova_GetComplianceScore(enum cf_rank_method method, double *k, double *rep);

// gets meters from DB, uses GetComplianceScore. rework.
Item *Nova_ClassifyHostState(char *search_string,int regex,enum cf_rank_method method,int max_return);
int Nova_GetHostColour(char *lkeyhash);
Item *Nova_GreenHosts(void);
Item *Nova_YellowHosts(void);
Item *Nova_RedHosts(void);
Item *Nova_BlueHosts(void);
Item *Nova_RankHosts(char *search_string,int regex,enum cf_rank_method method,int max_return);

// legacy
void ComplianceSummaryGraph(char *hubKeyHash, char *policy, bool constellation, char *buffer, int bufsize);
void Nova_Meter(bson *query,char *db,char *buffer,int bufsize);
void Nova_BarMeter(int pos,double kept,double rep,char *name,char *buffer,int bufsize);

#endif

#endif
