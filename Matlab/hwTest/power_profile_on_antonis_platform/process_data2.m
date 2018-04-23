load('v_idle.mat');
offset = (1.363*10^(-3)) - 20*10^(-6); 

avgcurrent(1)= 23*10^(-6);
x{1} = 'Idle';

load('v_MC3672.mat');
offset = (1.333*10^(-3)) - 20*10^(-6); 
avgcurrent(2)= mean(current-offset);



x{2} = 'MC3672';

load('v_mc6470_3_3v.mat');
offset = (1.333*10^(-3)) - 13*10^(-6); 
avgcurrent(3)= mean(current-offset);
x{3} = 'MC6470';

load('v_icm.mat');
offset = (1.333*10^(-3)) - 12*10^(-6); 
avgcurrent(4)= mean(current-offset);
x{4} = 'ICM20948';

load('v_data_collection.mat');
offset = (1.333*10^(-3)) - 4*10^(-6); 

avgcurrent(5)= mean(current-offset);
x{5} = 'ALL Raw data';

load('v_embedded_ml.mat');
offset = (1.323*10^(-3)) - 20*10^(-6); 
avgcurrent(6)= mean(current- offset);
x{6} = 'EML';

y = 1:1:6;
bar(y,avgcurrent);
ylabel('Mean current');
xlabel('Configuration');
grid on;
h = gca;
h.XTickLabel = x;
h.XTickLabelRotation = 90;
set(gca,'Xtick',1:1:6);
h.TickLabelInterpreter = 'none';
