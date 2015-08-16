function [fileNames,label,categories] = SearchImageFiles(strFolder, nDepth, nWidth, ext)

if ~exist('strFolder','var')
    strFolder = '.';
end

if ~exist('nDepth','var')
    nDepth = 0;
end

if ~exist('nWidth','var')
    nWidth = 0;
end

if ~exist('ext','var')
    ext = '.jpg';
end

% List content of the folder
Contents = dir(strFolder);
n = length(Contents);


% Retrieve the entries
entries = {};
[entries{1:n, 1}] = deal(Contents.name);



% Find hidden files.
hiddenFlags = nan(size(entries));
for i = 1:n
    hiddenFlags(i) = entries{i}(1) == '.';
end



% Find which entries are images
imageFlags = nan(size(entries));
for i = 1:n,
    [dummy1,  dummy2, Ext] = fileparts(entries{i});
    Ext = lower(Ext);
    imageFlags(i) = ismember(Ext, {lower(ext)});
end
imageFlags = imageFlags & (~hiddenFlags);
fileNames = entries(imageFlags);

if nWidth>0 && length(fileNames) > nWidth 
    fileNames = fileNames(1:nWidth);
end

m = length(fileNames);
for i = 1:m
    fileNames{i} = fullfile(strFolder, fileNames{i});
end


% Find which entries are directories.
label=[];
categories={};
inx=1;
if nDepth > 0
    directoryFlags = {};
    [directoryFlags{1:n,1}] = deal(Contents.isdir);
    directoryFlags = cell2mat(directoryFlags) & (~hiddenFlags);
    folderNames = entries(directoryFlags);
    
    fileNames_tmp = {};
    label_tmp=[];
    categories=[categories;folderNames];
    for i = 1:sum(directoryFlags)
        subFolder = fullfile(strFolder, folderNames{i});
        tmp = SearchImageFiles(subFolder, nDepth-1, nWidth, ext);
        tmp1=inx*ones(length(tmp),1);
        label_tmp=[label_tmp;tmp1];
        fileNames_tmp = [fileNames_tmp; tmp];
        
        inx=inx+1;
    end
    fileNames = [fileNames; fileNames_tmp];
    label=[label;label_tmp];
end


% if nargout > 1
% 
%     labels = [];
%     m = length(fileNames);
%     strOld = fileparts(fileNames{1});
%     label_i = 1;
%     for i = 1:m
%         strNew = fileparts(fileNames{i});
%         if ~strcmp(strOld, strNew)
%             label_i = label_i + 1;
%             strOld = strNew;
%         end
%         labels(i) = label_i;
%     end
% end