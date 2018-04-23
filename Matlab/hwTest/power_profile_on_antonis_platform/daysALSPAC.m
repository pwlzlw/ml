close all;
clear all;

royalb = 1/256*[65,105,225];
royalg = 1/255*[0,104,87];


avgcurrent(1)= 12.43;
x{1} = 'Acc RAW';

%avgcurrent(2)= 2.83;
%x{2} = 'Gyr RAW';


avgcurrent(2)= 2.82;
x{2} = 'Acc + Gyr RAW';

Ebat = 100*10^(-3)*3.7*3600;

%avgcurrent(3)= 0.808*110.52 + 0.188*113.39 + 0.004*110.17;
avgcurrent(3)= Ebat/(1.3949*10^(-4)* 0.808 + 0.188*1.3597*10^(-4)+ 0.004*1.3994*10^(-4))/3600/24;
x{3} = 'EML Acc';
y = 1:1:5;
%bar(y,avgcurrent);
avgcurrent(4) = Ebat/( 1.3949*10^(-4)* 0.808 + 0.0041 * 0.1888 + 0.004*1.3994*10^(-4))/3600/24;
x{4} = 'EML Acc + Gyr';

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



ylabel('Time [ days ]');
%xlabel('Configuration');
h = gca;
set(gca,'yscale','log');
h.XTickLabel = x;
h.XTickLabelRotation = 45;
set(gca,'Xtick',1:1:length(avgcurrent));
h.TickLabelInterpreter = 'none';
grid on


print('daysALSPAC','-dpng','-r300');