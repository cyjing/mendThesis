#!/bin/bash

startval=$1
endval=$2
nodenum=$3

echo "$startval $endval $nodenum"

for inter in `eval echo {$startval..$endval}`
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
        nodenum=$((nodenum+1))
        /root/scripts/eir_multi_router_moni.sh $arg_str

done
