close all;
clear all;

royalb = 1/256*[65,105,225];
royalg = 1/255*[0,104,87];


avgcurrent(1)= 33.7*10^(-6);
x{1} = 'Idle';

avgcurrent(2)= 0.0012;
x{2} = 'Acc RAW';

%avgcurrent(3)= 0.0054;
%x{3} = 'Gyr RAW';

offset = (1.333*10^(-3)) - 4*10^(-6); 

avgcurrent(3)= 0.0055;
x{3} = 'Acc + Gyr RAW';

avgcurrent(4) = 1.3082*10^(-4);
x{4} = 'Stage 1 only';

avgcurrent(5) = 1.3949*10^(-4);
x{5} = 'Sedentary';

offset = 1.3597*10^(-04);
avgcurrent(6)= 1.3597*10^(-4);
x{6} = 'Moderate Acc';
y = 1:1:5;
%bar(y,avgcurrent);
avgcurrent(7) = 0.0041;
x{7} = 'Moderate Acc + Gyr';

avgcurrent(8) = 1.3994*10^(-4);
x{8}= 'Rigorous';

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



ylabel('Power [ W ]');
%xlabel('Configuration');
h = gca;
set(gca,'yscale','log');

h.XTickLabel = x;
h.XTickLabelRotation = 45;
set(gca,'Xtick',1:1:length(avgcurrent));
h.TickLabelInterpreter = 'none';
grid on


print('power','-dpng','-r300');