/****************************************************************************
  FileName     [ ping.cpp ]
  PackageName  [ ping ]
  Synopsis     [ ping tool ]

  Usage: ./ping [-n number] [-t timeout] host_1:port_1 host_2:port_2 ...
****************************************************************************/
#include <arpa/inet.h>
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>


using namespace std;

typedef pair<string, string> Host;

void 
ping (int& sockfd, const addrinfo* res, const string& msg, char receivemsg[], const int& timeout) 
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

int 
main(int argc, char *argv[])
{
	clock_t mainstart = clock();
	string host;
	string ips[argc];					// Container to parsed IP Address 
	string ports[argc];					// Container to parsed Port
	string msg = "ping";				// Msg send to Server.
	int targets = 0;					// Count of Target Hosts
	char receivemsg[100];				// Buffer for receiving response
	int timeout = -1;				    // Timeout
	int number = -1;					// Number
	
	// Argument Parser
    for (int i = 1; i < argc; ++i) {
        if ((string)argv[i] == "-t") {
            if (timeout == -1 && i < argc - 1) {
                timeout = atoi(argv[++i]);
				if (timeout < 0){
					cerr << "Value Error: -t should equal or larger that 0, find " << timeout << endl; 
					return -1;
				}
			} else if (i < argc - 1) {
                cerr << "Argument Error: Repeat -t" << endl;
                return -1;
			} else {
				cerr << "Usage: ./ping [-n number] [-t timeout] host_1:port_1 host_2:port_2 ..." << endl;
			}
        } else if ((string)argv[i] == "-n") {
            if (number == -1 && i < argc - 1) {
				number = atoi(argv[++i]);
				if (number < 0){
					cerr << "Value Error: -n should equal or larger that 0, find " << number << endl; 
					return -1;
				}
			}
			else if (i < argc - 1) {
				cerr << "Argument Error: Repeat -n" << endl;
				return -1;
			} else {
				cerr << "Usage: ./ping [-n number] [-t timeout] host_1:port_1 host_2:port_2 ..." << endl;
			}
        } else {
			// Find IPAddress and Port
			host = (string)argv[i];

			if (host.find(':') == string::npos) {
				cerr << "Please specify port number, host_1:port_1 host_2:port_2 ..." << endl;
				return -1;
			} else {
				ips[targets]   = host.substr(0, host.find(':'));
				ports[targets] = host.substr(host.find(':') + 1);
			}

			++targets;
        }
    }

	// If no targets
	if (targets == 0) {
		cerr << "Usage: ./ping [-n number] [-t timeout] host_1:port_1 host_2:port_2 ..." << endl;
		return -1;
	}
	
	// Default Arugments
	if (number == -1)	number = 0;
	if (timeout == -1)	timeout = 1000;

	int sockfd[targets];					// File Descriptor
	addrinfo *res[targets];					// Result of DNS and Service Query
	addrinfo hints;							// 
	memset(&hints, 0, sizeof(hints));		// Make sure hints is empty
	hints.ai_family   = AF_UNSPEC;			// IPV4 or IPV6
	hints.ai_socktype = SOCK_STREAM;		// TCP Stream Socket
	hints.ai_flags    = AI_PASSIVE;			// My IP Address
	
	if(number != 0)
	{
		for(int i = 0; i < targets; i++) {
			// getaddrinfo()
			if (getaddrinfo(ips[i].c_str(), ports[i].c_str(), &hints, &res[i]))	{
				cerr << "Get Address Info Error (" << ips[i] << ":" << ports[i] << ")" << endl;
				continue;
			}
			
			sockfd[i] = socket(res[i]->ai_family, res[i]->ai_socktype,  res[i]->ai_protocol);
			
			if (connect(sockfd[i], res[i]->ai_addr, res[i]->ai_addrlen)) {
				cerr << "Connection failed. " << endl;
				continue;
			}

			for(int j = 0; j < number; j++) {
				ping(sockfd[i], res[i], msg, receivemsg, timeout);
			}

			close(sockfd[i]);
		}
	} else {
		bool status[targets];
		for(int i = 0; i < targets; i++)
			status[i] = false;

		for(int i = 0; i < targets; i++) {	
			if (getaddrinfo(ips[i].c_str(), ports[i].c_str(), &hints, &res[i])) {
				cerr << "Get Address Info Error (" << ips[i] << ":" << ports[i] << ")" << endl;
				continue;
			}

			status[i] = true;
			sockfd[i] = socket(res[i]->ai_family, res[i]->ai_socktype,  res[i]->ai_protocol);

			if (connect(sockfd[i], res[i]->ai_addr, res[i]->ai_addrlen)) {
				cerr << "Connect to " << ips[i] << ":" << ports[i] << " failed. " << endl;
				continue;
			}
		}
		
		while(true) {
			for(int i = 0; i < targets; i++) {
				if (status[i])
					ping(sockfd[i], res[i], msg, receivemsg, timeout);
			}
		}
		
		for (int i = 0; i < targets; ++i){
			close(sockfd[i]);
		}
    }

    return EXIT_SUCCESS;
}
