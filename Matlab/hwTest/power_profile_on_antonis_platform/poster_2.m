clear all;
close all;
royalb = 1/256*[65,105,225];
load('v_embedded_ml.mat');
offset = 1.323*10^(-3);
current = (current - offset) * 3.3;
figure(1); 
hold on;
timeSec = timeSec - 5.55;

x = timeSec(1:201831);
y = current(1:201831);

plot( x.*1000,y*1000);
%title('Power profile at 0dBm');
ylabel('Power [ mW ]');
xlabel('Time [ ms ]');
%xlim([5.55 5.558]);
xlim([0 8]);
%ylim([-0.001 0.25]);
%figure(12);
%histogram(current((current<0.0015)&(current>0.0012)),2000)
%sleepCurrent=current((current<0.0015)&(current>0.0012));
i = 42;
%0.00322 start, 0.003739 end or 0.01351
%index 
 
x = timeSec(201879:202028);
y = current(201879:202028);
e = trapz(y) * (x(3)-x(2));
e


plot(x.*1000,y*1000);

x = timeSec(201832:201878);
y = current(201832:201878);

plot(x.*1000,y*1000);
%grid on

x = timeSec([202028:467684]);
y = current([202028:467684]);
plot(x.*1000,y*1000, 'color', [ 0    0.4470    0.7410]);
grid on

legend('other','RF','processing','Location','northwest');

print('bleadv','-dpng','-r300');
