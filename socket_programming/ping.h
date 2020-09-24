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
	int targets;	/**< Number of ping targets */
	int numbers;
	int timeout;
	struct addrinfo *res;
	struct addrinfo hints;
} OPT_S;

enum PARSE_OPT_STATUS {
	PARSE_SUCCESS, 
	PARSE_NO_HOST,
	PARSE_MISSING_ARGS,
	PARSE_INVAILD_HOST,
	PARSE_INVAILD_NUMBER,
	PARSE_INVAILD_TIMEOUT,
	PARSE_REPEATED_FLAG,
	PARSE_UNDEFINED_FLAG,
};

void usage();
void init_opt(OPT_S& opt);
void ping(int& sockfd, const addrinfo* res, const std::string& msg, 
		  char receivemsg[], const int& timeout);
PARSE_OPT_STATUS parse(OPT_S& opt, int argc, char** argv, 
					   struct addrinfo* res, struct addrinfo& hints);

#endif