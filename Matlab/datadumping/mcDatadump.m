%Autohor : Original code by Antonis Vaefas and Pawel Zalewski, modified by Pawel Zalewski
%Date : 08/11/2017
%real time data plotting and saving utility
function mcDatadump
close all;
clear all;
cd('..')
addpath(pwd)
cd('datadumping')
port='COM9';

textBuff = [];
figureHandleMC = figure();
%button
btn = uicontrol('Parent',figureHandleMC,...
    'Position',[20 20 70 25],...
    'String','Close',...
    'Callback',@closeConnection);

% Defiitions
mc6470Mag=3;
icm20948=2;
mc3672=1;
ts=initTimeseries();
newSample=false;
loopExecuting=true;
%% Serial
serialInit(port);
%% main loop
startTime=now;
while(loopExecuting)
    if newSample
        plotTS(figureHandleMC,ts)
    end
end

    function instrcallback(obj,event)        
        
        [line, ~]= fscanf(obj);
        
        if size(line) ~=0
            txt=textscan(line,'%[^#]#%d%[^:]%s%s%[^:]%s%s%[^:]%s%s');
            %textBuff = [textBuff,txt];
            %get ID
            sampleID = int16(txt{2});        
            %get samples
            samples=[str2num(txt{5}{1}),str2num(txt{8}{1}),str2num(txt{11}{1})];

            switch sampleID
                case 1
                    samplesScaled=samples./2048.*8;%*78.456;
                    tsIndex=mc3672;
                case 2
                    samplesScaled=samples./131; %32768.0.*250;
                    tsIndex=icm20948;
                case 3
                    samplesScaled =samples.*0.15.*10^(-6);
                    tsIndex=mc6470Mag;
            end
            for i=1:3
                s.data=samplesScaled(i);
                s.time=now;
                ts{tsIndex}(i)=addsample(ts{tsIndex}(i),s);
            end
            newSample=true;
        end
    end

    function closeConnection(btn,callbackdata)
        loopExecuting=false;
        fclose(instrfind);
        %NB files must follow the format dataDump_X where X is the label for
        %the data
        prompt = {'Label for the data :'};
        label =  inputdlg(prompt);
        label = label{1};
        cd('..')
        addpath(pwd)
        cd('data')
        save(['dataDump_',label,datestr(now,'HH_MM'),'.mat'],'ts');
        delete(gcf)
    end

    function serialInit(port)
        serialObj=serial(port,'Timeout', 0.01);
        set(serialObj,'BaudRate',115200);
        try
            serialObj.BytesAvailableFcnCount = 63;
            serialObj.BytesAvailableFcnMode = 'byte';
            serialObj.Timeout = 0.01;
            %register callback to instrcallback function when 63 bytes are
            %available
            serialObj.BytesAvailableFcn = @instrcallback;
            fopen(serialObj);
        catch ME
            fclose(instrfind);
            serialObj.BytesAvailableFcnCount = 63;
            serialObj.BytesAvailableFcnMode = 'byte';
            serialObj.BytesAvailableFcn = @instrcallback;
            serialObj.Timeout = 0.01;
            fopen(serialObj);
        end
    end
    function ts=initTimeseries
        xAxisTS=1;
        yAxisTS=2;
        zAxisTS=3;        
        tsName{mc3672}='MC3672 Accelerometer';
        tsName{icm20948}='ICM20948 Gyroscope';        
        tsName{mc6470Mag}='MC6470 Magnetometer';
        axes{xAxisTS}='x';
        axes{yAxisTS}='y';
        axes{zAxisTS}='z';
        %Init timeseries
        for i=1:3
            for j=1:3
                ts{i}(j)=timeseries();
                ts{i}(j).TimeInfo.Format='HH:MM:SS.FFF';
                ts{i}(j).TimeInfo.StartDate=now;
                ts{i}(j).Name=[char(tsName{i}),' ',char(axes{j})];
            end
        end
    end
    function plotTS(figureHandleMC,samplesTimeSeries)
        cla reset
        legendStr={};
        hold on;
        for i=1:3
            if i==1
                yyaxis left
            else
                yyaxis right
            end
            for j=1:3
                sLength=length(samplesTimeSeries{i}(j).Data);
                if sLength>5
                    windowSize=samplesTimeSeries{i}(j).Time>(now-seconds(20));
                    plotTs=getsamples(samplesTimeSeries{i}(j),windowSize);
                    plot(plotTs)
                    dateFormat = 'SS';
                    datetick('x',dateFormat)
                    legendStr{end+1}=samplesTimeSeries{i}(j).Name;
                end
            end
        end
        if(sLength)>5
            legend(legendStr);
            yyaxis left
            ylabel('Accelerometer')
            yyaxis right
            ylabel('Magnetometer')
            drawnow
        end
    end
end