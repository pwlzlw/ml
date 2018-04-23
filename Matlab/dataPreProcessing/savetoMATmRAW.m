%Autohor : Pawel Zalewski, 
%Date : 30/11/2017
%feeature extraction and data trimming for themagnetometer data
function ML_01() 
clear all;
close all;
% Constants
royalb = 1/256*[65,105,225];
royalr = 1/255*[235,43,54];
royalg = 1/255*[0,104,87];
colorsR = {royalb;royalr;royalg};
acc = 1;
gyr = 2;
mag = 3;

%Acquitre the data
cd('..')
addpath(pwd)
cd('data')
folder = dir('*dataDump_*.mat');
[~,datenumSort]=sort(cell2mat({folder.datenum}));
fileList = {folder(datenumSort).name};
indexes = fileList';
%get gyroscope data ONLY
dataCellArray = getData(fileList, mag);
%%
sitting1 = dataCellArray{1};
sitting2 = dataCellArray{2};
sitting3 = dataCellArray{72}; %
sitting4 = dataCellArray{89}; %
sitting5 = dataCellArray{47};
sitting6 = dataCellArray{63};
sitting7 = dataCellArray{95};

lying1 = dataCellArray{3};
lying2 = dataCellArray{75}; %
lying3 = dataCellArray{83}; %
lying4 = dataCellArray{88}; %
lying5 = dataCellArray{48};
lying6 = dataCellArray{64};
lying7 = dataCellArray{98}; %

standing1 = dataCellArray{5};
standing2 = dataCellArray{104}; %
standing3 = dataCellArray{82}; %
standing4 = dataCellArray{87}; %
standing5 = dataCellArray{49}; 
standing6 = dataCellArray{62};
standing7 = dataCellArray{100}; %

turnL1 = dataCellArray{65};
turnL2 = dataCellArray{70};
turnL3 = dataCellArray{51};
turnL4 = dataCellArray{60};
turnL5 = dataCellArray{81}; %81
turnL6 = dataCellArray{85};
turnL7 = dataCellArray{94};

turnR1 = dataCellArray{66};
turnR2 = dataCellArray{71};
turnR3 = dataCellArray{52};
turnR4 = dataCellArray{61};
turnR5 = dataCellArray{79}; %79
turnR6 = dataCellArray{86};
turnR7 = dataCellArray{99};

walking1 = dataCellArray{26};
walking2 = dataCellArray{103}; %
walking3 = dataCellArray{78}; %
walking4 = dataCellArray{84}; %
walking5 = dataCellArray{50}; 
walking6 = dataCellArray{59};
walking7 = dataCellArray{96};

jumping1 = dataCellArray{9};
jumping2 = dataCellArray{102}; %
jumping3 = dataCellArray{80}; %
jumping4 = dataCellArray{34}; %
jumping5 = dataCellArray{54}; 
jumping6 = dataCellArray{91};
jumping7 = dataCellArray{97};

running1 = dataCellArray{31};
running2 = dataCellArray{67}; %
running3 = dataCellArray{76}; %
running4 = dataCellArray{90}; %
running5 = dataCellArray{53};
running6 = dataCellArray{58};
running7 = dataCellArray{93};

exercise1 = dataCellArray{37};
exercise2 = dataCellArray{68}; %
exercise3 = dataCellArray{77}; %
exercise4 = dataCellArray{92}; %
exercise5 = dataCellArray{55};
exercise6 = dataCellArray{56};
exercise7 = dataCellArray{101};

%turn into a table - we have raw data in a table format
sitting1 = getintoTables(sitting1);
sitting2 = getintoTables(sitting2);
sitting3 = getintoTables(sitting3);
sitting4 = getintoTables(sitting4);
sitting5 = getintoTables(sitting5);
sitting6 = getintoTables(sitting6);
sitting7 = getintoTables(sitting7);

lying1 = getintoTables(lying1);
lying2 = getintoTables(lying2);
lying3 = getintoTables(lying3);
lying4 = getintoTables(lying4);
lying5 = getintoTables(lying5);
lying6 = getintoTables(lying6);
lying7 = getintoTables(lying7);

standing1 = getintoTables(standing1);
standing2 = getintoTables(standing2); %redo
standing3 = getintoTables(standing3);
standing4 = getintoTables(standing4);
standing5 = getintoTables(standing5);
standing6 = getintoTables(standing6);
standing7 = getintoTables(standing7);

jumping1 = getintoTables(jumping1);
jumping2 = getintoTables(jumping2);
jumping3 = getintoTables(jumping3);
jumping4 = getintoTables(jumping4);
jumping5 = getintoTables(jumping5);
jumping6 = getintoTables(jumping6);
jumping7 = getintoTables(jumping7);

turnL1= getintoTables(turnL1);
turnL2= getintoTables(turnL2);
turnL3= getintoTables(turnL3);
turnL4= getintoTables(turnL4);
turnL5= getintoTables(turnL5);
turnL6= getintoTables(turnL6);
turnL7= getintoTables(turnL7);

turnR1= getintoTables(turnR1);
turnR2= getintoTables(turnR2);
turnR3= getintoTables(turnR3);
turnR4= getintoTables(turnR4);
turnR5= getintoTables(turnR5);
turnR6= getintoTables(turnR6);
turnR7= getintoTables(turnR7);

walking1= getintoTables(walking1);
walking2= getintoTables(walking2); %redo
walking3= getintoTables(walking3);
walking4= getintoTables(walking4);
walking5= getintoTables(walking5);
walking6= getintoTables(walking6);
walking7= getintoTables(walking7);

running1 = getintoTables(running1);
running2 = getintoTables(running2);
running3 = getintoTables(running3);
running4 = getintoTables(running4);
running5 = getintoTables(running5);
running6 = getintoTables(running6);
running7 = getintoTables(running7);

exercise1 = getintoTables(exercise1);
exercise2 = getintoTables(exercise2);
exercise3 = getintoTables(exercise3);
exercise4 = getintoTables(exercise4);
exercise5 = getintoTables(exercise5);
exercise6 = getintoTables(exercise6);
exercise7 = getintoTables(exercise7);

%truncate the crap out, tedious 
%2304 samples in each (72 x 32)


jumping1([1:96,2401:2507],:) = []; %jump
jumping2([1:96,2401:2583],:) = [];
jumping3([1:227,2532:2860],:) = [];
jumping4([1:227,2532:2959],:) = [];
jumping5([1:48,2353:2424],:) = [];
jumping6([1:227,2532:2698],:) = [];
jumping7([1:227,2532:2906],:) = [];

turnL1([1:48,2353:2461],:) = [];
turnL2([1:96,2401:2930],:) = [];
turnL3([1:96,2401:2693],:) = [];
turnL4([1:48,2353:2402],:) = [];
turnL5([1:227,2532:3027],:) = [];
turnL6([1:96,2401:2706],:) = [];
turnL7([1:96,2401:3003],:) = []; %dogdy

turnR1([1:96,2401:2515],:) = [];
turnR2([1:96,2401:2730],:) = [];
turnR3([1:48,2353:2403],:) = [];
turnR4([1:48,2353:2406],:) = [];
turnR5([1:227,2532:2879],:) = [];
turnR6([1:96,2401:2797],:) = [];
turnR7([1:227,2532:2789],:) = [];

walking1([1:96,2401:3578],:) = []; %walk
walking2([1:96,2401:2697],:) = [];
walking3([1:96,2401:3390],:) = [];
walking4([1:96,2401:2704],:) = [];
walking5([1:96,2401:2466],:) = [];
walking6([1:96,2401:2533],:) = [];
walking7([1:96,2401:2786],:) = [];

running1([1:96,2401:2729],:) = []; %run
running2([1:227,2532:2933],:) = [];
running3([1:227,2532:2717],:) = [];
running4([1:227,2532:2995],:) = [];
running5([1:96,2401:2454],:) = [];
running6([1:96,2401:2446],:) = [];
running7([1:227,2532:2624],:) = [];

exercise1([1:96,2401:3000],:) = []; %exercise
exercise2([1:96,2401:2934],:) = [];
exercise3([1:227,2532:2816],:) = [];
exercise4([1:96,2401:2690],:) = [];
exercise5([1:48,2353:2562],:) = [];
exercise6([1:96,2401:2427],:) = [];
exercise7([1:96,2401:2870],:) = [];

%plot raw data
%plotRawData(exercise5,1,'[ g ]',royalb);
%plotRawData(exercise6,2,'[ g ]',royalg);
%plotRawData(exercise7,3,'[ g ]',royalr);
%plotRawData(exercise4,4,'[ g ]',[0 0 0]);

%extract features: window 16 with 50% overlap, append primary label
%Sedentary, Moderate, Rigorous

windowF = 32;

sitting = [sitting1;sitting2;sitting3;sitting4;sitting5;sitting6;sitting7];


lying = [lying1;lying2;lying6;lying5;lying7;lying3;lying4];


standing = [standing1;standing3;standing6;standing5;standing4;standing7;standing2];


jumping = [jumping1;jumping6;jumping5;jumping4;jumping3;jumping7;jumping2];


running = [running1;running2;running6;running5;running3;running4;running7];


exercising = [exercise1;exercise2;exercise6;exercise5;exercise3;exercise4;exercise7];


walking = [walking1;walking7;walking6;walking5;walking3;walking4;walking2];


turnL = [turnL1;turnL2;turnL3;turnL4;turnL5;turnL6;turnL7];


turnR = [turnR1;turnR2;turnR3;turnR4;turnR5;turnR6;turnR7];
%}
cd('..')
cd('dataPreProcessing');
save(['dataMAGRAW_turnR.mat'],'turnR1','turnR2','turnR3','turnR4','turnR5','turnR6','turnR7');

%%
%feature extraction
    function features = extractFeatures(rawData,window,label_1)
        sLength=length(rawData.Time);
        %number of windows slide at 50%
        n = (sLength/window*2) - 1;         
        %n windows 
        for i=1:n
            %mmm hmmm index starting with 1 
            halfw = window/2;
            indexStart = ((i*halfw)-halfw+1);
            indexEnd = indexStart + window-1;
            label = rawData{:,5}; 
            time = rawData{:,1};
            %first column is actually the time
            %add time lol...
            for j=2:4 
                data = rawData{:,j};                       
                %for n each of x, y, z compute the features    
                %mean
                mu{i}(j-1) = mean(data(indexStart:indexEnd));
                %std
                sigma{i}(j-1) = std(data(indexStart:indexEnd));
                %variance
                variance{i}(j-1) = var(data(indexStart:indexEnd));
                %max
                maximum{i}(j-1) = max(data(indexStart:indexEnd));
                %min
                minimum{i}(j-1) = min(data(indexStart:indexEnd));
                %median
                med{i}(j-1) = median(data(indexStart:indexEnd));                   
                %spectral histogram                  
            end                   
        end
        len = length( mu );
        
        %labels
        labels = repmat(label(1),len,1);
             
        %x,y,z features
        [muX,muY,muZ] = cellToVector(mu);
        [siX,siY,siZ] = cellToVector(sigma);
        [vaX,vaY,vaZ] = cellToVector(variance);
        [maX,maY,maZ] = cellToVector(maximum);
        [miX,miY,miZ] = cellToVector(minimum);
        [meX,meY,meZ] = cellToVector(med);
        
        features = table(muX,muY,muZ,vaX,vaY,vaZ,siX,siY,siZ,maX,maY,maZ,miX,miY,miZ,meX,meY,meZ,labels);
        %features =table(muX,muY,muZ,vaX,vaY,vaZ,labels);
        %features =table(muX,muY,muZ,siX,siY,siZ,labels);
        %features.Properties.VariableNames = {'muX','muY','muZ','siX','siY','siZ','Label'};
        features.Properties.VariableNames = {'GmuX','GmuY','GmuZ','GvaX','GvaY','GvaZ','GsiX','GsiY','GsiZ',...
            'GmaX','GmaY','GmaZ','GmiX','GmiY','GmiZ','GmeX','GmeY','GmeZ','Label'};
    end
    %convert cell to 3 vectors
    function [x,y,z] = cellToVector(cells) 
        x = cellfun(@(v)v(1),cells);
        y = cellfun(@(v)v(2),cells);
        z = cellfun(@(v)v(3),cells);
        x = x';
        y = y';
        z = z';        
    end
    %standardize features
    function fN =  standardizeFeature(featureV)
        %zero-mean data
        sigma = std(featureV);
        mu = mean(featureV);
        fN = (featureV - mu)./sigma;
    end
    %normalize features
    function fN =  normalizeFeature(featureV)
        %normalize
        maxf = max(featureV);
        minf = min(featureV);        
        fN = (featureV - minf)./(maxf-minf);
    end
    %get ima 
    function IMA = getIMA(x,y,z)
        w_length =  length(x);
        sum = 0;
        for i=1:w_length 
            sum = sum + abs( sqrt( x(i)^2 + y(i)^2 + z(i)^2 ) -255);
        end
        IMA = sum/w_length;        
    end
    %outputs max of two numbers
    function max_ = max2(a,b) 
        maxa = max(a);
        maxb = max(b); 
        if maxa>maxb 
            max_ = maxa;
        else
            max_ = maxb;
        end
    end
    %outputs min of two numbers
    function min_ = min2(a,b) 
        maxa = min(a);
        maxb = min(b);
        if maxa<maxb 
            min_ = maxa;
        else
            min_ = maxb;
        end
    end
    %plot raw data from time series
    function plotRawData(table,i,ylegend,color)
        %cla reset;        
        legendStr={};       
        figure(i);        
        P=1;        
        for j=2:4                  
            P = subplot(3,1,j-1); 
            sLength=length(table{:,1});
            minL = min( table{:,j});
            maxL = max( table{:,j});
            plot(table.Time-table.Time(1),table{:,j},'Color',color); 
            axis tight;
            ylim([(minL-0.2*10^(-6)) (maxL+0.2*10^(-6))]);
            dateFormat = 'SS';
            datetick('x',dateFormat)
            set(gca,'YLabel', []);
            if(j == 2) 
                ylabel(ylegend); 
            end
            grid on;
            legendStr=table.Properties.VariableNames{j};
            legend(legendStr);    
            if(j ~= 4)
                set(gca,'XTickLabel',[]);              
            end                        
        end 
        set( get(P,'XLabel'), 'String', 'Seconds' );
        %set(gca,'XLabel', 'Seconds'); 
    end
    %gets data from multiple files and combines it into one array
    function dataTS =  getData(files, sensor)        
        for i =1:length(files)
            filePath=files(i);
            filePath=filePath{1};
            %put all files into a struct
            dataStructs{i} = load(filePath);
            %only the accelerometer data
            dataTS{i} = dataStructs{i}.ts(sensor);
            %get label from filename
            s = filePath(10:end-9);
            dataTS{i}(2) = {s};
        end   
    end
    %gets 1x3 timeseries and turns into a table of raw data
    function table_ = getintoTables(TSarray)
        sLength=length(TSarray{1}(1).Data);
        for i=1:3
            samples(i)=getsamples(TSarray{1}(i),1:sLength);
            time{i} = samples(i).Time;%samples(i).Time-samples(i).Time(1);
            data{i} = samples(i).Data;           
        end              
        labels(1:sLength) = {TSarray{2}};
        %create table time,x,label
        table_ = table(time{1},data{1},data{2},data{3},labels');
        table_.Properties.VariableNames = {'Time' 'X' 'Y' 'Z' 'Label'};
    end
    %plot features from table
    function plotFeatures(table,i,ylegend,color)
        %cla reset;        
        legendStr={};       
        figure(i);        
        P=1;        
        noColumns = width(table) -1;
        for j=1:noColumns                  
            P = subplot(3,1,j); 
            sLength=length(table{:,1});
            t = 1:+1:sLength;
            plot(t,table{:,j},'Color',color);
            set(gca,'YLabel', []);
            if(j == 1) 
                ylabel(ylegend); 
            end
            grid on;
            legendStr=table.Properties.VariableNames{j};
            legend(legendStr);    
            if(j ~= noColumns)
                set(gca,'XTickLabel',[]);              
            end            
            axis tight;
        end 
        set( get(P,'XLabel'), 'String', 'Feature #' );
    end
    %polot features as a 3 axis vector form table
    function plotFeatures3D(table,i,color)
        %cla reset;        
        legendStr={};       
        h = figure(i);        
        noColumns = width(table) -1;
        for j=1:3:noColumns                  
            P = subplot(3,1,j); 
            sLength=length(table{:,1});
            t = 1:+1:sLength;
            plot3(table{:,j},table{:,j+1},table{:,j+2});            
            xlabel(table.Properties.VariableNames{j});            
            ylabel(table.Properties.VariableNames{j+1}); 
            zlabel(table.Properties.VariableNames{j+2});
            ax = gca;
            ax.XTick = [-1.5:0.5:1.5];
            ax.YTick = [-1.5:0.5:1.5];
            ax.ZTick = [-1.5:0.5:1.5];           
            campos([-17,-17,23]);
            set(gcf, 'Units', 'pixels', 'Position', [20, 20, 400, 400]);
            ax.Position = [0.13 0.23 0.76 0.76];     
            set(gca,'BoxStyle','full','Box','on');  
            grid on;          
        end         
    end
    %polot features as a 3 axis vector for two sets from table
    function plotFeatures3Dtwo(tableA,tableB,i)
        %cla reset;        
        legendStr={};             
        noColumns = width(tableA) -1;
        for j=1:3:noColumns                  
           % P = subplot(3,1,j); 
            h = figure(i+j);  
            sLength=length(tableA{:,1});
            t = 1:+1:sLength;
            hold on;
            la = tableA{:,noColumns+1}(1);
            lb = tableB{:,noColumns+1}(1);
            plot3(tableA{:,j},tableA{:,j+1},tableA{:,j+2},'*','displayname',la{1},'color',royalb);  
            plot3(tableB{:,j},tableB{:,j+1},tableB{:,j+2},'o','displayname',lb{1},'color',royalr); 
            legend(la{1},lb{1});   
            hold off;
            xlabel(tableA.Properties.VariableNames{j});            
            ylabel(tableA.Properties.VariableNames{j+1}); 
            zlabel(tableA.Properties.VariableNames{j+2});
            ax = gca;
            maxL = max2(tableA{:,j},tableB{:,j});
            minL = min2(tableA{:,j},tableB{:,j});
            ax.XTick = [minL:maxL];
            maxL = max2(tableA{:,j+1},tableB{:,j+1});
            minL = min2(tableA{:,j+1},tableB{:,j+1});
            ax.YTick = [minL:maxL];
            maxL = max2(tableA{:,j+2},tableB{:,j+2});
            minL = min2(tableA{:,j+2},tableB{:,j+2});
            ax.ZTick = [minL:maxL];                   
            %set(gca,'BoxStyle','full','Box','on');  
            campos([-17,-17,23]);
            set(gcf, 'Units', 'pixels', 'Position', [20, 20, 400, 400]);
            ax.Position = [0.13 0.23 0.76 0.76];   
            grid on;          
        end         
    end
end