// ndn-grid.cc
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"

#include "ns3/random-variable.h"

#include "ns3/ndnSIM/utils/ndn-consumer-id-tag.h"

#include <boost/lexical_cast.hpp>
#include <math.h> 
#include <map>

#include "SimulatedUser.cc"

#define MAX_NODES 200
#define NUM_CONSUMERS 100
#define MAX_PRODUCERS 1000
#define M_MULTIPLIER 100

using namespace ns3;


//2d vector, first layer is every time bucket, second is all requests in that time bucket
std::vector<std::vector<std::pair<int, std::string> > * > generatedTrace;
int timeBucket = 0;
std::string outputFileName = "/0-locality-pathCache-2";

class PcapWriter
{
public:
  PcapWriter (const std::string &file)
  {
    PcapHelper helper;
    m_pcap = helper.CreateFile (file, std::ios::out, PcapHelper::DLT_PPP);
  }

  void
  TracePacket (Ptr<const Packet> packet)
  {
    static PppHeader pppHeader;
    pppHeader.SetProtocol (0x0077);
    m_pcap->Write (Simulator::Now (), pppHeader, packet);
    
  }

private:
  Ptr<PcapFileWrapper> m_pcap;
};




/*
//@param grid, xy coord location in grid
//  indexAndTime, index of movement steps, time till next call
*/

static void AdvancePosition (void){
    if ((uint32_t)timeBucket < generatedTrace.size()){
        std::vector<std::pair< int, std::string> > * requests = generatedTrace[timeBucket];

        ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerOnce");
        consumerHelper.SetAttribute("DoTrace", BooleanValue(true));
        consumerHelper.SetOutputFile(outputFileName);
        for(std::vector<std::pair< int, std::string> >::iterator it = requests->begin(); it != requests->end(); ++it) {
            std::string prefix = (*it).second;
            consumerHelper.SetPrefix (prefix);

            std::string sourceDNSNodeName = boost::lexical_cast<std::string>((*it).first);
            Ptr<Node> consumer = Names::Find<Node> (sourceDNSNodeName);
            consumerHelper.Install (consumer);  
        }


        timeBucket ++;
        Simulator::Schedule (Seconds (0.1), &AdvancePosition);
    }
}


static void StartSim(void){
    ndn::CsTracer::InstallAll ("cs-trace.txt", Seconds (1));
    Simulator::Schedule (Seconds (1), &AdvancePosition);
}


int floodNodeNum = 0;
/*
// Make requests for each node at a different time so that the network doesn't flood 
// and drop all requests at once
*/
static void FillCache (void){
    if(floodNodeNum < MAX_NODES){
        for (int j = 0; j< 10; j++){
            ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerOnce");

            int randomObject = rand() % (MAX_PRODUCERS);
            std::string prefix = "/" + boost::lexical_cast<std::string>(randomObject);
            consumerHelper.SetPrefix (prefix);
            std::string sourceDNSNodeName = boost::lexical_cast<std::string>(floodNodeNum);
            Ptr<Node> producer = Names::Find<Node> (sourceDNSNodeName);
            consumerHelper.Install (producer);  
        }
        floodNodeNum ++;
        Simulator::Schedule (Seconds (0.05), &FillCache); 
    }
}

//try to fill up cache of nodes with random objects to simulate a steady state at start of simulation
void steadyStateCSFull(){
    Simulator::Schedule (Seconds (0.01), &FillCache);
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


    // Creating topology from file
    AnnotatedTopologyReader topologyReader ("", 25);
    topologyReader.SetFileName ("myTreeTopology.txt");
    topologyReader.Read ();


    // Install NDN stack on all nodes
    ndn::StackHelper ndnHelper;
    std::string maxContentStoreSize = "10";
    std::string maxContentStoreAttr = "MaxSize";
    ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::BestRoute",
                                     "CacheOnlyEdge", "false");
    ndnHelper.SetContentStore("ns3::ndn::cs::Lru", maxContentStoreAttr, maxContentStoreSize); 
    //ndnHelper.SetContentStore("ns3::ndn::cs::Nocache"); 
    ndnHelper.InstallAll ();

    // Installing global routing interface on all nodes
    ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
    ndnGlobalRoutingHelper.InstallAll ();

    // Getting containers for the consumer/producer
    // Install NDN applications

    //read source data, create producers to nodes
    ndn::AppHelper producerHelper ("ns3::ndn::Producer");


    //read generated objects, add sources for objects
    std::string line;
    std::ifstream objects ("generatedObjects.txt");
    while ( getline (objects,line) )
    {
        std::istringstream lineBuffer (line);
        std::string object, nodeId;
        lineBuffer >> object >> nodeId;

        std::string prefix = "/" + object;
        Ptr<Node> producer = Names::Find<Node> (nodeId);

        producerHelper.SetPrefix (prefix);
        producerHelper.SetAttribute ("Signature", UintegerValue (producer->GetId()));
        producerHelper.SetAttribute ("PayloadSize", StringValue(boost::lexical_cast<std::string>(1024)));
        producerHelper.Install (producer);
        // Add /prefix origins to ndn::GlobalRouter
        ndnGlobalRoutingHelper.AddOrigins (prefix, producer);
    }
    objects.close();

    //read generated trace
    std::ifstream trace ("generatedTrace.txt");
    int timeStamp = -1;
    std::vector<std::pair< int, std::string> > * requests = new std::vector<std::pair< int, std::string> >();
            
    while ( getline (trace,line) )
    {
        std::istringstream lineBuffer (line);
        std::string prefix;
        int t, nodeId;
        lineBuffer >> t >> prefix >> nodeId;

        std::pair<int, std::string> req (nodeId, prefix);

        if (t > timeStamp){
            requests = new std::vector<std::pair< int, std::string> >();
            generatedTrace.push_back(requests);
            timeStamp = t;
        }
        requests->push_back(req);
    }

    trace.close();


    ndn::GlobalRoutingHelper::CalculateRoutes ();
    Simulator::Stop (Seconds (50));


    steadyStateCSFull();


    Simulator::Schedule (Seconds (10.0), &StartSim);

    Simulator::Run ();
    Simulator::Destroy ();
    return 0;
}
