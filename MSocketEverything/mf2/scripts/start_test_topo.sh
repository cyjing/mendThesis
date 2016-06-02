#!/bin/bash

while read line
do
  nodes+=("$line")
done < /root/scripts/test_topo

inter=3
for nodenum in "${nodes[@]}"
do
        var=$(sed -n "${nodenum}p" "/root/scripts/is_BR")
        temp_var=0
        for word in $var
        do
            a[$temp_var]=$word
            temp_var=$((temp_var+1))
        done

        arg_str="$nodenum $nodenum $inter ${a[1]} ${a[2]}"
        echo $arg_str
        inter=$((inter+1))
        nodenum=$((nodenum+1))
        /root/scripts/eir_multi_router_moni.sh $arg_str

done

ssh root@node1-"1" 'python /root/scripts/emulate_mobility.py' &
