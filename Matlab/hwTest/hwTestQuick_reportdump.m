%Autohor : Pawel Zalewski
%Date : 08/11/2017
%real time data plotting and saving utility
function hwTest
close all;
clear all;
instrreset
cd('..')
addpath(pwd)
cd('hwTest')
port='COM9';
royalb = 1/256*[65,105,225];
royalr = 1/255*[235,43,54];
royalg = 1/255*[0,104,87];
barc = [{'r'},{'g'},{'b'}];
colorsR = {royalb;royalr;royalg};

textBuff = [];
set(gcf,'units','points','position',[10,10,900,400]);
figureHandleMC = figure(1);

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
            
            txt=textscan(line,'%[^DATA]%s%d%d%d%d');
            %textBuff = [textBuff,txt];
            %get ID
            sampleID = double(txt{3}); 
           % a = [  )];
            % a
            tsIndex=1;
            s.data(1) = sampleID;
            s.data(2) = double(txt{4});
            s.data(3) = double(txt{5});
            s.data(4) = double(txt{6});
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
        label =  str2double(label);
        A = getAccuracy(ts, label);
        A
        %cd('..')
        %addpath(pwd)
        %cd('data_HW')
        %save(['dataHW_',label,datestr(now,'HH_MM'),'.mat'],'ts');
        %delete(gcf)
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
        
        legendStr={};
        %figure(1)
        subplot(1,2,1)
        cla reset
        hold on;
        sLength=length(samplesTimeSeries.Data(:,1));
        d = samplesTimeSeries.Data(:,1);
        d = d(end);
        str = 'bla';
        l = royalg;
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
        
        if(d < 50) 
            lejbel = [{'Standing'},{'Liying'},{'Sitting'}]; 
        elseif(d > 50 && d< 191)
            lejbel = [{'Walking'},{'Turn L'},{'Turn R'}];  
        elseif(d > 191) 
            lejbel = [{'Exercise'},{'Running'},{'Jumping'}];  
        end
                
        %set(t,'string',str);     
        %set(t,'color', l);
        
        if sLength>5
            windowSize=samplesTimeSeries.Time>(now-seconds(20));
            plotTs=getsamples(samplesTimeSeries,windowSize);
            %figure(1)
            plot(plotTs.Time,plotTs.Data(:,1),'color',l,'Linewidth',2)           
            dateFormat = 'SS';
            datetick('x',dateFormat)
            legendStr{end+1}=samplesTimeSeries.Name;            
        end      
        legend(legendStr);
        %title('On-board classification results');
        ylabel('Byte value');
        xlabel('Time [s]');
        grid on;        
        drawnow            
        subplot(1,2,2)
        cla reset
        if sLength>5     
           %figure(2)
           windowSize=samplesTimeSeries.Time>(now-seconds(20));
           plotTs=getsamples(samplesTimeSeries,windowSize);          
            x = plotTs.Data(:,2);
            y = plotTs.Data(:,3);
            z = plotTs.Data(:,4);
            %x = x(end);
            %y = y(end);
            %z = z(end);
            a = double([x y z]);
            ax = a(:,1);
            ay = a(:,2);
            az = a(:,3);
            s = ax(end) + ay(end) + az(end);          
            a = a./(156.25);
            %a = mat2gray(a);
            hold on;       
            for i = 1:3
                plot(plotTs.Time,a(:,i),'Linewidth',2 )           
                dateFormat = 'SS';
                datetick('x',dateFormat)
            end                
            hold off;            
        end    
        legend(lejbel);
        ylabel('Class probability');
        xlabel('Time [s]');
        ylim([0 1]);
        %title('Class probability');
        %h = gca;
        %h.XTickLabel = lejbel;
        %h.XTickLabelRotation = 45;
        % set(gca,'Xtick',1:1:3);
        grid on
        drawnow
    end        

    function a = getAccuracy(ts, label )
        ts = delsample(ts,'Index', 1:94);
        l = length (ts.Data);
        z = 0;
        for i=1:l 
            if (ts.Data(i) ~= label) 
                z = z + 1;
            end
        end
        a = 1- z/l;        
    end   
end