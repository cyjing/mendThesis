import matplotlib
#matplotlib.use('Agg')

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
        data = getZipfCacheHitMissSingle("./data/" +type+ "maxGrid20cSize"+str(csize)+"runNum"+str(j), totalData)
        
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

def getZipfHopCountRatioSingle(fileName, totalData, dataFileName=""):
    maxHops = 40
    totalPackets = 0

    dataHop = {}
    for j in range(totalData):
        dataHop[j] = []
        for i in range(0,40):
            dataHop[j].append([0,0])

    try: 
        f = open(fileName, 'r')
    except IOError:
        return dataHop,0

    for line in f:
        hopSplit = line.split("Hop count: ")

        if len(hopSplit) > 1:
            h = f.next()
            hopLong = int(hopSplit[1].split("\n")[0])
            while (len(h.split("Timeouts ")) == 1):
                data = h.split("Data Hop count: ")[1].split("\n")[0].split(" ")
                dataName = int(data[0]) - 1
                if dataName < totalData:
                    dataHop[dataName][hopLong][0] += float(data[1]) * int(data[2])
                    dataHop[dataName][hopLong][1] += int(data[2])
                    totalPackets += int(data[2])
                h = f.next()

    return dataHop, totalPackets


def graphCacheInfo (type, dataFilePercents, trials, loc):
    numPlot = len(dataFilePercents)
    fig = plt.figure()

    count = 0
    handle_patch = []
    for csize in [1,5,10]:#range(5,30,10):
        #totalData = 10000 / csize
        totalData = 2000
        csizeActual = csize * 20
        cacheDataKey, cacheInfo, cacheHitPercent =  getCacheKeys(type, dataFilePercents, totalData, trials, csizeActual)
        r = csize/float(100)
        print csize,totalData

        dataHop = {}
        totalPackets = 0

        # for j in range(totalData):
        #     dataHop[j] = []
        #     for i in range(0,40):
        #         dataHop[j].append([0,0])

        # for j in range(0, trials):
        #     data, packets = getZipfHopCountRatioSingle("./data/" +"lrucache"+ "1P"+str(j)+"AD"+str(0)+"CS"+str(r)+".txt", totalData)
        #     totalPackets += packets
        #     for i in range(0,40):
        #         for dataFile in range(totalData):
        #             if data[dataFile][i][1] > 0:
        #                 dataHop[dataFile][i][0] += data[dataFile][i][0]
        #                 dataHop[dataFile][i][1] += data[dataFile][i][1]

        plotDataX = []
        plotDataY = []


        # CDF
        hops = 0
        maxHops = 0   

        prevKey = 0

        sortedDataHopCacheHit =  [(cacheHitPercent[i], i) for i in range(len(cacheHitPercent))]
        sortedDataHopCacheHit.sort()
        #print cacheDataKey
        #print dataHop
        for dataName, percent in cacheDataKey:
            while sortedDataHopCacheHit[prevKey][0] < percent:
                #d = sortedDataHopCacheHit[prevKey][1]
                # for i in range(0,40):
                #     if dataHop[d][i][1] > 0:
                #         hops += dataHop[d][i][0]
                #         maxHops += i * dataHop[d][i][1]
                prevKey += 1
            #if maxHops > 0:
            #print percent
            plotDataX.append(percent)
            plotDataY.append(prevKey/float(len(sortedDataHopCacheHit)))
            #popt, pcov = curve_fit(func, x, y)

        
        plt.plot(plotDataX,plotDataY, 'ro', c=plotColors[count])

        alpha = csize
        handle_patch.append(mpatches.Patch(color=plotColors[count], label=str(alpha)+"%"))

        count += 1
    plt.legend(handles=handle_patch, prop={'size':10}, loc='center right',
          ncol=2, fancybox=True, shadow=True)

    plt.axis([0.0, 1, 0, 1])
    #plt.show()
    #print r
    fig.savefig(type+ "loc"+ str(loc)+ "cacheHit" + ".jpg")


dis = [i/float(100) for i in range(1,100,1) ]
#graphCacheInfo("trace/lru", dis, 10)
#graphCacheInfo("lfu", dis, 10)
#graphCacheInfo("trace/random", dis, 10)


def funcL(x, a, b):
    return a * x + b

def func(x, a, b, c):
    return (1 - (a * np.exp(-b * x)))+ c

def getSingleDataFileInfo(type, dataFileNames, totalData, trials, csize):
    numPlot = len(dataFileNames)
    plotColors = [(1/float(i),1/float((i+numPlot/3)%numPlot + 1),1/float((i+2*numPlot/3)%numPlot + 1)) for i in range(1,len(dataFileNames)+1)]


    #for csize in range(5,50,5):
    totalPackets = 0
    r = csize/float(100)
    fig = plt.figure()
    
    
 
    dataHop = {}
    for j in range(len(dataFileNames)):
        dataHop[dataFileNames[j]] = []
        for i in range(0,40):
            dataHop[dataFileNames[j]].append([0,0])

    for k in range(0,9):
        for j in range(0, trials):
            for dataFileName in dataFileNames:
                data = getZipfHopCountRatioSingle("./data/" +type+ "1P"+str(j)+"AD"+str(k)+"CS"+str(r)+".txt", totalData, dataFileName)
                for i in range(0,40):
                    if data[i][1] > 0:
                        dataHop[dataFileName][i][0] += data[i][0]
                        dataHop[dataFileName][i][1] += data[i][1]
        if k == 0:
            for j in range(5, 15):
                for dataFileName in dataFileNames:
                    data = getZipfHopCountRatioSingle("./data/" +type+ "1P"+str(j)+"AD"+str(k)+"CS"+str(r)+".txt", totalData, dataFileName)
                    for i in range(0,40):
                        if data[i][1] > 0:
                            dataHop[dataFileName][i][0] += data[i][0]
                            dataHop[dataFileName][i][1] += data[i][1]
    
    count = 0
    handle_patch = []
    for dataFileName in dataFileNames:   
        plotDataX = []
        plotDataY = []   
        for i in range(0,40):
            if dataHop[dataFileName][i][1] > 0:
                plotDataX.append(i)
                plotDataY.append((dataHop[dataFileName][i][0]/float(dataHop[dataFileName][i][1]))/float(i))
                

        y =  np.array([y1 for (x1,y1) in sorted(zip(plotDataX,plotDataY))])
        plotDataX.sort()
        x =  np.array(plotDataX)     

        popt, pcov = curve_fit(funcL, x, y)

        #plt.plot(plotDataX,plotDataY, 'ro', c=plotColors[count])
        plt.plot(x, funcL(x, *popt), c=plotColors[count],  linewidth=3) #same as line above \/
        alpha = float(dataFileName)/ float(totalData)
        alpha = trunc(alpha*1000)/1000.0 
        handle_patch.append(mpatches.Patch(color=plotColors[count], label=r"$\alpha$ = " + str(alpha)))

        count += 1

    plt.legend(handles=handle_patch, prop={'size':10}, loc='upper center', bbox_to_anchor=(0.5, 1.05),
          ncol=4, fancybox=True, shadow=True)
    plt.axis([0, 40, 0, 1])
    #plt.show()
    print r
    fig.savefig("LfuAlphaHopCachRatio" + str(r) + ".jpg")

def getSingleDataFileInfoAverageHopSaved(type, dataFilePercents, totalData, trials, csize):
    numPlot = len(dataFilePercents)
    #plotColors = [(i/numPlot, (i*2/numPlot) % 1 ,(i*3/numPlot) % 1) for i in range(0,len(dataFileNames))]
    #plotColors = [(1/float(i),1/float((i+numPlot/3)%numPlot + 1),1/float((i+2*numPlot/3)%numPlot + 1)) for i in range(1,len(dataFileNames)+1)]

    totalPackets = 0
    r = csize/float(100)
    fig = plt.figure()

    dataHop = {}
    print totalData
    for j in range(totalData):
        dataHop[j] = []
        for i in range(0,40):
            dataHop[j].append([0,0])


    for k in range(0,9):
        for j in range(0, trials):
            #
            data, packets = getZipfHopCountRatioSingle("./data/" +type+ "1P"+str(j)+"AD"+str(k)+"CS"+str(r)+".txt", totalData)
            totalPackets += packets
            for i in range(0,40):
                for dataFile in range(totalData):
                    if data[dataFile][i][1] > 0:
                        dataHop[dataFile][i][0] += data[dataFile][i][0]
                        dataHop[dataFile][i][1] += data[dataFile][i][1]
        if k == 0:
            for j in range(5, 15):
                data, packets = getZipfHopCountRatioSingle("./data/" +type+ "1P"+str(j)+"AD"+str(k)+"CS"+str(r)+"55.txt", totalData)
                totalPackets += packets
                for i in range(0,40):
                    for dataFile in range(totalData):
                        if data[dataFile][i][1] > 0:
                            dataHop[dataFile][i][0] += data[dataFile][i][0]
                            dataHop[dataFile][i][1] += data[dataFile][i][1]
    
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
    plt.axis([0, 40, 0, 30])
    plt.show()
    print r
    #fig.savefig(type + "AverageHopSavedCachRatioAlpha552" + str(r) + ".jpg")


#slicesPercent = [.02, .01, .005, .002, .001]
#totalData = 2000
#getSingleDataFileInfoAverageHopSaved("lfucache", slicesPercent, totalData, 10 , 5)
# getSingleDataFileInfoAverageHopSaved("lrucache", slicesPercent, totalData, 10 , 5)
# getSingleDataFileInfoAverageHopSaved("randomcache", slicesPercent, totalData, 10 , 5)
# totalData = 1000
# getSingleDataFileInfoAverageHopSaved("lfucache", slicesPercent, totalData, 10 , 10)
# getSingleDataFileInfoAverageHopSaved("lrucache", slicesPercent, totalData, 10 , 10)
# getSingleDataFileInfoAverageHopSaved("randomcache", slicesPercent, totalData, 10 , 10)
# totalData = 666
# getSingleDataFileInfoAverageHopSaved("lfucache", slicesPercent, totalData, 10 , 15)
# getSingleDataFileInfoAverageHopSaved("lrucache", slicesPercent, totalData, 10 , 15)
# getSingleDataFileInfoAverageHopSaved("randomcache", slicesPercent, totalData, 10 , 15)
# totalData = 500
# getSingleDataFileInfoAverageHopSaved("lfucache", slicesPercent, totalData, 10 , 20)
# getSingleDataFileInfoAverageHopSaved("lrucache", slicesPercent, totalData, 10 , 20)
# getSingleDataFileInfoAverageHopSaved("randomcache", slicesPercent, totalData, 10 , 20)


def getSingleDataFileCacheSize():
    #slices = [.001,.01,.05,.1,.2,.3,.5]
    slicesPercent = [.02, .01, .005, .002, .001]
    totalData = 5000
    #getSingleDataFileInfoAverageHopSaved("lfucacheRatio", slicesPercent, totalData, 10 , 2)
    
    for i in range(5, 20, 5):
        totalData = 100 * 100 / i
        getSingleDataFileInfoAverageHopSaved("lfucache", slicesPercent, totalData, 10 , i)
    
    # for i in range(30, 90, 10):
    #     totalData = 100  * 100 / i
    #     getSingleDataFileInfoAverageHopSaved("lfucacheRatio", slicesPercent, totalData, 5 , i)
	
#getSingleDataFileCacheSize()



def getZipfHopCountRatio(fileName, numberTotalZipf):
    f = open(fileName, 'r')

    zipfEntries = []
    averageHop = 0
    totalPackets = 0

    for i in range(0,numberTotalZipf):
        zipfEntries.append([0,0])


    for line in f:
        hopSplit = line.split("Hop count: ")

        if len(hopSplit) > 1:
            h = f.next()
            hopLong = int(hopSplit[1].split("\n")[0])
            while (len(h.split("Timeouts ")) == 1):
                data = h.split("Data Hop count: ")[1].split("\n")[0].split(" ")

                dataName = int(data[0]) - 1
                if dataName < numberTotalZipf:
                    prevHops = zipfEntries[dataName][1] * zipfEntries[dataName][0]
                    averageRatio = float(data[1]) / float(hopLong) #* (1+hopLong/40.0)
                    newRatio = (prevHops + averageRatio * float(data[2]))/(float(data[2]) + zipfEntries[dataName][1])
                    zipfEntries[dataName][0] = newRatio
                    zipfEntries[dataName][1] += int(data[2])
                    averageHop += hopLong * zipfEntries[dataName][1]
                    totalPackets += zipfEntries[dataName][1]
                h = f.next()
    #print fileName, totalPackets, dataHop/totalPackets, dataHop/longHop

    

    #return [fileName, totalPackets, longHop/totalPackets, dataHop/longHop]
    return zipfEntries, averageHop, totalPackets




def getZipfHopInfo(type, trials):
    totalZipfData = 1000
    plotColors = [(1/float(i),1/float((i+3)%10 + 1),1/float((i+6)%10 + 1)) for i in range(1,11)]

    for csize in range(5,30,10):
        totalPackets = 0
        r = csize/float(100)
        fig = plt.figure()

        for k in range(0,9):
            plotDataX = []
            plotDataY = []
            average = [[0,0] for i in range(10000)]
            averageHop = 0
            totalPackets = 0

            for j in range(0, trials):
                data = getZipfHopCountRatio("./data/" +type+ "1P"+str(j)+"AD"+str(k)+"CS"+str(r)+".txt", totalZipfData)

                for i in range(0,500):
                    if (data[0][i][0] != 0):
                        average[i][0] += float(data[0][i][0])
                        average[i][1] += 1
                averageHop += data[1]
                totalPackets += data[2]

            for i in range(0,500):
                if (average[i][1] != 0):
                    plotDataY.append(average[i][0]/float(average[i][1]))
                    plotDataX.append(i+1)

            y =  np.array([y1 for (x1,y1) in sorted(zip(plotDataX,plotDataY))])
            plotDataX.sort()
            x =  np.array(plotDataX)            
            popt, pcov = curve_fit(func, x, y)


            print averageHop/float(totalPackets)

            #plt.plot(plotDataX,plotDataY, 'ro', c=plotColors[k])
            plt.plot(x, func(x, *popt), c=plotColors[k],  linewidth=3) #same as line above \/

        fig.suptitle("Average Hops Traveled for Packets with Router Cache Size of " + str(csize) + " Using " + type)
        #plt.show()

#getZipfHopInfo("starRinglfucacheRatio", 5)
#getZipfHopInfo("lfucacheRatio", 5)






def createStarTraces(trials, loc):
    for i in range(0,trials):
       a = subprocess.Popen("./waf --run='generateTraceStar --loc=" +str(loc) +" --runNum=" + str(i) + "'", shell=True)
       a.wait()
    return 


def testAverageHopLFUCacheRatioTrace(trials, start, stop, step, loc):
    totalNum = 2000
    for ratio in range(start,stop,step):
        csize = totalNum / 100 * ratio
        r = 1
        for i in range(0,trials):
            a = subprocess.Popen("./waf --run='averageDistanceTrace --cRatio="+ str(r) +" --loc=" + loc + " --cSize="+str(csize) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=f' > ./data/trace/lfucacheRatio1P"+str(i)+ "AD" + str(0)+ "CS" + str(csize) + "loc" + loc +".txt 2>&1", shell=True)
            a.wait()
    return
def testAverageHopLRUCacheRatioTrace(trials, start, stop, step, loc):
    totalNum = 2000
    for ratio in range(start,stop,step):
        csize = totalNum / 100 * ratio
        r = 1
        for i in range(0,trials):
            a = subprocess.Popen("./waf --run='averageDistanceTrace --cRatio="+ str(r) + " --loc=" +loc +" --cSize="+str(csize) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=l' > ./data/trace/lrucacheRatio1P"+str(i)+ "AD" + str(0)+ "CS" + str(csize) + "loc" +loc +".txt 2>&1", shell=True)
            a.wait()
    return
def testAverageHopRandomCacheRatioTrace(trials, start, stop, step, loc):
    totalNum = 2000
    for ratio in range(start,stop,step):
        csize = totalNum / 100 * ratio
        r = 1
        for i in range(0,trials):
            a = subprocess.Popen("./waf --run='averageDistanceTrace --cRatio="+ str(r) +" --cSize="+str(csize) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=r' > ./data/trace/randomcacheRatio1P"+str(i)+ "AD" + str(0)+ "CS" + str(csize) + loc +".txt 2>&1", shell=True)
            a.wait()
    return

def testAverageHopLFUCacheRatioTraceStar(trials, start, stop, step, loc):
    totalNum = 2000
    for ratio in range(start,stop,step):
        csize = totalNum / 100 * ratio
        r = 1
        for i in range(0,trials):
            a = subprocess.Popen("./waf --run='averageDistanceStarRingTrace  --topo=ringStarTopo6Ring3LayerFinal.txt --cRatio="+ str(r) +" --loc=" + loc + " --cSize="+str(csize) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=f' > ./data/trace/lfucacheRatio1P"+str(i)+ "AD" + str(0)+ "CS" + str(csize) + "loc" + loc +".txt 2>&1", shell=True)
            a.wait()
    return
def testAverageHopLRUCacheRatioTraceStar(trials, start, stop, step, loc):
    totalNum = 2000
    for ratio in range(start,stop,step):
        csize = totalNum / 100 * ratio
        r = 1
        for i in range(0,trials):
            a = subprocess.Popen("./waf --run='averageDistanceStarRingTrace --topo=ringStarTopo6Ring3LayerFinal.txt --cRatio="+ str(r) + " --loc=" +loc +" --cSize="+str(csize) + " --d="+ str(0) + " --p=1 --RngRun="+str(i)+" --runNum="+str(i)+" --cType=l' > ./data/trace/lrucacheRatio1P"+str(i)+ "AD" + str(0)+ "CS" + str(csize) + "loc" +loc +".txt 2>&1", shell=True)
            a.wait()
    return

#createTraces(10, 0.0)
#testAverageHopLFUCacheRatioTrace(10, 5,11,5,"00")
#testAverageHopLRUCacheRatioTrace(10, 5,11,5,"00")

#createTraces(10, 0.3)
#testAverageHopLFUCacheRatioTrace(10, 5,11,5, "03")
#testAverageHopLRUCacheRatioTrace(10, 5,11,5, "03")

#createTraces(10, 0.6)
#testAverageHopLFUCacheRatioTrace(10, 5,11,5, "06")
#testAverageHopLRUCacheRatioTrace(10, 5,11,5, "06")





createStarTraces(10, 0.0)
testAverageHopLFUCacheRatioTraceStar(10, 5,6,5,"00")
testAverageHopLRUCacheRatioTraceStar(10, 5,6,5,"00")

createStarTraces(10, 0.3)
testAverageHopLFUCacheRatioTraceStar(10, 5,6,5, "03")
testAverageHopLRUCacheRatioTraceStar(10, 5,6,5, "03")

createStarTraces(10, 0.6)
testAverageHopLFUCacheRatioTraceStar(10, 5,6,5, "06")
testAverageHopLRUCacheRatioTraceStar(10, 5,6,5, "06")
