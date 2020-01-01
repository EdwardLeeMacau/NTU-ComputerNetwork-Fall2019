// functions that deal with registration.

#include <tuple>
#include <string>

using namespace std;

int meow();

int connect_to_server(int serv_port);

tuple<string, string> register_user(int serv_port);

string get_username();

string get_password();