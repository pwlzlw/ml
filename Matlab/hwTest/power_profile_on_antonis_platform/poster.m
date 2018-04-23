close all;
clear all;

royalb = 1/256*[65,105,225];
royalg = 1/255*[0,104,87];

load('v_idle.mat');
offset = (1.363*10^(-3)) - 13*10^(-6); 

avgcurrent(1)= 15*10^(-6);
x{1} = 'Idle';

load('v_MC3672.mat');
offset = (1.333*10^(-3)) - 13*10^(-6); 
avgcurrent(2)= mean(current-offset)
x{2} = 'Acc RAW';

%load('v_mc6470_3_3v.mat');
%offset = (1.333*10^(-3)) - 13*10^(-6); 
%avgcurrent(3)= mean(current-offset);
%x{3} = 'MC6470';

load('v_icm.mat');
offset = (1.333*10^(-3)) - 5*10^(-6); 
avgcurrent(3)= mean(current-offset);
x{3} = 'Gyro RAW';

load('v_data_collection.mat');
offset = (1.333*10^(-3)) - 4*10^(-6); 

avgcurrent(4)= mean(current-offset);
x{4} = 'A + G RAW';

load('v_embedded_ml.mat');
offset = (1.323*10^(-3)) - 13*10^(-6); 
avgcurrent(6)= mean(current- offset);
x{6} = 'Acc EML';

y = 1:1:5;
%bar(y,avgcurrent);
avgcurrent(5) = avgcurrent(6)*0.818 + avgcurrent(3)*0.188;
x{5} = 'Acc + Gyro EML';
avgcurrent = avgcurrent * 3.3;
%avgcurrent = avgcurrent.avgcurrent * 3.3;
for i=1:5
hold on
for i = 1:6
    h=bar(i,avgcurrent(i));
    %only the creme de la creme features
    if i ==5 || i == 6
        set(h,'FaceColor',royalg);
    else
        set(h,'FaceColor',royalb);
    end
end
hold off
end



ylabel('Power [ W ]');
%xlabel('Configuration');
h = gca;
h.XTickLabel = x;
h.XTickLabelRotation = 45;
set(gca,'Xtick',1:1:6);
h.TickLabelInterpreter = 'none';
grid on


print('power','-dpng','-r300');