CC = g++
CFLAGS = -std=c++11
#LFLAGS = -lpthread

OBJS = get_latency.o pebs_event.o

start  : $(OBJS)
	$(CC) $(CFLAGS) -o get_latency $(OBJS) $(LFLAGS)
pebs_event.o  : pebs_event.hpp pebs_event.cc
	$(CC) $(CFLAGS) -c pebs_event.cc
get_latency.o : %.o : %.cc
	$(CC) $(CFLAGS) -c get_latency.cc

clean :
	@rm *.o get_latency # @ means not show
