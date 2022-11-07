#!/bin/bash


#grep 'cpu ' /proc/stat | awk '{usage=($2+$4)*100/($2+$4+$5)} END {print usage "%"}'
#grep 'cpu1 ' /proc/stat | awk '{usage=($2+$4)*100/($2+$4+$5)} END {print cpu1 usage "%"}'
#awk '{u=$2+$4; t=$2+$4+$5; if (NR==1){u1=u; t1=t;} else print "cpu usage " ($2+$4-u1) * 100 / (t-t1) "%"; }'	<(grep 'cpu ' /proc/stat) <(sleep 1;grep 'cpu ' /proc/stat)
#awk '{u=$2+$4; t=$2+$4+$5; if (NR==1){u1=u; t1=t;} else printf ("%2.2f",$(($2+$4-u1) * 100 / (t-t1))) ; }'	<(grep 'cpu ' /proc/stat) <(sleep 1;grep 'cpu ' /proc/stat)


#awk '{u=$2+$4; t=$2+$4+$5; if (NR==1){u1=u; t1=t;} else printf ("%.0f",$((u-u1) * 100 / (t-t1))) ; }'	<(grep 'cpu ' /proc/stat) <(sleep 1;grep 'cpu ' /proc/stat)
#awk '{u=$2+$4; t=$2+$4+$5; if (NR==1){u1=u; t1=t;} else printf ("%f %f %f %f  %.0f", u, u1, t, t1, ((u-u1)*100/(t-t1)));}'	<(grep 'cpu ' /proc/stat) <(sleep 1;grep 'cpu ' /proc/stat)
awk '{u=$2+$4; t=$2+$4+$5; if (NR==1){u1=u; t1=t;} else printf ("%.0f", ((u-u1)*100/(t-t1)));}'	<(grep 'cpu35 ' /proc/stat) <(sleep 1;grep 'cpu35 ' /proc/stat)


