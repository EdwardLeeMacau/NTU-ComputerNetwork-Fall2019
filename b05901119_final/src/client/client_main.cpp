/****************************************************************************
  FileName     [ main.cpp ]
  PackageName  [ main ]
  Synopsis     [  ]
****************************************************************************/

#include "include_all.h"
#include "client_register.h"
#include "client_server.h"
#include "client_user.h"
#include "client.h"

using namespace std;

int 
get_action_start() 
{
    // system("clear");
    printf("What do you want to do?\n0: quit\n1: register\n2: login\n");

    struct pollfd P = {STDIN_FILENO, POLLIN, 0};
    if (poll(&P, 1, 10000) == 0) return 5555;
    
    string buf; getline(cin, buf);

    if (buf == "0") return 0; 
    if (buf == "1") return 1;
    if (buf == "2") return 2; 

    printf("Please choose a valid command!\n"); 
    return 555;
}

int 
get_action_session(string& username) 
{ 
    string buf; 

    // system("clear");

    // TODO: Enum
    // 0 = exit, 1 = send file, 2 = view files, 3 = send_messages, 4 = view msgs
    printf("Logged in as: %s\n", username.c_str());
    printf("What do you want to do?\n0: exit\n1: send file\n2: view files\n3: send_messages\n4: view msgs\n5: refresh\n");
    struct pollfd P = {STDIN_FILENO, POLLIN, 0};

    if (poll(&P, 1, 5000) == 0) return 5555;

    getline(cin, buf);
    if (buf == "0") return 0; 
    if (buf == "1") return 1;
    if (buf == "2") return 2;
    if (buf == "3") return 3;
    if (buf == "4") return 4;
    if (buf == "5") return 5;

    printf("Please choose a valid command!\n"); 
    return 555;
}

/*
	Create user directory if doesn't exists

	@params username 
*/
void 
create_user_dir(string& username) 
{
    struct stat st = {0}; 
    string dirname = username + "_file_dir";

    if (stat(dirname.c_str(), &st) == -1) 
    {
        mkdir(dirname.c_str(), 0766);
    }
}