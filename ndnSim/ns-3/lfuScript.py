import matplotlib.pyplot as plt
import subprocess
import random
import numpy as np
from scipy.optimize import curve_fit
import math
from mpl_toolkits.mplot3d import Axes3D


from datetime import datetime
random.seed(datetime.now())

def testAverageHopLFUCache(trials):
    for csize in range(10,30,10):
        for i in range(0,trials):
            a = subprocess.Popen("./waf --run='averageDistance --cSize="+str(csize) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=f' > ./data/lfucache1P"+str(i)+ "AD" + str(0)+ "CS" + str(csize) +".txt 2>&1", shell=True)
            a.wait()
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+" --cType=f' > ./data/lfucache3P"+str(i)+".txt 2>&1", shell=True)
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+" --cType=f' > ./data/lfucache5P"+str(i)+".txt 2>&1", shell=True)
    return



def testAverageHopLFUAverageDistance(trials, minDistance, start, stop, step, cSize):
    offset = 10
    for ratio in range(start,stop,step):
        for i in range(0 + offset,trials + offset):
            r = float(ratio)/float(100)
            a = subprocess.Popen("./waf --run='averageDistance --xSize=20 --ySize=20 --cRatio="+ str(r) +" --cSize="+str(cSize) + " --d="+ str(minDistance) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=f' > ./data/lfucacheRatio1P"+str(i)+ "AD" + str(minDistance)+ "CS" + str(r) +".txt 2>&1", shell=True)
            a.wait()
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+" --cType=f' > ./data/lfucache3P"+str(i)+".txt 2>&1", shell=True)
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+" --cType=f' > ./data/lfucache5P"+str(i)+".txt 2>&1", shell=True)
    return

# def testAverageHopLFUDis(trials):
#     testAverageHopLFUAverageDistance(trials, 0, 2, 10, 2, 100)
#     #testAverageHopLFUAverageDistance(trials, 0, 30, 100, 10, 300)
#     return

#testAverageHopLFUDis(10)

def testAverageHopLFUCacheRatio(trials, start, stop, step):
    for ratio in range(start,stop,step):
        r = float(ratio)/float(100)
        for i in range(0,trials):
            a = subprocess.Popen("./waf --run='averageDistance --xSize=20 --ySize=20 --cRatio="+ str(r) +" --cSize="+str(100) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=f' > ./data/lfucacheRatio1P"+str(i)+ "AD" + str(0)+ "CS" + str(r) +".txt 2>&1", shell=True)
            a.wait()
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+" --cType=f' > ./data/lfucache3P"+str(i)+".txt 2>&1", shell=True)
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+" --cType=f' > ./data/lfucache5P"+str(i)+".txt 2>&1", shell=True)
    return

def testAverageHopLFUCacheRatioStarRingTopo(trials, start, stop, step):
    for ratio in range(start,stop,step):
        r = float(ratio)/float(100)
        for i in range(0,trials):
            a = subprocess.Popen("./waf --run='averageDistanceStarRing --cRatio="+ str(r) +" --topo=ringStarTopo6Ring3LayerFinal.txt --cSize="+str(100) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=f' > ./data/starRinglfucacheRatio1P"+str(i)+ "AD" + str(0)+ "CS" + str(r) +".txt 2>&1", shell=True)
            a.wait()
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+" --cType=f' > ./data/lfucache3P"+str(i)+".txt 2>&1", shell=True)
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+" --cType=f' > ./data/lfucache5P"+str(i)+".txt 2>&1", shell=True)
    return

def testAverageHopLFUCache2(trials, start, stop, step, csize):
    for ratio in range(start, stop, step):
        r = float(ratio)/float(100)
        for i in range(0,trials):
            a = subprocess.Popen("./waf --run='averageDistance --xSize=20 --ySize=20 --cRatio="+ str(r) +" --cSize="+str(100) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=f' > ./data/lfucache1P"+str(i)+ "AD" + str(0)+ "CS" + str(r) +"55.txt 2>&1", shell=True)
            a.wait()
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+" --cType=f' > ./data/lfucache3P"+str(i)+".txt 2>&1", shell=True)
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+" --cType=f' > ./data/lfucache5P"+str(i)+".txt 2>&1", shell=True)
    return

def testAverageHopRandomCache(trials, start, stop, step, csize):
    for ratio in range(start, stop, step):
        r = float(ratio)/float(100)
        for i in range(0,trials):
            a = subprocess.Popen("./waf --run='averageDistance --xSize=20 --ySize=20 --cRatio="+ str(r) +" --cSize="+str(100) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=r' > ./data/randomcache1P"+str(i)+ "AD" + str(0)+ "CS" + str(r) +"55.txt 2>&1", shell=True)
            a.wait()
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+" --cType=f' > ./data/lfucache3P"+str(i)+".txt 2>&1", shell=True)
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+" --cType=f' > ./data/lfucache5P"+str(i)+".txt 2>&1", shell=True)
    return

def testAverageHopLRUCache(trials, start, stop, step, csize):
    for ratio in range(start, stop, step):
        r = float(ratio)/float(100)
        for i in range(0,trials):
            a = subprocess.Popen("./waf --run='averageDistance --xSize=20 --ySize=20 --cRatio="+ str(r) +" --cSize="+str(100) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=l' > ./data/lrucache1P"+str(i)+ "AD" + str(0)+ "CS" + str(r) +"55.txt 2>&1", shell=True)
            a.wait()
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+" --cType=f' > ./data/lfucache3P"+str(i)+".txt 2>&1", shell=True)
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+" --cType=f' > ./data/lfucache5P"+str(i)+".txt 2>&1", shell=True)
    return
#testAverageHopLFUCache(100)
testAverageHopLFUCache2(10, 5, 25, 5, 100)
testAverageHopRandomCache(10, 5, 25, 5, 100)
testAverageHopLRUCache(10, 5, 25, 5, 100)


#CurrentGrid is at 10x10, need to change outputFile name
#testAverageHopLFUCacheRatio(5, 5, 30, 5)
#testAverageHopLFUCacheRatio(5, 30, 100, 20)


#testAverageHopLFUCacheRatioStarRingTopo(10,5,30,5)
#testAverageHopLFUCacheRatioStarRingTopo(10,30,100,20)
#testAverageHopLFUCacheRatioStarRingTopo(5,40,100,20)

def getAverageCacheHits(fileName):
    f = open(fileName, 'r')

    totalHits = 0.0
    totalMisses = 0.0

    for line in f:
        hits = line.split("CacheHits\t")
        misses = line.split("CacheMisses\t")

        if len(hits) > 1:
            totalHits += int(hits[1].split("\n")[0])
        elif len(misses) > 1:
            totalMisses += int(misses[1].split("\n")[0])
    #print fileName, totalPackets, dataHop/totalPackets, dataHop/longHop

    if totalMisses > 0:
       print totalHits/float((totalMisses + totalHits))


def getCacheInfoFromAvgDist(type, trials):
    data = getAverageCacheHits("./data/"+type+"cSize"+str(10)+"runNum"+str(0))
    data = getAverageCacheHits("./data/"+type+"cSize"+str(30)+"runNum"+str(0))

    for csize in range(20,100,20):
        plotDataX = []
        plotDataY = []
        avgTimeout = 0
        totalPackets = 0
        for j in range(0, trials):
            data = getAverageCacheHits("./data/"+type+"cSize"+str(csize)+"runNum"+str(j))
    print "___"
        #     for i in range(0,40):
        #         if (data[0][i] != 0):
        #             plotDataY.append(data[0][i])
        #             plotDataX.append(i)
        #     avgTimeout += data[1]
        #     totalPackets += data[2]
        # print avgTimeout/10, totalPackets/10
        # plt.plot(plotDataX,plotDataY, 'ro')
        # plt.axis([0, 40, 0, 40])
        # plt.show()

#getCacheInfoFromAvgDist("lfu", 1)
#getCacheInfoFromAvgDist("random", 1)




def getAverageHopCountFromFileDistance(fileName, numConsumers):
    f = open(fileName, 'r')

    totalPackets = 0.0
    averageHop = 0.0

    longHop = 0.0
    dataHop = 0.0

    distanceList = []
    resultList = []

    maxDistance = 40


    for i in range(0,maxDistance):
        distanceList.append([0,0])
        
    timeoutReq = 0

    for line in f:
        hopSplit = line.split("Hop count: ")

        if len(hopSplit) > 1:
            h = f.next()
            hopLong = int(hopSplit[1].split("\n")[0])
            while (len(h.split("Timeouts ")) == 1):
                data = h.split("Data Hop count: ")[1].split("\n")[0].split(" ")

                distanceList[hopLong][0] += float(data[1]) * float(data[2])
                distanceList[hopLong][1] += int(data[2])
                h = f.next()
            timeoutReq += int(h.split("Timeouts ")[1].split("\n")[0])
    #print fileName, totalPackets, dataHop/totalPackets, dataHop/longHop

    
    for i in range(0,40):
        if (distanceList[i][1] != 0) :
            resultList.append(distanceList[i][0]/distanceList[i][1])
        else:
            resultList.append(0)
    #return [fileName, totalPackets, longHop/totalPackets, dataHop/longHop]
    return [resultList, timeoutReq, totalPackets, longHop]



def getHopInfoFromAvgDist(type, trials):

    for csize in range(10,30,10):
        plotDataX = []
        plotDataY = []
        avgTimeout = 0
        totalPackets = 0

        for j in range(0, trials):
            data = getAverageHopCountFromFileDistance("./data/" +type+ "1P"+str(j)+"AD"+str(0)+"CS"+str(csize)+".txt")
            for i in range(0,40):
                if (data[0][i] != 0):
                    plotDataY.append(data[0][i])
                    plotDataX.append(i)
            avgTimeout += data[1]
        print avgTimeout/trials

        fig = plt.figure()

        plt.plot(plotDataX,plotDataY, 'ro')
        plt.xlabel("Total Hop Distance From Origin")
        plt.ylabel("Total Hops Data Packet Traveled")


        y =  np.array([y1 for (x1,y1) in sorted(zip(plotDataX,plotDataY))])
        plotDataX.sort()
        x =  np.array(plotDataX)

        popt, pcov = curve_fit(func, x, y)
        plt.plot(x, func(x, *popt), label="Fitted Curve") #same as line above \/

        plt.axis([0, 40, 0, max(y) + 1])

        fig.suptitle("Average Hops Traveled for Packets with Router Cache Size of " + str(csize) + " Using " + type)
        fig.savefig("cSize" + str(csize) + type + ".jpg")

#getHopInfoFromAvgDist("2lfucache", 100)
def func(x, a, b):
    return a * x + b

def getHopInfoFromAvgDistLinear(type, trials):

    for csize in range(40,100,20):
        plotDataX = []
        plotDataY = []
        avgTimeout = 0
        totalPackets = 0

        for j in range(0, trials):
            data = getAverageHopCountFromFileDistance("./data/" +type+ "1P"+str(j)+"AD"+str(0)+"CS"+str(csize)+".txt")
            for i in range(0,40):
                if (data[0][i] != 0):
                    plotDataY.append(data[0][i])
                    plotDataX.append(i)
            avgTimeout += data[1]
        print avgTimeout/trials

        fig = plt.figure()

        plt.plot(plotDataX,plotDataY, 'ro')
        plt.xlabel("Total Hop Distance From Origin")
        plt.ylabel("Total Hops Data Packet Traveled")


        y =  np.array([y1 for (x1,y1) in sorted(zip(plotDataX,plotDataY))])
        plotDataX.sort()
        x =  np.array(plotDataX)

        popt, pcov = curve_fit(func, x, y)
        plt.plot(x, func(x, *popt), label="Fitted Curve") #same as line above \/

        plt.axis([0, 40, 0, max(y) + 1])

        fig.suptitle("Average Hops Traveled for Packets with Router Cache Size of " + str(csize) + " Using " + type)
        fig.savefig("cSize" + str(csize) + type + ".jpg")

def getHopInfoFromAvgDistRatio3D(type, trials, numConsumers):

    plotDataX = []
    plotDataY = []
    plotDataZ = []


    for csize in range(2,20,1):
        avgTimeout = 0
        totalPackets = 0

        r = csize/float(100)

        for j in range(0, trials):
            data = getAverageHopCountFromFileDistance("./data/" +type+ "1P"+str(j)+"AD"+str(0)+"CS"+str(r)+".txt", numConsumers)
            for i in range(0,14):
                if (data[0][i] != 0):
                    plotDataY.append(data[0][i])
                    plotDataX.append(i)
                    plotDataZ.append(csize)

    for csize in range(20,100,20):
        avgTimeout = 0
        totalPackets = 0

        r = csize/float(100)

        for j in range(0, trials):
            data = getAverageHopCountFromFileDistance("./data/" +type+ "1P"+str(j)+"AD"+str(0)+"CS"+str(r)+".txt", numConsumers)
            for i in range(0,14):
                if (data[0][i] != 0):
                    plotDataY.append(data[0][i])
                    plotDataX.append(i)
                    plotDataZ.append(csize)
            avgTimeout += data[1]
        print avgTimeout/trials

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter(plotDataX, plotDataZ, plotDataY, c="blue", depthshade=True)

    plotDataX2 = []
    plotDataY2 = []
    plotDataZ2 = []
    numConsumers = 40

    for csize in range(2,30,1):
        avgTimeout = 0
        totalPackets = 0

        r = csize/float(100)

        for j in range(0, trials):
            data = getAverageHopCountFromFileDistance("./data/" +"starRinglfucacheRatio"+ "1P"+str(j)+"AD"+str(0)+"CS"+str(r)+".txt", numConsumers)
            for i in range(0,14):
                if (data[0][i] != 0):
                    plotDataY2.append(data[0][i])
                    plotDataX2.append(i)
                    plotDataZ2.append(csize)

    for csize in range(20,100,20):
        avgTimeout = 0
        totalPackets = 0

        r = csize/float(100)

        for j in range(0, trials):
            data = getAverageHopCountFromFileDistance("./data/" +"starRinglfucacheRatio"+ "1P"+str(j)+"AD"+str(0)+"CS"+str(r)+".txt", numConsumers)
            for i in range(0,14):
                if (data[0][i] != 0):
                    plotDataY2.append(data[0][i])
                    plotDataX2.append(i)
                    plotDataZ2.append(csize)
            avgTimeout += data[1]
        print avgTimeout/trials

    ax.scatter(plotDataX2, plotDataZ2, plotDataY2, c="red", depthshade=True)


    plt.xlabel("Total Hop Distance From Origin")
    plt.ylabel("Cache Size")
    ax.set_zlabel("Total Hops Data Packet Traveled")
    ax.set_zlim(0, 14)
    ax.set_ylim(0, 100)
    ax.set_xlim(0, 14)

    #plt.axis([0, 40, 0, max(plotDataY) + 1, 0, 1])

    fig.suptitle("Average Hops Traveled for Packets with Router Cache Size of " + str(csize) + " Using " + type)
    plt.show()

#getHopInfoFromAvgDistRatio3D("starRinglfucacheRatio", 5)
#getHopInfoFromAvgDistRatio3D("lfucacheRatio", 5, 40)
#getHopInfoFromAvgDistLinear("lfucache", 10)
#getHopInfoFromAvgDistLinear("randomcache", 10)
