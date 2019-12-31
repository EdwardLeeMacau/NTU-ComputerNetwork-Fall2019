#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unordered_map>
#include <string>
#include <pthread.h>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/tcp.h>

using namespace std;

class Server
{
public:
    /* Constructor */
    Server();
    Server(int p);

    /* data member accessor */
    Server& setPort();
    const int& getPort();

    /* public member function */
    void clean_msg(char*, int);
    string get_string(char*, int);
    string read_file(char*, int);
    string getfilelist(string /* file_path */);
    int CheckFileExist(string /* file_path */, string /*filename*/);

    void load_username();
    void handle_client(int /* sockfd */, string /* */);
    void* start_connect();
    void setup();

    int port;
    char *login_file_path;
    unordered_map<string, string> username_to_psd;
    unordered_map<string, int> online;
    unordered_map<string, int> username_to_sockfd;
    string userlist;

private:
    int _port;
};



#endif /* server.h */