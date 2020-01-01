server.d: ../../include/server.h 
../../include/server.h: server.h
	@rm -f ../../include/server.h
	@ln -fs ../src/server/server.h ../../include/server.h
