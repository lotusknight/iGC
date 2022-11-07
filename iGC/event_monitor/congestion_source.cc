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
#include <string>
#include "pebs_event.hpp"
#include "utils.hpp"

using namespace std;

#define GET_ARR_LEN(array)                 \
	{                                      \
		(sizeof(array) / sizeof(array[0])) \
	}

#define M 1048576 //1M
#define RAW_EVT_NUM 0x01cd
//#define RAW_EVT_NUM 0x06a3 // the correct event
#define PROF_INT 1000000 // in us
#define IDLE_INT 1000	 // in us
int S = 600;			 // allocation size per thread
char ***chunks;
mutex uncompleted_mutex;
int uncompleted_count;

mutex thread_mtx;
condition_variable cv;
bool accessing;

mutex latency_mtx;
vector<long> latencies;

int sockets[2][32] =
	{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	  32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},
	 {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	  48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63}};

int nodes[2] = {0, 1};

long *chunk_access_time;

int set_affinity(int tid, int *core_ids)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	int len;
	len = GET_ARR_LEN(core_ids);
	for (int i = 0; i < len; i++)
	{
		CPU_SET(core_ids[i], &mask);
	}
	int ret = sched_setaffinity(0, sizeof(mask), &mask);
}

int set_affinity_one(int tid, int core_id)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core_id, &mask);
	int ret = sched_setaffinity(0, sizeof(mask), &mask);
}

void allocate(int tid, char **p)
{
	for (int i = 0; i < S; i++)
	{
		p[i] = new char[M];
		for (int j = 0; j < M; j++)
		{
			p[i][j] = 'a';
		}
	}
}

void deallocate(int tid, char **p)
{
	for (int i = 0; i < S; i++)
	{
		delete[] p[i];
	}
}

void do_access(int tid, char **p)
{
	struct timeval start;
	struct timeval end;
	std::srand(std::time(NULL));
	//cout << "thread " << tid << " start access" << endl;

	for (int i = 0; i < S; i++)
	{
		gettimeofday(&start, NULL);
		for (int n = 0; n < M; n++)
		{
			p[i][n] = 'b';
		}
		gettimeofday(&end, NULL);
		long interval = (long)get_interval(start, end);
		latency_mtx.lock();
		latencies.push_back(interval);
		latency_mtx.unlock();
	}

	//cout << "interval : " << interval << " count: " << count <<
	//	" mean: " << mean_interval << endl;
}

void _access_(int index)
{
	// set the affinity of this thread
	int tid = syscall(SYS_gettid);
	set_affinity_one(tid, sockets[0][index]);
	chunks[index] = new char *[S];
	allocate(index, chunks[index]);

	uncompleted_mutex.lock();
	uncompleted_count--;
	if (uncompleted_count == 0)
	{
		accessing = true;
		cv.notify_one();
	}
	uncompleted_mutex.unlock();

	do_access(index, chunks[index]);
	deallocate(index, chunks[index]);
}

void report(ofstream &outfile)
{
	for (auto latency : latencies)
	{
		outfile << latency << endl;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cout << "using default allocation size 600MB" << endl;
	}
	else
	{
		S = atoi(argv[1]);
	}
	int pid = getpid();
	int tid = syscall(SYS_gettid);
	set_affinity_one(tid, 63);
	char dummy;
	vector<int> thread_nums = {1, 16, 24, 32};
	//vector<int> thread_nums = {1,2,4,6,8,10,12,14,16,20,24,28,32};
	/*
	vector<int> thread_nums;
	for (int i = 1; i <= 32; i++)
	{
		thread_nums.push_back(i);
	}
	*/
	vector<thread> access_threads;
	struct perf_event_attr pe;
	vector<int> efds;
	cout << "pid: " << pid << endl;
	cout << "press any key to start" << endl;
	dummy = getchar();

	for (auto config : thread_nums)
	{
		latencies.clear();
		accessing = false;
		chunks = new char **[config];
		chunk_access_time = new long[config];
		uncompleted_count = config;
		for (int i = 0; i < config; i++)
		{
			access_threads.push_back(thread(&_access_, i));
		}
		std::unique_lock<mutex> lock(thread_mtx);

		//cout << "main waits on threads" << endl;
		//cv.wait(lock, []{return accessing;});
		while (!accessing)
		{
			cv.wait(lock);
		}

		//terminate all threads
		for (auto &th : access_threads)
		{
			th.join();
		}

		// report this round
		string prefix = "/home/epi/numa-project/data/";
		string file_name = prefix + "latency-" + std::to_string(config) + "-remote.txt";
		ofstream outfile(file_name);
		report(outfile);
		cout << "press enter to continue." << endl;
		dummy = getchar();
		// clean up memory
		delete[] chunks;

		delete[] chunk_access_time;
		access_threads.clear();
		cout << "# of thread: " << config << "done" << endl;
	}
	return 0;
}
