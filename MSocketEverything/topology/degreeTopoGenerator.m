function [ asID, asLinks, asPrefix ] = degreeTopoGenerator( asNum )
% analyze the degree of each AS, extract the first asNum ASes, and generate
% the topology of those asNum ASes (delay, prefix)
load topologyData;
asInfo = unique([topologyData(:,1);topologyData(:,2)]);
numAS = length(asInfo);
asInfo = [asInfo zeros(numAS,1)];  %[AS_ID, degree]

for i = 1:length(asInfo)
    asInfo(i,2) = sum(topologyData(:,1) == asInfo(i)) + sum(topologyData(:,2) == asInfo(i));
    if mod(i,1000) == 0
        i
    end
end
asInfo = sortrows(asInfo,-2);

asID = asInfo(1:asNum,1);
asLinks = zeros(0,3);
linkNum = 0;

%find links among those asNum ASes
for i= 1:length(topologyData)
    if(isempty(find(asID==topologyData(i,1)))==0 && isempty(find(asID==topologyData(i,2)))==0)
        linkNum = linkNum +1;
        asLinks(linkNum,:) = topologyData(i,:);
    end
end


%find the corresponding prefix of those top asNum ASes
load ASPrefixData;
numPrefix = length(inputFile);
asPrefix = zeros(0,6);
asPrefixNum = 0;

for i = 1:numPrefix
    if(isempty(find(asID==inputFile(i,1)))==0)
        asPrefixNum = asPrefixNum + 1;
        asPrefix(asPrefixNum,:) = inputFile(i,:);
    end
end

end
