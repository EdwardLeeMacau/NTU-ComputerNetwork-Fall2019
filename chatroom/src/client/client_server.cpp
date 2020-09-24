#include <netdb.h>
#include <iostream>
#include <fcntl.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <errno.h>
#include <cstring>

#include "client.h"

using namespace std;

#define DONTWAIT 0
#define NUMBER_OF_FDS 1

int 
connect_to_server(int sockfd, string& IP, string& port) 
{
	// socket programming thingy; shoud look SOMETHING like this
	char IP_addr[32], portnum[32];
	strcpy(IP_addr, IP.c_str()); strcpy(portnum, port.c_str());
	// printf("IP: %s, port: %s\n", IP_addr, portnum);
	struct addrinfo hints; 
	
	memset(&hints, 0, sizeof(hints)); 	// make sure the struct is empty
	hints.ai_family = AF_UNSPEC;      	// don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;  	// TCP stream sockets
	hints.ai_flags = 0;     		  	// fill in my IP for me
	struct addrinfo *servinfo;  		// will point to the results

	// printf("trying to connect to %s:%s\n", IP_addr, portnum);
	if (getaddrinfo(IP_addr, portnum, &hints, &servinfo)) 
		die("getaddrinfo");

	if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
		if (errno == ECONNREFUSED) 
		{
			printf("timeout when connect to %s\n", IP_addr), 
			exit(0);
		}
		else 
			die("connect");
	}

	return 1;
}

void 
send_msg_ignore_response(int sockfd, string& msg) 
{
	char request_msg[4096] = {};
	strcpy(request_msg, msg.c_str());
	send(sockfd, request_msg, strlen(request_msg), 0);
}

string 
send_msg_wait_result(int sockfd, string& msg) 
{
	char *request_msg = (char *)malloc(4096 * sizeof(char));
	strcpy(request_msg, msg.c_str());
	// printf("send message: |%s|\n", request_msg);
	send(sockfd, request_msg, strlen(request_msg), 0);
	free(request_msg);
	char response[4096] = {};
	int msglen = recv(sockfd, response, 4095, 0);
	response[msglen] = '\0';
	// printf("response: %s\n", response);
	return string(response);
}


/*
	@return bool true if msg == SUCCESS
*/
bool 
server_request_success(int sockfd, string& to_server_msg) 
{
	string response = send_msg_wait_result(sockfd, to_server_msg);
	// printf("response: %s\n", response.c_str());
	return response == "SUCCEED"; 
}

/*
	Get the message that not read.

	@params sockfd
*/
void 
print_mailbox_msgs(int sockfd) 
{ 
	// prints messages from the mailbox
	struct pollfd P = {sockfd, POLLIN, 0};
	if (poll(&P, NUMBER_OF_FDS, 5) == 0) {
		// printf("(no new messages, you technically don't need to print anything)\n");
	}
	else {
		printf("New message! Press [Enter] to leave.\n");
		printf("==MSG START==\n");
		char buf[4096];
		int n = recv(sockfd, buf, 4095, 0);
		buf[n] = '\0';
		printf("%s\n", buf);
		printf("==MSG END==\n");
		string whatever; 
		getline(cin, whatever); 
	}
}

void 
send_actual_file(int sockfd, string& username, string& filename) 
{
	string dirname = username + "_file_dir/" + filename; 
	int fd, size;
	
	if ((fd = open(dirname.c_str(), O_RDONLY)) < 0) 
	{ 
		puts("can't open file"), exit(0);
	}
	
	while (1) 
	{
		char buf[1001] = {}, whatever[20]; 
		int size = read(fd, buf, 1000);
		
		if (size <= 0) break;
		
		buf[size] = '\0';
		string msg = "SENDFILE_TRANSFER\n";
		send(sockfd, msg.c_str(), msg.length(), 0);
		recv(sockfd, whatever, 19, 0);
		msg = string(buf);
		// cout << "sending: " << msg << endl;
		send_msg_ignore_response(sockfd, msg);
	}
}

void 
send_actual_msg(int sockfd, string& target_user, string& user_message) 
{
	for (int i = 0; i < user_message.length(); i+= 1000) {
		string msg = "SENDMSG_TRANSFER\n" + target_user + "\n" + user_message.substr(i, i + 1000);
		// cout << "sending: " << msg << endl;
		send_msg_ignore_response(sockfd, msg);
	}
}
