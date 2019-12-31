/****************************************************************************
  FileName     [ register.h ]
  PackageName  [ client ]
  Synopsis     [  ]
****************************************************************************/
#ifndef REGISTER_H
#define REGISTER_H

#include <tuple>
#include <string>

using namespace std;

// int connect_to_server(int serv_port);

tuple<string, string> register_user(int serv_port);

string get_username();

string get_password();

#endif /* register.h */