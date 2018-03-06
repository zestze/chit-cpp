TARGET 		= sockio
LIBTARG         = libsockio
CC 		= clang++-5.0
CXX 		= clang++-5.0
INCLUDES 	=
CFLAGS 		= -g -Wall $(INCLUDES)
CXXFLAGS 	= -g -Wall $(INCLUDES) -std=c++17
LDFLAGS 	= -g
LDLIBS 		= -lboost_system

$(LIBTARG): $(TARGET).o
	ar rc $(LIBTARG).a $(TARGET).o
	ranlib $(LIBTARG).a

$(TARGET).o:

.PHONY: clean
clean:
	rm -rf a.out core *.o $(LIBTARG).a
