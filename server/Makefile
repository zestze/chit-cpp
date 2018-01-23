TARGET 		= server
CC 		= g++
CXX 		= g++
INCLUDES 	= -I../libs/
CFLAGS 		= -g -Wall $(INCLUDES)
CXXFLAGS 	= -g -Wall $(INCLUDES) -std=c++11 -pthread -O2
LDFLAGS 	= -g -pthread -L../libs/
LDLIBS 		= -lboost_system -lsockio

$(TARGET): server.o servlet.o

server.o: user.h servlet.h consts_globs_shared.h

servlet.o: user.h consts_globs_shared.h

.PHONY: clean
clean:
	rm -rf a.out core *.o $(TARGET)
