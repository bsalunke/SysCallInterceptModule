This is Linux kernel module to demonstrate, How to intercept Linux system call.
In this module we developed a miscellaneous character device and a module that intercepts the system call
i.e. When any user process invoked "open" system call, we have intercept this system call.

Here we used "IOCTL" to send command/data to the kernel module from the user process or from a client.



I have tested this module on following version of Linux kernel.
* Linux - 2.6.32-358.el6.i686 i686 i686 i386 GNU/Linux

#### How to build this build?<br>
######Module: This will build the SysCallInterceptModule module on the Linux platform.<br>
cd Module<br>
./compile 

<br><br>
######Client: This will build the client program, that will send command/data to above build module.<br>
cd Client<br>
make

<br><br>

####How to Install the Linux kernel module?<br>
cd Module<br>
insmod SysCallInterceptModule.ko<br>


####You can check installed list of modules using:<br>
lsmod<br>

####How to communicate with our installed kernel module?<br>
cd Client<br>
./Client<br>

####How to Un-install the Linux kernel module?<br>
rmmod SysCallInterceptModule<br>

####Output:<br>
You can look for the following log messages in /var/log/messages file.<br>
e.g.<br>
######After loading of module successfully:<br>
SysCallInterceptModule : We are in kernel space<br>

######When miscellaneous character device has been opened:<br>
SysCallInterceptModule : device has been opened<br>

######When any user process invoke "open" system call, look for following statement:<br>
SysCallInterceptModule : open() system call is invoked form User <br>

<br>
######After unloading of module successfully:<br>
SysCallInterceptModule : Unloading the module from linux<br>
<br>
######When miscellaneous character device has been closed:<br>
SysCallInterceptModule : device has been opened<br>

<br>
######Disclaimer: This code interacts with the Linux kernel code. It may result into crash of system or loss of your data. Use of this code is Users responsibility.
