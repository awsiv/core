/*****************************************************************************/
/*                                                                           */
/* File: make_license_file.c                                                 */
/*                                                                           */
/* Created: Sun Sep 27 12:26:59 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <openssl/evp.h>
#include <sys/stat.h>
#define CF_MD5_LEN 16

#define CF_MAXVARSIZE 1024

// gcc -o make_license_file make_license_file.c -lcrypto

char *MONTH_TEXT[12] =
   {
   "January",
   "February",
   "March",
   "April",
   "May",
   "June",
   "July",
   "August",
   "September",
   "October",
   "November",
   "December"
   };

/*******************************************************************/

char *ThisHashPrint(unsigned char digest[EVP_MAX_MD_SIZE+1]);
void ThisHashString(char *fna,char *buffer,int len,unsigned char digest[EVP_MAX_MD_SIZE+1]);
int Month2Int(char *string);

/*******************************************************************/

int main(int argc,char **argv)

{ char name[CF_MAXVARSIZE],hash[CF_MAXVARSIZE],filename[2048],mydate[CF_MAXVARSIZE];
  FILE *fp;
  struct stat sb;
  int r_day,r_year,imonth;
  char r_month[18];
  int m_now,m_expire,d_now,d_expire;
  char f_day[18],f_month[18],f_year[18];
  int number = 1;
  char u_day[18],u_month[18],u_year[18];
  unsigned char digest[EVP_MAX_MD_SIZE+1];
  time_t now = time(NULL);

if (argc != 3)
   {
   printf("Illegal arguments %d\n",argc);
   return;
   }

strcpy(filename,argv[1]);


snprintf(mydate,CF_MAXVARSIZE,"%s",ctime(&now));

sscanf(mydate,"%*s %s %d %*s %d",r_month,&r_day,&r_year);

printf("Today's date is the %d of %s in %d<br>\n",r_day,r_month,r_year);

imonth = (Month2Int(r_month) + 1) % 12;

if (r_day > 28)
   {
   snprintf(f_day,7,"28");
   }
else
   {
   snprintf(f_day,7,"%d",r_day);
   }

strcpy(f_month,MONTH_TEXT[imonth]);

if (imonth == 0)
   {
   snprintf(f_year,7,"%d",r_year+1);
   }
else
   {
   snprintf(f_year,7,"%d",r_year);
   }

printf("Expiry date set to (%s) of %s %s<br>\n",f_day,f_month,f_year);

number = 5;

printf("\nNumber of licenses granted: %d<br>\n",number);
  
if (stat(filename,&sb) == -1)
   {
   printf("Public key file %s not found<br>\n",filename);
   exit(1);
   }

snprintf(name,CF_MAXVARSIZE-1,argv[2]);

if ((fp = fopen(name,"w")) != NULL)
   {
   snprintf(name,CF_MAXVARSIZE-1,"%s-%o.%s Nova %s",f_month,number,f_day,f_year);

   ThisHashString(filename,name,strlen(name),digest);
   
   fprintf(fp,"%2s %x %2s %4s %s",f_day,number,f_month,f_year,ThisHashPrint(digest));
   fclose(fp);
   printf("\nWrote %s - install this in WORKDIR/masterfiles on the policy server\n",name);
   }
}

/*******************************************************************/

void ThisHashString(char *filename,char *buffer,int len,unsigned char digest[EVP_MAX_MD_SIZE+1])

{ EVP_MD_CTX context;
  const EVP_MD *md = NULL;
  char *file_buffer;
  FILE *fp;
  int md_len;
  char fbuf[2048];

OpenSSL_add_all_algorithms();
OpenSSL_add_all_digests();
ERR_load_crypto_strings();

md = EVP_get_digestbyname("md5");       

if (md == NULL)
   {
   printf(" !! Digest type not supported by OpenSSL library\n");
   }

EVP_DigestInit(&context,md);     
EVP_DigestUpdate(&context,(unsigned char*)buffer,len);

if ((fp = fopen(filename,"r")) == NULL)
   {
   printf("No public key file could be read");
   exit(1);
   }

fbuf[0] = '\0';
while (!feof(fp))
   {
   fgets(fbuf,2048,fp);
   EVP_DigestUpdate(&context,(unsigned char*)fbuf,strlen(fbuf));
   }
fclose(fp);
    
EVP_DigestFinal(&context,digest,&md_len);
}

/*********************************************************************/

char *ThisHashPrint(unsigned char digest[EVP_MAX_MD_SIZE+1])

{ unsigned int i;
  static char buffer[EVP_MAX_MD_SIZE*4];

sprintf(buffer,"MD5=  ");

for (i = 0; i < CF_MD5_LEN; i++)
   {
   sprintf((char *)(buffer+4+2*i),"%02x", digest[i]);
   }

return buffer; 
}    

/*********************************************************************/

int Month2Int(char *string)

{ int i;

if (string == NULL)
   {
   return -1;
   }
 
for (i = 0; i < 12; i++)
   {
   if (strncmp(MONTH_TEXT[i],string,strlen(string))==0)
      {
      return i;
      break;
      }
   }

return -1;
}

