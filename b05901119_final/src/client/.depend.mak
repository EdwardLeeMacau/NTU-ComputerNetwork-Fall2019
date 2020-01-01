client_main.o: client_main.cpp client.h
client_cmd.o: client_cmd.cpp client_cmd.h ../../include/cmdParser.h \
  ../../include/cmdCharDef.h
client_user.o: client_user.cpp client.h
client.o: client.cpp ../../include/sha256.h client.h
client_server.o: client_server.cpp client.h
client_register.o: client_register.cpp client.h
