function img = GetImage(name)
info = imfinfo(name);
if(strcmp('truecolor',info.ColorType))
    I = imread(name);
    img = uint8(I);
    clear I name
elseif(strcmp('grayscale',info.ColorType))
    img = uint8(imread(name));
    clear name
elseif(strcmp('indexed',info.ColorType))
    [I,map] = imread(name);
    img = uint8(ind2rgb(I,map));
    clear I map name
else
    error('statPart:FormatImage','Image format error');
end
end

