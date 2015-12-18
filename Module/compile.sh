# Extract the system call table address from the /proc/kallsyms
TABLE=$(cat /proc/kallsyms | grep "sys_call_table" | cut -d" " -f1)

# Replace "TABLE" string with actual address of system call table from above command
sed -i s/TABLE/$TABLE/g SysCallInterceptModule.c

# Build module
make
