TARGET 		= sockio
LIBTARG         = libsockio
CC 		= g++
CXX 		= g++
INCLUDES 	=
CFLAGS 		= -g -Wall $(INCLUDES)
CXXFLAGS 	= -g -Wall $(INCLUDES) -std=c++11
LDFLAGS 	= -g
LDLIBS 		= -lboost_system

$(LIBTARG): $(TARGET).o
	ar rc $(LIBTARG).a $(TARGET).o
	ranlib $(LIBTARG).a

$(TARGET).o:

.PHONY: clean
clean:
	rm -rf a.out core *.o $(LIBTARG).a
