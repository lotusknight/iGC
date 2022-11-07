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

#define RAW_EVT_NUM 0x02a3
//#define RAW_EVT_NUM 0x06a3 // the correct event
#define PROF_INT 1000000 // in us

int sockets[2][32] =
	{{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47},
	 {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63}};

int nodes[2] = {0, 1};

void report(vector<int> efds, ofstream &outfile)
{
	uint64_t values[3];
	uint64_t event_mean;
//	uint64_t event_stdev;
	int core_id = 0;
	uint64_t sum = 0;
	vector<uint64_t> counts;
	for (auto efd : efds)
	{
		read(efd, values, sizeof(values));
		counts.push_back(values[0]);
		sum += values[0];
		core_id++;
	}
	event_mean = (uint64_t)((double)sum / core_id);
	sum = 0;
	/*
	for (auto c : counts)
	{
		sum += (c - event_mean) * (c - event_mean);
	}
	event_stdev = (uint64_t)sqrt((double)sum / core_id);
	*/
	cout<<event_mean;
//	cout << "avg. cycles: " << event_mean << " stdev: " << event_stdev << endl;
//	outfile << event_mean << " " << event_stdev << " " << endl;
	outfile << event_mean << endl;
}

int main(int argc, char *argv[])
{
	int pid = getpid();
	ofstream outfile("/home/zhao/test/GetLatency/latency.txt");
	outfile << "event_mean event_stdev\n";
	char dummy;
	struct perf_event_attr pe;
	vector<int> efds;

	int round = 1;

	// we only need to initialize the event once
	perf_event_init(&pe, RAW_EVT_NUM);

	do{
		// initialize the event
		for (int i = 0; i < 32; i++)
		{
			int efd = perf_event_open(&pe, -1, sockets[0][i], -1, 0);
			if (efd == -1)
			{
				cerr << "error opening leader " << strerror(errno) << endl;
				exit(-1);
			}
			efds.push_back(efd);
		}

		// enable the event
		for (auto efd : efds)
		{
			perf_event_reset(efd);
			perf_event_enable(efd);
		}
		usleep(PROF_INT);

		// disable the event
		for (auto efd : efds)
		{
			perf_event_disable(efd);
		}

		// report this round
	//	cout << "report round: " << round << endl;
		report(efds, outfile);

		// clean up memory
		for (auto efd : efds)
		{
			close(efd);
		}

		efds.clear();
		round++;
		cout << endl;
	}while (1);
	return 0;
}
