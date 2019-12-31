CLIENT_SOURCES = $(shell find src -type f -name "client*.cpp")
SERVER_SOURCES = $(shell find src -type f -name "server*.cpp")
UTIL_PKGS      = $()

PORT    = 19798
SERV_IP = 127.0.0.1

all: client server util
	
util:
# 	@for pkg in $(UTIL_PKGS); \
# 	do \
# 		echo "Checking $$pkg... "; \
# 		cd src/$$pkg; make -f make.$$pkg --no-print-directory PKGNAME=$$pkg; \
# 		cd ../..; \
# 	done

client: $(CLIENT_SOURCES)
	g++ -std=c++17 $(CLIENT_SOURCES) -Iinclude -o client

server: $(SERVER_SOURCES)
	g++ -std=c++17 $(SERVER_SOURCES) -Iinclude -o server -lpthread -Wno-write-strings

rc:
	./client $(SERV_IP) $(PORT)	 

rs:
	./server $(PORT)

clean: 
	rm -rf client server *_file_dir
