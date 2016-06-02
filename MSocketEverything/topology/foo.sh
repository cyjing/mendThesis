#!/usr/bin/octave -qf

arg_list = argv ();
method = arg_list{1};
numAs = arg_list{2};
source('degreeTopoGenerator.m')
source('jellyfishTopoGenerator.m')
source('sizeTopoGenerator.m')
source('topoGenerator.m')

topoGenerator(method,numAs)
