#!/bin/bash
export MF_CLICK_LOG_LEVEL=1

click_config=$MF_SRC/router/click/conf/test/GNRS_InterfaceTest.click

delay=5
period=30

my_GUID=1
gnrs_server_ip=10.40.1.8
gnrs_server_port=5001
local_ip=10.40.1.1
local_port=4001

/usr/local/bin/click -j 4 $click_config \
                     my_GUID=$my_GUID delay=$delay period=$period\
                     GNRS_server_ip=$gnrs_server_ip GNRS_server_port=$gnrs_server_port \
                     GNRS_listen_ip=$local_ip GNRS_listen_port=$local_port 

