#!/bin/bash

PATH="$PATH":.

BASE_URL="https://bitbucket.org/romoore/gnrs/downloads/"
WGET="wget --timeout=10 -N"
REAL_SCRIPT="foo.sh"

PREFIX_DATA="ASPrefixData.mat"
TOPO_DATA="topologyData.mat"
SHELL_DATA="shellMem.mat"
HANG_DATA="hangMem.mat"
CLIQUE_DATA="clique.mat"
LINK_PREFIX_DATA="linkPrefixofLOC.mat"

DOWNLOADED=(	"$PREFIX_DATA" "$TOPO_DATA" "$SHELL_DATA" \
		"$HANG_DATA" "$CLIQUE_DATA" "$LINK_PREFIX_DATA");

# Download the required files
for I in "${DOWNLOADED[@]}"; do
	echo "$WGET $BASE_URL$I"
	$WGET $BASE_URL$I || exit 1;
done
# Exec the actual script
$REAL_SCRIPT $1 $2

for I in "${DOWNLOADED[@]}"; do
	rm $I;
done
