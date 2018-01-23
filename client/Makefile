TARGET 		= client
CC 		= g++
CXX 		= g++
INCLUDES 	= -I../libs/
CFLAGS 		= -g -Wall $(INCLUDES)
CXXFLAGS 	= -g -Wall $(INCLUDES) -std=c++11 -O2
LDFLAGS 	= -g -L../libs/
LDLIBS 		= -lboost_system -lsockio

$(TARGET): user.h

.PHONY: clean
clean:
	rm -rf a.out core *.o $(TARGET)
