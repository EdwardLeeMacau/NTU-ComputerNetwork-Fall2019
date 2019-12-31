/****************************************************************************
  FileName     [ main.cpp ]
  PackageName  [ main ]
  Synopsis     [  ]
****************************************************************************/

#include "include_all.h"
#include "client/client_register.h"
#include "client/client_server.h"
#include "client/client_user.h"
#include "client/client.h"

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

int 
main(int argc, char const *argv[]) 
{
    // system("clear");

    if (argc != 3) 
        cout << "Usage: ./client [hostIP] [port_number]\n", exit(0);

    // Parsing Arguments
    string hostIP      = argv[1]; 
    string port_number = argv[2];
    string username, password;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); 

    connect_to_server(sockfd, hostIP, port_number);

    // login / register loop
    while (true) 
    {
        int action = get_action_start(); // 0 = exit, 1 = register, 2 = login
        
        if (action == 0) 
        {    
            printf("Goodbye!\n"), close(sockfd), exit(0);
        }
        else if (action == 1) 
        { 
            // register
            tie(username, password) = register_user(sockfd);
            cout << "username: |" << username << "|" << endl;
            cout << "password: |" << password << "|" << endl << endl;
            // check if username already exists
            string msg = "SIGNUP\n" + username + "\n" + password;

            if (server_request_success(sockfd, msg)) 
            { 
                printf("Register successful!\n"); 
            }
            else 
            { 
                printf("Register failed, please try again\n"); 
                string whatever; 
                getline(cin, whatever); 
            }
        }
        else if (action == 2) 
        { 
            // login
            username = get_username();
            password = get_password();
            cout << "username: |" << username << "|" << endl;
            cout << "password: |" << password << "|" << endl << endl;
            string msg = "LOGIN\n" + username + "\n" + password;
            // cout << "msg: |" << msg << "|" << endl;
            if (server_request_success(sockfd, msg)) 
            {
                printf("Login successful!\n"); 
                // create file / message directories if they don't already exist
                create_user_dir(username);
                break;
            }
            else 
            {
                printf("Login failed, please try again.\n"); 
                string whatever; 
                getline(cin, whatever); 
            }
        }
    }
    // ^ this should work fine now

    while (true) 
    {
        // show mailbox messages 
        print_mailbox_msgs(sockfd);
        
        // ask action
        int action = get_action_session(username); // 0 = exit, 1 = send file, 2 = view files, 3 = send_messages, 4 = view msgs
        
        if (action == 0) 
        {
            printf("Goodbye!\n"); 
            close(sockfd); 
            exit(0);
        }
        if (action == 1) 
        { 	// send file; ask server for who to send to
            string filename; int filesize; 
            tie(filename, filesize) = get_file_name(username); // wrapper to get_str, but also lists current directory?
            if (filename == "") {
                printf("Press [Enter] to continue.\n");
                string whatever; cin.ignore(); getline(cin, whatever);
                continue;
            }
            // printf("filename: %s, size: %d\n", filename.c_str(), filesize);
            string request = "GETUSERLIST\n";
            string response = send_msg_wait_result(sockfd, request); // sends msg & returns response
            vector<string> user_list; parse_userlist(response, user_list);
            string target_user = get_name_from_list(user_list);
            request = "SENDFILE_START\n";
            string msg = request + target_user + "\n" + filename + "\n" + to_string(filesize);
            send_msg_ignore_response(sockfd, msg);
            // cout << "msg: " << msg << endl;
            send_actual_file(sockfd, username, filename);
        }
        else if (action == 2) 
        { 
            // view files
            string request = "GETFILELIST\n";
            string response = send_msg_wait_result(sockfd, request); 
            vector<string> usr_filelist; parse_userlist(response, usr_filelist);
            if (usr_filelist.size() == 0) {
                printf("You don't have any files! Press [Enter] to continue.\n");
                string whatever; cin.ignore(); getline(cin, whatever);
                continue;
            }
            string target_file = get_name_from_list(usr_filelist); // if file in file_username/, skip? nah
            request = "GETFILE\n";
            string msg = request + target_file;
            string filesize = send_msg_wait_result(sockfd, msg); // gets file size
            printf("File saved to %s_file_dir/%s. Press [Enter] to continue.\n", username.c_str(), target_file.c_str());
            save_file(sockfd, filesize, username, target_file); // save file to current directory
            string whatever; cin.ignore(); getline(cin, whatever); 
            // dump_file(file); // shows file to user
        }
        else if (action == 3) 
        { 
            // send msg
            // note: we don't actually need to care if the user is online or offline!
            string request = "GETUSERLIST\n";
            string response = send_msg_wait_result(sockfd, request); // sends msg & returns response
            vector<string> user_list; parse_userlist(response, user_list);
            string target_user = get_name_from_list(user_list);
            string user_message = get_msg_from_user(); // user literally types up a message
            request = "SENDMSG_START\n";
            string msg = request + target_user + "\n" + to_string(user_message.length());
            send_msg_ignore_response(sockfd, msg); // doesn't care about the response
            request = "SENDMSG_TRANSFER\n";
            send_actual_msg(sockfd, target_user, user_message); // sends the message over
        }
        else if (action == 4) 
        { 
            // view msg
            string request = "GETMAILBOX\n";
            string response = send_msg_wait_result(sockfd, request); // get list of users
            string file_dirname = username + "_file_dir/message_records.txt";
            
            int fd = open(file_dirname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            write(fd, response.c_str(), response.length());
            close(fd);

            printf("Message history sent, saved to %s. Press [Enter] to contine.\n", file_dirname.c_str());
            string whatever; cin.ignore(); getline(cin, whatever); 
        }
    }

    return 0;
}
