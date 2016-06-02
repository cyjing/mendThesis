function [ asID, asLinks, asPrefix ] = locTopoGenerator( asNum )
% location model topology generation: 
% extract the top asNum countries with most ASes
% aggregate ASes within a country into an AS  

load linkPrefixofLOC.mat;

asID=[1:asNum]';

asLinks = zeros(0,3);
linkNum = 0;
for i=1:length(asLinks_)
    if(asLinks_(i,1) <= asNum && asLinks_(i,2) <= asNum)
        linkNum = linkNum+1;
        asLinks(linkNum,:) = asLinks_(i,:);
    end
end

asPrefix_ = asPrefix;
asPrefix = zeros(0,6);
asPrefixNum = 0;
for i=1:length(asPrefix_)
    if(asPrefix_(i,1) <= asNum)
        asPrefixNum = asPrefixNum+1;
        asPrefix(asPrefixNum,:) = asPrefix_(i,:);
    end
end

end
