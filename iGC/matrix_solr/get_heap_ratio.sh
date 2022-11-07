#!/bin/bash

n=0
heap_size=0
use_size=0
./get_heap_used.sh | while read line
do
	if [ $n -eq 1 ]
	then
		heap_size=$(echo $line | tr -cd "[0-9]")
#		echo $heap_size
	fi
	if [ $n -eq 4 ]
	then
		use_size=$(echo $line | tr -cd "[0-9]")
#		echo $use_size
		let ratio=$use_size*100/$heap_size
		echo $ratio
	fi
	n=$(($n+1))
done

