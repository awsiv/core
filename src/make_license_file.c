/*****************************************************************************/
/*                                                                           */
/* File: make_license_file.c                                                 */
/*                                                                           */
/* Created: Sun Sep 27 12:26:59 2009                                         */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <sys/stat.h>

#define CF_SHA256_LEN 32
#define CF_MAXVARSIZE 1024

// gcc -o make_license_file make_license_file.c -lcrypto

/*******************************************************************/

char *ThisHashPrint(unsigned char digest[EVP_MAX_MD_SIZE+1]);
void ThisHashString(char *fna,char *buffer,int len,unsigned char digest[EVP_MAX_MD_SIZE+1]);

int main()

{ char name[CF_MAXVARSIZE],hash[CF_MAXVARSIZE],filename[2048];
  char company[2048];
  FILE *fp;
  struct stat sb;
  int m_now,m_expire,d_now,d_expire;
  char f_day[8],f_month[32],f_year[8];
  int number = 1;
  char u_day[8],u_month[32],u_year[8];
  unsigned char digest[EVP_MAX_MD_SIZE+1];


printf("Enter unique company string: ");

fflush(stdin);
fgets(company,2047,stdin);
Chop(company);

printf("Enter the filename of the client\'s public key: ");
scanf("%s",filename);

if (stat(filename,&sb) == -1)
   {
   printf("Public key file %s not found\n",filename);
   exit(1);
   }

printf("Enter license expiry date:\n");
printf("Enter day (e.g. 28): ");
scanf("%s",f_day);

if (atoi(f_day) > 31)
   {
   printf("Day must be < 31\n");
   exit(1);
   }

printf("Enter month (e.g. July): ");
scanf("%s",f_month);
printf("Enter year (e.g. 2023): ");
scanf("%s",f_year);

printf("Enter number of licenses: ");
scanf("%d",&number);


printf("\nDate confirmed as: %s %s %s\n",f_day,f_month,f_year);
printf("\nNumber of licenses granted: %d\n",number);
  
// if license file exists, set the date from that, else write one for 30 days?

snprintf(name,CF_MAXVARSIZE-1,"license.dat");

if ((fp = fopen(name,"w")) != NULL)
   {
   snprintf(name,CF_MAXVARSIZE-1,"%s-%o.%s Nova %s %s",f_month,number,f_day,f_year,company);

   ThisHashString(filename,name,strlen(name),digest);
   
   fprintf(fp,"%2s %x %2s %4s %s %s",f_day,number,f_month,f_year,ThisHashPrint(digest),company);
   fclose(fp);
   printf("\nWrote license.dat - install this in WORKDIR/masterfiles on the policy server\n");
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

md = EVP_get_digestbyname("sha256");       

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

sprintf(buffer,"SHA=  ");

for (i = 0; i < CF_SHA256_LEN; i++)
   {
   sprintf((char *)(buffer+4+2*i),"%02x", digest[i]);
   }

return buffer; 
}    

/*********************************************************************/

int Chop(char *str) /* remove trailing spaces */

{ int i;
 
if ((str == NULL) || (strlen(str) == 0))
   {
   return;
   }

if (strlen(str) > 2047)
   {
   printf("Too long");
   return;
   }

for (i = strlen(str)-1; isspace((int)str[i]); i--)
   {
   str[i] = '\0';
   }
}



