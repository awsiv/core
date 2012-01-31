/*
  This file is (C) Cfengine AS. See LICENSE for details.
*/

#ifndef CFENGINE_SCORECARDS_H
#define CFENGINE_SCORECARDS_H

#ifdef HAVE_LIBMONGOC

void Nova_BarMeter(int pos,double kept,double rep,char *name,char *buffer,int bufsize);
void Nova_Meter(bson *query,char *db,char *buffer,int bufsize);
void Nova_GetHourlyData(char *search_string,char *buffer,int bufsize);
Item *Nova_RankHosts(char *search_string,int regex,enum cf_rank_method method,int max_return);
Item *Nova_GreenHosts(void);
Item *Nova_YellowHosts(void);
Item *Nova_RedHosts(void);
Item *Nova_BlueHosts(void);
Item *Nova_ClassifyHostState(char *search_string,int regex,enum cf_rank_method method,int max_return);
int Nova_GetComplianceScore(enum cf_rank_method method,double *k,double *rep);
int Nova_IsGreen(int level);
int Nova_IsYellow(int level);
int Nova_IsRed(int level);
int Nova_IsBlue(int level);
void ComplianceSummaryGraph(char *hubKeyHash, char *policy, bool constellation, char *buffer, int bufsize);
void Nova_DrawComplianceAxes(DataView *cfv,int col);
int Nova_GetHostColour(char *lkeyhash);

#endif

#endif
