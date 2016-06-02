This repo was originally pulled from http://ndnsim.net/2.1/getting-started.html

All the work was done in the ns-3 folder of the repo.


<b>Generating a star ring topology:</b>

We used createStarRingjson.py to generate the nodes and links required for our topology. 

For a valid topology, ndnSim requires a X and Y coordinate value. In order for our generated topology
to have valid sane x/y coordinates for each node, we used the javascript d3 library to create a force 
graph with our nodes and links using the StarRingGraph.html file and passing it the topology raw txt file.
Once you open the html file and wait for the nodes to come to equilibrium, we got the x/y coordinates from calling
the printNodeXY() function from the console.

Once the x/y coordinates are put into a txt file, we run createStarRingjson.py again to generate the final topology 
for ndnSim.

</n>
<Enter>

<b>Generating a trace file for each node:</b>

In the ns-3/scratch folder, there are two files that generate traces for a mesh topo and the star/ring topo:
generateTrace.cc and generateTraceStar.cc 

<Enter>
<Enter>

<b>Running experiments:</b>

In the ns-3/scratch folder, we have all of our experimental runs. An experiment can be run through the termainal, and 
examples of python script running multiple runs and saving the output into the ns-3/data folder can be found in lfuScript.py
