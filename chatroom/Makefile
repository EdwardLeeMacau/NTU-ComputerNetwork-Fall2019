REFPKGS = cmd
SRCPKGS = util sha256 client server
LIBPKGS = $(REFPKGS) $(SRCPKGS)
MAIN    = main

EXEC    = client server

LIBS    = $(addprefix -l, $(LIBPKGS))
SRCLIBS = $(addsuffix .a, $(addprefix lib, $(SRCPKGS)))

PORT    = 19798
SERV_IP = 127.0.0.1

all: bin libs main
	
bin:
	mkdir bin

libs:
	@for pkg in $(SRCPKGS); \
	do \
		echo "Checking $$pkg... "; \
		cd src/$$pkg; make -f make.$$pkg --no-print-directory PKGNAME=$$pkg; \
		cd ../..; \
	done

main: 
	@for exec in $(EXEC); \
	do \
		echo "Checking main_$$exec... "; \
		cd src/main_$$exec; \
			make -f make.$$exec --no-print-directory INCLIB="$(LIBS)" EXEC=$$exec; \
		cd ../..; \
		ln -fs bin/$$exec; \
	done

rc:
	./client $(SERV_IP) $(PORT)	 

rs:
	./server $(PORT)

cleanall: clean
	@echo "Removing bin/* ..."
	@rm -rf bin/*

clean: 
	@for pkg in $(SRCPKGS); \
	do \
		echo "Cleaning $$pkg..."; \
		cd src/$$pkg; \
			make -f make.$$pkg --no-print-directory PKGNAME=$$pkg clean; \
		cd ../..; \
	done
	@echo "Removing $(SRCLIBS)..."
	@cd lib; rm -f $(SRCLIBS); cd ../
	@for exec in $(EXEC); \
	do \
		echo "Removing main_$$exec..."; \
		rm -f bin/$$exec $$exec; \
	done

linux18 linux16 mac:
	@for pkg in $(REFPKGS); \
	do \
	        cd lib; ln -sf lib$$pkg-$@.a lib$$pkg.a; cd ../..; \
	done

