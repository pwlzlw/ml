clear all
close all

data_path = 'C:\Users\xf14883\Google Drive\goodDataAlspac\';

fs = 50; % sampling frequency in Hz
Wt = 5; % window in seconds
Ws = fs * Wt; % window in samples

filename = strcat(data_path, '*.mat'); %272105044A
list = dir(filename);

types = zeros(3,size(list,1));
ratio = zeros(3,size(list,1));
windows = zeros(1,size(list,1));
for f = 1:size(list,1)
%for f = 1:10
    
    disp(list(f).name)
         
    filename = strcat(data_path, list(f).name);
    data = open(filename);

    mag = abs(sqrt(data.xAxis.^2 + data.yAxis.^2 + data.zAxis.^2) - 1);

    len = data.goodDataIndex(2);
    ima = zeros(round(2*len/Ws) - 1,1);
    idx = 1;
    for i = 1:Ws/2:len - Ws
        if(i+Ws>size(mag,1))
            break
        end
        s = sum(mag(i:i+Ws));
        ima(idx,1)  = s/Ws;
        idx = idx + 1;
    end

    for i = 1:size(ima,1)
        if ima(i) <= 0.1184 
            types(1,f) = types(1,f) + 1;
        elseif ima(i) <= 0.5931 
            types(2,f) = types(2,f) + 1;
        else
            types(3,f) = types(3,f) + 1;
        end
    end
    windows(1,f) = size(ima,1);
    ratio(:,f) = types(:,f)./windows(1,f);
    %ratio(:,f)
end
%plot(mag)

hist(ratio(1,ratio(1,:)>0),0.05:0.05:0.95)

