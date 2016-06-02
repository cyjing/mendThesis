#!/bin/bash
export MF_CLICK_LOG_LEVEL=1
MF_SRC=/root/git_repos/mf
click_config=$MF_SRC/router/click/conf/MF_IPAccessMulticastRouter.click
router_GUID=$1
intnum=$2
portnum=$3
echo $1 $2 $3
topology_file=/root/topology/testcfg_1-gstar_3node.tp
core_dev_interface="eth"$intnum
#intnum=$((intnum+1))
#edge_dev_interface="eth"$intnum
#edge_dev_interface_ip="10.15.1."$intnum
edge_dev_interface="eth0"
edge_dev_interface_ip="192.168.1.2"
gnrs_server_ip=10.15.1.10
gnrs_server_port=5001
local_ip=10.15.1.4
local_port=$((4000+$portnum))
log="/root/logs/log"
logfile="$log$router_GUID"

/usr/local/bin/click -j 4 $click_config \
                     my_GUID=$router_GUID topo_file=$topology_file \
                     core_dev=$core_dev_interface \
                     edge_dev=$edge_dev_interface edge_dev_ip=$edge_dev_interface_ip \
                     GNRS_server_ip=$gnrs_server_ip GNRS_server_port=$gnrs_server_port \
                     GNRS_listen_ip=$local_ip GNRS_listen_port=$local_port &> $logfile &
echo "/usr/local/bin/click -j 4" $click_config \
                     "my_GUID="$router_GUID "topo_file="$topology_file \
                     "core_dev="$core_dev_interface \
                     edge_dev=$edge_dev_interface edge_dev_ip=$edge_dev_interface_ip \
                     GNRS_server_ip=$gnrs_server_ip GNRS_server_port=$gnrs_server_port \
                     GNRS_listen_ip=$local_ip GNRS_listen_port=$local_port
