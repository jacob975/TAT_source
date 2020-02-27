

#include <stdio.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ApogeeLinux.h"
 
int fapogee;

int main(void)
{
   unsigned long reg;
   int value;
   int status;
   struct apIOparam request;

   fapogee = open("/dev/apppi0",O_RDONLY);

   for (reg=0;reg<32;reg++) {
       request.reg = reg;
       request.param1=(unsigned long)&value;
       status=ioctl(fapogee,APPPI_READ_USHORT,(unsigned long)&request);
       printf("register %d = %x\n",reg,value);
   }

   close(fapogee);
}
