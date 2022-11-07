#!/bin/bash

cpu=3,4

cassandra_id=$(ps -aux | grep "cassandra" | grep "data" | tr -cd "[0-9 ]"  | awk '{print $1;}')
echo taskset -cp -a $cpu $cassandra_id
taskset -cp -a $cpu $cassandra_id 1>/dev/null
