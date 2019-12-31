#include <stdio.h>
#include <iostream>
#include <ctype.h>
#include <tuple>
#include <string.h>
#include "client/client_register.h"

using namespace std;

/*
	Core function of get_password() and get_username()

	@params getwhat
*/
string 
get_str(string getwhat) 
{ 
	// can only have: {ascii} U {'_'}
	char name[128] = {}; 
	char get_what[32]; 
	strcpy(get_what, getwhat.c_str());
	
	while (true) 
	{
		printf("Enter %s: ", get_what);
		char c = getchar(); //eat "\n"
		if (c != '\n') ungetc(c, stdin);
		fgets(name, 128, stdin); 
		int len = strlen(name); 
		name[len-1] = '\0'; 
		len--;
		bool valid_name = true;
		
		if (len > 16) 
			{printf("Name too long (Max 16 characters!)\n"); continue;} 
		if (len == 0) 
			{printf("Can't be empty!\n"); continue;}
		
		for (int i = 0; i < len; i++) {
			if (!(isalpha(name[i]) || name[i] == '_')) {
				printf("can only consist of _ and alphabet characters, please try again.\n");
				valid_name = false; 
				break;
			}
		}
		if (valid_name) break;
	}
	
	return string(name);
}

string 
get_password() 
{
	return get_str("password");
}

string 
get_username() 
{
	return get_str("username");
}

tuple<string, string> 
register_user(int serv_port) 
{
	// TODO: Limited the length of username and password
	string username, password;
	username = get_username();
	password = get_password();

	return make_tuple(username, password);
}
