// ndn-grid.cc
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"

#include "ns3/random-variable.h"
#include <boost/lexical_cast.hpp>
#include <math.h> 
#include <map>


#define MAX_NODES 200
#define NUM_CONSUMERS 100
#define MAX_OBJECTS 1000
#define M_MULTIPLIER 20
#define TIMESTEPSSTARTING 20

using namespace ns3;


//2d vector, first layer is every time bucket, second is all requests in that time bucket
std::vector<std::vector<std::pair<int, std::string> > * > generatedTrace;
int timeBucket = 0;


int poissonK(double a, double lambda){
    int k = 0;
    double e_neg_lamT = exp (-lambda);

    double p_k = e_neg_lamT;
    double total = p_k;


    while (total < a - 0.000001){
        k++;
        p_k = p_k * lambda /double(k);
        total += p_k;
    }

    return k;
}


int poissonProcess(double a, double lambda, int t){
    int k = 0;
    double e_neg_lamT = exp (-lambda*t);
    double p_k = e_neg_lamT;
    double total = e_neg_lamT;

    //printf("p_k value %f, a value %f\n", p_k, a);

    while (total < a){
        k++;
        p_k = p_k * lambda * t/double(k);
        total += p_k;
    }
    return k;
}


// produces a trace for a single object through all time buckets
// currently, there are independent requests from time 0-TIMESTEPSTARTING, and then decay from there 
int HawkesProcess(double prob, double locFactor, std::string content){

    int totalNum = 0;

    std::vector<std::pair< int , int> > v;

    //generate first generation
    for (int i =0; i < TIMESTEPSSTARTING; i++){
        double b = ((double) rand() / (RAND_MAX));
        int totalNumOneTime = poissonK(b, prob);
        totalNum += totalNumOneTime;
        std::vector<std::pair< int, std::string> > * requests= generatedTrace[i];
        for(int j=0; j< totalNumOneTime; j++){
            int randomLocation = rand() % (MAX_NODES);
            std::pair<int, int> input (randomLocation, i);
            v.push_back(input);

            std::pair<int, std::string> req (randomLocation, content);
            requests->push_back(req);
        }
    }

    int idx = 1;
    int end = totalNum;


    //fisrt-n generation offsprings, locFactor has to be < 1 so we don't blow up
    //each loop goes through one offspring, produces more
    //idx = index of the offspring
    while (idx < end){
        uint32_t parentLayer = v[idx].second;

        double a = ((double) rand() / (1.05*RAND_MAX));
        int totalOffspring = poissonK(a, locFactor);

        if (totalOffspring > 0){
            std::string sourceDNSNodeName = boost::lexical_cast<std::string>(v[idx].first);
            Ptr<Node> parent = Names::Find<Node> (sourceDNSNodeName);


            std::vector<std::pair< int, std::string> > * childRequests = new std::vector<std::pair< int, std::string> >();
            if (generatedTrace.size() > parentLayer + 2){
                childRequests = generatedTrace[parentLayer + 1];
            }else{
                generatedTrace.push_back(childRequests);
            }

            //printf("%i   %i    %i\n", parentLayer, totalOffspring, v[idx].first);
            if (parent->neighborLinks.size()>0){
                for (int j =0; j< totalOffspring; j++){
                    int randNeighbor = rand() % (parent->neighborLinks.size());
                    int child_loc = atoi(parent->neighborLinks[randNeighbor].c_str());

                    std::pair<int, int> input (child_loc, parentLayer + 1);
                    v.push_back(input);
                    std::pair<int, std::string> childReq (child_loc, content);
                    childRequests->push_back(childReq);
                }
            }
        }
        
        end += totalOffspring;
        idx +=1;
    }

    return 0;
}

//returns a producer node based on the source data and the topography currently used
Ptr<Node> getRandomProducer(){
    int randomLocation = rand() % (MAX_NODES);
    std::string sourceDNSNodeName = boost::lexical_cast<std::string>(randomLocation);
    Ptr<Node> producer = Names::Find<Node> (sourceDNSNodeName);
    return producer;
}


// generate requests for all objects
void generateAllTraces(){
    uint32_t i;
    double locFactor = 0.8;

    //generate first generation
    for (int i =0; i < TIMESTEPSSTARTING; i++){
        std::vector<std::pair< int, std::string> > * requests= new std::vector<std::pair< int, std::string> >();
        generatedTrace.push_back(requests);
    }



    std::ofstream os;
    os.open("generatedTrace.txt", std::ios_base::app);
    


    for (i=1; i<= MAX_OBJECTS; i++){
        double zipf = 1/(double)i;
        std::string prefix = "/" + boost::lexical_cast<std::string>(i);
        HawkesProcess(zipf * M_MULTIPLIER, locFactor, prefix);
    }

    uint32_t totalRequests = 0;

    for (i=0; i< generatedTrace.size(); i++){
        std::vector<std::pair< int, std::string> > * requests = generatedTrace[i];
        totalRequests += requests->size();
        for(std::vector<std::pair< int, std::string> >::iterator it = requests->begin(); it != requests->end(); ++it) {
            std::string prefix = (*it).second;
            std::string sourceDNSNodeName = boost::lexical_cast<std::string>((*it).first);
            
            std::ostringstream ss;
            ss<< i ;
            os<<ss.str();

            os<<"    "<< prefix.c_str();
            os<<"    "<<sourceDNSNodeName.c_str()<<std::endl;
        }
    }

    os.close();
    printf("%i\n", totalRequests);
}


int 
main (int argc, char *argv[])
{
    srand (time(NULL));


    // Creating topology from file
    AnnotatedTopologyReader topologyReader ("", 25);
    topologyReader.SetFileName ("myTreeTopology.txt");
    topologyReader.Read ();


    std::ofstream os;
    os.open("generatedObjects.txt", std::ios_base::app);

    //Add sources
    for(int i=1; i<=MAX_OBJECTS;i++){
        std::string prefix = "/" + boost::lexical_cast<std::string>(i);
        int randomLocation = rand() % (MAX_NODES);
       
        std::ostringstream ss;
        ss<< i ;
        os<<ss.str();

        std::ostringstream ss2;
        ss2<<"    "<< randomLocation;
        os<<ss2.str()<<std::endl;    
    }
    os.close();
    
    generateAllTraces();


    return 0;
}
