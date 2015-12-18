This is Linux kernel module to demonstrate, How to intercept Linux system call.
In this module we developed a miscellaneous character device and a module that intercepts the system call
i.e. When any user process invoked "open" system call, we have intercept this system call.

Here we used "IOCTL" to send command/data to the kernel module from the user process or from a client.



I have tested this module on following version of Linux kernel.
* Linux - 2.6.32-358.el6.i686 i686 i686 i386 GNU/Linux

How to build this build?
Module: This will build the SysCallInterceptModule module on the Linux platform.
cd Module
./compile 



Client: This will build the client program, that will send command/data to above build module.
cd Client
make


How to Install the Linux kernel module?
cd Module
insmod SysCallInterceptModule.ko


You can check installed list of modules using:
lsmod

How to communicate with our installed kernel module?
cd Client
./Client

How to Un-install the Linux kernel module?
rmmod SysCallInterceptModule

Output:
You can look for the following log messages in /var/log/messages file.
e.g.
After loading of module successfully:
SysCallInterceptModule : We are in kernel space

When miscellaneous character device has been opened:
SysCallInterceptModule : device has been opened

When any user process invoke "open" system call, look for following statement:
SysCallInterceptModule : open() system call is invoked form User 


After unloading of module successfully:
SysCallInterceptModule : Unloading the module from linux


When miscellaneous character device has been closed:
SysCallInterceptModule : device has been opened


Disclaimer: This code interacts with the Linux kernel code. It may result into crash of system or loss of your data.
Use of this code is Users responsibility.
