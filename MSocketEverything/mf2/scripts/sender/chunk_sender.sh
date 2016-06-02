#!/bin/bash
export MF_CLICK_LOG_LEVEL=1

click_config=$MF_SRC/router/click/conf/test/MF_ChunkSender.click
sender_GUID=$3
dest_GUID=$2

#topology_file=/root/topology/testcfg_1-gstar_10nodeclean.tp
#topo_latency_file=/root/topology/testcfg_1-gstar_10node_latencyclean.tp
topology_file=/root/topology/mobility_topo
topo_latency_file=/root/topology/mobility_latency


intnum=$1
interface="eth"$intnum

gnrs_server_ip=10.14.1.8
gnrs_server_port=5001
local_ip=10.14.1.1
local_port=$((4000+$1))

#Expt starts after delay secs
#we send chk_limit+1 number of chunks in this expt
#we send chk_size/pkt_size+1 num of pkts in 1 chunk 
#A chunk is set out every chk_interval msecs

service_id=0
#1MB 1048576
#chk_size=1048576
chk_size=10000
chk_limit=1000
chk_interval=10000
delay=20
pkt_size=1400
window_size=1500
loss_prob=0
dest_NAs=$4":"
ASID=$6
aNode_num=$5

/usr/local/bin/click -j 4 $click_config \
                     my_GUID=$sender_GUID topo_file=$topology_file \
                     latency_file=$topo_latency_file \
                     core_dev=$interface \
                     GNRS_server_ip=$gnrs_server_ip GNRS_server_port=$gnrs_server_port \
                     GNRS_listen_ip=$local_ip GNRS_listen_port=$local_port \
                     src_GUID=$sender_GUID \
                     dst_GUID=$dest_GUID \
                     service_id=$service_id \
                     chk_size=$chk_size \
                     chk_lmt=$chk_limit \
                     chk_intval=$chk_interval \
                     delay=$delay \
                     pkt_size=$pkt_size \
                     window_size=$window_size \
                     loss_prob=$loss_prob \
                     dst_NAs=$dest_NAs \
                     asid=$ASID aNodeID=$aNode_num is_border=0
