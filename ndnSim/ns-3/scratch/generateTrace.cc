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


#define MAX_NODES 400
#define NUM_CONSUMERS 40
#define MAX_OBJECTS 2000
#define M_MULTIPLIER 5
#define TIMESTEPSSTARTING 4000

// for when we use the grid instead of a input file
#define MAX_X 20
#define MAX_Y 20

using namespace ns3;


//2d vector, first layer is every time bucket, second is all requests in that time bucket
std::vector<std::vector<std::pair<int, std::string> > * > generatedTrace;
std::map< int, std::vector<int> > neighbors;
std::vector<double> m_Pcum;
int timeBucket = 0;


int poissonK(double a, double lambda){
    if (lambda < 0.001){
        return 0;
    }
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

void setNumContents(int numOfContents, double alpha){
    m_Pcum = std::vector<double>(numOfContents + 1);
    m_Pcum[0] = 0.0;
    for (int i = 1; i <= numOfContents; i++) {
        m_Pcum[i] = m_Pcum[i - 1] + 1.0 / std::pow(i, alpha);
    }

    for (int i = 1; i <= numOfContents; i++) {
        m_Pcum[i] = m_Pcum[i] / m_Pcum[numOfContents];
    } 
    printf("%f\n",m_Pcum[1]);
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

std::pair<int, int> getLocGridFromId(int id){
    int x = id % MAX_X;
    int y = id / MAX_X;
    return std::make_pair(x,y);
}

int getIdGridFromLoc(std::pair<int,int> loc){
    return loc.first + loc.second * MAX_X;
}

bool isValidGrid(std::pair<int, int> loc){
    if (loc.first < 0 || loc.first >= MAX_X){
        return false;
    }
    if (loc.second < 0 || loc.second >= MAX_Y){
        return false;
    }
    return true;
}

std::vector< int> getNeighborsFromGridNode(int node){
    std::vector<int> neighbors;
    std::pair<int, int> gridLoc = getLocGridFromId(node);
    std::pair<int, int> top = std::make_pair(gridLoc.first, gridLoc.second-1);
    std::pair<int, int> down = std::make_pair(gridLoc.first, gridLoc.second+1);
    std::pair<int, int> left = std::make_pair(gridLoc.first-1, gridLoc.second);
    std::pair<int, int> right = std::make_pair(gridLoc.first+1, gridLoc.second);

    if (isValidGrid(top)){
        neighbors.push_back(getIdGridFromLoc(top));
    }
    if (isValidGrid(down)){
        neighbors.push_back(getIdGridFromLoc(down));
    }    
    if (isValidGrid(left)){
        neighbors.push_back(getIdGridFromLoc(left));
    }    
    if (isValidGrid(right)){
        neighbors.push_back(getIdGridFromLoc(right));
    }
    return neighbors;
}

// produces a trace for a single object through all time buckets
// currently, there are independent requests from time 0-TIMESTEPSTARTING, and then decay from there 
int HawkesProcess(int process, double locFactor, std::string content){

    int totalNum = 0;

    std::vector<std::pair< int , int> > v;

    //generate first generation
    for (int i =0; i < TIMESTEPSSTARTING; i++){
        int totalNumOneTime = 0;
        for(int k=0; k<MAX_NODES; k++){
            double b = ((double) rand() / (RAND_MAX));
            if (b>m_Pcum[process-1] && b<m_Pcum[process]){
                totalNumOneTime++;
            }
        }

        if (totalNumOneTime > 1){
            totalNumOneTime = totalNumOneTime * .5;
        }
/*
        if(totalNumOneTime > 0 && i > TIMESTEPSSTARTING / 2){
            if (locFactor > 0.4){ // total hack to try to even out total number of requests
                double b = ((double) rand() / (RAND_MAX));
                if(b < 0.5){
                    totalNumOneTime = 0;
                }
            }
            if (locFactor > 0.2){ // total hack to try to even out total number of requests
                double b = ((double) rand() / (RAND_MAX));
                if(b < 0.25){
                    totalNumOneTime = 0;
                }
            }
            //printf("%i\n", totalNumOneTime);
        }
*/
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
            //Ptr<Node> parent = Names::Find<Node> (sourceDNSNodeName);
            int parent = v[idx].first;

            std::vector<std::pair< int, std::string> > * childRequests = new std::vector<std::pair< int, std::string> >();
            if (generatedTrace.size() > parentLayer + 2){
                childRequests = generatedTrace[parentLayer + 1];
            }else{
                generatedTrace.push_back(childRequests);
            }

            std::vector<int> neighbors = getNeighborsFromGridNode(parent);
            //printf("%i   %i    %i\n", parentLayer, totalOffspring, v[idx].first);
            if (neighbors.size()>0){
                for (int j =0; j< totalOffspring; j++){
                    int randNeighbor = rand() % (neighbors.size());
                    int child_loc = neighbors[randNeighbor];

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
void generateAllTraces(int runNum, double locFactor){
    //uint32_t i;
    //generate first generation
    for (int i =0; i < TIMESTEPSSTARTING; i++){
        std::vector<std::pair< int, std::string> > * requests= new std::vector<std::pair< int, std::string> >();
        generatedTrace.push_back(requests);
    }

    double alpha = 0.7;
    setNumContents(MAX_OBJECTS, alpha);

    std::ofstream os;
    


    for (int i=1; i<= MAX_OBJECTS; i++){
        std::string prefix = "/" + boost::lexical_cast<std::string>(i);
        HawkesProcess(i, locFactor, prefix);
    }

    uint32_t totalRequests = 0;
    std::map<int, std::vector<std::string> > nodeTrace;


    for (uint32_t i=0; i< generatedTrace.size(); i++){
        std::vector<std::pair< int, std::string> > * requests = generatedTrace[i];
        totalRequests += requests->size();
        for(std::vector<std::pair< int, std::string> >::iterator it = requests->begin(); it != requests->end(); ++it) {
            std::string timeStr = boost::lexical_cast<std::string>(i);

            std::string prefix = (*it).second;
            std::string sourceDNSNodeName = boost::lexical_cast<std::string>((*it).first);
            std::string line = timeStr + "    " + prefix + "    " + sourceDNSNodeName;
            nodeTrace[(*it).first].push_back(line);
        }
    }

    printf("%i\n", runNum);
    for (uint32_t i=0; i < nodeTrace.size(); i++){
        std::string sourceDNSNodeName = boost::lexical_cast<std::string>(i);
        os.open("./trace/" + std::to_string(runNum) +"/" + sourceDNSNodeName + ".txt", std::ios::out);
        for (uint32_t j=0; j < nodeTrace[i].size(); j++){
            os<<nodeTrace[i][j].c_str()<<std::endl;;
        }

        os.close();
    }

    printf("%i\n", totalRequests);
}


int 
main (int argc, char *argv[])
{
    int runNum  = 0;
    double loc = 0;
    CommandLine cmd;
    cmd.AddValue("runNum", "run number( same as RngRun)", runNum);
    cmd.AddValue("loc", "localization", loc);
    cmd.Parse(argc, argv);

    srand (runNum);


    std::ofstream os;
    os.open("generatedObjects.txt", std::ios_base::app);

    //Add sources
    for(int i=1; i<=MAX_OBJECTS;i++){
        std::string prefix = "/" + boost::lexical_cast<std::string>(i);
        int randomLocation = 0;//rand() % (MAX_NODES);
       
        std::ostringstream ss;
        ss<< i ;
        os<<ss.str();

        std::ostringstream ss2;
        ss2<<"    "<< randomLocation;
        os<<ss2.str()<<std::endl;    
    }
    os.close();
    
    generateAllTraces(runNum, loc);


    return 0;
}
