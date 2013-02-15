#ifndef STORIES_H
#define STORIES_H

#include "cf.nova.h"

enum storytype
{
    cfi_cause,
    cfi_connect,
    cfi_part,
    cfi_access,
    cfi_none
};

int PossibleInference(char *assoc,enum storytype type,enum cf_direction direction,Chapter *this,char *topic,char *context);
void Nova_GetWeatherReport(char *hostkey,Item **low, Item **high, Item **anomaly, Item **focused, Item **unfocused,Item **ldt);
void Nova_ListPossibleStoriesCmdLine(void);
void Nova_GenerateStoriesCmdLine(char *typed_topic,enum storytype type);
int Nova_GenerateStories_by_name_JSON(char *typed_topic,enum storytype type,char *buffer,int bufsize);
int Nova_GenerateStories_by_id_JSON(int topic_id,enum storytype type,char *buffer,int bufsize);
void GenerateStoriesAbout(Chapter **story_topic,enum storytype type,StoryLine **finished,int depth,enum cf_direction direction);

Chapter *AppendToStory(Chapter **last,char *topic,char *context,char *assoc,int topic_id);
StoryLine *PrependStoryLine(StoryLine **start,Chapter *story);
Chapter *NewChapter(char *topic,char *context,int topic_id);
int SameStoryDifferentOutcome(Chapter *s1,Chapter *s2);
void DeleteStoryLine(StoryLine *start);
void DeleteStory(Chapter *start);
void Nova_ShowStoryLine(StoryLine *list,enum storytype x);
void Nova_ShowStoryLine_JSON(StoryLine *list,enum storytype type,char *buffer,int bufsize);

void Nova_ScanAccessRelationships(FILE *fp, Promise *pp,char *promise_id);
char *Nova_Relevant3Inference(char *prev,char *this,char *table[CF_INFSIZE][3]);
char *Nova_Relevant2Inference(char *this,char *table[CF_INFSIZE][2]);
char *Nova_Possible3Inference(char *this,char *table[CF_INFSIZE][3]);
double Nova_Inference(Chapter *path[CF_MAX_STORY_DEPTH],enum storytype x,enum cf_direction y,int quiet);
void Nova_HostStory(const Policy *policy,char *hostkey,char *buffer, int bufsize);
int AlreadySaidThis(Chapter *history[CF_MAX_STORY_DEPTH],int topic_id);
void Nova_GetLocations(const Policy *policy,char *hostkey, Rlist **locations);

#endif // STORIES_H
