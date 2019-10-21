CC     =g++
CFLAGS =-c -std=gnu++11 -MMD
DEBUG  =
OPT	   =
INCLUDE=-I../../data_structure -I../../data_structure/priority_queues -I../../tools -I../../ -I../ 

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)

SUBDIRS=$(wildcard */.)

all: $(OBJ)

lib:
	make all
ifneq ($(SUBDIRS),)
	for dir in $(SUBDIRS); do \
		(make lib -C $$dir); \
	done
endif

clean:
	rm -f *.o
	rm -f *.d
	rm -f *.gch

rclean:
	rm -f *.o
	rm -f *.d
	rm -f *.gch
ifneq ($(SUBDIRS),)
	for dir in $(SUBDIRS); do \
		(make rclean -C $$dir); \
	done
endif

%.o: %.cpp %.h
	$(CC) $(CFLAGS) $(DEBUG) $(OPT) $(INCLUDE) -o $@ -c $<
		
-include $(DEP)
