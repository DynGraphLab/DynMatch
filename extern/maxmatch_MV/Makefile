CC   =g++
FLAGS=
CLAGS=-march=native -std=gnu++14 -Wall
OPT  =

SRC  =$(wildcard *.cpp)
OBJ  =$(SRC:.cpp=.o)

all:
	make libmv.a
	make mmmv

libmv.a: $(OBJ)
	rm -f libmv.a
	ar -cvq libmv.a $(OBJ)

mmmv: $(OBJ)
	g++ -o mmmv $(OBJ)

clean:
	rm -f *.o
	rm -f mmmv
	rm -f libmv.a

%.o: %.cpp
	$(CC) $(CFLAGS) $(OPT) $(FLAGS) -o $@ -c $<
