TARGET 		= server
CC 		= g++
CXX 		= g++
INCLUDES 	=
CFLAGS 		= -g -Wall $(INCLUDES)
CXXFLAGS 	= -g -Wall $(INCLUDES) -std=c++11
LDFLAGS 	= -g
LDLIBS 		= -lboost_system

$(TARGET): server.o servlet.o

server.o: user.h servlet.h

servlet.o: user.h server.h

.PHONY: clean
clean:
	rm -rf a.out core *.o $(TARGET)
