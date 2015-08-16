clear all
warning('off'); %#ok<WNOFF>
% create variables for important paths
images = './images/';
label_1 = 'no-logo'; label_2 = 'has-logo';
% add paths to images
addpath(genpath(images), './temp')
run('~/src/vlfeat/toolbox/vl_setup.m')
%% Partition the data.
% proportion of to cd ngrab. leave 1 - prop = 0.1 for testing
prop = 0.9; 
% get a list of the filenames and shuffle them
[fileNames_1, ~, ~] = SearchImageFiles([images, label_1], 1000, 10000000);
[fileNames_2, ~, ~] = SearchImageFiles([images, label_2], 1000, 10000000);
fileNames_1 = fileNames_1(randperm(length(fileNames_1)));
fileNames_2 = fileNames_2(randperm(length(fileNames_2)));
% Get prop of the data. total_1, total_2 represent how much many files of
% each class to get. 
total_1 = int32(prop * length(fileNames_1));
total_2 = int32(prop * length(fileNames_2));
clear prop images
%% Gather the descriptors.
% preallocate cell arrays to store the data so we can run parallel for
% loops.
parfor idx = 1:total_1
    % get the image based on the index and compute the SIFT descriptors
    try
        I = GetImage(idx, fileNames_1);
    catch
        continue
    end
    [~, D] = vl_sift(I);
    I = []; %free up memory
    % store the labels of the vectors and the descriptors so that we can
    % classify using SVM
    parsave(['./temp/temp_', int2str(idx), '.mat'], double(D'), repmat({ label_2 }, size(D, 2), 1));
    D = []; %free up memory
end
parfor idx = 1:total_2
    % repeat the above process.
    try
        I = GetImage(idx, fileNames_2);
    catch
        continue
    end
    [~, D] = vl_sift(I);
    I = [];
    parsave(['./temp/temp_', int2str(total_1 + idx), '.mat'], double(D'), repmat({ label_2 }, size(D, 2), 1));
    D = [];
end
%% ASSEMBLE THE DATA
% create a vertical vector of labels
% totalLabels = []; totalDescriptors = [];
for idx = 1380:(total_1 + total_2)
    try
        load(['./temp/temp_', int2str(idx), '.mat']);
    catch
        disp('damn');
        continue
    end
    totalLabels = vertcat(totalLabels, labels); %#ok<AGROW>
    % vertcat since we've taken the transpose already. don't need to make
    % it a double matrix since we've already done that
    totalDescriptors = vertcat(totalDescriptors, descrip);  %#ok<AGROW>
    clear descrip labels;
end
% concatenate the matrices of descriptors together, then take the transpose
% so each row is an observation
disp('SIFTing is done!')
%% Train...
disp('Training the SVM...');
C = [0.01, 0.1, 1, 10, 100, 1000]; % typical values for cost
fold = 5; % 5-fold cross-validation to reduce training time.
iterLimit = 1e6; % set the maximum number of iterations
solver = 'L1QP'; % this is chosen because something l1 loss something
stdize = true; % center and reduce deviation, suggested as important
kernelFunc = 'linear';
switch kernelFunc
    case 'linear' % Linear Kernel
        models = cell(1, length(C));
        parfor ii = 1:length(C)
            models{ii} = fitcsvm(descrip, labels, ...
                'KFold', fold, ...
                'KernelFunction', kernelFunc, ...
                'BoxConstraint', C(ii), ...
                'IterationLimit', iterLimit, ...
                'Solver', solver, ...
                'Standardize', stdize ...
                );
            fprintf('Loss for the parameter C = %d: %d', C(ii), ...
                kfoldLoss(models{ii}));
        end
    case 'rbf' % Gaussian Kernel
        % parameter for rbf kernel. these are typical values (log scale)
        gamma = [1e-5, 1e-4, 1e-3, 1e-2, 1e-1, 1]; 
        % compute cartesian product of (C, gamma) pairs
        [CMat, gammaMat] = meshgrid(C, gamma);
        CMat = CMat(:); gammaMat = gammaMat(:);
        % linearly enumerate the pairs
        models = cell(1, length(CMat));
        % train models in parallel, and store the results for the future.
        parfor ii = 1:length(models)
            models{ii} = fitcsvm(descrip, labels, ...
                'KFold', fold, ...
                'KernelFunction', kernelFunc, ...
                'BoxConstraint', CMat(ii), ...
                'KernelScale', gammaMat(ii), ...
                'IterationLimit', iterLimit, ...
                'Solver', solver, ...
                'Standardize', stdize ...
                );
            fprintf('Loss for the parameters (C, gamma) = (%d, %d): %d', ...
                CMat(ii), gammaMat(ii), kfoldLoss(models{ii}));
        end
        clear gamma
end
clear ii C fold iterLimit solver stdize kernelFunc
disp('Training is done!');

%% Test on the test data. THIS IS AFTER DOING PARAMETER SEARCH. DO NOT TOUCH UNTIL THEN
remaining_1 = length(fileNames_1) - total_1;
remaining_2 = length(fileNames_2) - total_2;
correct = zeros(1, remaining_1 + remaning_2);
offset = -total_1; % start the index from 1
parfor idx = total_1 + 1: length(fileNames_1)
    % compute SIFT descriptors
    try
        I = GetImage(idx, fileNames_1);
    catch
        correct(idx + offset) = -1;
        continue
    end
    [~, D] = vl_sift(I);
    I = [];
    % get the SVM predictions
    pred = predict(SVMModel, double(D'));
    D = [];
    % find the unique labels. should always be 2
    labs = unique(pred, 'stable');
    % find the number of each label
    counts = cellfun(@(x) sum(ismember(pred,x)),labs,'un',0);
    % if labs is empty, then D' was empty. don't count this in the
    % classification error/accuracy.
    if isempty(labs)
        correct(idx + offset) = -1;
        continue
    end
    % if the label is correct, change correct's entry from 0 to 1
    if ~isempty(labs) && strcmp(labs{1}, label_1)
        correct(idx +  offset) = 1;
    end
end
offset = -total_2 + remaining_1; % start the index from remaining_1 + 1
parfor idx = total_2 + 1: length(fileNames_2)
    try
        I = GetImage(idx, fileNames_2);
    catch
        correct(idx + offset) = -1;
        continue
    end
    [~, D] = vl_sift(I);
    I = [];
    pred = predict(SVMModel, double(D'));
    D = [];
    labs = unique(pred, 'stable');
    disp(length(labs));
    counts = cellfun(@(x) sum(ismember(pred,x)),labs,'un',0);
    if isempty(labs)
        correct(idx + offset) = -1;
        continue
    end
    if ~isempty(labs) && strcmp(labs{1}, label_2)
        correct(idx + offset) = 1;
    end
end
correct = correct(correct > -1); %ignore the images which had no descriptors
fprintf('Accuracy is %d', sum(correct == 1)/length(correct));

