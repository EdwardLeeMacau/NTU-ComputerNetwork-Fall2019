client.d: ../../include/client.h 
../../include/client.h: client.h
	@rm -f ../../include/client.h
	@ln -fs ../src/client/client.h ../../include/client.h
