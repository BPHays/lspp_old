CPP    		 = clang
LIBS 			 = -lstdc++
CPPFLAGS 	 = -std=c++14 -O3 -Wall -Wextra -W -Wpedantic -Werror

DEPS       = lspp.hpp format.hpp fileEnt.hpp
OBJ 			 = lspp.o format.o fileEnt.o

all: lspp

debug: CPPFLAGS += -DDEBUG -g
debug: lspp

lspp.o : lspp.cpp lspp.hpp format.hpp fileEnt.o formatTab.hpp
		$(CPP) -c -o $@ $< $(CPPFLAGS)

fileEnt.o : fileEnt.cpp fileEnt.hpp fileEnt.inl format.hpp
		$(CPP) -c -o $@ $< $(CPPFLAGS)

lspp: lspp.o fileEnt.o
		$(CPP) -o $@ $^ $(CPPFLAGS) $(LIBS)

clean:
	rm *.o lspp
