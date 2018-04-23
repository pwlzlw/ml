%Autohor : Pawel Zalewski, 
%Date : 30/11/2017
%feeature extraction, training and data plotting utility for SVM classifier
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
FeaturesACC = load('dataACCintMK3.mat');
FeaturesIMA = load('dataIMAMK3.mat');
gyroPCA = load('dataGYRintMK3.mat');

Features = combineFeatures (FeaturesACC, FeaturesIMA);
[train,test] =  splitFeatures(Features);


%figure(3);
%trainPlotSVM(sittbl1,lyitbl1);
%md2 = trainPlotKNN(walking,turning,picking);
%figure(3)
kNNModel = trainPlotSVM(train.jumping,train.exercising);
%{
losst = validateKNN(test.lying,test.sitting,test.standing,...
    test.walking,test.turnR,test.turnL,...
    test.running,test.jumping,test.exercising, kNNModel);
%}
i = 42;

    function losst = validateKNN(varargin)
        trainingTable = {};        
        for j=1:nargin-1
            data2Append = varargin{j};
            trainingTable = [trainingTable;data2Append];           
        end    
        labels = trainingTable(:,{'Label_P'});
        trainingTable(:,{'Label','Index'}) = []; 
        md = varargin{end};
        losst = loss(md,trainingTable);
    end    

    %compute the SVM and plot results
    function model = trainPlotSVM(varargin) 
        %combine features
        trainingTable = {};        
        for j=1:nargin
            data2Append = varargin{j};
            trainingTable = [trainingTable;data2Append];           
        end          
        
        labels = trainingTable(:,{'Label'});
        trainingTable(:,{'Label','Index','Label_P'}) = [];       

        gamma = optimizableVariable('gamma',[1e-5,1e5],'Transform','log');
        kernel = optimizableVariable('kernel',{'linear','rbf','gaussian'});
        C = optimizableVariable('C',[1e-5,1e5],'Transform','log');
        cv = cvpartition(2002,'Kfold',10);
        
        fun = @(x)kfoldLoss(fitcsvm(trainingTable,labels,'CVPartition',cv,...
                 'KernelFunction','Gaussian','KernelScale',x.gamma,'BoxConstraint',x.C,...
                  'Standardize',1));                
                
        results = bayesopt(fun,[gamma,C],'Verbose',0,...
             'AcquisitionFunctionName','expected-improvement-plus','MaxObjectiveEvaluations',20)        
        
        %md = fitcecoc(trainingTable,labels,'Learners',t,'Coding','onevsall','OptimizeHyperparameters','auto',...
        %    'HyperparameterOptimizationOptions',struct('AcquisitionFunctionName',...
        %     'expected-improvement-plus'))
         
         
         
        %x=md.HyperparameterOptimizationResults; 
         
        figure(1);
        grid on;
        zlabel('Loss');
        title('');
        h = findobj(gcf, 'type', 'surface');
        %h.EdgeColor = 'none';
        h.FaceColor = 'g';
        h.FaceAlpha = 0.2; 
        x = bestPoint(results);
        x
        hold on;
        legend();
        %{
        hold on   
        x =h.XData(:);
        y =h.YData(:);
        z= h.ZData(:);
        [~,i] =  min(h.ZData(:));
        w = scatter3(x(i),y(i),z(i),'filled','MarkerFaceColor','r');
        w.SizeData = 200;
        w.MarkerFaceColor = 'r';
        hold off
        %}
        h1 = findobj(gca,'Type','line');
        sz = h1(1);
        
        hold on   
        x =sz.XData(:);
        y =sz.YData(:);
        z= sz.ZData(:);        
        w = scatter3(x,y,z,'filled','MarkerFaceColor','r');
        w.SizeData = 200;
        w.MarkerFaceColor = 'r';
        hold off        
        legend([h1(3) h w],{'Observed points','Model mean','Model minimum feasable'},'Position','NW');
        %md = fitcknn(trainingTable ,'Label_P','NumNeighbors',6);
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
        split_point = round(1001*1);
            for i=1:numel(names)
                name = names{i};
                tabl = struct.(name);
                train.(name) = tabl(idx(1:split_point),:); 
                test.(name) = tabl(idx(split_point+1:end),:);            
            end           
    end
end