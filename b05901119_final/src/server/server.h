#ifndef SERVER_H
#define SERVER_H

class Server
{
public:
    Server();
    Server(int /* */);
    ~Server();

    int port;
    char* login_file_path;

private:
    int _port;
};

#endif