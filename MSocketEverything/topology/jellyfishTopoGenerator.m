function [ asID, asLinks, asPrefix ] = jellyfishTopoGenerator( asNum )
%file to generate jellyfish topology for orbit grid testbed

% we emulate the Internet as the following layers: shell-0, hang-0, shell-1, hang-1, shell-2, hang-2
% AS number starts from 1, and maps to different layers of the Internet in the following sequence: shell-0, hang-0, shell-1, hang-1, shell-2, hang-2


%%% generate AS for each layer
%asNum=200;  %total number of ASes as input parameter
AS_num=zeros(1,6);
AS_num(1)= ceil(asNum*0.09/100);   %shell-0
AS_num(2)= ceil(asNum*8.04/100);   %hang-0
AS_num(4)= ceil(asNum*20.5/100);   %hang-1
AS_num(5)= ceil(asNum*22.43/100);   %shell-2
AS_num(6)= ceil(asNum*0.85/100);   %hang-2
AS_num(3)= asNum-AS_num(1)-AS_num(2)-AS_num(4)-AS_num(5)-AS_num(6);   %shell-1

asID=[1:asNum]';
linkNum=0;
asLinks=zeros(0,3);
prefixNum=0;
asPrefix=zeros(0,6);

%generate the AS list file
as_fid = fopen('AS.data', 'wt');
as = 1;
for i=1:6
    for j=1:AS_num(i);
        fprintf(as_fid, '%d\n', as);
        as = as +1;
    end
end


%%% generate transit connection
%% delay: uniform distributed between 0.555~100 ms. 
%TODO: normal distribution; different range at different cases

%%TODO: preferential attachment
fid = fopen('jellyfish_topo.data', 'wt');
as = 1;

% linkNum01: number of transit links between shell-0 and shell-1 (invalid
% if AS_num(1)*AS_num(3)<linkNum01
% linkNum12: number of transit links between shell-1 and shell-2
linkNum01 = floor(asNum*34376/26424);
linkNum12 = floor(asNum*14280/26424);

% src: upper layer. dst: lower layer
for i=2:6
    for j=1:AS_num(i)
        dst = as + j;
        % i=2: shell-0  <--> hang-0
        % i=3: shell-0  <--> shell-1
        if(i==2 || i==3)
                src = floor(1+ AS_num(1)*rand(1));
        % i=4: shell-1  <--> hang-1
        % i=5: shell-1  <--> shell-2
        elseif(i==4 || i==5)
                src = floor(AS_num(1) + AS_num(2) + 1 + AS_num(3)*rand(1));
        % i=6: shell-2  <--> hang-2
        else
                src= floor(AS_num(1) + AS_num(2) + AS_num(3) + AS_num(4) + 1 + AS_num(5)*rand(1));
        end
        delay = floor(0.555+100*rand(1));
        fprintf(fid, '%d %d %d\n', src, dst, delay);
        linkNum = linkNum + 1;
        asLinks(linkNum,:) = [src dst delay];
    end
    as = as + AS_num(i);
end

%other transit links
%between shell-0 and shell-1
i=0;
while(i < linkNum01-AS_num(3) && AS_num(3)+i < AS_num(1)*AS_num(3))
    src = floor(1+ AS_num(1)*rand(1));
    dst = floor(AS_num(1) + AS_num(2) + 1 + AS_num(3)*rand(1));
    if(ismember([src dst],asLinks(:,1:2),'rows')==0 && ismember([dst src],asLinks(:,1:2),'rows')==0)
        i = i+1;
        delay = floor(0.555+100*rand(1));
        fprintf(fid, '%d %d %d\n', src, dst, delay);
        linkNum = linkNum + 1;
        asLinks(linkNum,:) = [src dst delay];
    end
end
%between shell-1 and shell-2
i=0;
while(i < linkNum12-AS_num(5) && AS_num(5)+i < AS_num(3)*AS_num(5))
    src = floor(AS_num(1) + AS_num(2) + 1 + AS_num(3)*rand(1));
    dst = floor(AS_num(1) + AS_num(2) + AS_num(3) + AS_num(4) + 1 + AS_num(5)*rand(1));
    if(ismember([src dst],asLinks(:,1:2),'rows')==0 && ismember([dst src],asLinks(:,1:2),'rows')==0)
        i = i+1;
        delay = floor(0.555+100*rand(1));
        fprintf(fid, '%d %d %d\n', src, dst, delay);
        linkNum = linkNum + 1;
        asLinks(linkNum,:) = [src dst delay];
    end
end


%%% generate peer connection
% linkNum11: peer links within shell-1
% linkNum22: peer links within shell-2
linkNum11 = ceil(asNum*32732/26424);
linkNum22 = ceil(asNum*792/26424);

% shell-1
i=0;
while(i < linkNum11)
    src = floor(AS_num(1) + AS_num(2) + 1 + AS_num(3)*rand(1));
    dst = floor(AS_num(1) + AS_num(2) + 1 + AS_num(3)*rand(1));
    if(ismember([src dst],asLinks(:,1:2),'rows')==0 && ismember([dst src],asLinks(:,1:2),'rows')==0)
        i=i+1;
        delay = floor(0.555+100*rand(1));
        fprintf(fid, '%d %d %d\n', src, dst, delay);
        linkNum = linkNum + 1;
        asLinks(linkNum,:) = [src dst delay];
    end
end

% shell-2
i=0;
while(i < linkNum22)
    src = floor(AS_num(1) + AS_num(2) + AS_num(3) + AS_num(4) + 1 + AS_num(5)*rand(1));
    dst = floor(AS_num(1) + AS_num(2) + AS_num(3) + AS_num(4) + 1 + AS_num(5)*rand(1));
    if(ismember([src dst],asLinks(:,1:2),'rows')==0 && ismember([dst src],asLinks(:,1:2),'rows')==0)
        i=i+1;
        delay = floor(0.555+100*rand(1));
        fprintf(fid, '%d %d %d\n', src, dst, delay);
        linkNum = linkNum + 1;
        asLinks(linkNum,:) = [src dst delay];
    end
end

%% now is the prefix
load clique;
load hangMem;
load shellMem;

prefixInput = csvread('ASNumDataMod-2_1_11.csv');
numPrefix = length(prefixInput);
asPrefix = zeros(0,6);
asPrefixNum = 0;

for i=1:numPrefix
    flag=0;
    %shell-0
    if(isempty(find(clique==prefixInput(i,1)))==0)
        flag=1;
        tempasID = floor(1+ AS_num(1)*rand(1));
    %shell-1
    elseif(isempty(find(shellMem(1,:)==prefixInput(i,1)))==0)
        flag=1;
        tempasID = floor(AS_num(1) + AS_num(2) + 1 + AS_num(3)*rand(1));
    %shell-2    
    elseif(isempty(find(shellMem(2,:)==prefixInput(i,1)))==0)
        flag=1;
        tempasID = floor(AS_num(1) + AS_num(2) + AS_num(3) + AS_num(4) + 1 + AS_num(5)*rand(1));
    %hang-0
    elseif(isempty(find(hangMem(1,:)==prefixInput(i,1)))==0)
        flag=1;
        tempasID = floor(AS_num(1) + 1 + AS_num(2)*rand(1));
    %hang-1    
    elseif(isempty(find(hangMem(2,:)==prefixInput(i,1)))==0)
        flag=1;
        tempasID = floor(AS_num(1) + AS_num(2) + AS_num(3) + 1 + AS_num(4)*rand(1));
    %hang-2
    elseif(isempty(find(hangMem(3,:)==prefixInput(i,1)))==0)
        flag=1;
        tempasID = floor(AS_num(1) + AS_num(2) + AS_num(3) + AS_num(4) + AS_num(5) + 1 + AS_num(6)*rand(1));
    end
    if flag==1
        asPrefixNum = asPrefixNum + 1;
        asPrefix(asPrefixNum,:) = prefixInput(i,:);
        asPrefix(asPrefixNum,1) = tempasID;
    end    
    if mod(i,10000) == 0
        i
    end
end

fclose(fid);
fclose(as_fid);

end
