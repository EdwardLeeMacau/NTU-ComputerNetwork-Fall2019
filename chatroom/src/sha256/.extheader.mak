sha256.d: ../../include/sha256.h 
../../include/sha256.h: sha256.h
	@rm -f ../../include/sha256.h
	@ln -fs ../src/sha256/sha256.h ../../include/sha256.h
