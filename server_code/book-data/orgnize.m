clc
clear all;
imdb=importdata('imdb_books_v2.mat');
num=length(unique(imdb.labels));
fin=fopen('bookList_v2','w');
for i=1:num
    inx=find(imdb.labels==i);
    
    %write files
    for j=1:length(inx)
        try
            I=imread(imdb.fileNames{inx(j)});
            descriptors = ExtractKeyFeatures(I, 'SURF');
            if (size(descriptors,2) < 5)
                fprintf('This is blank');
                continue;
            end
        catch
            continue;
        end
        sz=size(I);
        l=0
        t=0
        fprintf(fin,'%s %d %d %d %d %d\n',imdb.fileNames{inx(j)},l,t,sz(2),sz(1),i);
    end
    
end
fclose(fin);