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

    logfile="/root/logs/log_host${file}"
    mfstack $conf  -d &> $logfile &
    sleep 2
done
