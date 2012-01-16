/*

  This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

int FileRead(const char *filename, char *buffer, size_t bufsize)
{
FILE *f = fopen(filename, "rb");
if (f == NULL)
   {
   return -1;
   }
int ret = fread(buffer, bufsize, 1, f);
if (ferror(f))
   {
   fclose(f);
   return -1;
   }
fclose(f);
return ret;
}

