[fileNames, labels, ~] = SearchImageFiles('images', 100000000,10000000);
idx = randperm(length(fileNames));
fileNames = fileNames(idx);
labels = labels(idx) - 1;
options = 'wt';

exclude = fopen('exclude.log', options);
prop = 0.8;
root = '/home/rips_tc/caffe/data/logos/';
remainder = int32(prop*length(fileNames));
for jj = 1:10
    croV_jj = fopen(['crov_', num2str(jj), '.txt'], options);
    for ii = (int32((jj - 1)/10 * remainder) + 1):int32(jj/10 * remainder)
        try
            fprintf(croV_jj, '%s%s %d\n', root, Resize(fileNames{ii}), int32(labels(ii)));
        catch
            fprintf('    ... and failed.\n');
            fprintf(exclude, '%s%s', root, fileNames{ii});
        end
    end
    fclose(croV_jj);
end
%% TEST SET DO NOT TOUCH EVER AGAIN UNTIL DONE COMPLETELY
test = fopen('FINAL_TEST_SET.txt', options);
for ii = (int32(prop*length(fileNames)) + 1):length(fileNames)
    try 
        fprintf(test, '%s%s %d\n', root, Resize(fileNames{ii}), int32(labels(ii)));
    catch
        fprintf(' and failed.\n');
        fprintf(exclude, '%s%s', root, fileNames{ii});
    end
end
fclose(test);
fclose(exclude);
exit;