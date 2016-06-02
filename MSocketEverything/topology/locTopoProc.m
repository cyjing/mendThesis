%generate linkPrefixofLOC.mat by processing the location data
clc;
close;
clear;

locationData = importdata('ASArray.LOC2');

%sort the locationData based on its size: from larger to smaller
[m,n] = size(locationData);
%locInfo: first column is the index, the second column is the number of
%ASes per countries
locInfo = zeros(m,2);
locInfo(:,1) = (1:m)';

for i=1:m
    locInfo(i,2) = length(find(locationData(i,:)));
end
sortLocInfo = sortrows(locInfo, -2);

sortLocData = zeros(m,n);
for i=1:m
    sortLocData(i,:) = locationData(sortLocInfo(i,1),:);
end

% input value asNum: the number of ASes that will be generated
% we will aggregate the whole row in sortLocData into one AS here (one synthetic AS
% per country)
asNum = 200;

topologyData = importdata('topology_intra_adjusted.data');
topoLength = length(topologyData);
asLinks = zeros(0,3);
linkNum = 0;

for i= 1:topoLength
    [AS1,temp] = find(sortLocData(1:asNum,:)==topologyData(i,1));
    [AS2,temp] = find(sortLocData(1:asNum,:)==topologyData(i,2));
    if(isempty(AS1)==0 && isempty(AS2)==0)
        linkNum = linkNum +1;
        asLinks(linkNum,1) = AS1;
        asLinks(linkNum,2) = AS2;
        asLinks(linkNum,3) = topologyData(i,3);
    end
    if mod(i,500) == 0
        i
    end
end


prefixData = csvread('ASNumDataMod-2_1_11.csv');
prefixLength = length(prefixData);
asPrefix = zeros(0,6);
asPrefixNum = 0;

for i = 1:prefixLength
    [AS0,temp] = find(sortLocData(1:asNum,:)==prefixData(i,1));
    if(isempty(AS0)==0)
        asPrefixNum = asPrefixNum + 1;
        asPrefix(asPrefixNum,:) = prefixData(i,:);
        asPrefix(asPrefixNum,1) = AS0;
    end
    if mod(i,1000) == 0
        i
    end
end


%post-process asLinks
for i=1:length(asLinks)
    if(asLinks(i,1) > asLinks(i,2))
        temp = asLinks(i,1);
        asLinks(i,1) = asLinks(i,2);
        asLinks(i,2) = temp;
    end
end

asLinks = sortrows(asLinks, [1 2]);

asLinks_ = zeros(0,3);
asLinks_(1,:) = asLinks(1,:);
linkNum_ = 1;
count = 1;
for i=2:length(asLinks)
    if(asLinks(i,1)==asLinks_(linkNum_,1) && asLinks(i,2)==asLinks_(linkNum_,2))
        asLinks_(linkNum_,3) = asLinks_(linkNum_,3) + asLinks(i,3);
        count = count + 1;
    else
        asLinks_(linkNum_,3) = round(asLinks_(linkNum_,3)/count);
        count = 1;
        linkNum_ = linkNum_ + 1;
        asLinks_(linkNum_,:) = asLinks(i,:);
    end
end

save('linkPrefixofLOC.mat','asLinks_','asPrefix');

