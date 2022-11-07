#!/bin/bash
ps -aux | grep "solr" | grep "XX" | tr -cd "[0-9 ]"  | awk '{print $1;}'
