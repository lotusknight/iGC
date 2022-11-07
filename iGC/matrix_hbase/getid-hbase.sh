#!/bin/bash
ps -aux | grep "hbase" | grep "XX" | tr -cd "[0-9 ]"  | awk '{print $1;}'
