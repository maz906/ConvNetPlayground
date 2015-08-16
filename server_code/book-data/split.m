clc
clear all;
imdb=importdata('imdb_flick32.mat');
num=length(unique(imdb.labels));
fin=fopen('trainingList','w');
fin1=fopen('testList','w');
for i=1:num
inx=find(imdb.labels==i);
num_i=length(inx);
inx_r=randperm(num_i);
inx_r=inx(inx_r);
inx_train=inx_r(1:floor(num_i*9/10));
inx_test=setdiff(inx_r,inx_train);
%write files
for j=1:length(inx_train)
I=imread(['./' imdb.fileNames{inx_train(j)}]);
sz=size(I);
l=0
t=0
fprintf(fin,'%s %d %d %d %d %d\n',imdb.fileNames{inx_train(j)},l,t,sz(2),sz(1),i);
end

for j=1:length(inx_test)
I=imread(['./' imdb.fileNames{inx_test(j)}]);
sz=size(I);
l=0
t=0
fprintf(fin1,'%s %d %d %d %d %d\n',imdb.fileNames{inx_test(j)},l,t,sz(2),sz(1),i);
end


end
fclose(fin);
fclose(fin1);