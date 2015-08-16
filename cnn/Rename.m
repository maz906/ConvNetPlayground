function rename = Rename(imgName)
% return 1 if the image is readable and resizable, otherwise return -1
fprintf('Renaming %s ...\n', imgName);
[pathstr, name, ~] = fileparts(imgName);
rename = [pathstr, '/', name, '.jpg'];
try
    movefile(imgName, rename);
catch
end
end

