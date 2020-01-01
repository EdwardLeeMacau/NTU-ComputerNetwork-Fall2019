// functions that deal with server directly. 
#include <string>

using namespace std;

int connect_to_server(int sockfd, string& IP, string& port);

bool server_request_success(int serv_port, string& to_server_msg);

string send_msg_wait_result(int sockfd, string& msg);

void send_msg_ignore_response(int sockfd, string& msg);

void print_mailbox_msgs(int sockfd);

void send_actual_file(int sockfd, string& username, string& filename);

void send_actual_msg(int sockfd, string& target_user, string& user_message);