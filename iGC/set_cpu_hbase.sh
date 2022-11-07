#!/bin/bash

#cpu=0-1
cpu=3
cpuz=35
#cpuz=32-33
#cpuz=34-35

cassandra_id=$(ps -aux | grep "hbase" | grep "XX" | tr -cd "[0-9 ]"  | awk '{print $1;}')
echo taskset -cp -a $cpu $cassandra_id
taskset -cp -a $cpu $cassandra_id 1>/dev/null

#echo "$cassandra_id
#jstack 1424176 | awk '/nid/ && /Z/' | awk 'NR>2 {print $6;}' | cut -f2 -d"x"
arr=$(jstack $cassandra_id | awk '/nid/ && /Z/' | awk 'NR>2 {print $6;}' | cut -f2 -d"x")
for id in $arr
do
	echo taskset -cp $cpuz $((16#$id))
	taskset -cp $cpuz $((16#$id)) 1>/dev/null
done
