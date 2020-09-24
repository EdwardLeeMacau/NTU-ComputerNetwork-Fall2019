/****************************************************************************
  FileName     [ server.cpp ]
  PackageName  [ ping ]
  Synopsis     [ server side of ping ]

  Usage: ./server <listen_port>
****************************************************************************/

#include "server.h"

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int 
main(int argc, char *argv[])
{
	int sockfd, clientsockfd;
	char receivemsg[100];
    string port;
    string msg = "ok";
	addrinfo *res;
	addrinfo hint;
	sockaddr_in their_addr;
	socklen_t addr_size = sizeof(their_addr);
	char s[INET6_ADDRSTRLEN];
	memset(&hint, 0, sizeof(hint));
	hint.ai_family   = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags    = AI_PASSIVE;

    if (argc == 2) {
        port = argv[1];
    	getaddrinfo(NULL, port.c_str(), &hint, &res);
    } else {
        cerr << "Usage: ./ping-server listen_port" << endl;
        return -1;
    }

    cout << "Listening to port: " << port << endl;

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);    
	bind(sockfd, res->ai_addr, res->ai_addrlen);                            // A socket bind to a port that could receive message from kernel
	listen(sockfd, 10);                                                     // server start to listen to connections from client
	
    try
    {
        while(true) {
            clientsockfd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);	
            
            // Fork a sub-process
            if (fork() == 0) {
                while (recv(clientsockfd, receivemsg, sizeof(receivemsg), 0) > 0) {
                    inet_ntop(AF_INET, &(their_addr.sin_addr), s, sizeof(s));
                    cout << "receive from " << s << ":" << ntohs(their_addr.sin_port) << endl;
                    send(clientsockfd, msg.c_str(), msg.length(), 0);
                }
                close(clientsockfd);
            }
        }
    }
    catch (...) 
    {
        freeaddrinfo(res);
    }
}
