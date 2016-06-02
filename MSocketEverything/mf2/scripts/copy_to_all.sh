#!/bin/bash
filename=$1
emptystr=""
while read line
do
  nodes+=("$line")
done < /root/scripts/activenode_init.txt


for node in "${nodes[@]}"
do
  echo "$node"

  if [ "$node" != "$emptystr" ]
  then

    scp $MF_SRC/router/click/elements/gstar/* root@node1-$node:mobilityfirst/router/click/elements/gstar/
    #scp $MF_SRC/../scripts/mobility root@node1-$node:scripts/
  fi
done
