import matplotlib.pyplot as plt
import subprocess

trials = 100

def testNoCache():
    for i in range(0,trials):
        a = subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=1 --RngRun="+str(i)+"' > ./data/nocache1P"+str(i)+".txt 2>&1", shell=True)
        a.wait()
        #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+"' > ./data/nocache3P"+str(i)+".txt 2>&1", shell=True)
        #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+"' > ./data/nocache5P"+str(i)+".txt 2>&1", shell=True)
    return

def testLRUCache():
    for i in range(0,trials):
        a = subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=1 --RngRun="+str(i)+" --cType=l' > ./data/lrucache1P"+str(i)+".txt 2>&1", shell=True)
        a.wait()
        #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+" --cType=l' > ./data/lrucache3P"+str(i)+".txt 2>&1", shell=True)
        #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+" --cType=l' > ./data/lrucache5P"+str(i)+".txt 2>&1", shell=True)
    return


def testRandomCache():
    for i in range(0,trials):
        a = subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --s=n --p=1 --RngRun="+str(i)+" --cType=r' > ./data/randomcache1P"+str(i)+".txt 2>&1", shell=True)
        a.wait()
        #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --s=n --p=3 --RngRun="+str(i)+" --cType=r' > ./data/randomcache3P"+str(i)+".txt 2>&1", shell=True)
        #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --s=n --p=5 --RngRun="+str(i)+" --cType=r' > ./data/randomcache5P"+str(i)+".txt 2>&1", shell=True)
    return


def testLFUCache():
    for i in range(0,trials):
        a = subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=1 --RngRun="+str(i)+" --cType=f' > ./data/lfucache1P"+str(i)+".txt 2>&1", shell=True)
        a.wait()
        #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+" --cType=f' > ./data/lfucache3P"+str(i)+".txt 2>&1", shell=True)
        #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+" --cType=f' > ./data/lfucache5P"+str(i)+".txt 2>&1", shell=True)
    return

def testAverageHopRandomCache():
    trials = 5
    for csize in range(40,60,20):
        for j in range(3,15):
            for i in range(1,trials):
                a = subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='averageDistance --cSize="+str(csize) + " --d="+ str(j) + " --p=1 --RngRun="+str(i)+" --cType=r' > ./data/randomcache1P"+str(i)+ "AD" + str(j)+ "CS" + str(csize) +".txt 2>&1", shell=True)
                a.wait()
                #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+" --cType=f' > ./data/lfucache3P"+str(i)+".txt 2>&1", shell=True)
                #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+" --cType=f' > ./data/lfucache5P"+str(i)+".txt 2>&1", shell=True)
    return

def testAverageHopRandomCache():
    trials = 10
    for csize in range(80,100,20):
        for i in range(1,trials):
            a = subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='averageDistance --cSize="+str(csize) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --cType=r' > ./data/randomcache1P"+str(i)+ "AD" + str(0)+ "CS" + str(csize) +".txt 2>&1", shell=True)
            a.wait()
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+" --cType=f' > ./data/lfucache3P"+str(i)+".txt 2>&1", shell=True)
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+" --cType=f' > ./data/lfucache5P"+str(i)+".txt 2>&1", shell=True)
    return


#testAverageHopRandomCache()
#testRandomCache()
#testLFUCache()
#testLRUCache()
#testNoCache()
#testAverageHopRandomCache()

def getAverageHopCountFromFile(fileName):
    f = open(fileName, 'r')

    totalPackets = 0.0
    averageHop = 0.0

    longHop = 0.0
    dataHop = 0.0


    for line in f:
        hopSplit = line.split("Hop count: ")
        if len(hopSplit) > 1:
            totalPackets += 1.0
            h = f.next()
            longHop += int(hopSplit[1].split("\n")[0])
            dataHop += int(h.split("Data Hop count: ")[1].split("\n")[0])
            #averageHop += int(h.split("Data Hop count: ")[1].split("\n")[0])  #int(hopSplit[1].split("\n")[0])

    #print fileName, totalPackets, dataHop/totalPackets, dataHop/longHop
    return [fileName, totalPackets, longHop/totalPackets, dataHop/longHop]

def getAverageHopCountFromFileDistance(fileName):
    f = open(fileName, 'r')

    totalPackets = 0.0
    averageHop = 0.0

    longHop = 0.0
    dataHop = 0.0

    distanceList = []
    resultList = []
    for i in range(0,40):
        distanceList.append([0,0])
        
    timeoutReq = 0

    for line in f:
        hopSplit = line.split("Hop count: ")
        if len(hopSplit) > 1:
            totalPackets += 1.0
            h = f.next()
            hopLong = int(hopSplit[1].split("\n")[0])
            hopData = int(h.split("Data Hop count: ")[1].split("\n")[0])
            longHop += hopLong
            dataHop += hopData
            distanceList[hopLong] = [distanceList[hopLong][0] + hopData,distanceList[hopLong][1] + 1]
            #averageHop += int(h.split("Data Hop count: ")[1].split("\n")[0])  #int(hopSplit[1].split("\n")[0])
        elif len(line.split("OnTimeout")) > 1:
            timeoutReq += 1
    #print fileName, totalPackets, dataHop/totalPackets, dataHop/longHop

    for i in range(0,40):
        if (distanceList[i][1]!= 0):
            resultList.append(distanceList[i][0]/float(distanceList[i][1]))
        else:
            resultList.append(0)
    #return [fileName, totalPackets, longHop/totalPackets, dataHop/longHop]
    return [resultList, timeoutReq, totalPackets, longHop]
# print getAverageHopCountFromFileDistance("./data/randomcache1P0AD0CS20.txt")
# print getAverageHopCountFromFileDistance("./data/randomcache1P0AD0CS40.txt")
# print getAverageHopCountFromFileDistance("./data/randomcache1P0AD0CS60.txt")
# print getAverageHopCountFromFileDistance("./data/randomcache1P0AD0CS80.txt")



def getHopInfoFromAvgDist():
    trials = 10
    for csize in range(80,100,20):
        plotDataX = []
        plotDataY = []
        avgTimeout = 0
        totalPackets = 0
        for j in range(0, 1):
            data = getAverageHopCountFromFileDistance("./data/randomcache1P"+str(1)+"AD"+str(0)+"CS"+str(csize)+".txt")
            for i in range(0,40):
                if (data[0][i] != 0):
                    plotDataY.append(data[0][i])
                    plotDataX.append(i)
            avgTimeout += data[1]
            totalPackets += data[2]
        print avgTimeout/10, totalPackets/10
        plt.plot(plotDataX,plotDataY, 'ro')
        plt.axis([0, 40, 0, 15])
        plt.show()

#getHopInfoFromAvgDist()

def getHopInfoFromLRUTest(outputFile):
    lru1list = []
    lru3list = []
    lru5list = [] 

    for i in range(0,trials):
        lru1list.append(getAverageHopCountFromFile("./data/lrucache1P"+str(i)+".txt"))
        #lru3list.append(getAverageHopCountFromFile("./data/lrucache3P"+str(i)+".txt"))
        #lru5list.append(getAverageHopCountFromFile("./data/lrucache5P"+str(i)+".txt"))

    print lru1list
    #print lru3list
    #print lru5list



def getHopInfoFromRandomTest(outputFile):
    lru1list = []
    lru3list = []
    lru5list = [] 

    f = open(outputFile, 'w')


    for i in range(0,trials):
        o = getAverageHopCountFromFile("./data/randomcache1P"+str(i)+".txt")
        f.write(str(o).strip('[]') + "\n")
        #lru1list.append(getAverageHopCountFromFile("./data/randomcache1P"+str(i)+".txt"))
        #lru3list.append(getAverageHopCountFromFile("./data/randomcache3P"+str(i)+".txt"))
        #lru5list.append(getAverageHopCountFromFile("./data/randomcache5P"+str(i)+".txt"))

    #print lru1list
    #print lru3list
    #print lru5list
def plotHopInfoFromRandomTest(outputFile):
    f = open(outputFile, 'r')

    for line in f:
        ratio = line.split(",")[3]
        averageOriginalHop = line.split(",")[2]

def getHopInfoFromLFUTest(outputFile):
    lru1list = []
    lru3list = []
    lru5list = [] 

    f = open(outputFile, 'w')


    for i in range(0,trials):
        o = getAverageHopCountFromFile("./data/lfucache1P"+str(i)+".txt")
        f.write(str(o).strip('[]')+ "\n")
 
        #lru1list.append(getAverageHopCountFromFile("./data/lfucache1P"+str(i)+".txt"))
        #lru3list.append(getAverageHopCountFromFile("./data/lfucache3P"+str(i)+".txt"))
        #lru5list.append(getAverageHopCountFromFile("./data/lrucache5P"+str(i)+".txt"))

    print lru1list
    #print lru3list
    #print lru5list

#getHopInfoFromLFUTest("./data/lfuOutput.txt")
#getHopInfoFromRandomTest("./data/randomOutput.txt")



def readProducerFile(fileName):
    f = open(fileName, 'r')

    totalProducers = 0
    for line in f:
        s = line.split('node(0) respodning with Data')
        if (len(s)>1):
            totalProducers += 1

    print totalProducers

#getAverageHopCountFromFile('nocache.txt')
#getAverageHopCountFromFile('randomcache.txt')
#getAverageHopCountFromFile('lrucache.txt')
#readProducerFile('lrproducer.txt')
