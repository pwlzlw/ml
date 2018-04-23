load('v_idle.mat');
%%Sleep current 
offset = 1.333*10^(-3);
current = current - offset;
figure(1); 
plot(timeSec,current);
grid on
title('Idle');
%figure(2);
%histogram(current((current<0.0015)&(current>0.0012)),2000)
%sleepCurrent=current((current<0.0015)&(current>0.0012));
title('Idle');
load('v_mc3672.mat');
current = current - offset;
figure(3); 
plot(timeSec,current);
grid on
title('MC3672_raw');
%figure(4);
%histogram(current((current<0.0015)&(current>0.0012)),2000)
%sleepCurrent=current((current<0.0015)&(current>0.0012));
title('MC3672_raw');
load('v_mc6470_3_3v.mat');
current = current - offset;
figure(5); 
plot(timeSec,current);
grid on
title('MC6470_raw');
%figure(6);
%histogram(current((current<0.0015)&(current>0.0012)),2000)
%sleepCurrent=current((current<0.0015)&(current>0.0012));
title('MC6470_raw');
load('v_icm.mat');
current = current - offset;
figure(7); 
plot(timeSec,current);
grid on
title('ICM_raw');
%figure(8);
%histogram(current((current<0.0025)&(current>0.0012)),2000)
%sleepCurrent=current((current<0.0025)&(current>0.0012));
title('ICM_raw');
load('v_data_collection.mat');
current = current - offset;
figure(9); 
plot(timeSec,current);
grid on
title('ALL_raw');
%figure(10);
%histogram(current((current<0.0025)&(current>0.0012)),2000)
%sleepCurrent=current((current<0.0025)&(current>0.0012));
title('ALL_raw');
load('v_embedded_ml.mat');
offset = 1.323*10^(-3);
current = current - offset;
figure(11); 
plot(timeSec,current);
grid on
title('Embedded Machine Learning');
%figure(12);
%histogram(current((current<0.0015)&(current>0.0012)),2000)
%sleepCurrent=current((current<0.0015)&(current>0.0012));
title('EML');