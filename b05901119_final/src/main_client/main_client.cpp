/****************************************************************************
  FileName     [ main_client.cpp ]
  PackageName  [ main ]
  Synopsis     [ Define main() ]
****************************************************************************/

#include <fcntl.h>
#include <sys/socket.h>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include "client.h"
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Global cmd Manager
//----------------------------------------------------------------------
CmdParser* cmdMgr = new CmdParser("CNChatRoom >");

extern bool initCommonCmd();
extern bool initClientCmd();

static void
usage()
{
   cout << "Usage: CNChatRoom <-Server ipAddress> <-Port port>" << endl;
}

static void
myexit()
{
   usage();
   exit(-1);
}

/*
int
main(int argc, char** argv)
{
    myUsage.reset();
    
    ifstream dof;

    if (argc == 3) {  // -file <doFile>
        if (myStrNCmp("-File", argv[1], 2) == 0) {
            if (!cmdMgr->openDofile(argv[2])) {
                cerr << "Error: cannot open file \"" << argv[2] << "\"!!\n";
                myexit();
            }
        }
        else {
            cerr << "Error: unknown argument \"" << argv[1] << "\"!!\n";
            myexit();
        }
    }
    else if (argc != 1) {
        cerr << "Error: illegal number of argument (" << argc << ")!!\n";
        myexit();
    }

    if (!initCommonCmd() || !initClientCmd())
        return 1;

    CmdExecStatus status = CMD_EXEC_DONE;
    while (status != CMD_EXEC_QUIT) {  // until "quit" or command error
        status = cmdMgr->execOneCmd();
        cout << endl;  // a blank line between each command
    }

    return 0;
}
*/

int 
main(int argc, char** argv) 
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