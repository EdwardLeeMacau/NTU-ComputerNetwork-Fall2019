#include "include_all.h"
#include <unordered_map>
#include <server.h>


using namespace std;

char *login_file_path = "config.txt";
unordered_map<string,string> username_to_psd;
unordered_map<string,int> online;
unordered_map<string,int> username_to_sockfd;
string userlist;
// unordered_map<string,int> mail_number;

Server::Server()
{
	
}

Server::Server(int p): port(p), login_file_path("config.txt"), _port(p)
{

}

void 
clean_msg(char *receivemsg, int len) 
{
	for (int i = 0; i < len; i++) 
		receivemsg[i] = '\0';
}

/*
	Core function of reading Command
*/
string 
get_string(char *message, int read_count)
{
	string temp;

	while(message[read_count] && message[read_count] != '\n')
	{
		temp += message[read_count];
		read_count++;
	}

	return temp;
}

string 
read_file(char *message, int read_count)
{
	string temp;
	
	while(message[read_count])
	{
		if (read_count == 1000) 
			return temp;
		temp += message[read_count];
		read_count++;		
	}

	return temp;
}

string 
getfilelist(string file_path)
{
	string filelist;
	struct dirent **namelist;
	int n;
	n = scandir(file_path.c_str(), &namelist, NULL, alphasort);
	// printf("number of files: %d\n", n);
	while (n--)
	{
		// printf("number of files: %d\n", n);
		if (strcmp(namelist[n]->d_name, ".") && strcmp(namelist[n]->d_name, ".."))
		{
			filelist += namelist[n]->d_name;
			filelist += "\n";
		}
		free(namelist[n]);
	}
	return filelist;
}

/*
	TODO: Return bool
	
	@return bool True if 
*/
int 
CheckFileExist(string file_path, string filename)
{
	struct dirent **namelist;
	int n;
	n = scandir(file_path.c_str(), &namelist, NULL, alphasort);

	while (n--)
	{
		if (filename == namelist[n]->d_name)
			return 1;
	}
	return 0;
}

void 
load_username()
{
	FILE *fd;
	string name_buffer;
	string psd_buffer;
	char buffer[100];
	string temp;

	fd = fopen(login_file_path, "r+");
	if (fd == NULL) 
		fd = fopen(login_file_path, "a+");

	mkdir("./USER/", S_IRWXU | S_IRWXG);
	
	while (fgets(buffer, 100, fd))
	{
		name_buffer = get_string(buffer, 0);

		fgets(buffer,100, fd);
		psd_buffer = get_string(buffer,0);

		username_to_psd[name_buffer] = psd_buffer;
		userlist += name_buffer;
		userlist += "\n";

		//cout << name_buffer << "    " << psd_buffer << endl;
		//mail_number[name_buffer] = 0;

		temp = "./USER/" + name_buffer;
		mkdir(temp.c_str(), S_IRWXU | S_IRWXG);
		
		/*
		string mail_path = "./USER/"+name_buffer+"/.mail_config";
		FILE *fd2 = fopen(mail_path.c_str(), "r+");
		if (fd2 == NULL){
			fd2 = fopen(mail_path.c_str(), "a+");
			mail_number[name_buffer] = 0;
			fwrite("0", sizeof(char), 1, fd2);
			fclose(fd2);
		}else{
			char mail_number_buff[1];
			fread(mail_number_buff,sizeof(char),1,fd2);
			mail_number[name_buffer] = atoi(mail_number_buff);
			fclose(fd2);
		}
		*/

		temp += "/file";
		mkdir(temp.c_str(), S_IRWXU | S_IRWXG);
		temp.clear();

		temp = "./USER/" + name_buffer + "/msg";
		mkdir(temp.c_str(), S_IRWXU | S_IRWXG);
		temp.clear();

		string mail_record = "./USER/" + name_buffer + "/msg/.mail_record";

		// mail_number[name_buffer] = 0;
		name_buffer.clear();
		psd_buffer.clear();
	}

	fclose(fd);
	return;
}

/*
	TODO: Make Function
	- SENDMSG_START
	- SENDMSG_TRANSFER
	- GETUSERLIST
	- SENDFILE_START
	- SENDFILE_TRANSFER
	- GETFILELIST
	- GETFILE
	- GETMAILBOX

	@params
*/
void 
handle_client(int clientsockfd, string username)
{
	char receivemsg[1000];
	string user_dest;
	string file_user_dest;
	string command;
	string msg;
	string filesize_buff;
	string file_segment;
	string filename;
	string path;
	int file_ready = 0;
	int file_size = 0;
	std::unordered_map<std::string,std::string>::const_iterator UserExist;
	char *send_succeed = "SEND_SUCCEED";
	char *send_fail = "FAIL";

	// send users back the message stored when offline
	int ret = 0;
	string filelist;
	string file_path = "./USER/" + username + "/file";
	string msg_path = "./USER/" + username + "/msg";
	string get_filename;
	string send_file_path;
	struct stat st;
	string target_filesize;
	string msg_size_buff;
	char read_buffer[1000];
	int msg_ready;
	int message_size;
	string send_msg_path;
	string msg_segment;

	// unsend mail
	string my_unsend_mail = "./USER/"+username+"/msg/.mail_unsend";
	int fd3 = open(my_unsend_mail.c_str(), O_RDONLY);
	if (fd3 == -1) // No message
	{
		printf("User %s has no unsent mail\n", username.c_str());	
	}
	else
	{
		string unsend;
		int unsend_size;
		
		printf("User %s has unsent mail, sending over\n", username.c_str());
		while ((unsend_size = read(fd3, read_buffer, 1000)) > 0)
		{
			unsend += read_buffer;
			clean_msg(read_buffer, 1000);
		}
		send(clientsockfd, unsend.c_str(), unsend.length(), 0);
		close(fd3);
		remove(my_unsend_mail.c_str());
	}

	while (1)
	{
		int read_count;
		clean_msg(receivemsg, 1000);
		if (ret = (recv(clientsockfd,receivemsg,sizeof(receivemsg),0)) > 0){
			printf("message: |%s|\n",receivemsg);
			
			// Load command, user_dest and msg
			read_count = 0;
			
			command = get_string((char *)receivemsg, read_count);
			if (command == "SENDMSG_START")
			{
				read_count += command.length();
				read_count += 1;
				user_dest = get_string((char *)receivemsg, read_count);
				
				read_count += 1;
				read_count += user_dest.length();
				msg_size_buff = get_string((char *)receivemsg, read_count);

				message_size = atoi(msg_size_buff.c_str());
				UserExist = username_to_psd.find(user_dest);

				// such user exists
				if(UserExist != username_to_psd.end())
				{			
					msg_ready = 1;
				}
				user_dest.clear();			
			
			}
			else if (command == "SENDMSG_TRANSFER")
			{
				if(msg_ready == 1){
					read_count += command.length();
					read_count += 1;
					user_dest = get_string((char *)receivemsg, read_count);
				
					read_count += 1;
					read_count += user_dest.length();
					msg_segment = read_file((char *)receivemsg, read_count);

					msg += msg_segment;

					// if send finish
					if((message_size = message_size-1000+18+user_dest.length()) <= 0)
					{
						//user online
						if(online[user_dest] == 1)
						{
							//send <username>\n<file size>
							string msg_username = "From: " + username + "\n" + msg;
							//send(username_to_sockfd[user_dest], msg_size_user.c_str(), msg_size_user.length(), 0);
							send(username_to_sockfd[user_dest], msg_username.c_str(), msg_username.length(), 0);
							//send(clientsockfd, send_succeed, strlen(send_succeed), 0);
							//recording
							string mail_record_client = "./USER/"+username+"/msg/.mail_record";
							string mail_record = "./USER/"+user_dest+"/msg/.mail_record";

							string user_from = "From: "+ username + "\n";
							string user_to = "SEND TO: "+ user_dest + "\n";

							FILE *fd2 = fopen(mail_record.c_str(), "a+");
							fwrite(user_from.c_str(), sizeof(char), user_from.length(), fd2);
							fwrite(msg.c_str(),sizeof(char),msg.length(), fd2);
							fwrite("\n",sizeof(char),1,fd2);
							fclose(fd2);

							fd2 = fopen(mail_record_client.c_str(), "a+");
							fwrite(user_to.c_str(), sizeof(char), user_to.length(), fd2);
							fwrite(msg.c_str(),sizeof(char),msg.length(), fd2);
							fwrite("\n",sizeof(char),1,fd2);
							fclose(fd2);

							printf("SEND: User online\n");
						}
						else // user offline
						{
							// write to file
							string mail_unsend = "./USER/"+user_dest+"/msg/.mail_unsend";
							string mail_record_client = "./USER/"+username+"/msg/.mail_record";
							string mail_record = "./USER/"+user_dest+"/msg/.mail_record";

							FILE *fd2 = fopen(mail_unsend.c_str(), "a+");
							string user_from = "From: "+ username + "\n";
							string user_to = "SEND TO: "+ user_dest + "\n";

							fwrite(user_from.c_str(), sizeof(char), user_from.length(), fd2);
							fwrite(msg.c_str(),sizeof(char),msg.length(), fd2);
							fwrite("\n",sizeof(char),1,fd2);
							fclose(fd2);

							// User From
							fd2 = fopen(mail_record.c_str(), "a+");
							fwrite(user_from.c_str(), sizeof(char), user_from.length(), fd2);
							fwrite(msg.c_str(),sizeof(char),msg.length(), fd2);
							fwrite("\n",sizeof(char),1,fd2);
							fclose(fd2);

							// User To
							fd2 = fopen(mail_record_client.c_str(), "a+");
							fwrite(user_to.c_str(), sizeof(char), user_to.length(), fd2);
							fwrite(msg.c_str(),sizeof(char),msg.length(), fd2);
							fwrite("\n",sizeof(char),1,fd2);
							fclose(fd2);

							printf("SEND: User offline\n");
						}

						msg.clear();
						msg_ready = 0;
						message_size = 0;
						msg_size_buff.clear();
					}

					msg_segment.clear();
					user_dest.clear();
				}
			}
			else if (command == "GETUSERLIST")
			{
				printf("Giving user list: |%s|\n", userlist.c_str());
				send(clientsockfd, userlist.c_str(), userlist.length(), 0);
				cout << endl;
			}
			else if (command == "SENDFILE_START")
			{
				printf("file is being sent\n");
				read_count += command.length();
				read_count += 1;
				file_user_dest = get_string((char *)receivemsg, read_count);
				
				read_count += 1;
				read_count += file_user_dest.length();
				filename = get_string((char *)receivemsg, read_count);

				read_count += 1;
				read_count += filename.length();
				filesize_buff = get_string((char *)receivemsg, read_count);
				
				file_size = atoi(filesize_buff.c_str());

				UserExist = username_to_psd.find(file_user_dest);
				if (UserExist != username_to_psd.end())
				{
					file_ready = 1;
					path = "./USER/"+file_user_dest+"/file/"+filename;
					cout << file_segment << " is write to " << path << endl;
					remove(path.c_str());

				}
				else
				{
					//user not exist
				}
			}
			else if (command == "SENDFILE_TRANSFER")
			{
				if (file_ready == 1){
					// read_count += command.length();
					// read_count += 1;
					usleep(300);
					char *check_ok = "OK";
					send(clientsockfd, check_ok, strlen(check_ok), 0);
					clean_msg(receivemsg, 1000);
					int file_segment_size = recv(clientsockfd, receivemsg, 1000, 0);
					file_segment = read_file((char *)receivemsg, 0);
					cout << file_segment << " is write to " << path << endl;
					printf("size: %d\n", file_segment_size);
					//cout << file_segment << " is write to " << path << endl;
					FILE *fd = fopen(path.c_str(), "a+");
					fwrite(file_segment.c_str(), sizeof(char), file_segment.length(), fd);
					
					file_segment.clear();
					fclose(fd);
					if ((file_size = (file_size-1000)) <= 0)
					{
						filename.clear();
						filesize_buff.clear();
						file_user_dest.clear();
						file_size = 0;
						file_ready = 0;
						path.clear();
					}
				}
				
			}
			else if (command == "GETFILELIST"){
				filelist = getfilelist(file_path);
				if (filelist == "") filelist = "\n";
				send(clientsockfd, filelist.c_str(), filelist.length(), 0);
				// cout << "file list: " << filelist << endl;
				filelist.clear();
				
			}
			else if (command == "GETFILE")
			{
				read_count += command.length();
				read_count += 1;
				get_filename = read_file((char *)receivemsg, read_count);
				int fileExist = CheckFileExist(file_path, get_filename);
				if (fileExist){
					send_file_path = "./USER/" + username + "/file/" + get_filename;
					struct stat st;
					stat(send_file_path.c_str(), &st);
					target_filesize = to_string(st.st_size);
					send(clientsockfd, target_filesize.c_str(), target_filesize.length(), 0);
					int size;
					int fd = open(send_file_path.c_str(), O_RDONLY);
					clean_msg(read_buffer, 1000);
					while ((size = read(fd, read_buffer, 1000)) > 0){
						printf("sending: %s\n", read_buffer);
						usleep(100);
						string file_content = string(read_buffer);
						send(clientsockfd, file_content.c_str(), file_content.length(), 0);
						clean_msg(read_buffer, 1000);
					}
					target_filesize.clear();
					send_file_path.clear();
				}
				get_filename.clear();
			}
			else if (command == "GETMAILBOX")
			{
				string my_mail_record = "./USER/"+username+"/msg/.mail_record";
				int fd = open(my_mail_record.c_str(), O_RDONLY);
				if (fd == -1)
				{
					char *no_record = "NO RECORD";
					// No record
					send(clientsockfd, no_record, strlen(no_record), 0);
				}
				else
				{
					string record;
					int record_size;
					while ((record_size = read(fd, read_buffer, 1000)) > 0)
					{
						record += read_buffer;
						clean_msg(read_buffer, 1000);
					}
					send(clientsockfd, record.c_str(), record.length(), 0);
					close(fd);
				}

			}
			else
			{
				printf("wrong command\n");
			}
			user_dest.clear();
			msg.clear();
			command.clear();
		}
		else if (ret == 0) // Check if the socket disconnect
		{
			online[username] = 0;
			return;	
		}	
	}
}

/*
	TODO: Make Function
	- LOGIN
	- SIGNUP
	- 
*/
void* 
start_connect(void* sockfd)
{
	int clientsockfd = *((int*)sockfd);
	
	char *fail = "FAIL";
	char *succeed = "SUCCEED";
	char receivemsg[1000];
	string passwordstring;
	string command;
	string usernamestring;
	int login = 0;
	unordered_map<string,string>::const_iterator UsernameExist;
	string temp;

	while (login == 0)
	{
		clean_msg(receivemsg, 1000);
		int clientAlive = recv(clientsockfd,receivemsg,sizeof(receivemsg),0);
		if (!clientAlive) break; // client has closed connection
		else if (clientAlive > 0) {
		//Load command, username and password
			int read_count = 0;
			command = get_string((char *)receivemsg, read_count);
			//cout << command<< endl;
			//if login
			printf("recieved: %s\n", receivemsg);
			if (command == "LOGIN")
			{
				read_count += command.length();
				read_count += 1;
				usernamestring = get_string((char *)receivemsg, read_count);
				
				read_count += 1;
				read_count += usernamestring.length();
				passwordstring = get_string((char *)receivemsg, read_count);

				cout << "username (in c++ string): " << usernamestring << endl;
				cout << "password (in c++ string): " << passwordstring << endl;
				printf("login request: username = %s, password = %s\n", usernamestring.c_str(), passwordstring.c_str());
				
				UsernameExist = username_to_psd.find(usernamestring);
				if(UsernameExist != username_to_psd.end())
				{
					// if username exists
					// username and password match
					// cout << "actual password: " << username_to_psd[usernamestring] << endl;
					cout << "requested password: " << passwordstring << endl;
					if(username_to_psd[usernamestring] == passwordstring)
					{
						login = 1;
						send(clientsockfd, succeed, strlen(succeed), 0);
						usleep(1000);
						//set online
						online[usernamestring] = 1;
						//set sockfd
						username_to_sockfd[usernamestring] = clientsockfd;
						printf("login succeed\n");
						// send(clientsockfd, succeed, strlen(succeed), 0);
						// handle client
						handle_client(clientsockfd, usernamestring);
					}
					else //wrong password
					{
						send(clientsockfd, fail, strlen(fail), 0);
						printf("wrong password\n");
					}
				}
				else
				{
					//username doesn't exist					
					send(clientsockfd, fail, strlen(fail), 0);
					printf("no users\n");
				}
	
			}
			else if (command == "SIGNUP") // sign up
			{
				read_count += command.length();
				read_count += 1;
				usernamestring = get_string((char *)receivemsg, read_count);
				//cout << usernamestring << endl;
				read_count += 1;
				read_count += usernamestring.length();
				passwordstring = get_string((char *)receivemsg, read_count);
				//cout << passwordstring << endl;
				UsernameExist = username_to_psd.find(usernamestring);
				//if username exists
				if(UsernameExist != username_to_psd.end())
				{
					send(clientsockfd, fail, strlen(fail), 0);
					printf("FAILED: Username already exists\n");

				}else
				{
					//store username/password in file
					send(clientsockfd, succeed, strlen(succeed), 0);
					username_to_psd[usernamestring] = passwordstring;
					userlist += usernamestring;
					userlist += "\n";

					temp = "./USER/"+usernamestring;
					mkdir(temp.c_str(), S_IRWXU | S_IRWXG);
					temp += "/msg";
					mkdir(temp.c_str(), S_IRWXU | S_IRWXG);
					temp.clear();
					temp = "./USER/"+usernamestring+"/file";
					mkdir(temp.c_str(), S_IRWXU | S_IRWXG);
					temp.clear();
					
					//mail_number[usernamestring] = 0;

					FILE *fd = fopen(login_file_path, "a+");
					fwrite(usernamestring.c_str(), sizeof(char), usernamestring.length(), fd);
					fwrite("\n", sizeof(char), 1, fd);
					fwrite(passwordstring.c_str(), sizeof(char), passwordstring.length(), fd);
					fwrite("\n", sizeof(char), 1, fd);
					fclose(fd);
					printf("CREAT USER:%s PASS:%s\n", usernamestring.c_str(), passwordstring.c_str());
				}
				
			}
			else
			{
				send(clientsockfd, fail, strlen(fail), 0);
				printf("command wrong\n"); //command wrong
			}
			command.clear();
			usernamestring.clear();
			passwordstring.clear();
		}
		else 
		{
			printf("recv error on fd %d\n", clientsockfd);
		}
	}
	printf("User (fd %d) closed connection\n", clientsockfd);
	close(clientsockfd);

	return NULL;
}

int 
main(int argc, char *argv[])
{
	// TODO: Parse Argument
	if (argc != 2) 
		{ cout << "Usage: ./server <port port_num>" << endl; exit(0); }

	int sockfd, clientsockfd;

	struct addrinfo *res;
	struct addrinfo hint;
	struct sockaddr_in their_addr;
	socklen_t addr_size;

	char s[INET6_ADDRSTRLEN];
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, argv[1], &hint, &res);
	sockfd = socket(res->ai_family, res->ai_socktype,  res->ai_protocol);
	
	bind(sockfd, res->ai_addr, res->ai_addrlen);
	
	listen(sockfd, 10);
	addr_size = sizeof their_addr;

	// load username and password and set all user offline
	load_username();

	pthread_t connect_thread;
	printf("listening on port %s\n", argv[1]);
	
	while(clientsockfd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size))
	{
		printf("a user (fd %d) trying to login...\n", clientsockfd);
		pthread_create(&connect_thread, NULL, &start_connect, (void *)&clientsockfd);
	}
}
