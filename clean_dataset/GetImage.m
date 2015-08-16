function img = GetImage(idx, filename_list)
%GETIMAGE Summary of this function goes here
%   Detailed explanation goes here
name = filename_list(idx);
try
    name = name{:}; % just in case filename_list is a cell array
catch
end
disp(['SIFTing image ', name]);
% try and read the image, doing different things if it's gray scale
info = imfinfo(name);
try
    if(strcmp('truecolor',info.ColorType))
        I = imread(name);
        img = uint8(rgb2gray(I));
        clear I name
    elseif(strcmp('grayscale',info.ColorType))
        img = uint8(imread(name));
        clear name
    elseif(strcmp('indexed',info.ColorType))
        [I,map] = imread(name);
        img = uint8(ind2gray(I,map));
        clear I map name
    else
        error('statPart:FormatImage','Image format error');
    end
catch
    disp('...failed');
end
img = single(img);
end

