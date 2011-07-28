/*****************************************************************************/
/*                                                                           */
/* File: check_km_substrings.c                                               */
/*                                                                           */
/* Created: Thu Jul 28 15:25:28 2011                                         */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#define true 1
#define false 0
#define CF_BUFSIZE 2048


char *ToUpperStr (char *str);
char *ToLowerStr (char *str);


int main()

{ char *keywords[8000];
 char *exceptions[] = { "or", "and","the", "there","then", "what", "how", "ci","on","at","int","now","not","any", NULL };
 
  char *otherwords[] =  { "convergence", "promise", "scheduling", "workflow","bundles", "hierarchy", "cloud",
                         "package", "policy", "security", "virtualization", "scalability",
                         "hierarchies","file",  NULL  };
 char *sp;
 int i,j,k,skip;
 FILE *fp;
 char w1[1024],w2[1024],line[1024],word1[1024],class1[1024],word2[1024],class2[1024];

if ((fp = fopen("words","r")) == NULL)
   {
   return;
   }

i = 0;

while(!feof(fp))
   {
   memset(line,0,1024);
   memset(word1,0,1024);
   memset(class1,0,1024);
   fgets(word1,1023,fp);

   if (strlen(word1) == 0)
      {
      continue;
      }
   
   keywords[i] = strdup(word1);

   i++;

   if (i >= 8000)
      {
      break;
      }
   }

keywords[i] = NULL;
fclose(fp);


printf("bundle knowledge substrings\n{\ntopics:\n");

for (k = 0; keywords[k] != NULL; k++)
   {
   for (i = 0; keywords[i] != NULL; i++)
      {
      skip = false;
      
      for (j = 0; exceptions[j] != NULL; j++)
         {
         if (strcmp(keywords[i],exceptions[j]) == 0)
            {
            skip = true;
            continue;
            }
         }
      
      if (skip)
         {
         continue;
         }
      
      //Check for canonified form too

      sscanf(keywords[i],"%[^:]::%[^\n]",class1,word1);
      sscanf(keywords[k],"%[^:]::%[^\n]",class2,word2);

      Chop(keywords[k]);
      Chop(keywords[i]);
      
      strcpy(w1,ToLowerStr(word2));
      strcpy(w2,ToLowerStr(word1));

      if (strlen(w1) == 0||strlen(w2)==0)
         {
         continue;
         }
      
      if (strcmp(w1,w2) == 0)
         {
         continue;
         }

      if (sp = strstr(w1,w2))
         {
         // Check for at least one space around the word
         
         if ((sp > keywords[k] && *(sp-1) != ' ') || !isspace(*(sp+strlen(keywords[i]))))
            {
            // continue;
            }
         
         printf(" %s:: \"%s\" association => a(\"seems to refer to\",\"%s\",\"seems to be referred to in\");\n",class1,word1,keywords[k]);
         }      
      }
   }

printf("}");
return 0;
}


/*********************************************************************/
/* TOOLKIT : String                                                  */
/*********************************************************************/

char ToLower (char ch)

{
if (isdigit((int)ch) || ispunct((int)ch))
   {
   return(ch);
   }

if (islower((int)ch))
   {
   return(ch);
   }
else
   {
   return(ch - 'A' + 'a');
   }
}


/*********************************************************************/

char ToUpper (char ch)

{
if (isdigit((int)ch) || ispunct((int)ch))
   {
   return(ch);
   }

if (isupper((int)ch))
   {
   return(ch);
   }
else
   {
   return(ch - 'a' + 'A');
   }
}

/*********************************************************************/

char *ToUpperStr (char *str)

{ static char buffer[CF_BUFSIZE];
  int i;

memset(buffer,0,CF_BUFSIZE);
  
if (strlen(str) >= CF_BUFSIZE)
   {
   char *tmp;
   tmp = malloc(40+strlen(str));
   return NULL;
   }

for (i = 0;  (str[i] != '\0') && (i < CF_BUFSIZE-1); i++)
   {
   buffer[i] = ToUpper(str[i]);
   }

buffer[i] = '\0';

return buffer;
}


/*********************************************************************/

char *ToLowerStr (char *str)

{ static char buffer[CF_BUFSIZE];
  int i;

memset(buffer,0,CF_BUFSIZE);

if (strlen(str) >= CF_BUFSIZE-1)
   {
   char *tmp;
   tmp = malloc(40+strlen(str));
   printf("String too long in ToLowerStr: %s",str);
   return NULL;
   }

for (i = 0; (str[i] != '\0') && (i < CF_BUFSIZE-1); i++)
   {
   buffer[i] = ToLower(str[i]);
   }

buffer[i] = '\0';

return buffer;
}

/*************************************************************************/

int Chop(char *str) /* remove trailing spaces */

{ int i;
 
if ((str == NULL) || (strlen(str) == 0))
   {
   return;
   }

for (i = strlen(str)-1; i >= 0 && isspace((int)str[i]); i--)
   {
   str[i] = '\0';
   }
}

