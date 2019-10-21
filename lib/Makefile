SRCFS = $(wildcard *.cpp) \
		$(wildcard */*.cpp) \
		$(wildcard */*/*.cpp) \
		$(wildcard */*/*/*.cpp)
CFLAGS =-c -std=gnu++11 -MMD
THFS  = data_structure/avl_tree.h
OBJFS = $(SRCFS:.cpp=.o)
GCHFS = $(THFS:.h=.h.gch)
HDFS  = $(wildcard *.h) \
		$(wildcard */*.h) \
		$(wildcard */*/*.h) \
		$(wildcard */*/*/*.h) 

DEPS  = $(SRCFS) \
		$(THFS) \
		$(HDFS)

DEBUG =
export DEBUG

OPT=
export OPT

SUBDIRS=$(wildcard */.)

libdynamic_matching.a: $(DEPS)
	make compile
	rm -f *.a
	ar -cvq libdynamic_matching.a $(OBJFS) $(GCHFS)

compile:
	for dir in $(SUBDIRS); do \
		(make lib -eC $$dir); \
	done

clean:
	rm -f *.a
	rm -f *.o
	rm -f *.d
	rm -f *.gch
	for dir in $(SUBDIRS); do \
		(make rclean -C $$dir); \
	done

