#!/bin/bash

emptystr=""
while read line
do
  files+=("$line")
done < /root/scripts/activenode_init.txt

nodenum=1
for file in "${files[@]}"
do
  echo "$file"
  if [ "$file" != "$emptystr" ]
  then

    ssh root@node1-"$file" 'python /root/scripts/emulate_mobility.py' &

  fi
done
