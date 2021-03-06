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

typedef pair<string, string> Host;

void 
ping(int& sockfd, const addrinfo* res, const string& msg, char receivemsg[], const int& timeout) 
{
			
	sockaddr_in* saddr = (sockaddr_in*) res->ai_addr;
    clock_t before = clock();
	char* ip = inet_ntoa(saddr->sin_addr);				// Convert to string
    uint16_t port = htons(saddr->sin_port);			// Convert to 
	float ms = 0;

	send(sockfd, msg.c_str(), msg.length(), 0);
    
    while (true) {
        ms = (float)(clock() - before) * 1000 / CLOCKS_PER_SEC;

        // Until receive a reply or timeout
        if (ms <= timeout) {
            if (recv(sockfd, receivemsg, strlen(receivemsg), 0) > 0) {
                cout << "recv from " << ip << ":" << port << ", RTT = " << ms << " msec" << endl;
                break;
            }
        } else {
            cout << "timeout when connect to " << ip << ":" << port << endl;
            break;
        }
    }
}


void
init_opt(OPT_S& opt)
{
	opt.targets = 0;
	opt.numbers = -1;
	opt.timeout = -1;
	memset(&opt.hints, 0, sizeof(opt.hints));
}

void 
usage()
{
	cout << "Usage: ./ping [-n number] [-t timeout] host_1:port_1 host_2:port_2 ..." << endl
		 << "    -n --number		The number of packets to send, default 0"		 << endl
		 << "    -t --timeout		Set timeout value, default -1"					 << endl;
}


/**
 * @brief the argument parser of main program
 * 
 * @param[out] opt Options
 * @param[in] argc
 * @param[in] argv
 * @param[in] hints 
 * @param[out] res array of res
 */
PARSE_OPT_STATUS
parse(OPT_S& opt, int argc, char** argv, struct addrinfo* res, struct addrinfo& hints)
{
	string arg;
	string host;
	string ips[argc];					// Container to parsed IP Address 
	string ports[argc];					// Container to parsed Port

	// Argument Parser
    for (int i = 1; i < argc; ++i) 
	{
		arg = static_cast<string>(argv[i]);

        if (arg == "-t" || arg == "--timeout") 
		{
            if (opt.timeout != -1) {
                // cerr << "Argument error: Repeat -t" << endl;
				// usage();
                return PARSE_REPEATED_FLAG;
			} else if (i == argc - 1) {
				// cerr << "Missing arguments: [-t timeout]" << endl;
				// usage();
				return PARSE_MISSING_ARGS;
			}

			if ((opt.timeout = atoi(argv[++i])) < 0){
				// cerr << "Value Error: -t should equal or larger that 0, find " << opt.timeout << endl; 
				return PARSE_INVAILD_TIMEOUT;
			}
        } 
		else if (arg == "-n" || arg == "--number") 
		{
			if (opt.numbers != -1) {
                // cerr << "Argument error: Repeat -t" << endl;
				// usage();
                return PARSE_REPEATED_FLAG;
			} else if (i == argc - 1) {
				// cerr << "Missing arguments: [-n number]" << endl;
				// usage();
				return PARSE_MISSING_ARGS;
			} 

			if ((opt.numbers = atoi(argv[++i])) < 0){
				// cerr << "Value Error: -n should equal or larger that 0, find " << opt.numbers << endl; 
				return PARSE_INVAILD_NUMBER;
			}

        } 
		else if (arg[0] == '-')
		{
			// usage();
			return PARSE_UNDEFINED_FLAG;
		}

		// Find IPAddress and Port
		host = (string)argv[i];

		if (host.find(':') == string::npos) {
			// cout << "Please specify port number." << endl;
			// usage();
			return PARSE_INVAILD_HOST;
		} 

		ips[opt.targets]   = host.substr(0, host.find(':'));
		ports[opt.targets] = host.substr(host.find(':') + 1);
		++opt.targets;
    }

	// If no targets
	if (opt.targets == 0) {
		// cout << "Please specify host." << endl;
		// usage();
		return PARSE_NO_HOST;
	}

	for (int i = 0; i < opt.targets; ++i) {
		if (getaddrinfo(ips[i].c_str(), ports[i].c_str(), &hints, &res)) {
			cerr << "Get Address Info Error (" << ips[i] << ":" << ports[i] << ")" << endl;
			return PARSE_INVAILD_HOST;			
		}
	}

	// Default Arugments
	if (opt.numbers == -1) {
		opt.numbers = 0;
	}
	if (opt.timeout == -1) {
		opt.timeout = 1000;
	}

	return PARSE_SUCCESS;
}