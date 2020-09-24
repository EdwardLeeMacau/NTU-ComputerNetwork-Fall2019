#ifndef PING_H
#define PING_H

#include <arpa/inet.h>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
	int targets;
	int numbers;
	int timeout;
} OPT_S;

enum PARSE_OPT_STATUS {
	PARSE_SUCCESS, 
	PARSE_INVAILD_NUM,
	PARSE_INVAILD_TIMEOUT,
	PARSE_INVAILD_HOST,
	PARSE_NO_HOST,
	PARSE_REPEATED_FLAG,
};

void ping(int& sockfd, const addrinfo* res, const std::string& msg, char receivemsg[], const int& timeout);
void usage();
void init_opt(OPT_S& opt);
PARSE_OPT_STATUS parse(OPT_S& opt, int argc, char** argv, addrinfo* res[], addrinfo& hints);

#endif