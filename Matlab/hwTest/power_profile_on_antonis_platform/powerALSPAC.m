close all;
clear all;

royalb = 1/256*[65,105,225];
royalg = 1/255*[0,104,87];


avgcurrent(1)= 33.7*10^(-6);
x{1} = 'Idle';

avgcurrent(2)= 0.0012;
x{2} = 'Acc RAW';

%avgcurrent(3)= 0.0022;
%x{3} = 'Gyr RAW';

offset = (1.333*10^(-3)) - 4*10^(-6); 

avgcurrent(3)= 0.0054;
x{3} = 'Acc + Gyr RAW';

avgcurrent(4)= 0.808*1.3949*10^(-4) + 0.188*1.3597*10^(-4) + 0.004*1.3994*10^(-4);
x{4} = 'EML Acc';
y = 1:1:5;
%bar(y,avgcurrent);
avgcurrent(5) = 0.808*1.3949*10^(-4) + 0.188*0.0041 + 0.004*1.3994*10^(-4);
x{5} = 'EML Acc + Gyr';
%avgcurrent = avgcurrent./10^(-6);
%avgcurrent = avgcurrent./10^(-3);
%avgcurrent = 10*log10(avgcurrent);
%avgcurrent = avgcurrent.avgcurrent * 3.3;
for i=1:length(avgcurrent)
hold on
for i = 1:length(avgcurrent)
    h=bar(i,avgcurrent(i));
            set(h,'FaceColor',royalb);

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


print('powerALSPAC','-dpng','-r300');