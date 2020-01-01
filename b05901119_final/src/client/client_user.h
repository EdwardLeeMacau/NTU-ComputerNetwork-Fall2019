#include <string>
#include <vector>
#include <tuple>

using namespace std;

string get_name_from_list(vector<string>& list);

void save_file(int sockfd, string& filesize, string& username, string& target_file);

void dump_file(string& file); // shows file to user

string get_msg_from_user(); // ends by "__END__" or something

void show_message(string& msg); // literally just a print

tuple<string, int> get_file_name(string& username); // get_name_from_list except the list is os.listdir(".")

void save_file(int sockfd, string& filesize, string& username, string& target_file);

void parse_userlist(string& response, vector<string> &user_list);