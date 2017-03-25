CPP    		 = clang
LIBS 			 = -lstdc++
CPPFLAGS 	 = -std=c++14

DEPS       = lspp.hpp format.hpp fileEnt.hpp
OBJ 			 = lspp.o format.o fileEnt.o

all: test

debug: CPPFLAGS += -DDEBUG -g
debug: test

test: CPPFLAGS += -Wall -Wextra -W -Wpedantic -Werror
test: lspp

release: CPPFLAGS += -O3
release: lspp

lspp.o : lspp.cpp lspp.hpp format.hpp fileEnt.o formatTab.hpp
		$(CPP) -c -o $@ $< $(CPPFLAGS)

fileEnt.o : fileEnt.cpp fileEnt.hpp fileEnt.inl format.hpp
		$(CPP) -c -o $@ $< $(CPPFLAGS)

lspp: lspp.o fileEnt.o
		$(CPP) -o $@ $^ $(CPPFLAGS) $(LIBS)

clean:
	rm *.o lspp
