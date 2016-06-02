#!/bin/bash
CLICK_SRC=/root/click
MF_SRC=/root/git_repos/mf
rm $CLICK_SRC/elements/local/mf*
rm $CLICK_SRC/elements/local/gnrs*
cp $MF_SRC/router/click/elements/gstar/* \
   $MF_SRC/router/click/elements/gnrs/* \
   $MF_SRC/router/click/elements/app/* \
   $MF_SRC/router/click/elements/test/* \
   $MF_SRC/router/click/elements/utils/* \
   $MF_SRC/common/include/* \
   $CLICK_SRC/elements/local/
cd $CLICK_SRC
./configure --disable-linuxmodule --enable-local --enable-user-multithread 
make elemlist
make
make install
