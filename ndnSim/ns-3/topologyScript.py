import random

f = open('myTreeTopology','w')
f.write('router\n') # python will convert \n to os.linesep

f2 = open('myTreeEdges', 'w')

random.seed()
nodes = {}
nodesInv = {}
nodesLink = {}

for i in range(1,201):
	x = random.randint(0, 99)
	y = random.randint(0, 99)
	while((x,y) in nodes.keys()):
		x = random.randint(0, 99)
		y = random.randint(0, 99)
	
	nodes[(x,y)] = i
	nodesInv[i] = (x,y)
	nodesLink[i] = []
	f.write("Node" + str(i-1) + "   NA   "+ str(y) + "    " +str(x) + "\n")


f.write("\n")
f.write("link\n")

edges = []

for i in range(1,201):
	xpos, ypos = nodesInv[i]
	hasChild = False;

	for x in range(-12,13):
		for y in range(-12, 13):
			if (x+xpos,y+ypos) in nodes.keys() and (x!=0 and y!=0): #and nodes[(x+xpos,y+ypos)] > i
				otherNode = nodes[(x+xpos,y+ypos)]
				nodesLink[i].append(otherNode)
				hasChild = True
	if not hasChild:
		edges.append(i);
        else:
                a = random.random()
                if a < 0.1:
                        edges.append(i)
		



for i in range(1, 201):
	for link in nodesLink[i]:
		f.write("Node" + str(i-1) +"       Node" + str(link-1)+"       1Mbps       1       10ms    10\n");

for e in edges:
        f2.write(str(e)+"\n")
f.close() # you can omit in most cases as the destructor will call if
f2.close()
