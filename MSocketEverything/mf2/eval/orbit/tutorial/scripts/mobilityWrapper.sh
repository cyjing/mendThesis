MOBILITY_HOME=/root/MSocketEverything/msocketSource/msocket1/
export CLASSPATH=$(MOBILITY_HOME)/dist/jars/GNS.jar:$(MOBILITY_HOME)/dist/jars/GNS-CLI.jar:.
java edu.umass.cs.msocket.apps.MSokcetClientMobility $1 $2