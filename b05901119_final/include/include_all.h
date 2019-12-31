#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/file.h>
#include <poll.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <dirent.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define die(a) { perror(a); exit(1); }
