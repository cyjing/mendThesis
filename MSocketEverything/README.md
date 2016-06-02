This repo combines the one from Umass Amhearst (https://github.com/MobilityFirst/GNS) and Rutgers (ssh://username@external1.orbit-lab.org/common/git/mf), though it has diverged from both since then. The goal of this repo was to integrate the router/forwarding layer of the Rutgers code with the msocket session application of the Umass Amhearst code.

A demo of this can be run through the orbit-lab grid testbed found through Rutgers.

Omf is a ruby tool created by orbit-labs to run their experiments (go to http://www.orbit-lab.org/wiki/Tutorials for more information). 

1) To load an image of a working demo on all the nodes on the grid, run 
omf load -i cyjing-node-node1-1.sb4.orbit-lab.org-2016-02-29-16-47-27.ndz all
   Sometimes, not all the nodes will image due to timeouts or in error in responding. In that case, you should try to image the nodes individually. In the place of "all", just name the specific nodes you want to image, separated by a comma and no spaces.

2) To run the experiment, copy over the cyjingLargeHopChunkSizeExp.rb and nodelist.rb from ./mf2 to the orbit console. Omf looks at only the most recent successfully imaged topology, which is stored in the /tmp folder. For our purposes, we have defined our own topology stored in nodelist.rb. To be able to use it, assuming you have imaged all the required nodes, copy over nodelist.rb to /tmp/(your most recently successfully imaged experiment name). If you weren't able to image all the nodes, you can ignore all the unimaged nodes in the code of cyjingLargeHopChunkSizeExp.rb.
 
   Turn the nodes on: "omf tell -a on -t all"
      run experiment: "omf exec cyjingLargeHopChunkSizeExp.rb"



To run the msocket portion of the code:
Msocket requires that you have mongodb running. If you don't have mongodb, install it from https://www.mongodb.com.
https://gns.name/wiki/index.php?title=Main_Page is a good tutorial

Msocket requires java 8, but the default java version on orbit is java 7. Once you have installed java 8 from 
https://www.java.com/en/download/help/download_options.xml. To point your console to the new java 8 version, run:
"export PATH=~/jdk1.8.0_20/bin:$PATH", or add that to the .bashrc file.


1) To initiate the Auspice GNRS server:
   Auspice requires mongodb. To run it locally, go to ./msocket/ and call: "mongod --dbpath ./data"
   copy over ./msocket/jars to ./msocket/dist/jars   
   run: "./msocket/dist/script/singlenodetest/run-nossl.sh"

2) To initiate local connections to the GNRS server:
   go to each individual node
   cd into ./MSocketEverything/msocket/dist/jars
   run: "java -jar gns-cli-1.16-2015-11-16.jar"
   Once in the program, run:

   "gns_connect 10.10.0.10 24398 true"
   "account_create #email #password"
   "account_verify #email #verifyCode"

   You can do this for one node and copy over the ./derbyDB folder that gets generated to the rest of your nodes

3) In ./dist/jars, run:
   "java -cp GNS.jar:GNS-CLI.jar:. edu.umass.cs.msocket.apps.MSocketServer #GUIDofServer"

4) In another node, run:
   "java -cp GNS.jar:GNS-CLI.jar:. edu.umass.cs.msocket.apps.MSocketClientMoving #GUIDofServer #moveNumber #myNA"

   One example would be to be on node1-1 and run:
   "java -cp GNS.jar:GNS-CLI.jar:. edu.umass.cs.msocket.apps.MSocketClientMoving #GUIDofServer 0 1"

   The file 10003.txt provides the mobility file for the application. It notes the node and the NA associated with that node. The NA is the GUID of the router the node hostStack is attached to. In our topology, some nodes are hostStack nodes and some are router nodes. Only hostStack nodes can run applications.







   

