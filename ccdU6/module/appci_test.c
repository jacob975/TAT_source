

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
   int res;
   int value;
   int status;
   struct apIOparam request;
    fapogee = open("/dev/appci0",O_RDONLY);

   for (reg=0;reg<16;reg++) {
       request.reg = reg*4;
       request.param1=(unsigned long)&value;
       status=ioctl(fapogee,APPCI_READ_USHORT,(unsigned long)&request);
       printf("%04x ",value);
   }
   printf("\n");
   close(fapogee);
}
