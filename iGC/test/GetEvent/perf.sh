#!/bin/bash
#mem_inst_retired.all_loads
#       [All retired load instructions. (Precise Event) Supports address when
#mem_inst_retired.all_stores
#       [All retired store instructions. (Precise Event) Supports address when

#report each 1000ms for CPU 3
sudo perf stat -e mem_inst_retired.all_loads,mem_inst_retired.all_stores -C 3 -I 1000
