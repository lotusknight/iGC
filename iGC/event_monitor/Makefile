CC = g++
CFLAGS = -std=c++11
LFLAGS = -lpthread

OBJS = $(OBJS1) $(OBJS2) $(OBJS3)

OBJS1 = event_monitor.o pebs_event.o
OBJS2 = check_latency.o pebs_event.o
OBJS3 = congestion_source.o

start  : $(OBJS1) $(OBJS2) $(OBJS3) utils.o
	$(CC) $(CFLAGS) -o event_monitor $(OBJS1) $(LFLAGS)
	$(CC) $(CFLAGS) utils.o -o check_latency $(OBJS2) $(LFLAGS)
	$(CC) $(CFLAGS) utils.o -o congestion_source $(OBJS3) $(LFLAGS)
utils.o : utils.hpp utils.cc
	$(CC) $(CFLAGS) -c utils.cc
pebs_event.o  : pebs_event.hpp pebs_event.cc
	$(CC) $(CFLAGS) -c pebs_event.cc
congestion_source.o : congestion_source.cc
	$(CC) $(CFLAGS) -c congestion_source.cc
event_monitor.o : %.o :%.cc
	$(CC) $(CFLAGS) -c event_monitor.cc
check_latency.o : %.o : %.cc
	$(CC) $(CFLAGS) -c check_latency.cc

clean :
	@rm *.o event_monitor check_latency congestion_source # @ means not show
