# LOGFIND

This is a small CLI application i wrote that searches through given log files for list of strings with OR or AND logic and returns the corresponding lines as it reads it.

## Using it
For start up, you should set this environment variable so you can run the binary faster.
```shell
export PATH="$PATH:<The logfind binary file path>"
```
Now you should insert the list of logs in your machine into the .logfind file without empty lines. The default location of .logfind is at /home/user, but you should change it in the logfind.h header file:
```C
#define LOGFIND_PATH "/home/user"
```
The .logfind file should look something like this:
```
/var/log/abc.log
/var/log/boos.log
/var/log/fdfdHH.log
/var/log/dpkg.log
```
After this, you can run the command like this:
### AND LOGIC
```shell
logfind username package linux
```
### OR LOGIC
```shell
logfind -o username package linux
```

## OPTIONS
- -h / --help
- -o (OR logic)
- NO OPTIONS (default to AND logic)
