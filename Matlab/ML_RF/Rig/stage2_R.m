%Autohor : Pawel Zalewski, 
%Date : 01/02/2018
%IMA !ACTUAL! Random Forrest: confusion matrix, predictor analysis and pca analysis.
function ML_01() 
delete(allchild(groot))
clear all;
close all;
% Constants
royalb = 1/256*[65,105,225];
royalr = 1/255*[235,43,54];
royalg = 1/255*[0,104,87];
colorsR = {royalb;royalr;royalg};

cd('..')
cd('..')
cd('dataPreProcessing');
FeaturesACC = load('dataACCint.mat');
FeaturesIMA = load('dataIMA.mat');
gyroPCA = load('dataGYRint.mat');
cd('..');
cd('figures');
cd('Final_Figures');
Features = combineFeatures (FeaturesACC, FeaturesIMA);

%split into train and test data
[train,test] =  splitFeatures(Features);

%Features = FeaturesIMA;
%{
pc = getPCA(Features.lying,Features.sitting,Features.standing,...
    Features.walking,Features.turnR,Features.turnL,...
    Features.running,Features.jumping,Features.exercising);
%}


rfmodel = trainPlotRF(train.running,train.jumping,train.exercising);

validateRF(test.running,test.jumping,test.exercising, rfmodel);

%}
%{
k=IMAPLOT(Features.lying,Features.sitting,Features.standing,...
    Features.walking,Features.turnR,Features.turnL,...
    Features.running,Features.jumping,Features.exercising);
%}
%{
plotIMAinPCspace(Features.lying,Features.sitting,Features.standing,...
    Features.walking,Features.turnR,Features.turnL,...
    Features.running,Features.jumping,Features.exercising,k);
%}    
%use multiple binary in one 
    %test the rf model
    function validateRF(varargin)
        %combine features
        trainingTable = {};
        for j=1:nargin-1
            data2Append = varargin{j};
            trainingTable = [trainingTable;data2Append];           
        end         
        trainingTableMem = trainingTable;
        
        %trainingTable(:,[1:3,7:9,11:13,15,17:18]) = []; %all va, max,miny,mex,ima
        trainingTable(:,[1,3:5,7:10,12:13,15:18]) = [];  %miy,mey: add may and mey to the one we already have

        labels = trainingTable(:,{'Label'});
        trainingTable(:,{'Label','Index','Label_P'}) = [];  
        md = varargin{end};
        oofLabel = predict(md,trainingTable);
        %numMisclass = sum(~strcmp(oofLabel,labels));  
        labels = table2cell(labels);
        confMat = confusionmat(labels,oofLabel,'order',{'Exercise' 'Jumping' 'Running'});    
        n = height(trainingTable(:,1));
        isLabels = unique(labels);
        isLabels = [ isLabels(3); isLabels(1); isLabels(2)];
        nLabels = numel(isLabels);        
        % Convert the integer label vector to a class-identifier matrix.
        [~,grpOOF] = ismember(oofLabel,isLabels); 
        oofLabelMat = zeros(nLabels,n); 
        idxLinear = sub2ind([nLabels n],grpOOF,(1:n)'); 
        oofLabelMat(idxLinear) = 1; % Flags the row corresponding to the class 
        [~,grpY] = ismember(labels,isLabels); 
        YMat = zeros(nLabels,n); 
        idxLinearY = sub2ind([nLabels n],grpY,(1:n)'); 
        YMat(idxLinearY) = 1; 
        figure(4);
        plotconfusion(YMat,oofLabelMat);
        h = gca;
        %title('Confusion matrix  - test set');
        title('');
        h.XTickLabel = [isLabels; {''}];
        h.YTickLabel = [isLabels; {''}];       
        set(findobj(gcf,'facecolor',[120,230,180]./255),'facecolor',royalg);
        set(findobj(gcf,'facecolor',[230,140,140]./255),'facecolor',royalr);
        set(findobj(gcf,'facecolor',[0.5,0.5,0.5]),'facecolor',[1 1 1]); 
         %cd('..')    
        cd('..')
        cd('..');
        cd('figures');
        cd('Final_Figures');
        %print('cmr','-dpng','-r300');
    end    
    %compute the RF and plot results
    function model = trainPlotRF(varargin)
        %combine features
        trainingTable = {};
        for j=1:nargin
            data2Append = varargin{j};
            trainingTable = [trainingTable;data2Append];           
        end         
        trainingTableMem = trainingTable;
         
        %trainingTable(:,[1:3,7:9,11:13,15,17:18]) = []; %all va, max,miny,mex,ima
        trainingTable(:,[1,3:5,7:10,12:13,15:18]) = [];  %miy,mey: add may and mey to the one we already have
        
        labels = trainingTable(:,{'Label'});
        trainingTable(:,{'Label_P','Index'}) = [];  
        
        md = TreeBagger(10,trainingTable,'Label','Method','classification',...
        'OOBPredictorImportance','on','MaxNumSplits',5,'MinLeafSize',74);    
                
        cd('..')    
        cd('..')
        cd('_RF_proper')    
        cd('Rig')
        %save(['forest.mat'], 'md');  

        %err = error(md,trainingTable);        
        oobErrorBaggedEnsemble = oobError(md);
        figure(1);
        plot(oobErrorBaggedEnsemble);
        grid on;
        xlabel 'Number of grown trees';
        ylabel 'Out-of-bag classification error';    
        view(md.Trees{1},'Mode','graph');
        view(md.Trees{2},'Mode','graph');
        view(md.Trees{3},'Mode','graph');
        view(md.Trees{4},'Mode','graph');
        view(md.Trees{5},'Mode','graph');
        view(md.Trees{6},'Mode','graph');
        view(md.Trees{7},'Mode','graph');
        view(md.Trees{8},'Mode','graph');
        view(md.Trees{9},'Mode','graph');
        view(md.Trees{10},'Mode','graph');
        oofLabel = oobPredict(md);
        labels = trainingTable(:,{'Label'});
        labels = table2cell(labels);
        confMat = confusionmat(labels,oofLabel,'order',{'Exercise' 'Jumping' 'Running'});     
        n = height(trainingTable(:,1));
        isLabels = unique(labels);
        nLabels = numel(isLabels);        
        % Convert the integer label vector to a class-identifier matrix.
        [~,grpOOF] = ismember(oofLabel,isLabels); 
        oofLabelMat = zeros(nLabels,n); 
        idxLinear = sub2ind([nLabels n],grpOOF,(1:n)'); 
        oofLabelMat(idxLinear) = 1; % Flags the row corresponding to the class 
        [~,grpY] = ismember(labels,isLabels); 
        YMat = zeros(nLabels,n); 
        idxLinearY = sub2ind([nLabels n],grpY,(1:n)'); 
        YMat(idxLinearY) = 1; 
        figure(2);
        plotconfusion(YMat,oofLabelMat);
        h = gca;
        title('Confusion matrix  - rigorous');
        h.XTickLabel = [isLabels; {''}];
        h.YTickLabel = [isLabels; {''}];       
        set(findobj(gcf,'facecolor',[120,230,180]./255),'facecolor',royalg);
        set(findobj(gcf,'facecolor',[230,140,140]./255),'facecolor',royalr);
        set(findobj(gcf,'facecolor',[0.5,0.5,0.5]),'facecolor',[1 1 1]);    
        %print('rig','-dpng','-r300');
        model = md;
    end    
    %plot PCA of the data
    function pc = getPCA(varargin)
            dataTable = {};
            for j=1:nargin
                data2Append = varargin{j};
                dataTable = [dataTable;data2Append];           
            end 
        dataTable(:,{'Label','Index','Label_P'}) = [];  
        %pcacandidates = [6,9,11,13,14,17];
        %dataTable(:,[1:5,7:8,12,15:16,18]) = [];
        %dataTable(:,[1:12,14:16,18]) = [];
        %dataTable(:,[2:12,14:16,18]) = [];     %mux mix mey ima 
        dataTable(:,[1:3,5,7:12,14:18]) = []; %vax vaz mix ima
        dataMatrix = table2array(dataTable);
        %dataMatrix = bsxfun(@minus,dataMatrix,mean(dataMatrix));
        [coeff,score,latent,explained]  = pca(dataMatrix);
        covarianceMatrix = cov(dataMatrix);
        covarianceMatrix
        a = [1,2,4];
        labels = dataTable.Properties.VariableNames;
        coeff
        biplot(coeff(:,a),'scores',score(:,a),'varlabels',labels);
        xlabel('1st Principal Component');
        ylabel('2nd Principal Component');
        eigenTbl = array2table(coeff);
        covTbl = array2table(covarianceMatrix);
        covTbl.Properties.VariableNames = labels;
        covTbl = [labels' covTbl];
        %eigenTbl.Properties.VariableNames = labels;
        eigenTbl = [labels' eigenTbl];
        %labels = [{' '} labels];        
        labels = eigenTbl.Properties.VariableNames;
        eigenTbl = table2cell(eigenTbl);
        %eigenTbl{1,1} = [' '];
        T = eigenTbl;        
        eigenTbl;
        pc = 1;
    end
    %normalize vector
    function fN =  normalizeFeature(featureV)
        %normalize
        %cellfun(@(v)v(1),featureV);
        maxf = max(featureV);
        minf = min(featureV);        
        fN = (featureV - minf)./(maxf-minf);
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
    %plot features fixing IMA
    function k = IMAPLOT(varargin)
        table = {};
        for j=1:nargin
            data2Append = varargin{j};
            table = [table;data2Append];           
        end 
        table(:,{'Label','Index'}) = [];         
        %table(:,[1:5,7:8,10,12,14:16,18]) = [];
        table(:,[2:12,14:16,18]) = [];
        noColumns = width(table) -1;
        hold off;
        markers = {'+','*','.','x','s','d','^','v','>','<','p','h'}; 
        tblr = table;
        tbl = {};
        tbl{1} = tblr(strcmp(tblr{:,end}, 'Sedentary' ),:);
        tbl{2} = tblr(strcmp(tblr{:,end}, 'Moderate' ),:);
        tbl{3} = tblr(strcmp(tblr{:,end}, 'Rigorous' ),:);           
        %pcacandidates = [6,9,11,13,14,17];
        for j=1:noColumns 
            figure(j);
            hold on;
            z=1;
            for n=1:3
                table = tbl{n};
                plot(table{:,z},table{:,j},markers{n},'Color',colorsR{n});  
                xlabel(table.Properties.VariableNames{z});    
                ylabel(table.Properties.VariableNames{j});
                legends(n) = table{:,end}(1);             
            end 
            legend(legends);
            grid on;
            hold off;
        end
        k = noColumns;
    end
    %combine features from multiple tables
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
    %plot features in the best resultant PCA spaces
    function plotIMAinPCspace(varargin) 
        table = {};
        for j=1:nargin-1
            data2Append = varargin{j};
            table = [table;data2Append];           
        end 
        tableM = table;
        table(:,{'Label','Index','Label_P'}) = []; 
        %table(:,[1:5,7:8,10,12,14:16,18]) = [];
        %table(:,[2:12,14:16,18]) = []; %mu mey etc..
        %table(:,[1,3:9,11:13,15:18]) = []; %ones prefered by RF
        table(:,[1:3,5,7:12,14:18]) = []; %vax vaz mix ima : ones solely based on variance
        dataMatrix = table2array(table);
        %substract themeans
        dataMatrix = bsxfun(@minus,dataMatrix,mean(dataMatrix));
        %get the coefficients
        [coeff,score,latent,explained]  = pca(dataMatrix);   
        %get data in principal component space
        PrincipalComponent =  dataMatrix*coeff;    
        tblr = array2table(PrincipalComponent);
        tblr = [tblr tableM(:,{'Label_P'})];
        %sort by primary label
        tbl{1} = tblr(strcmp(tblr{:,end}, 'Sedentary' ),:);
        tbl{2} = tblr(strcmp(tblr{:,end}, 'Moderate' ),:);
        tbl{3} = tblr(strcmp(tblr{:,end}, 'Rigorous' ),:);
        noColumns = width(table);
        hold off;
        markers = {'+','*','.','x','s','d','^','v','>','<','p','h'}; 
        %pcacandidates = [6,9,11,13,14,17];
        off = varargin{end}; 
        for j=1:noColumns
            figure(j+off);
            z = 1;
            hold on
            for n=1:3
                table = tbl{n};
                plot(table{:,z},table{:,j},markers{n},'Color',colorsR{n});  
                xlabel(table.Properties.VariableNames{z});    
                ylabel(table.Properties.VariableNames{j});
                legends(n) = table{:,end}(1);             
            end 
            legend(legends);
            grid on;
            hold off;            
        end
    end
    function [train, test ] = splitFeatures (struct) 
    names = fieldnames(struct);   
    %rng('default');
    %rng(10);
    idx = randperm(1001);
    split_point = round(1001*0.7);
        for i=1:numel(names)
            name = names{i};
            tabl = struct.(name);
            train.(name) = tabl(idx(1:split_point),:); 
            test.(name) = tabl(idx(split_point+1:end),:);            
        end           
    end
end