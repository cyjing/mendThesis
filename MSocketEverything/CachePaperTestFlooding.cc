// ndn-grid.cc
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"

#include "ns3/ndnSIM/utils/ndn-consumer-id-tag.h"

#include <boost/lexical_cast.hpp>
#include <math.h> 
#include "User.cc"

#define MAX_X 10
#define MAX_Y 10
#define NUM_CONSUMERS 9


using namespace ns3;

//returns a producer node based on the source data and the topography currently used
Ptr<Node> getProducer(int sourceDNSNumber, PointToPointGridHelper &grid){
    int a,x,y;
    a = sourceDNSNumber % (MAX_X*MAX_Y);
    y = a % MAX_Y;
    x = (a - y) / MAX_Y;
    Ptr<Node> producer = grid.GetNode(x, y);
    return producer;
}

int 
main (int argc, char *argv[])
{
   srand (time(NULL));

    NS_LOG_COMPONENT_DEFINE ("myScript");
    // Setting default parameters for PointToPoint links and channels
    Config::SetDefault ("ns3::PointToPointNetDevice::DataRate", StringValue ("1Mbps"));
    Config::SetDefault ("ns3::PointToPointChannel::Delay", StringValue ("10ms"));
    Config::SetDefault ("ns3::DropTailQueue::MaxPackets", StringValue ("10"));

    Config::SetGlobal ("ndn::WireFormat", StringValue ("1"));

    // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
    CommandLine cmd;
    cmd.Parse (argc, argv);


        // Creating MAX_X x MAX_Y topology
    PointToPointHelper p2p;
    PointToPointGridHelper grid (MAX_X, MAX_Y, p2p);
    grid.BoundingBox(100,100,1000,1000);


    // Install NDN stack on all nodes
    ndn::StackHelper ndnHelper;
    ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::Flooding");
    //ndnHelper.SetContentStore("ns3::ndn::cs::Nocache");
    ndnHelper.InstallAll ();

    // Installing global routing interface on all nodes
    ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
    ndnGlobalRoutingHelper.InstallAll ();

    // Getting containers for the consumer/producer
    // Install NDN applications

    //read source data, create producers to nodes
    ndn::AppHelper producerHelper ("ns3::ndn::Producer");

    std::string prefix = "/1";
    Ptr<Node> producer =  getProducer(15, grid);

    producerHelper.SetPrefix (prefix);
    producerHelper.SetAttribute ("Signature", UintegerValue (producer->GetId()));
    producerHelper.SetAttribute ("PayloadSize", StringValue(boost::lexical_cast<std::string>(1024)));
    producerHelper.Install (producer);
    // Add /prefix origins to ndn::GlobalRouter
    ndnGlobalRoutingHelper.AddOrigins (prefix, producer);



    //steadyStateCSFull();
    ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerOnce");
    std::string prefix2 = "/1";
    consumerHelper.SetPrefix (prefix2);
    int consumerId = 45;
    Ptr<Node> consumer = getProducer(consumerId, grid);
    consumerHelper.Install (consumer); 




    ndn::GlobalRoutingHelper::CalculateRoutes ();
    Simulator::Stop (Seconds (5));
    Simulator::Run ();
    Simulator::Destroy ();
    return 0;
}