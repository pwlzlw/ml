function plotNoise ()
    clear all;
    close all;
    %set(groot,'defaultfigureposition',[400   250   560   420]);
    % Constants
    royalb = 1/256*[65,105,225];
    royalr = 1/255*[235,43,54];
    royalg = 1/255*[0,104,87];
    colorsR = {royalb;royalr;royalg};
    cd('..')
    cd('data_hw');
    noise = load('dataDump_noise17_24');
    %noise = load('dataDump_noise17_24');

    for j=1:3 
        ts{j} = noise(1).ts{j}; 
    end
    
    pd = fitDistribution(ts);
    
    i = 42;
    cd('..')
    cd('figures')
    cd('Final_Figures')
    
    function pd = fitDistribution(tscells)        
        for i=1:3
            %hold on;
            h = figure(i);    
            set(gca,'Position',[400   15   560   650] );
            for j=1:3
                subplot(3,1,j, 'Color', 'w')
                pd{i}(j) = createFit(ts{i}(j), colorsR{j},i); 
                if j<3
                    set(gca,'XLabel',[]);
                end                
            end  
            %hold off;
            set(gcf,'Position',[400   15   560   650] );
        end 
    end

    function pd1 = createFit(s, c, sid)
        %   Creates a plot, similar to the plot in the main distribution fitting
        %   window, using the data that you provide as input.  You can
        %   apply this function to the same data you used with dfittool
        %   or with different data.  You may want to edit the function to
        %   customize the code and this help message.
        %
        %   Number of datasets:  1
        %   Number of fits:  1
        %
        %   See also FITDIST.

        % This function was automatically generated on 01-Aug-2017 09:46:51

        % Output fitted probablility distribution: PD1

        % Force all inputs to be column vectors
        sensor = s.Data;            
        sensor = sensor(:);
        if(sid == 1)
            sensor = sensor.*2048./8;
        elseif (sid == 2) 
            sensor = sensor.*131;
        elseif (sid == 3) 
            sensor = sensor./0.15./10^(-6);
        end
        legendStr = s.Name;

        % Prepare figure
        %clf;
        hold on;
        LegHandles = []; LegText = {};
        % --- Plot data originally in dataset 
        [CdfF,CdfX] = ecdf(sensor,'Function','cdf');  % compute empirical cdf
        BinInfo.rule = 1;
        [~,BinEdge] = internal.stats.histbins(sensor,[],[],BinInfo,CdfF,CdfX);
        [BinHeight,BinCenter] = ecdfhist(CdfF,CdfX,'edges',BinEdge);
        hLine = bar(BinCenter,BinHeight,'hist');
        set(hLine,'FaceColor','none','EdgeColor',[0.5 0.5 0.5],...
            'LineStyle','-', 'LineWidth',1);
        if(sid == 1)
            xlabel('Axis error [g]');
        elseif(sid == 2) 
            str = sprintf('%c', char(176));
            xlabel(['Axis error [',str,'/s]']);
        elseif(sid == 3)
            xlabel('Axis error [T]');
        end
        ylabel('Density')
        LegHandles(end+1) = hLine;
        LegText{end+1} = legendStr;

        % Create grid where function will be computed
        XLim = get(gca,'XLim');
        XLim = XLim + [-1 1] * 0.01 * diff(XLim);
        XGrid = linspace(XLim(1),XLim(2),100);

        % --- Create fit "fit 1"

        % Fit this distribution to get parameter values
        % To use parameter estimates from the original fit:
        %     pd1 = ProbDistUnivParam('normal',[ 0.02461074977422, 0.003084796738257])
        pd1 = fitdist(sensor, 'normal');
        YPlot = pdf(pd1,XGrid);
        hLine = plot(XGrid,YPlot,'Color',c,...
            'LineStyle','-', 'LineWidth',2,...
            'Marker','none', 'MarkerSize',6);
        LegHandles(end+1) = hLine;
        LegText{end+1} = ['fit ',legendStr(end)];

        % Adjust figure
        box on;
        hold off;

        % Create legend from accumulated handles and labels
        hLegend = legend(LegHandles,LegText,'Orientation', 'vertical', 'FontSize', 9, 'Location', 'northeast');
        set(hLegend,'Interpreter','none','location','northwest');
        grid on;
        end
end