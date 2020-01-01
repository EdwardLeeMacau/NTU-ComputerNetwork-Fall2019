/****************************************************************************
  FileName     [ client.cpp ]
  PackageName  [ client ]
  Synopsis     [  ]
****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <poll.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <tuple>
#include <vector>
#include <dirent.h> 
#include <iostream>

#include "sha256.h"
#include "client.h"

#define DONTWAIT 0
#define NUMBER_OF_FDS 1

using namespace std;



/**************************************************
 * Public Function (From Client User)             *
 **************************************************/

string 
get_name_from_list(vector<string>& list) 
{
	int choice;

	while (1) {
		printf("Choices:\n"); 
		for (int i = 0; i < list.size(); ++i) 
			printf("%2d: %s\n", i, list[i].c_str()); puts("");
		
		printf("Enter your choice: "); cin >> choice;
		if (cin.fail()) 
		{	
			continue;
		}
		if (0 <= choice && choice < list.size()) 
		{
			cout << "you chose: " << list[choice] << endl; 
			return list[choice];
		}
		else printf("Invalid choice, please try again!\n");
	}  
}

void 
save_file(int sockfd, string& filesize, string& username, string& target_file) 
{
	string file_dirname = username + "_file_dir/" + target_file;
	
	int file_size = stoi(filesize); // get file size
	int recieved_amount = 0, n; 
	char buf[1024] = {};
	int fd;
	
	printf("file size: %d\n", file_size);
	fd = open(file_dirname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	
	// printf("reading...\n");
	while ((n = recv(sockfd, buf, 1000, 0)) > 0) 
	{
		// printf("reading: %s\n", buf);
		write(fd, buf, n);
		recieved_amount += n; 
		if (recieved_amount >= file_size) break;
	}
	close(fd);
	// printf("done.\n");
}

void 
dump_file(string& file) 
{

}

string 
get_msg_from_user() 
{
	printf("Start typing your message. Type __END__ to end the message. Please do not exceed 500 characters!\n");					
	printf("--Message start--\n");
	string full_msg, buf;

	while (true) {
		getline(cin, buf);
		if (full_msg.length() + buf.length() > 500) break;
		if (buf == "__END__") break;
		full_msg += buf + "\n";
	}

	return full_msg;
}

void 
show_message(string& msg) 
{

}

tuple<string, int> 
get_file_name(string& username) 
{
	string dirname = username + "_file_dir";
	char c_dirname[128]; strcpy(c_dirname, dirname.c_str());
	vector<string> dirlist;
	printf("Select file to send (from %s):\n", c_dirname);
	DIR *d; struct dirent *dir;
	d = opendir(c_dirname);
	
	if (d) {
		for (int i = 0; (dir = readdir(d)) != NULL; i++) 
		{
			if (dir->d_type == DT_REG) 
				dirlist.push_back(string(dir->d_name));
		}
		if (!dirlist.size()) { 
			printf("You currently have no files to send. Consider placing one in the %s directory.\n", c_dirname); 
			return make_tuple("", 0);
		}
		closedir(d);
		string filename = get_name_from_list(dirlist); 
		string fname_path = dirname + "/" + filename;
		struct stat st; stat(fname_path.c_str(), &st);

		return make_tuple(filename, st.st_size);
	} 
	
	printf("Error in opening directory!\n"); 
	return make_tuple("", -1);
}

void 
parse_userlist(string& response, vector<string> &user_list) 
{ 
	// unpack
	char c_response[4096], name[128]; strcpy(c_response, response.c_str());
	char *ptr = strtok(c_response, "\n");
	
	while (ptr) {
		strcpy(name, ptr);
		string S = string(name);
		user_list.push_back(S);
		ptr = strtok(NULL, "\n");
	}
}

/**************************************************
 * Public Function (From Client Register)         *
 **************************************************/

/*
	Core function of get_password() and get_username()

	@params getwhat
*/
string 
get_str(string getwhat) 
{ 
	// can only have: {ascii} U {'_'}
	char name[128] = {}; 
	char get_what[32]; 
	strcpy(get_what, getwhat.c_str());
	
	while (true) 
	{
		printf("Enter %s: ", get_what);
		char c = getchar(); //eat "\n"
		if (c != '\n') ungetc(c, stdin);
		fgets(name, 128, stdin); 
		int len = strlen(name); 
		name[len-1] = '\0'; 
		len--;
		bool valid_name = true;
		
		if (len > 16) 
			{printf("Name too long (Max 16 characters!)\n"); continue;} 
		if (len == 0) 
			{printf("Can't be empty!\n"); continue;}
		
		for (int i = 0; i < len; i++) {
			if (!(isalpha(name[i]) || name[i] == '_')) {
				printf("can only consist of _ and alphabet characters, please try again.\n");
				valid_name = false; 
				break;
			}
		}
		if (valid_name) break;
	}
	
	return string(name);
}

string 
get_password() 
{
	return get_str("password");
}

string 
get_username() 
{
	return get_str("username");
}

tuple<string, string> 
register_user(int serv_port) 
{
	// TODO: Limited the length of username and password
	string username, password;
	username = get_username();
	password = get_password();

	return make_tuple(username, password);
}

/**************************************************
 * Public Function (From Client Server)           *
 **************************************************/


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

/**************************************************
 * class Client                                   *
 **************************************************/

Client::Client()
{

}

Client::Client(const string& IPAddress, const int& port)
{

}

Client::~Client()
{

}