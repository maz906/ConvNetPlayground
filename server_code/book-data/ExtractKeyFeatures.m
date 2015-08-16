% feature detection and extraction algorithm
% 2014.1.13 @Lenovo
function [descriptors, keypoints] = ExtractKeyFeatures(image, method)

max_image_size = 512;
H = size(image,1); W = size(image,2);
if (max(H,W) > max_image_size)
    scale = max_image_size/max(H,W);
    image = imresize(image, scale);
end

% feature extraction
switch method
    case 'SURF'
        image = rgb2gray(image);
        % feature descriptor
        keypoint = detectSURFFeatures(image);
        [descriptors, keypoints] = extractFeatures(image, keypoint);
        
    case 'BRISK'
        % init 
        brisk('init','threshold',60,'octaves',4);
        % load image to brisk
        brisk('loadImage',image);
        % detect keypoints in case some feature need
        keypoint = brisk('detect');
        % feature descriptor
        [keypoints, descriptors]=brisk('describe');
        % free memory
        brisk('terminate');
        
    case 'FREAK'
%         % load image to brisk
%         freak('loadImage',rim);
%         % detect keypoints in case some feature need
%         keypoint = freak('detect');
%         % feature descriptor
%         [keypoints, descriptors]=freak('describe');
%         % free memory
%         freak('terminate');
        % feature descriptor
        keypoint =  detectHarrisFeatures(image);
        [descriptors, keypoints] = extractFeatures(image, keypoint);   
end
descriptors = descriptors';