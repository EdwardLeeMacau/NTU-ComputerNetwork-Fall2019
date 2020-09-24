/****************************************************************************
  FileName     [ ping.cpp ]
  PackageName  [ ping ]
  Synopsis     [ ping tool ]

  Usage: ./ping [-n number] [-t timeout] host_1:port_1 host_2:port_2 ...
****************************************************************************/
#include "ping.h"

#include <iostream>
#include <time.h>
#include <unistd.h>

using namespace std;

int 
main(int argc, char *argv[])
{
	OPT_S opt;
	clock_t mainstart = clock();
	bool status[100];
	int sockfd[100];						// File Descriptor
	addrinfo *res[100];						// Result of DNS and Service Query
	addrinfo hints;							// 
	string msg = "ping";					// Msg send to Server.
	char receivemsg[100];					// Buffer for receiving response
	hints.ai_family   = AF_UNSPEC;			// IPV4 or IPV6
	hints.ai_socktype = SOCK_STREAM;		// TCP Stream Socket
	hints.ai_flags    = AI_PASSIVE;			// My IP Address

	memset(&hints, 0, sizeof(hints));		// Make sure hints is empty

	init_opt(opt);
	if (parse(opt, argc, argv, res, hints) != PARSE_SUCCESS) {
		cout << "parse error." << endl;
		return EXIT_FAILURE;
	}

	// Default Arugments
	if (opt.numbers == -1) {
		opt.numbers = 0;
	}
	if (opt.timeout == -1) {
		opt.timeout = 1000;
	}

	// getaddrinfo()
	for(int i = 0; i < opt.targets; i++) {		
		sockfd[i] = socket(res[i]->ai_family, res[i]->ai_socktype,  res[i]->ai_protocol);
		
		if (connect(sockfd[i], res[i]->ai_addr, res[i]->ai_addrlen)) {
			cerr << "Connection failed. " << endl;
			continue;
		}
	}

	if (opt.numbers != 0)
	{
		for (int i = 0; i < opt.targets; i++) {
			for (int j = 0; j < opt.numbers; j++) {
				ping(sockfd[i], res[i], msg, receivemsg, opt.timeout);
			}

			close(sockfd[i]);
		}
	} 
	else 
	{
		for (int i = 0; i < opt.targets; i++)
			status[i] = false;
		
		while (true) {
			for (int i = 0; i < opt.targets; i++) {
				if (status[i])
					ping(sockfd[i], res[i], msg, receivemsg, opt.timeout);
			}
		}
		
		for (int i = 0; i < opt.targets; ++i){
			close(sockfd[i]);
		}
    }

    return EXIT_SUCCESS;
}
