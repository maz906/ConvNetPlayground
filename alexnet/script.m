% Well, this is a function that write the
% iteration vs accurancy
% iteration vs loss
% To a file

clc;
clear;

% log file of caffe model
logName = strcat('crov_', num2str(8), '/caffe-train.log');
ii = 1
fid = fopen(logName, 'r');
fid_accuracy = fopen(strcat('output_accuracy_',num2str(4),'.txt'), 'w');
fid_loss = fopen(strcat('output_loss_',num2str(4),'.txt'), 'w');

tline = fgetl(fid);

while ischar(tline)
    % First find the accuracy line
    k = strfind(tline, 'Test net output');
    if (k)
        k = strfind(tline, 'accuracy');
        if (k)
            % If the string contain test and accuracy at the same time
            % The bias from 'accuracy' to the float number
            indexStart = k + 11; 
            indexEnd = size(tline);
            str = tline(indexStart : indexEnd(2));
        end
        
        % Get the number of index
        k = strfind(tline, '#');
        if (k)
            indexStart = k + 1;
            indexEnd = strfind(tline, ':');
            str2 = tline(indexStart : indexEnd - 1);
        end
        
        % Concatenation of two string
        res_str = strcat(str2, '/', str);
        fprintf(fid_accuracy, '%s\r\n', res_str);
        disp(ii);
        ii = ii + 1;
    end
    
    % Then find the loss line
    k1 = strfind(tline, 'Iteration');
    if (k1)
       k2 = strfind(tline, 'loss');
       if (k2)
           indexStart = k2 + 7;
           indexEnd = size(tline);
           str1 = tline(indexStart:indexEnd(2));
           indexStart = k1 + 10;
           indexEnd = strfind(tline, ',') - 1;
           str2 = tline(indexStart:indexEnd);
           res_str1 = strcat(str2, '/', str1);
           fprintf(fid_loss, '%s\r\n', res_str1);
           
       end
    end
    tline = fgetl(fid);
end
fclose(fid);
fclose(fid_accuracy);
