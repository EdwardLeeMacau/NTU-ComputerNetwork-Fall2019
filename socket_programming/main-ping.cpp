/****************************************************************************
  FileName     [ ping.cpp ]
  PackageName  [ ping ]
  Synopsis     [ ping tool ]
****************************************************************************/
#include "ping.h"

#include <iostream>
#include <time.h>
#include <unistd.h>

using namespace std;

int
sigint_handler()
{
	return 0;
}

int 
main(int argc, char *argv[])
{
	OPT_S opt;
	clock_t mainstart = clock();
	string msg = "ping";					// Msg send to Server.
	char receivemsg[100];					// Buffer for receiving response
	int sockfd;								// File Descriptor

	init_opt(opt);
	if (parse(opt, argc, argv, opt.res, opt.hints) != PARSE_SUCCESS) {
		cout << "parse error." << endl;
		return EXIT_FAILURE;
	}

	sockfd = socket(opt.res->ai_family, opt.res->ai_socktype, opt.res->ai_protocol);
	
	if (connect(sockfd, opt.res->ai_addr, opt.res->ai_addrlen)) {
		cerr << "Connection failed. " << endl;
		return EXIT_FAILURE;
	}

	if (opt.numbers != 0)
	{
		for (int j = 0; j < opt.numbers; j++) {
			ping(sockfd, opt.res, msg, receivemsg, opt.timeout);
		}
	} 
	else 
	{
		while (true) {
			ping(sockfd, opt.res, msg, receivemsg, opt.timeout);
		}
    }

	close(sockfd);

    return EXIT_SUCCESS;
}
