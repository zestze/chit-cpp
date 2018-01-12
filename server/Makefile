TARGET 		= server
CC 		= g++
CXX 		= g++
INCLUDES 	=
CFLAGS 		= -g -Wall $(INCLUDES)
CXXFLAGS 	= -g -Wall $(INCLUDES) -std=c++11
LDFLAGS 	= -g
LDLIBS 		= -lboost_system

$(TARGET): user.h

.PHONY: clean
clean:
	rm -rf a.out core *.o $(TARGET)
