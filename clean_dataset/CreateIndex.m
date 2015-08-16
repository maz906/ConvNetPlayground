[fileNames, labels, categories] = SearchImageFiles('./images', 1000000,10000000);
fileNames = fileNames(randperm(length(fileNames)));
train = fopen('train.txt', 'w');
prop = 0.9;
for ii = 1:int32(prop*length(fileNames))
    fprintf(train, '%s %d\r\n', fileNames{ii}, labels(ii));
end
fclose(train);

test = fopen('test.txt', 'w');
prop = 0.9;
for ii = (int32(prop*length(fileNames)) + 1):length(fileNames)
    fprintf(test, '%s %d\r\n', fileNames{ii}, labels(ii));
end
fclose(test);