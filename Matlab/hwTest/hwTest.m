%Autohor : Pawel Zalewski
%Date : 08/11/2017
%real time data plotting and saving utility
function hwTest
close all;
clear all;
cd('..')
addpath(pwd)
cd('hwTest')
port='COM9';
royalb = 1/256*[65,105,225];
royalr = 1/255*[235,43,54];
royalg = 1/255*[0,104,87];
colorsR = {royalb;royalr;royalg};

textBuff = [];

figureHandleMC = figure(1);
        
t = annotation('textbox','FitBoxToText','on');
%annotation('rectangle','FaceColor',royalr,'FaceAlpha',.2)
sz = t.FontSize;
t.FontSize = 36;
t.Position = [0.18 0.8 0.1 0.1];
%button
btn = uicontrol('Parent',figureHandleMC,...
    'Position',[20 20 70 25],...
    'String','Close',...
    'Callback',@closeConnection);

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
            
            txt=textscan(line,'%[^DATA]%s%d%d');
            %textBuff = [textBuff,txt];
            %get ID
            sampleID = int16(txt{3});  
            tsIndex=1;
            s.data = sampleID;
            s.time = now;
            ts=addsample(ts,s);            
            newSample=true;            
        end
    end

    function closeConnection(btn,callbackdata)
        loopExecuting=false;
        fclose(instrfind);
        prompt = {'True label for the data :'};
        label =  inputdlg(prompt);
        label = label{1};
        cd('..')
        addpath(pwd)
        cd('data_HW')
        save(['dataHW_',label,datestr(now,'HH_MM'),'.mat'],'ts');
        delete(gcf)
    end

    function serialInit(port)
        serialObj=serial(port,'Timeout', 0.01);
        set(serialObj,'BaudRate',115200);
        try
            serialObj.BytesAvailableFcnCount = 63;
            serialObj.BytesAvailableFcnMode = 'byte';
            serialObj.Timeout = 0.01;
            %serialObj.Terminator = 'LF';
            %register callback to instrcallback function when 63 bytes are
            %available
            serialObj.BytesAvailableFcn = @instrcallback;
            fopen(serialObj);
        catch ME
            fclose(instrfind);
            serialObj.BytesAvailableFcnCount = 63;
            serialObj.BytesAvailableFcnMode = 'byte';
            serialObj.BytesAvailableFcn = @instrcallback;
            %serialObj.Terminator = 'LF';
            serialObj.Timeout = 0.01;
            fopen(serialObj);
        end
    end
    function ts=initTimeseries
        xAxisTS=1;
        axes{xAxisTS}='Class';
        tsName{1}='Class';
        ts = timeseries();
        ts.TimeInfo.Format='HH:MM:SS.FFF';
        ts.TimeInfo.StartDate=now;
        ts.Name='Encoded byte';
    end
    function plotTS(figureHandleMC,samplesTimeSeries)        
        cla reset
        legendStr={};
        figure(1);
        hold on;
        sLength=length(samplesTimeSeries.Data);
        d = samplesTimeSeries.Data;
        d = d(end);
        str = 'bla';
        if (d == 00) 
            str = 'Sitting';
            l = royalg;
        elseif (d == 16)
            str = 'Lying';
            l = royalg;
        elseif ( d == 48) 
            str = 'Standing';
            l = royalg;
        elseif (d == 64)
            str = 'Walking';
            l = royalb;
        elseif ( d == 68) 
            str = 'Turn L';
            l = royalb;
        elseif (d == 76)
            str = 'Turn R';
            l = royalb;
        elseif ( d == 192) 
            str = 'Jumping'; 
            l = royalr;
        elseif (d == 193)
            str = 'Exercise';
            l = royalr;
        elseif ( d == 195) 
            str = 'Running';  
            l = royalr;
        end             
        set(t,'string',str);     
        set(t,'color', l);
        if sLength>5
            windowSize=samplesTimeSeries.Time>(now-seconds(20));
            plotTs=getsamples(samplesTimeSeries,windowSize);
            plot(plotTs)
            dateFormat = 'SS';
            datetick('x',dateFormat)
            legendStr{end+1}=samplesTimeSeries.Name;
        end      
        legend(legendStr);
        title('On-board classification results');
        ylabel('Byte value');
        grid on;        drawnow    
        %figure(2);
        %thetext(str);
    end        
end