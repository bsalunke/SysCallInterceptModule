#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/highmem.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/file.h>

static char* msg_ptr; 

/* IOCTL commands */
#define IOCTL_PATCH 0x00000001
#define IOCTL_FIX 0x00000004

long *sys_call_table = (unsigned long*)0xTABLE;

/* We will set this variable to 1 in our open handler and erset it
   back to zero in release handler*/
int in_use = 0;

int is_set=0;

/* Pointer to the original system call functions */
asmlinkage int (*real_open)(const char* __user, int, int);
asmlinkage int (*real_read)(int fd, void* buf, size_t);
asmlinkage int (*real_write)(int fd, const void* buf, size_t);
asmlinkage int (*real_close)(int);

/*
 * *****************************************************************************
 * This is intercepted call for "open" system call. 
 * It will be invoked when any user process invokes "open" system call.
 * *****************************************************************************
 * */ 
asmlinkage int custom_open(const char* __user file_name, int flags, int mode)
{
   printk(KERN_INFO "SysCallInterceptModule : open(%s, %d, %d) system call is invoked form User\n", file_name, flags, mode);

   return real_open(file_name, flags, mode);
}

/*
 * *****************************************************************************
 * This is intercepted call for "read" system call. 
 * It will be invoked when any user process invokes "read" system call.
 * *****************************************************************************
 * */ 
asmlinkage int custom_read(int fd, void* buf, size_t size)
{
  if(fd != 1 && fd != 2)
  printk(KERN_INFO "SysCallInterceptModule : read(%d) system call is invoked from User\n", fd);
  return real_read(fd, buf, size);
  if(buf)
    printk("%s\n", (char*)buf);
}

/*
 *******************************************************************************
 * This is intercepted call for "write" system call. 
 * It will be invoked when any user process invokes "write" system call.
 * *****************************************************************************
 * * */
asmlinkage int custom_write(int fd, const void* buf, size_t size)
{
  if(fd != 1 && fd != 2)
  printk(KERN_INFO "SysCallInterceptModule : write (%d) system call is invoked from User\n", fd);
  return real_write(fd, buf, size);
}

/*
 *******************************************************************************
 * This is intercepted call for "close" system call.
 * It will be invoked when any user process invokes "close" system call.
 *******************************************************************************
 **/
asmlinkage int custom_close(int fd)
{
  printk(KERN_INFO "SysCallInterceptModule : close(%d) system call is invoked from User\n", fd);
  return real_close(fd);
}

/*
 *******************************************************************************
 * This method will make page writable.
 *  
 *******************************************************************************
 **/
int make_writable(unsigned long address)
{
   unsigned int level;
   pte_t *pte = lookup_address(address, &level);
   if(pte->pte &~ _PAGE_RW)
      pte->pte |= _PAGE_RW;
   return 0;
}

/*
 *******************************************************************************
 * This method will make page write protected.
 *
 *******************************************************************************
 **/
int make_readonly(unsigned long address)
{
   unsigned int level;
   pte_t *pte = lookup_address(address, &level);
   pte->pte = pte->pte &~ _PAGE_RW;
   return 0;
}

/*
 *******************************************************************************
 * This function will be invoked each time a user process attempts to open 
 * our device (miscellaneous device). 
 *******************************************************************************
 **/
static int device_open(struct inode *inode, struct file *file)
{

   /* We would allow only one processes to open this device */
   if(in_use)
      return -EBUSY;
   in_use++;
   printk(KERN_INFO "SysCallInterceptModule : device has been opened\n");

   return 0;
}

/*
 *******************************************************************************
 * This function will be called when a process closes our device 
 *******************************************************************************
 **/
static int device_release(struct inode *inode, struct file *file)
{
   in_use--;
   printk(KERN_INFO "SysCallInterceptModule : device has been closed\n");
   return 0;
}

/*
 *******************************************************************************
 * This function will be called when a user process perform "read" call on 
 * our device.
 *******************************************************************************
 **/
static int device_read(struct file* filep, char* buffer, size_t len, loff_t offset){
  printk(KERN_INFO "SysCallInterceptModule : into read method of device\n");

  return 0;
}

/*
 *******************************************************************************
 * This function will be called when a user process perform "write" call on 
 * our device.
 *******************************************************************************
 **/
static ssize_t device_write(struct file* filep, const char* buff, size_t len, loff_t *off)
{
  printk(KERN_INFO "SysCallInterceptModule : into write method of device\n");
  sprintf(msg_ptr, buff, len);
  if(*msg_ptr)
  printk(KERN_INFO "SysCallInterceptModule : data sent to device write is : %s\n", msg_ptr);
  
  return len;
}

/*
 *******************************************************************************
 * This method handles IOCTL() function call from user process. 
 * User can send command/data to kernel module by calling "IOCTL()" from user space.
 *******************************************************************************
 **/
static int device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int retval = 0;
    switch(cmd)
    {
      case IOCTL_PATCH:
         make_writable((unsigned long)sys_call_table);

	 /*
	  ********************************************************************* 
	  * Store the addresses of intecepted method into system cal table and
	  * save the original address of system call for restroring it.
	  ********************************************************************* 
 	  **/   
         real_open = (void*)*(sys_call_table + __NR_open);
         *(sys_call_table + __NR_open) = (unsigned long)custom_open;

#if 0
         real_read = (void*)*(sys_call_table + __NR_read);
         *(sys_call_table + __NR_read) = (unsigned long)custom_read;

         real_write = (void*)*(sys_call_table + __NR_write);
         *(sys_call_table + __NR_write) = (unsigned long)custom_write;

         real_close = (void*)*(sys_call_table + __NR_close);
         *(sys_call_table + __NR_close) = (unsigned long)custom_close;
#endif

         make_readonly((unsigned long)sys_call_table);

         is_set=1;
         break;
      case IOCTL_FIX:
         make_writable((unsigned long)sys_call_table);

	 /*
	  ********************************************************************* 
	  * Restore the original addresses of system call into system call
	  * table
	  ********************************************************************* 
 	  **/   
         *(sys_call_table + __NR_open) = (unsigned long)real_open;
#if 0
         *(sys_call_table + __NR_read) = (unsigned long)real_read;
         *(sys_call_table + __NR_write) = (unsigned long)real_write;
         *(sys_call_table + __NR_close) = (unsigned long)real_close;
#endif

         make_readonly((unsigned long)sys_call_table);
         is_set=0;
         break;
      default:
         printk(KERN_INFO "SysCallInterceptModule : Wrong command from user in IOCTL \n");
         break;
   }
   
   return retval;
}

/*
 ******************************************************************************* 
 * This structure holds the function pointers to the function defined by driver.
 * These functions will be used to perform operation on the device. 
 ******************************************************************************* 
 **/
static const struct file_operations device_fops =\
{
   .owner = THIS_MODULE,
   .open = &device_open,
   .read = &device_read,
   .write = &device_write,
   .release = &device_release,
   .unlocked_ioctl = (void*)&device_ioctl,
   .compat_ioctl = (void*)&device_ioctl
};

/*
 ******************************************************************************* 
 * This is declaration of miscellaneous character device.
 ******************************************************************************* 
 **/
static struct miscdevice miscchar_device = \
{
   MISC_DYNAMIC_MINOR,
   "MiscCharDevice",
   &device_fops
};

/*
 ******************************************************************************* 
 * This is Linux kernel module's entry function.
 * When we laod the module into linux, this method is invoked.
 ******************************************************************************* 
 **/
static int __init initModule(void) 
{
      int retval;
      printk(KERN_INFO "SysCallInterceptModule : We are in kernel space\n");
      retval = misc_register(&miscchar_device);
      return retval;
}

/*
 ******************************************************************************* 
 * This is Linux kernel module's exit function.
 * When we remove the module from linux, this method is invoked.
 ******************************************************************************* 
 **/
static void __exit exitModule(void) /* Same here - you may use
                                              any name instead of
                                              cleanup_module */
{
      printk(KERN_INFO "SysCallInterceptModule : Unloading the module from linux\n");
      misc_deregister(&miscchar_device);
      if(is_set)
      {
	    make_writable((unsigned long)sys_call_table);
	    *(sys_call_table + __NR_open) = (unsigned long)real_open;
	    *(sys_call_table + __NR_read) = (unsigned long)real_read;
	    *(sys_call_table + __NR_write) = (unsigned long)real_write;
	    *(sys_call_table + __NR_close) = (unsigned long)real_close;
	    make_readonly((unsigned long)sys_call_table);
      }
      return;
}


/*
 ******************************************************************************* 
 * Pass module entry function to the module_init() macro and 
 * pass module exit function to the module_exit() macro.
 ******************************************************************************* 
 **/
module_init(initModule);
module_exit(exitModule);

/*
 ******************************************************************************* 
 * Macro used to set the addtional information about the module.
 ******************************************************************************* 
 **/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("DeveloperName");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("This module demonstrate how to intercept system calls in Linux");
