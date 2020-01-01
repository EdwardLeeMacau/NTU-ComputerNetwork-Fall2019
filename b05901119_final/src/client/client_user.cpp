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

#include "client.h"

using namespace std;

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
