#!/bin/bash

count=$1

guid=1
addr1="88:53:2e:8e:31:"
ipaddr="10.15.1."
startid=3
ethaddr="eth"
mask="255.255.255.0"

guid1=$((guid - 1))
baseval=16
addr2=$((guid1*$2 + baseval))
p1=$((guid1*count+3))
p2=$((p1+count-1))
addr3=1
addr4=1
addr=1
j=1
for ((i=p1; i<=p2; i++))
do

   addr3=$((addr2 + i))
   addr4=`echo "obase=16; $addr3" | bc`
   addr="$addr1$addr4"
   j=$i

   interface="$ethaddr$j"
   ip="$ipaddr$startid"
   startid=$((startid+1))

   echo $addr
   echo $interface
   echo $ip
   sudo ip link add link eth1 $interface address $addr type macvlan mode bridge
   sudo ip link set up dev $interface 
   sudo ifconfig $interface $ip netmask $mask up
done

