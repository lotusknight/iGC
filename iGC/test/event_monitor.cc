#include <unistd.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <sched.h>
#include <stdio.h>
#include <inttypes.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <ctime>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <cstdlib>
#include <cmath>
#include "pebs_event.hpp"
//#include "utils.hpp"

using std::cerr;
using std::cin;
using std::condition_variable;
using std::cout;
using std::endl;
using std::mutex;
using std::ofstream;
using std::thread;
using std::vector;

#define GET_ARR_LEN(array)                 \
	{                                      \
		(sizeof(array) / sizeof(array[0])) \
	}

#define M 1048576 //1M
//#define RAW_EVT_NUM 0x25302a3 // L3 miss cycles
#define RAW_EVT_NUM 0x14a3
//#define RAW_EVT_NUM 0x06a3
#define PROF_INT 1000000 // in us
#define IDLE_INT 1000	 // in us
#define MAX_ROUND 50

mutex thread_mtx;
condition_variable cv;
bool should_access;
bool accessing;

int sockets[2][32] =
	{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	  32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},
	 {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	  48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63}};

int nodes[2] = {0, 1};

void report(vector<int> efds, ofstream &outfile)
{
	uint64_t values[3];
	uint64_t event_mean1;
	uint64_t event_mean2;
	uint64_t event_stdev1;
	uint64_t event_stdev2;
	int core_id = 0;
	uint64_t sum1 = 0;
	uint64_t sum2 = 0;
	vector<uint64_t> counts;
	for (auto efd : efds)
	{
		read(efd, values, sizeof(values));
//		cout << "core: " << core_id << " events: " << values[0] << endl;
		//printf("core: %d cycles: %'" PRIu64 "\n", core_id, values[0]);
		counts.push_back(values[0]);
		if (core_id >= 0 && core_id <= 15 || core_id >= 32 && core_id <= 47)
		{
			sum1 += values[0];
		}
		else
		{
			sum2 += values[0];
		}
		core_id++;
	}
	event_mean1 = (uint64_t)((double)sum1 / (core_id / 2));
	event_mean2 = (uint64_t)((double)sum2 / (core_id / 2));
	sum1 = 0;
	sum2 = 0;
	core_id = 0;
	for (auto c : counts)
	{
		uint64_t diff;
		if (core_id >= 0 && core_id <= 15 || core_id >= 32 && core_id <= 47)
		{
			if (c > event_mean1)
			{
				diff = c - event_mean1;
			}
			else
			{
				diff = event_mean1 - c;
			}
			sum1 += diff * diff;
		}
		else
		{
			if (c > event_mean2)
			{
				diff = c - event_mean2;
			}
			else
			{
				diff = event_mean2 - c;
			}
			sum2 += diff * diff;
		}
		core_id++;
	}
	event_stdev1 = (uint64_t)sqrt((double)sum1 / (core_id / 2));
	event_stdev2 = (uint64_t)sqrt((double)sum2 / (core_id / 2));

	cout << "avg. cycles1: " << event_mean1 << " avg. cycles2: " << event_mean2 << " stdev1: " << event_stdev1 << " stdev2: " << event_stdev2 << endl;
	outfile << event_mean1 << " " << event_mean2 << " " << event_stdev1 << " " << event_stdev2 << endl;
	//printf("avg. cycles: %'"PRIu64"\n", count);
}

int main(int argc, char *argv[])
{
	int pid = getpid();
	ofstream outfile("/home/zhao/test/event_count.txt");
	outfile << "event_mean  event_stdev\n";
	char dummy;
	//vector<int> thread_nums = {1, 2, 4, 8, 16, 32};
	//vector<int> thread_nums = {1,2,4,6,8,10,12,14,16,20,24,28,32};
	struct perf_event_attr pe;
	vector<int> efds;
	cout << "pid: " << pid << endl;
	cout << "press any key to start" << endl;
	dummy = getchar();

	// we only need to initialize the event once
	perf_event_init(&pe, RAW_EVT_NUM);

	//for (auto config: thread_nums)
	for (int r = 0; r < MAX_ROUND; r++)
	{
		// initialize the event
		for (int i = 0; i < 64; i++)
		{
			int efd = perf_event_open(&pe, -1, i, -1, 0);
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
		report(efds, outfile);

		// clean up memory
		for (auto efd : efds)
		{
			close(efd);
		}
		efds.clear();
	}
	return 0;
}
