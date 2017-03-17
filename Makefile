CPP    		 = clang
LIBS 			 = -lstdc++
CPPFLAGS 	 = -std=c++14 -Werror -Wall -Wextra -W -Wpedantic

DEPS       = lspp.hpp
OBJ 			 = lspp.o

all: lspp

debug: CPPFLAGS += -DDEBUG -g
debug: lspp

%.o: %.cpp $(DEPS)
		$(CPP) -c -o $@ $< $(CPPFLAGS)

lspp: $(OBJ)
		$(CPP) -o $@ $^ $(CPPFLAGS) $(LIBS)

clean:
	rm *.o lspp
