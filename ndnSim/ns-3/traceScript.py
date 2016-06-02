import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

import subprocess
import random
import numpy as np
from scipy.optimize import curve_fit
from mpl_toolkits.mplot3d import Axes3D

import math
from math import trunc

from datetime import datetime
random.seed(datetime.now())


colors = [(2,63,165),(125,135,185),(190,193,212),(214,188,192),(187,119,132),
(142,6,59),(74,111,227),(133,149,225),(181,187,227),(230,175,185),(224,123,145),
(211,63,106),(17,198,56),(141,213,147),(198,222,199),(234,211,198),(240,185,141),
(239,151,8),(15,207,192),(156,222,214),(213,234,231),(243,225,235),(246,196,225),
(247,156,212)]

plotColors = [(i[0]/float(255),i[1]/float(255),i[2]/float(255)) for i in colors]



def getZipfHopCountRatio(fileName, numberTotalZipf):
    f = open(fileName, 'r')

    averageHop = 0
    totalPackets = 0

    dataHop = {}
    for j in range(numberTotalZipf):
        dataHop[j] = []
        for i in range(0,40):
            dataHop[j].append([0,0])


    for line in f:
        hopSplit = line.split("Hop count: ")

        if len(hopSplit) > 1:
            h = f.next()
            hopLong = int(hopSplit[1].split("\n")[0])
            while (len(h.split("Timeouts ")) == 1):
                data = h.split("Data Hop count: ")[1].split("\n")[0].split(" ")

                dataName = int(data[0]) - 1
                if dataName < numberTotalZipf:
                    dataHop[dataName][hopLong][0] += float(data[1]) * int(data[2])
                    dataHop[dataName][hopLong][1] += int(data[2])
                    averageHop += hopLong * int(data[2])
                    totalPackets += int(data[2])
                h = f.next()
    #print fileName, totalPackets, dataHop/totalPackets, dataHop/longHop
    #return [fileName, totalPackets, longHop/totalPackets, dataHop/longHop]
    return dataHop, averageHop, totalPackets


def getZipfHopInfo(type, trials, dataFilePercents):
    for csize in range(5,6,2):
        totalPackets = 0
        r = csize/float(100)
        fig = plt.figure()
        totalData = 10000/csize

        averageHop = 0
        totalPackets = 0

        dataHop = {}
        for j in range(totalData):
            dataHop[j] = []
            for i in range(0,40):
                dataHop[j].append([0,0])


        for j in range(0, trials):
            data = getZipfHopCountRatio("./data/" +type+ "1P"+str(j)+"AD"+str(0)+"CS"+str(100)+"loc00.txt", totalData)
            for i in range(0,totalData):
                for j in range(0,40):
                    dataHop[i][j][0] += data[0][i][j][0]
                    dataHop[i][j][1] += data[0][i][j][1]
            averageHop += data[1]
            totalPackets += data[2]


        count = 0
        handle_patch = []


        dataPercents = [0] * totalData
        weightedDataHop = []
        for i in range(0,40):
            weightedDataHop.append([0,0])


        for i in range(totalData):
            dataFilePackets = 0
            for j in range(0,40):
                if dataHop[i][j][1] > 0:
                    dataFilePackets += dataHop[i][j][1]
                    weightedDataHop[j][0] +=  dataHop[i][j][0]
                    weightedDataHop[j][1] +=  dataHop[i][j][1]
            dataPercents[i] = dataFilePackets / float(totalPackets)

        topThirty = []
        for i in range(0,40):
            topThirty.append([0,0])

        datapCount = 0.0
        for i in range(totalData):
            if datapCount < 0.3:
                for j in range(0,40):
                    if dataHop[i][j][1] > 0:
                        topThirty[j][0] +=  dataHop[i][j][0]
                        topThirty[j][1] +=  dataHop[i][j][1]
                datapCount += dataPercents[i]
            else:
                print i
                break


        for percent in dataFilePercents:   
            plotDataX = []
            plotDataY = []
            minKey = dataPercents.index(min(dataPercents, key=lambda x:abs(x-percent)))
            dataFilePackets = 0
            for i in range(0,40):
                if dataHop[minKey][i][1] > 0:
                    dataFilePackets += dataHop[minKey][i][1]
                    plotDataX.append(i)
                    averageHops= dataHop[minKey][i][0]/float(dataHop[minKey][i][1])
                    plotDataY.append(averageHops)

            y =  np.array([y1 for (x1,y1) in sorted(zip(plotDataX,plotDataY))])
            plotDataX.sort()
            x =  np.array(plotDataX)     

            #popt, pcov = curve_fit(func, x, y)

            plt.plot(plotDataX,plotDataY, 'ro', c=plotColors[count])
            #plt.plot(x, func(x, *popt), c=plotColors[count],  linewidth=3) #same as line above \/
            alpha = float(dataFilePackets)/ float(totalPackets)
            alpha = trunc(alpha*10000)/10000.0 
            handle_patch.append(mpatches.Patch(color=plotColors[count], label=str(alpha)))

            count += 1

        plotDataTotalX = []
        plotDataTotalY = [] 
        for i in range(0,40):
            if weightedDataHop[i][1] > 0:
                plotDataTotalX.append(i)
                averageHops = weightedDataHop[i][0]/float(weightedDataHop[i][1])
                plotDataTotalY.append(averageHops)

        plotDataThirtyX = []
        plotDataThirtyY = [] 
        for i in range(0,40):
            if topThirty[i][1] > 0:
                plotDataThirtyX.append(i)
                averageHops = topThirty[i][0]/float(topThirty[i][1])
                plotDataThirtyY.append(averageHops)
        
        handle_patch.append(mpatches.Patch(color=(0,0,0), label="overall"))
        plt.plot(plotDataTotalX,plotDataTotalY, 'ro', c=(0,0,0))

        handle_patch.append(mpatches.Patch(color=(1,0,0), label="top 30%"))
        plt.plot(plotDataThirtyX,plotDataThirtyY, 'ro', c=(1,0,0))


        plt.legend(handles=handle_patch, prop={'size':10}, loc='upper center', bbox_to_anchor=(0.5, 1.05),
              ncol=3, fancybox=True, shadow=True)
        plt.axis([0, 15, 0, 15])
        fig.savefig("StarRing05CachRatioloc00lfu" + str(r) + ".jpg")
                
slicesPercent = [.02, .01, .005, .002, .001]


def testAverageHopLFUCacheRatioStarRingTopo(trials, start, stop, step):
    for ratio in range(start,stop,step):
        r = float(ratio)/float(100)
        for i in range(0,trials):
            a = subprocess.Popen("./waf --run='averageDistanceStarRing --cRatio="+ str(r) +" --topo=ringStarTopo6Ring3LayerFinal.txt --cSize="+str(100) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=f' > ./data/starRinglfucacheRatio1P"+str(i)+ "AD" + str(0)+ "CS" + str(r) +".txt 2>&1", shell=True)
            a.wait()
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+" --cType=f' > ./data/lfucache3P"+str(i)+".txt 2>&1", shell=True)
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+" --cType=f' > ./data/lfucache5P"+str(i)+".txt 2>&1", shell=True)
    return

#testAverageHopLFUCacheRatioStarRingTopo(10,5,20,5)
getZipfHopInfo("trace/lfucacheRatio", 10, slicesPercent)

def getZipfCacheHitMissSingle(fileName, totalData, dataFileName=""):
    maxHops = 40
    totalPackets = 0
    cacheInfo = {}
    for j in range(totalData):
        cacheInfo[j] = []
        for i in range(0,400):
            cacheInfo[j].append([0,0])

    try: 
        f = open(fileName, 'r')
    except IOError:
        print fileName
        return cacheInfo

    f.next()

    for line in f:
        hopSplit = line.split("\t")
        if len(hopSplit) > 1:
            h = f.next()
            dataName = int(hopSplit[3].split("\n")[0]) - 1
            node = int(hopSplit[1])
            time = int(hopSplit[0])
            if time == 30:
                cacheDataType = h.split("\t")[2]
                cacheData = int(h.split("\t")[3].split("\n")[0])
                if cacheDataType == "CacheHits":
                    cacheInfo[dataName][node][0] += cacheData
                else:
                    cacheInfo[dataName][node][1] += cacheData
            #h = f.next()

    #print cacheInfo[0][0]
    return cacheInfo


def getTotalRequests(trials, totalData):
    totalPackets = 0
    cacheInfo = [0] * totalData
    totalNodes = 400

    for i in range(1, trials):
        for j in range(totalNodes):
            fileName = "./trace/" + str(i)+"/"+str(j)+".txt"
            try: 
                f = open(fileName, 'r')
            except IOError:
                print fileName
                return cacheInfo


            for line in f:
                split = line.split("    ")
                req = int(split[1][1:]) - 1
                cacheInfo[req] += 1
                totalPackets += 1

    return cacheInfo, totalPackets



def getCacheKeys(type, dataFilePercents, totalData, trials, csize):
    r = csize/float(100)
    r = format(r, '.6f')

    nodeNum = 400

    cacheInfo = {}
    for j in range(totalData):
        cacheInfo[j] = []
        for i in range(0,nodeNum):
            cacheInfo[j].append([0,0])


    for j in range(0, trials):
        #data = getZipfCacheHitMissSingle("./data/" +type+ "maxGrid20cSize"+str(r)+"runNum"+str(j), totalData)
        data = getZipfCacheHitMissSingle("./data/" +type+ "loc06maxGrid20cSize"+str(csize)+"runNum"+str(j), totalData)
        
        for i in range(0,nodeNum):
            for dataFile in range(totalData):
                if data[dataFile][i][0] > 0:
                    cacheInfo[dataFile][i][0] += data[dataFile][i][0]
                if data[dataFile][i][1] > 0:
                    cacheInfo[dataFile][i][1] += data[dataFile][i][1]
    
    cacheHit = [0] * totalData
    cacheMiss = [0] * totalData

    for i in range(totalData):
        for j in range(0,nodeNum):
            cacheHit[i] += cacheInfo[i][j][0]
            cacheMiss[i] += cacheInfo[i][j][1]

    cacheHitPercent = [0] * totalData
    for i in range(totalData):
        if (cacheHit[i] + cacheMiss[i]) > 0:
            cacheHitPercent[i] = cacheHit[i]/float(cacheHit[i] + cacheMiss[i])

    cacheKeys = []

    for percent in dataFilePercents:   
        plotDataX = []
        plotDataY = []
        minKey = cacheHitPercent.index(min(cacheHitPercent, key=lambda x:abs(x-percent)))
        cacheKeys.append((minKey,cacheHitPercent[minKey]))

    return cacheKeys, cacheInfo, cacheHitPercent

def graphCacheInfo (type, dataFilePercents, trials, loc="0"):
    numPlot = len(dataFilePercents)
    fig = plt.figure()

    count = 0
    handle_patch = []
    for csize in [5]:#range(5,30,10):
        totalData = 10000 / csize
        #totalData = 2000
        csizeActual = csize * 20
        cacheDataKey, cacheInfo, cacheHitPercent =  getCacheKeys(type, dataFilePercents, totalData, trials, csize)
        r = csize/float(100)
        print csize,totalData

        dataHop = {}
        totalPackets = 0

        plotDataX = []
        plotDataY = []

        index = 0
        for percent in cacheHitPercent:
            plotDataX.append(index)
            plotDataY.append(percent)
            index += 1
        
        plt.plot(plotDataX,plotDataY, 'ro', c=plotColors[count])

        alpha = csize
        handle_patch.append(mpatches.Patch(color=plotColors[count], label=str(alpha)+"%"))

        count += 1
    
    plt.legend(handles=handle_patch, prop={'size':10}, loc='center right',
          ncol=2, fancybox=True, shadow=True)

    plt.axis([0.0, 2000, 0, 1])
    #plt.show()
    #print r
    fig.savefig(type+ "byIndexcacheHit" + ".jpg")

def graphCacheInfo2 (type, dataFilePercents, trials, loc):
    numPlot = len(dataFilePercents)
    fig = plt.figure()

    count = 0
    handle_patch = []
    for csize in [5,10]:#range(5,30,10):
        #totalData = 10000 / csize
        totalData = 2000
        csizeActual = csize * 20
        cacheDataKey, cacheInfo, cacheHitPercent =  getCacheKeys(type, dataFilePercents, totalData, trials, csizeActual)
        r = csize/float(100)
        print csize,totalData

        dataHop = [0] * totalData
        totalPackets = 0

        plotDataX = []
        plotDataY = []

        for j in range(0, trials):
            data = getZipfHopCountRatio("./data/" +type+"cacheRatio1P"+str(j)+"AD0CS"+str(csizeActual)+ "loc"+ loc+ ".txt", totalData)
            for i in range(0,totalData):
                for j in range(0,40):
                    dataHop[i] += data[0][i][j][1]
            totalPackets += data[2]


        print sum(dataHop), totalPackets
        # CDF
        hops = 0
        maxHops = 0   

        prevKey = 0
        cdfPackets = 0

        sortedDataHopCacheHit =  [(cacheHitPercent[i], i) for i in range(len(cacheHitPercent))]
        sortedDataHopCacheHit.sort()


        for percent in dataFilePercents:
            while prevKey < totalData and sortedDataHopCacheHit[prevKey][0] <= percent:
                index = sortedDataHopCacheHit[prevKey][1]
                cdfPackets+= dataHop[index]
                prevKey += 1
            plotDataX.append(percent)
            plotDataY.append(cdfPackets/float(totalPackets))
            #popt, pcov = curve_fit(func, x, y)

        
        plt.plot(plotDataX,plotDataY, 'ro', c=plotColors[count])

        alpha = csize

        handle_patch.append(mpatches.Patch(color=plotColors[count], label=str(alpha)+"%"))

        count += 1
    plt.legend(handles=handle_patch, prop={'size':10}, title='Cache Budget', loc='lower right',
          ncol=2, fancybox=True, shadow=True)

    plt.axis([0.0, 1.01, 0, 1.01])
    plt.title(type.upper())
    plt.xlabel('Cache Hit Ratio')
    plt.ylabel('CDF')
    #plt.show()
    #print r
    fig.savefig(type+ "loc"+ str(loc)+ "cacheHitReq2" + ".jpg")
dis = [i/float(100) for i in range(1,101,1) ]

def testAverageHopLRUCache(trials, start, stop, step, csize):
    for ratio in range(start, stop, step):
        r = float(ratio)/float(100)
        for i in range(0,trials):
            a = subprocess.Popen("./waf --run='averageDistance --xSize=20 --ySize=20 --cRatio="+ str(r) +" --cSize="+str(100) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=l' > ./data/lrucache1P"+str(i)+ "AD" + str(0)+ "CS" + str(r) +".txt 2>&1", shell=True)
            a.wait()
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=3 --RngRun="+str(i)+" --cType=f' > ./data/lfucache3P"+str(i)+".txt 2>&1", shell=True)
            #subprocess.Popen("NS_LOG=ndn.Consumer ./waf --run='baseline --p=5 --RngRun="+str(i)+" --cType=f' > ./data/lfucache5P"+str(i)+".txt 2>&1", shell=True)
    return


#testAverageHopLRUCache(10, 5, 25, 5, 100)
#graphCacheInfo2("trace/lru", dis, 10, "06")
#graphCacheInfo2("trace/lfu", dis, 10, "06")
#graphCacheInfo("lru", dis, 10)
#graphCacheInfo("lfu", dis, 10)
#graphCacheInfo("random", dis, 10)
#graphCacheInfo2("random", dis, 10, "00")
#graphCacheInfo2("lfu", dis, 10, "00")
#graphCacheInfo2("lru", dis, 10, "00")
