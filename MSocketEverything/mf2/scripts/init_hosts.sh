#!/bin/bash

emptystr=""
eir_log="/root/logs/start_log"


while read line
do
  files+=("$line")
done < /root/scripts/activehost_init.txt

for file in "${files[@]}"
do
    conf="/root/scripts/confs/${file}.conf"
    echo $conf 
    rm $conf 
    guid=$((${file}+3))
    arg=$(sed -n "${file}p" "/root/scripts/exec_param_hosts")
    echo "INTERFACE = $arg" >> $conf
    echo "DEFAULT_GUID = $guid" >> $conf
    echo "POLICY = bestperformance" >>$conf
    echo "BUFFER_SIZE = 10" >> $conf
    echo "IF_SCAN_PERIOD = 5" >> $conf

    logfile="/root/logs/log_host${file}"
    mfstack $conf  -d &> $logfile &
    sleep 2
done
