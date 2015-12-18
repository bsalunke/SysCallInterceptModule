obj-m := SysCallInterceptModule.o

default:
	make -C /lib/modules/`uname -r`/build M=${PWD} modules 
