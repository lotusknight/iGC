#include <unistd.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <sched.h>
#include <stdio.h>
#include <inttypes.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <cmath>
#include "pebs_event.hpp"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ofstream;
using std::vector;

#define EVT_NUM0 0x08d1 // L1 miss retired load instructions
#define EVT_NUM1 0x10d1 // L2 miss retired load instructions

#define EVT_NUM2 0x08a3 // L1 miss cycle
//#define EVT_NUM2 0x01a3 // L2 miss cycle

//#define EVT_NUM2 0x0100 // retired

//#define EVT_NUM2 0x0151 // L1D replace

//#define EVT_NUM2 0x01d1 // L1 hit retired load instructions
//#define EVT_NUM2 0x02d1 // L2 hit retired load instructions

//#define RAW_EVT_NUM 0x06a3 // the correct event
#define PROF_INT 1000000 // in us

int sockets[2][32] =
	{{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47},
	 {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63}};

int nodes[2] = {0, 1};

struct read_format {
	uint64_t nr;
	struct {
		uint64_t value;
		uint64_t id;
	}values[];
};

int evt_ids[3] = {0, 0, 0};

void report(int efd, ofstream &outfile)
{
	char buf[4096];
	struct read_format* rf = (struct read_format*) buf;
	uint64_t count[3];
	read(efd, buf, sizeof(buf));
	for (int j = 0; j < rf->nr; j++) 
	{
		if (rf->values[j].id == evt_ids[0])
		{
		//	cout<<"0\n";
			count[0] = rf->values[j].value;
		}
		else if (rf->values[j].id == evt_ids[1])
		{
		//	cout<<"1\n";
			count[1] = rf->values[j].value;
		}
		else if (rf->values[j].id == evt_ids[2])
		{
		//	cout<<"2\n";
			count[2] = rf->values[j].value;
		}
	}
	cout << count[0] << ", \t" << count[1] << ", \t" << count[2] << "." << endl;
	outfile << count[0] << ", \t" << count[1] << ", \t" << count[2] << "." << endl;

}

int main(int argc, char *argv[])
{

	ofstream outfile("/home/zhao/test/GetEvent/GetEvents.txt");
	outfile << "event0, \tevent1, \tevent2\n";
	struct perf_event_attr pe_0;
	struct perf_event_attr pe_1;
	struct perf_event_attr pe_2;

	// we only need to initialize the event once
	perf_event_init(&pe_0, EVT_NUM0);
	perf_event_init(&pe_1, EVT_NUM1);
	perf_event_init(&pe_2, EVT_NUM2);

	do{
		// initialize the event
		int efd = perf_event_open(&pe_0, -1, sockets[0][3], -1, 0);
		ioctl(efd, PERF_EVENT_IOC_ID, &evt_ids[0]);
		if (efd == -1)
		{
			cerr << "error opening leader event0 " << strerror(errno) << endl;
			exit(-1);
		}

		int efd1 = perf_event_open(&pe_1, -1, sockets[0][3], efd, 0);
		ioctl(efd1, PERF_EVENT_IOC_ID, &evt_ids[1]);
		if (efd1 == -1)
		{
			cerr << "error opening event1 " << strerror(errno) << endl;
			exit(-1);
		}

		int efd2 = perf_event_open(&pe_2, -1, sockets[0][3], efd, 0);
		ioctl(efd2, PERF_EVENT_IOC_ID, &evt_ids[2]);
		if (efd2 == -1)
		{
			cerr << "error opening event2 " << strerror(errno) << endl;
			exit(-1);
		}

		// enable the event
		perf_event_reset(efd);
		perf_event_enable(efd);
		
		usleep(PROF_INT);

		// disable the event
		perf_event_disable(efd);
		
		// report this round
		report(efd, outfile);

		// clean up memory
		close(efd);
	}while (1);
	return 0;
}
