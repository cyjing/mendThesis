#!/bin/bash

emptystr=""
eir_log="/root/logs/start_log"

#/stoprouter.sh &
#/deletelog.sh &

while read line
do
  files+=("$line")
done < /root/scripts/activenode_init.txt

for file in "${files[@]}"
do
    /root/scripts/ip_access_router.sh $(sed -n "${file}p" "/root/scripts/exec_param_1node") &
    sleep 2
done
