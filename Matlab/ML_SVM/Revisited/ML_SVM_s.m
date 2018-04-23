%Autohor : Pawel Zalewski, 
%Date : 30/11/2017
%feeature extraction, training and data plotting utility for kNN classifier
function ML_01() 
clear all;
close all;
% Constants
royalb = 1/256*[65,105,225];
royalr = 1/255*[235,43,54];
royalg = 1/255*[0,104,87];

colorsR = {royalb;royalr;royalg};
%Acquitre the data
cd('..')
cd('..')
cd('dataPreProcessing');
FeaturesACC = load('dataACCint.mat');
FeaturesIMA = load('dataIMA.mat');
gyroPCA = load('dataGYRint.mat');

Features = combineFeatures (FeaturesACC,FeaturesIMA);

lossvector = predictors();


lmu = mean(lossvector);
lstd = std(lossvector);

lmu
lstd

i = 42;

    function [lossv] = predictors()
        for j=1:1000
            [train,test] =  splitFeatures(Features);
            kNNModel = trainPlotKNN(train.lying,train.sitting,train.standing);        
            losst = validateKNN(test.lying,test.sitting,test.standing, kNNModel);        
            lossv(j) = 1 - losst;
        end
    end

    function losst = validateKNN(varargin)
        trainingTable = {};        
        for j=1:nargin-1
            data2Append = varargin{j};
            trainingTable = [trainingTable;data2Append];           
        end    
        labels = trainingTable(:,{'Label'});
        trainingTable(:,{'Label_P','Index'}) = []; 
        md = varargin{end};
        losst = loss(md,trainingTable);
    end    

    %compute the SVM and plot results
    function model = trainPlotKNN(varargin) 
        %combine features
        trainingTable = {};        
        for j=1:nargin
            data2Append = varargin{j};
            trainingTable = [trainingTable;data2Append];           
        end          
        labels = trainingTable(:,{'Label'});
        trainingTable(:,{'Label','Index','Label_P'}) = [];  
        t = templateSVM('KernelFunction', 'gaussian','KernelScale',5.4322,'BoxConstraint',15649,'Standardize',1);
        md = fitcecoc(trainingTable,labels,'Learners',t);%,'ClassNames',{'Exercise' 'Jumping' 'Running' });                    
        model = md;
    end    
    %feature extraction
    
    %convert cell to 3 vectors
    function [x,y,z] = cellToVector(cells) 
        x = cellfun(@(v)v(1),cells);
        y = cellfun(@(v)v(2),cells);
        z = cellfun(@(v)v(3),cells);
        x = x';
        y = y';
        z = z';        
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
            ylim([(minL-0.2) (maxL+0.2)]);
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
    function dataTS =  getData(files)        
        for i =1:length(files)
            filePath=files(i);
            filePath=filePath{1};
            %put all files into a struct
            dataStructs{i} = load(filePath);
            %only the accelerometer data
            dataTS{i} = dataStructs{i}.ts(1);
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


    function ftr = combineFeatures (varargin)
        s = [];               
        for j=1:nargin
            stru = varargin{j};
            s = [s stru];                              
        end         
        fields = fieldnames(s);
        features = struct();
        for i=1:nargin
            for k = 1:numel(fields)
                st = s(i).(fields{k});
                if i ~= nargin
                    st(:,19) = [];
                end
                if i == 1
                    features.(fields{k}) = [st];
                else 
                    features.(fields{k}) = [features.(fields{k}) st];
                end
            end            
        end        
        ftr = features;
    end
    function [train, test ] = splitFeatures (struct) 
        names = fieldnames(struct);   
        %rng('default');
        %rng(10);
        idx = randperm(1001);
        split_point = round(1001*0.5);
            for i=1:numel(names)
                name = names{i};
                tabl = struct.(name);
                train.(name) = tabl(idx(1:split_point),:); 
                test.(name) = tabl(idx(split_point+1:end),:);            
            end           
    end
end