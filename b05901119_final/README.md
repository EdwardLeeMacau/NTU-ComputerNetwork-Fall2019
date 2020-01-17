# Computer Network Fall 2019 Final Project: CNChatRoom

## Team member

b05901119 李華健

_ please fill in your name here

## Outline

1. Program Setup
2. Support Functions
3. User Guide
4. System and Program Design
5. Reference (or library)

## How to use?

Execute `make` to generate two files, `./client` and `./server`.

### Client

Especially, if want to send any file to other user, the client should first place the file in their own directory, named as `<username>_fileDir`

To setup the client, please execute
```
./client <ip_address> <port>
```

Support commands:
- Login
- Register
- GetListOfUsers
- GetMailbox
- SendMsg
- SendFile
- GetFile

### Server 

If received any message, such as someone login, require for historical message, send message. The server side will print out the debug message. 

To setup the server, please execute

```
./server <port_number>
```

Support functions:
- Encryption
  - The server will use `SHA256` to encrypt the password when user registered.

## System & Program Design

This program is implemented using a client-server architecture. The server is assumed to always on, and it's IP:Port number is known. It recieves requests from clients and responds accordingly.

When start the client side, it first try to connect to server, and attempt to **Login** or **Register**. If the login is successful, the server creates a new thread to keep connect with the client.

When login, client will send requests(or say commands) to the server to ask for records, such as historical and current messages and files. 

To send a message (or file) to another online user, client should first send the message to the server
- If target user is online
  - Server will immediately notify the target user if his is online. Server also stores this record for later retrieval. 
- If the target is not online
  - Server will help keeping this message and send it to the target user when login.

## Reference

- NTUEE, DSnP for Ric.
  - Makefile architecture
  - Command line tools.
- SHA-256 Encryption