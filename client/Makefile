TARGET 		= main
CC 		= clang++-5.0
CXX 		= clang++-5.0
INCLUDES 	= -I../libs/
CFLAGS 		= -g -Wall $(INCLUDES)
CXXFLAGS 	= -g -Wall $(INCLUDES) -std=c++17 -O2
LDFLAGS 	= -g -L../libs/
LDLIBS 		= -lboost_system -lsockio

$(TARGET): client.o

$(TARGET).o: client.h

client.o: user.h

.PHONY: clean
clean:
	rm -rf a.out core *.o $(TARGET)
