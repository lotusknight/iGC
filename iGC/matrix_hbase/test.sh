#!/bin/bash

:<<!
!

#if [ "$#" -ne 1 ]
#then
#	echo "Illegal number of parameters, input cassandra PID"
#	exit 8
#fi

pid_cassandra=$(exec ./getid-hbase.sh)
dur=2
cpu_percentage=95
heap_percentage=40


while true
do
	cpu=$(exec ./cpu.sh)
	gc_heap=$(exec ./get_heap_ratio.sh)
	echo "cpu=$cpu%, gc_heap=$gc_heap%"
	if [ $cpu -lt $cpu_percentage ] && [ $gc_heap -gt $heap_percentage ]
	then
#		echo "pin more cores for cassandra"
#		taskset -cp 0-31 $pid_cassandra	
		echo "GC() for cassandra"
		./trigger_gc_cassandra.sh
		sleep $dur
#		echo "resume for cassandra"
#		taskset -cp 1-2 $pid_cassandra
	fi
	sleep $((4-$dur)) # wait 5 seconds until next GC
done


