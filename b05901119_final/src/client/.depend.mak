client_main.o: client_main.cpp ../../include/include_all.h \
  ../../include/client/client_register.h \
  ../../include/client/client_server.h \
  ../../include/client/client_user.h ../../include/client/client.h
client_cmd.o: client_cmd.cpp ../../include/client/client_cmd.h \
  ../../include/cmd/cmdParser.h ../../include/cmd/cmdCharDef.h
client_user.o: client_user.cpp ../../include/client/client_user.h
client_server.o: client_server.cpp ../../include/include_all.h \
  ../../include/client/client_server.h
client_register.o: client_register.cpp \
  ../../include/client/client_register.h
