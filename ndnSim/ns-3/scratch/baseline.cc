/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <stdlib.h>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/applications-module.h"

namespace ns3 {

int
main(int argc, char* argv[])
{
  // Setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("10"));
  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize

  char ctype = 'n';
  std::string cSize  = "100";
  int producerNum = 5;
  int consumerNum = 40;
  char stype = 'r';

  CommandLine cmd;
  cmd.AddValue("cType", "cache type", ctype);
  cmd.AddValue("cSize", "cache size", cSize);
  cmd.AddValue("p", "number of producers", producerNum);
  cmd.AddValue("c", "number of consumers", consumerNum);
  cmd.AddValue("s", "strategy type", stype);
  cmd.Parse(argc, argv);
 
 // Creating 3x3 topology
  PointToPointHelper p2p;
  PointToPointGridHelper grid(10, 10, p2p);
  grid.BoundingBox(100, 100, 200, 200);

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  
 
  if (ctype == 'r'){
    ndnHelper.SetOldContentStore("ns3::ndn::cs::Random", "MaxSize", cSize);
  }
  else if (ctype == 'l'){
    ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", cSize);
  }
  else if( ctype == 'f'){
    ndnHelper.SetOldContentStore("ns3::ndn::cs::Lfu", "MaxSize", cSize);
  }
  else{
    ndnHelper.SetOldContentStore("ns3::ndn::cs::Nocache");
  }
 

  //ndnHelper.SetOldContentStore("ns3::ndn::cs::Fifo", "MaxSize", "10000");
  ndnHelper.InstallAll();

  // Set BestRoute strategy
  if (stype == 'b'){
    ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/broadcast");
  }
  else if (stype == 'n'){
    ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/ncc"); 
  }else{
    ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route"); 
  }

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  NodeContainer consumerNodes;
  NodeContainer producerNodes;

  for (int i = 0; i < producerNum; i++){
    int x = rand() % 10;
    int y = rand() % 10;

    //int x = 0;
    //int y = 0;
    Ptr<Node> producer = grid.GetNode(x, y);
    producerNodes.Add(producer);
  }
   

  for (int i = 0; i < producerNum; i++){
    std::string prefix = "/prefix/" + std::to_string(i);
    ndn::AppHelper producerHelper("ns3::ndn::Producer");
    producerHelper.SetPrefix(prefix);
    producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
    producerHelper.Install(producerNodes[i]);
    ndnGlobalRoutingHelper.AddOrigins(prefix, producerNodes[i]);
  }
 
  // Install NDN applications
  for (int i = 0; i < consumerNum; i ++){
    int x = rand() % 10;
    int y = rand() % 10;
    Ptr<Node> consumer = grid.GetNode(x,y);
    consumerNodes.Add(consumer);
  }

  for (int i = 0; i < consumerNum; i ++){
    ndn::AppHelper consumerHelper("ns3::ndn::ConsumerZipfMandelbrot");

    int producerPre = rand() % producerNum;

    consumerHelper.SetPrefix("/prefix/" + std::to_string(producerPre));
    consumerHelper.SetAttribute("NumberOfContents", StringValue("100"));
    consumerHelper.SetAttribute("Frequency", StringValue("100")); // 100 interests a second
    consumerHelper.Install(consumerNodes[i]);
  }
  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  ndn::CsTracer::InstallAll("cs-trace.txt", Seconds(1));

  Simulator::Stop(Seconds(20.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
