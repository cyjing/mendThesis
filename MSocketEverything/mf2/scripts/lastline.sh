#!/bin/bash

index=$1
#for file in {$1..$2}
while [ $index -le $2 ] 
do
   echo "last line  of file $index"
   tac "eir_log$index" | sed -n '/^\s*$/!{p;q}'
   index=$((index+1))

done
