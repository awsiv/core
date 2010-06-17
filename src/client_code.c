/*****************************************************************************/
/*                                                                           */
/* File: client_code.c<2>                                                    */
/*                                                                           */
/* Created: Wed Jun 16 18:22:22 2010                                         */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/*********************************************************************/

int QueryForKnowledgeMap(char *menu,time_t since)

{ int done = false,tosend,cipherlen=0,value;
  char *buf,in[CF_BUFSIZE],out[CF_BUFSIZE],workbuf[CF_BUFSIZE],cfchangedstr[265];
  unsigned char iv[32] = {1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8};
  long n_read_total = 0;  
  EVP_CIPHER_CTX ctx;
  int plainlen,finlen,more;
  struct cfagent_connection *conn;// = pp->conn;

snprintf(cfchangedstr,255,"%s%s",CF_CHANGEDSTR1,CF_CHANGEDSTR2);

workbuf[0] = '\0';
EVP_CIPHER_CTX_init(&ctx);  

snprintf(in,CF_BUFSIZE-CF_PROTO_OFFSET,"QUERY %s %ld",menu,(long)since);
cipherlen = EncryptString(conn->encryption_type,in,out,conn->session_key,strlen(in)+1);
snprintf(workbuf,CF_BUFSIZE,"SQUERY %4d",cipherlen);
memcpy(workbuf+CF_PROTO_OFFSET,out,cipherlen);
tosend=cipherlen+CF_PROTO_OFFSET;   

/* Send proposition C0 - query */

if (SendTransaction(conn->sd,workbuf,tosend,CF_DONE) == -1)
   {
   //cfPS(cf_error,CF_INTERPT,"",pp,attr,"Couldn't send data");
   return false;
   }

buf = (char *) malloc(CF_BUFSIZE + sizeof(int));

n_read_total = 0;

while (more)
   {
   if ((cipherlen = ReceiveTransaction(conn->sd,buf,&more)) == -1)
      {
      free(buf);
      return false;
      }

   // Fill buf
   
   EVP_DecryptInit(&ctx,CfengineCipher(CfEnterpriseOptions()),conn->session_key,iv);

   if (!EVP_DecryptUpdate(&ctx,workbuf,&plainlen,buf,cipherlen))
      {
      Debug("Decryption failed\n");
      free(buf);
      return false;
      }

   if (!EVP_DecryptFinal(&ctx,workbuf+plainlen,&finlen))
      {
      Debug("Final decrypt failed\n");
      free(buf);
      return false;
      }

   printf("QUERY GOT: %s\n",workbuf);
   // print or store workbuf in DB with driver
   }

free(buf);
EVP_CIPHER_CTX_cleanup(&ctx);
return true;
}

