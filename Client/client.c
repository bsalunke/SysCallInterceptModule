/*
 *******************************************************************************
 * This is client program that will handle the "" kernle module by sending 
 * instruction using "ioctl"
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>


/* Command send to Kernel module */
#define IOCTL_PATCH 0x00000001
#define IOCTL_FIX   0x00000004


int main(void)
{
  
   fd_set s;
   struct timeval timeout;
   timeout.tv_sec = 0;
   timeout.tv_usec = 10;

   int device = open("/dev/MiscCharDevice", O_RDWR);
   ioctl(device, IOCTL_PATCH);

   /*
    ****************************************************************************
    *  This loop will run till user press "Enter" key to stop the intercepted 
    *  system interceoption and restore the system call as it is.
    ****************************************************************************
    **/
   printf("Press \"Enter\" key to exit\n");
   do{
      fflush(stdout);
      FD_ZERO(&s);
      FD_SET(STDIN_FILENO, &s);
      select(STDIN_FILENO+1, &s, NULL, NULL, &timeout);
   }while(FD_ISSET(STDIN_FILENO, &s) == 0);

   sleep(1);
   
   ioctl(device, IOCTL_FIX);
   close(device);
   return 0;
}
