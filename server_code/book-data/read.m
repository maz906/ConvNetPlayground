%prepare training dataset 1
% This dataset only contains flicker 32 logo graphic images

clc
clear all;
close all;

[fileNames, label,categories]=SearchImageFiles('./samples',1,0,'.jpg');
num=length(fileNames);
for i=1:num

%name=strrep(fileNames{i},' ', '_');
%if ~strcmp(name,fileNames{i})
%movefile(fileNames{i},name);
%end
 
% I=imread(fileNames{i});
% if(size(I,1)<100&&size(I,2)<100)
% delete(fileNames{i});
% end

%delete(fileNames{i});
%I=imread(fileNames{i});
%fileNames{i}(end-2:end)='jpg';
%imwrite(I,fileNames{i});
end

imdb.labels=label;
imdb.categories=categories;
imdb.fileNames=fileNames;

save('imdb_booklist_v2.mat','imdb');