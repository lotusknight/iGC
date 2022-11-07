#!/bin/bash
ps -aux | grep "cassandra" | grep "data" | tr -cd "[0-9 ]"  | awk '{print $1;}'
