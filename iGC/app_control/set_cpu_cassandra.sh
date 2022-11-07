#!/bin/bash

#cpu=0-1
cpu=3,4
cpuz=35,36
#cpuz=32-33
#cpuz=34-35

cassandra_id=$(ps -aux | grep "cassandra" | grep "data" | tr -cd "[0-9 ]"  | awk '{print $1;}')
echo taskset -cp -a $cpu $cassandra_id
taskset -cp -a $cpu $cassandra_id 1>/dev/null

#echo "$cassandra_id
arr=$(jstack $cassandra_id | awk '/nid/ && /Z/' | awk '{print $6;}' | cut -f2 -d"x")
for id in $arr
do
	echo taskset -cp $cpuz $((16#$id))
	taskset -cp $cpuz $((16#$id)) 1>/dev/null
done
