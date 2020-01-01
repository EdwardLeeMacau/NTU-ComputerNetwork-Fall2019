// functions that deal with registration.
#ifndef CLIENT_H
#define CLIENT_H

#include <tuple>
#include <string>
#include <vector>

#define DONTWAIT 0
#define NUMBER_OF_FDS 1

using namespace std;

/* client_main.h */
int get_action_start();
int get_action_session(string& /* username */);
void create_user_dir(string& /* username */);

/* client_register */
int meow();
int connect_to_server(int serv_port);
tuple<string, string> register_user(int serv_port);
string get_username();
string get_password();

/* client_user.h */
string get_name_from_list(vector<string>& list);
void save_file(int sockfd, string& filesize, string& username, string& target_file);
void dump_file(string& file); // shows file to user
string get_msg_from_user(); // ends by "__END__" or something
void show_message(string& msg); // literally just a print
tuple<string, int> get_file_name(string& username); // get_name_from_list except the list is os.listdir(".")
void save_file(int sockfd, string& filesize, string& username, string& target_file);
void parse_userlist(string& response, vector<string> &user_list);

/* client_server.h */
int connect_to_server(int sockfd, string& IP, string& port);
bool server_request_success(int serv_port, string& to_server_msg);
string send_msg_wait_result(int sockfd, string& msg);
void send_msg_ignore_response(int sockfd, string& msg);
void print_mailbox_msgs(int sockfd);
void send_actual_file(int sockfd, string& username, string& filename);
void send_actual_msg(int sockfd, string& target_user, string& user_message);

class Client
{
public:
    Client();
    Client(const string& /* IP Address */, const int& /* port */);
    ~Client();

    /* client_main.h */
    int get_action_start();
    int get_action_session(string& /* username */);
    void create_user_dir(string& /* username */);

    /* client_register */
    int meow();
    int connect_to_server(int serv_port);
    tuple<string, string> register_user(int serv_port);
    string get_username();
    string get_password();

    /* client_user.h */
    string get_name_from_list(vector<string>& list);
    void save_file(int sockfd, string& filesize, string& username, string& target_file);
    void dump_file(string& file); // shows file to user
    string get_msg_from_user(); // ends by "__END__" or something
    void show_message(string& msg); // literally just a print
    tuple<string, int> get_file_name(string& username); // get_name_from_list except the list is os.listdir(".")
    void save_file(int sockfd, string& filesize, string& username, string& target_file);
    void parse_userlist(string& response, vector<string> &user_list);

    /* client_server.h */
    int connect_to_server(int sockfd, string& IP, string& port);
    bool server_request_success(int serv_port, string& to_server_msg);
    string send_msg_wait_result(int sockfd, string& msg);
    void send_msg_ignore_response(int sockfd, string& msg);
    void print_mailbox_msgs(int sockfd);
    void send_actual_file(int sockfd, string& username, string& filename);
    void send_actual_msg(int sockfd, string& target_user, string& user_message);

private:
}

#endif /* client.h */
