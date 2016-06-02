#! /bin/bash


NS_LOG=ndn.Consumer ./waf --run="baseline --p=1" > nocache1P.txt 2>&1
NS_LOG=ndn.Consumer ./waf --run="baseline --p=1 --cType=l" > lrucache1P.txt 2>&1
NS_LOG=ndn.Consumer ./waf --run="baseline --p=1 --cType=r" > randomcache1P.txt 2>&1

NS_LOG=ndn.Consumer ./waf --run="baseline --p=3" > nocache3P.txt 2>&1
NS_LOG=ndn.Consumer ./waf --run="baseline --p=3 --cType=l" > lrucache3P.txt 2>&1
NS_LOG=ndn.Consumer ./waf --run="baseline --p=3 --cType=r" > randomcache3P.txt 2>&1

NS_LOG=ndn.Consumer ./waf --run="baseline --p=5" > nocache5P.txt 2>&1
NS_LOG=ndn.Consumer ./waf --run="baseline --p=5 --cType=l" > lrucache5P.txt 2>&1
NS_LOG=ndn.Consumer ./waf --run="baseline --p=5 --cType=r" > randomcache5P.txt 2>&1



NS_LOG=ndn.Consumer ./waf --run="baseline " > nocache.txt 2>&1
NS_LOG=ndn.Consumer ./waf --run="baseline --cType=l" > lrucache.txt 2>&1
NS_LOG=ndn.Consumer ./waf --run="baseline --cType=r" > randomcache.txt 2>&1
