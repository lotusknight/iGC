#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Illegal number of parameters"
fi

arg=$1

start_time="$(date -u +%s.%N)"
#running process here
./$arg
end_time="$(date -u +%s.%N)"
elapsed="$(bc <<<"$end_time-$start_time")"
echo "Total of $elapsed seconds elapsed for process"

