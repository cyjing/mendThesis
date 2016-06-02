import math


def createJson(fileName, nodesPerRing, layers, edgeLeavesPerRing):
    linksList = []
    f = open(fileName, 'w')
    f.write("{ \n")

    finalLayer = nodesPerRing**layers
    totalInnerLayers = sum([nodesPerRing**x for x in range(layers)]) 

    totalNodes = totalInnerLayers*nodesPerRing + finalLayer * edgeLeavesPerRing

    print finalLayer, totalInnerLayers, totalNodes

    f.write("  \"nodes\":[\n")

    for i in range(totalNodes-1):
        f.write("    {\"name\":\""+ str(i) +"\"},\n")
    f.write("    {\"name\":\"" + str(totalNodes-1) + "\"}\n")
    f.write("  ],\n")
    f.write("  \"links\":[\n")

    # connect root ring
    for k in range(nodesPerRing - 1):
        f.write("    {\"source\":" + str(k) + ",\"target\":" + str(k + 1)+ ",\"value\":1},\n")
        linksList.append([k, k + 1])
    f.write("    {\"source\":" + str(nodesPerRing - 1) + ",\"target\":" + str(0) + ",\"value\":1},\n")
    linksList.append([nodesPerRing - 1, 0])

    startTransportNodes = totalNodes - finalLayer*edgeLeavesPerRing

    # connect each layer
    transportNodeCount = 0
    for i in range(1, layers):
        totalInnerNodes = sum([nodesPerRing**x for x in range(i)])*nodesPerRing
        nodesThisLayer = nodesPerRing**(i + 1)

        for j in range(nodesThisLayer/nodesPerRing):
            # connect each ring
            for k in range(nodesPerRing - 1):
                f.write("    {\"source\":" + str(totalInnerNodes + j * nodesPerRing + k ) + ",\"target\":" + str(totalInnerNodes + j * nodesPerRing + k + 1)+ ",\"value\":1},\n")
                linksList.append([totalInnerNodes + j * nodesPerRing + k, totalInnerNodes + j * nodesPerRing + k + 1])
            # connect final node to make ring
            f.write("    {\"source\":" + str(totalInnerNodes + j * nodesPerRing + nodesPerRing - 1) + ",\"target\":" + str(totalInnerNodes + j * nodesPerRing)+ ",\"value\":1},\n")
            linksList.append([totalInnerNodes + j * nodesPerRing + nodesPerRing - 1, totalInnerNodes + j * nodesPerRing])

            #connect ring to parent 
            f.write("    {\"source\":" + str(totalInnerNodes + j * nodesPerRing) + ",\"target\":" + str(totalInnerNodes - j - 1)+ ",\"value\":1},\n")
            linksList.append([totalInnerNodes + j * nodesPerRing, totalInnerNodes - j - 1])
            transportNodeCount += 1

    # connect the edges
    startFinalLayer = totalNodes - finalLayer*edgeLeavesPerRing

    print startFinalLayer

    for i in range(1, finalLayer):
        for j in range(edgeLeavesPerRing):
            f.write("    {\"source\":" + str(totalNodes - i*edgeLeavesPerRing- j - 1) + ",\"target\":" + str(startFinalLayer - i -1)+ ",\"value\":1},\n")
            linksList.append([totalNodes - i*edgeLeavesPerRing- j - 1, startFinalLayer  - i -1])
    for j in range(1, edgeLeavesPerRing):
        f.write("    {\"source\":" + str(totalNodes- j - 1) + ",\"target\":" + str(startFinalLayer  -1)+ ",\"value\":1},\n")
        linksList.append([totalNodes - j - 1, startFinalLayer  -1])

    f.write("    {\"source\":" + str(totalNodes- 1) + ",\"target\":" + str(startFinalLayer -1) + ",\"value\":1}\n")
    linksList.append([totalNodes - 1, startFinalLayer  -1])

    f.write("  ]\n")
    f.write("}")


    print "total nodes: " + str(totalNodes)
    return linksList, finalLayer * edgeLeavesPerRing
  
def readRawLocation(rawLocationFileName, topologyFinalFileName, myLinks, edgeTopologyFileName,edges):
    f = open(rawLocationFileName, 'r')
    xShift = 1000
    yShift = 1000
    maxX = 0
    maxY = 0

    scaleX = 300
    scaleY = 300

    totalNodes = 0

    for line in f:
        x = float(f.next())
        y = float(f.next())
        if x < xShift:
            xShift = x
        if y < yShift:
            yShift = y
        if x > maxX:
            maxX = x
        if y > maxY:
            maxY = y
        totalNodes += 1

    f.close()

    f3 = open(topologyFinalFileName, 'w')
    f3.write("router\n")

    f2 = open(fileName, 'r')

    for line in f2:
        x = (float(f2.next()) - xShift)*scaleX/maxX
        y = (float(f2.next()) - yShift)*scaleY/maxY

        f3.write("Node"+str(int(line))+"   NA   "+ str(int(x)) +"    "+ str(int(y)) + "\n")
    f3.write("link\n")

    print len(myLinks)
    for link in myLinks:
        f3.write("Node"+str(link[0])+"       Node"+str(link[1])+"       1Mbps       1       10ms    10\n")
        f3.write("Node"+str(link[1])+"       Node"+str(link[0])+"       1Mbps       1       10ms    10\n")

    f4 = open(edgeTopologyFileName, 'w')
    for i in range(totalNodes-1, totalNodes-edges -1, -1):
        f4.write(str(i) + "\n")

myLinks, edges = createJson("starRingTest3ring.json", 6, 3, 1)
readRawLocation("starRinglocation6ring3Layer1edgeRawTransitNode.txt", "ringStarTopo6Ring3LayerFinalTransitNode.txt", myLinks, "ringStarTopoFinalEdgesTransitNode.txt", edges)
