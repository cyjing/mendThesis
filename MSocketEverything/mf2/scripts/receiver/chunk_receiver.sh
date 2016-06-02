#!/bin/bash
export MF_CLICK_LOG_LEVEL=1

click_config=$MF_SRC/router/click/conf/test/MF_ChunkReceiver.click \
receiver_GUID=2
topology_file=/root/conf/topology/testcfg_1-gstar_3node.tp \
interface=eth0

/usr/local/bin/click -j 4 $click_config \
                     my_GUID=$receiver_GUID topo_file=$topology_file \
                     core_dev=$interface
echo "/usr/local/bin/click -j 4 $click_config my_GUID=$receiver_GUID topo_file=$topology_file core_dev=$interface"

