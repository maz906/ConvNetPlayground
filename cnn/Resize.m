function rename = Resize(imgName)
% return 1 if the image is readable and resizable, otherwise return -1
%fprintf('Resizing %s ...', imgName);
img = GetImage(imgName);
img = im2single(imresize(img, [256, 256]));
delete(imgName);
[pathstr, name, ~] = fileparts(imgName);
rename = [pathstr, '/', name, '.jpg'];
imwrite(img, rename, 'jpg');
%fprintf(' and renaming to %s\n', rename);
end

