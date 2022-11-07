#!/bin/bash

cpu0=0-2,5-31,35,36
#cpu0=0-2,5-33
#mb=100

ids=$(jps | grep CoarseGrainedExecutorBackend | tr -cd "[0-9 ]")
id1=$(jps | grep NodeManager | tr -cd "[0-9]")
id2=$(jps | grep DataNode | tr -cd "[0-9]")
id3=$(jps | grep ExecutorLauncher | tr -cd "[0-9]")

declare -a arr
index=0

for i in $(echo $ids | awk '{print $1,$2,$3,$4,$5,$6,$7,$8}')
do
    arr[$index]=$i
    let "index+=1"
done

#echo "sudo rdtset -I -r $cpu0 -t \"m=$mb;cpu=$cpu0\" -c $cpu0 -p ${arr[0]},${arr[1]},${arr[2]},${arr[3]},${arr[4]},${arr[5]},${arr[6]},${arr[7]},$id1,$id2,$id3"
#sudo rdtset -I -r $cpu0 -t "m=$mb;cpu=$cpu0" -c $cpu0 -p ${arr[0]},${arr[1]},${arr[2]},${arr[3]},${arr[4]},${arr[5]},${arr[6]},${arr[7]},$id1,$id2,$id3

for i in {0..7}
do
	echo "taskset -cp -a $cpu0 ${arr[$i]}"
	taskset -cp -a $cpu0 ${arr[$i]} 1>/dev/null
done
# ./myscript 1>/dev/null 2>&1
# 1>/dev/null sends standard output to garbage
# 2>&1 sends error output to same at 1.

echo "taskset -cp -a $cpu0 $id1"
echo "taskset -cp -a $cpu0 $id2"
echo "taskset -cp -a $cpu0 $id3"
taskset -cp -a $cpu0 $id1 1>/dev/null
taskset -cp -a $cpu0 $id2 1>/dev/null
taskset -cp -a $cpu0 $id3 1>/dev/null
