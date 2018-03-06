TARGET 		= main
CC 		= clang++-5.0
CXX 		= clang++-5.0
INCLUDES 	= -I../libs/
CFLAGS 		= -g -Wall $(INCLUDES)
CXXFLAGS 	= -g -Wall $(INCLUDES) -std=c++17 -pthread -O2
LDFLAGS 	= -g -pthread -L../libs/
LDLIBS 		= -lboost_system -lsockio

$(TARGET): server.o servlet.o notifier.o

$(TARGET).o: server.h

server.o: user.h servlet.h consts_globs_shared.h notifier.h

servlet.o: user.h consts_globs_shared.h

notifier.o:


.PHONY: clean
clean:
	rm -rf a.out core *.o $(TARGET)
