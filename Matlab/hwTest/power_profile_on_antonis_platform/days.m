close all;
clear all;

royalb = 1/256*[65,105,225];
royalg = 1/255*[0,104,87];


%avgcurrent(1)= (100*10^(-3)*3.7*3600/33.7*10^(-6))/3600/24;
%x{1} = 'Idle';

avgcurrent(1)= 12.43;
x{1} = 'Acc RAW';

%avgcurrent(2)= 2.83;
%x{2} = 'Gyr RAW';

offset = (1.333*10^(-3)) - 4*10^(-6); 

avgcurrent(2)= 2.82;
x{2} = 'Acc + Gyr RAW';

avgcurrent(3) = 117.85;
x{3} = 'Stage 1 only';

avgcurrent(4) = 110.52;
x{4} = 'Sedentary';

offset = (1.323*10^(-3)) - 13*10^(-6); 
avgcurrent(5)= 113.39;
x{5} = 'Moderate Acc';

y = 1:1:5;
%bar(y,avgcurrent);
avgcurrent(6) = 3.76;
x{6} = 'Moderate Acc + Gyr';

avgcurrent(7) = 110.17;
x{7}= 'Rigorous';

%avgcurrent = avgcurrent.avgcurrent * 3.3;
for i=1:length(avgcurrent)
hold on
for i = 1:length(avgcurrent)
    h=bar(i,avgcurrent(i));
            set(h,'FaceColor',royalg);

    %{
    %only the creme de la creme features
    if i ==5 || i == 6
        set(h,'FaceColor',royalg);
    else
        set(h,'FaceColor',royalb);
    end
    %}
end
hold off
end



ylabel('Time [ days ]');
%xlabel('Configuration');
h = gca;
set(gca,'yscale','log');

h.XTickLabel = x;
h.XTickLabelRotation = 45;
set(gca,'Xtick',1:1:length(avgcurrent));
h.TickLabelInterpreter = 'none';
grid on


print('days1','-dpng','-r300');