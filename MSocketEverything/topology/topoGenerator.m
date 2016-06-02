function [ asID, asLinks, asPrefix ] = topoGenerator( method, asNum )
% this function generates topology files (topology file containing links,
% prefix file) based on different methods
% method=0: extract the top asNum ASes from DIMES dataset based on degree
% method=1: extract the top asNum ASes from DIMES dataset based on AS size
% (prefix)
% method=2: generate a synthetic AS-level topology based on jellyfish model
% method=3: geographical location model based synthetic topology

method=str2num(method);
asNum=str2num(asNum);

if(method==0)
    [ asID, asLinks, asPrefix ] = degreeTopoGenerator( asNum );
    
elseif(method==1)
    [ asID, asLinks, asPrefix ] = sizeTopoGenerator( asNum );
    
elseif(method==2)
    [ asID, asLinks, asPrefix ] = jellyfishTopoGenerator( asNum );

elseif(method==3)
    [ asID, asLinks, asPrefix ] = locTopoGenerator( asNum );
end


%asID mapping: map asID to a continuous integer space starting from 1. For
%orbit grid evaluation purpose
for i=1:length(asLinks)
    asLinks(i,1)=find(asID==asLinks(i,1));
    asLinks(i,2)=find(asID==asLinks(i,2));
end
for i=1:length(asPrefix)
    asPrefix(i,1)=find(asID==asPrefix(i,1));
end

%print out the links for visualization (halfviz)
link_fid = fopen('link.data', 'wt');
for i=1:length(asLinks)
    fprintf(link_fid, '%d -- %d\n', asLinks(i,1), asLinks(i,2));
end

%generate two files for orbit evaluation: topology.data and prefix.data
topo_fid = fopen('topology.data','wt');
if method==2
    for i=1:length(asLinks)
        fprintf(topo_fid, '%d %d %d\n', asLinks(i,1), asLinks(i,2), asLinks(i,3));
    end
else
    for i=1:length(asLinks)
        fprintf(topo_fid, '%d %d %d\n', asLinks(i,1), asLinks(i,2), round(asLinks(i,3)/1000));
    end
end
prefix_fid = fopen('prefix.data','wt');
for i=1:length(asPrefix)
    fprintf(prefix_fid, '%d.%d.%d.%d/%d %d\n', asPrefix(i,2), asPrefix(i,3), asPrefix(i,4), asPrefix(i,5), asPrefix(i,6), asPrefix(i,1));
end

save('datatraceInput.mat','asLinks', 'asPrefix');
    
fclose(link_fid);
fclose(topo_fid);
fclose(prefix_fid);
end
