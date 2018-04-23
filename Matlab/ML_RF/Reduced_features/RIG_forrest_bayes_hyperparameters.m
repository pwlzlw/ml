%Autohor : Pawel Zalewski, 
%Date : 01/02/2018
%IMA random forrest: confusion matrix, predictor analysis and pca analysis.
function ML_01() 
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


    bayes(Features.running,Features.jumping,Features.exercising);

    function bayes(varargin)
        %combine features
        trainingTable = {};
        for j=1:nargin
            data2Append = varargin{j};
            trainingTable = [trainingTable;data2Append];           
        end         
        trainingTableMem = trainingTable;
        %trainingTable(:,[1:9,11:18]) = [];
        %trainingTable(:,[2:12,14:16,18]) = [];     %mux mix mey ima 
        %trainingTable(:,[2:9,11:16,18]) = [];     %mux max mey ima 
        %trainingTable(:,[1:9,11:13,15:18]) = []; %mu,max,miy,ima
        %trainingTable(:,[1:18]) = []; %max,ima
        %trainingTable(:,[1:5,7:18]) = []; %vaZ
        %trainingTable(:,[1:5,7:12,14:18]) = []; %vaZ + miX
        %trainingTable(:,[1:3,5,7:12,14:18]) = []; %vaZ + miX + vaX
        %trainingTable(:,[1:7,11:13,15:18]) = []; %siY siZ maX miY IMA
        %trainingTable(:,[1,3,5:10,14:17]) = [];
        %trainingTable(:,[1,2:6,7:10,12:13,15,18]) = [];
        %trainingTable(:,[1,3:5,7:10,12:13,15:18]) = [];

        labels = trainingTable(:,{'Label'});
        trainingTable(:,{'Label_P','Index',}) = [];  
       
        maxMinLS = 143;
        minLS = optimizableVariable('Leaf',[15,143],'Type','integer');
        minSP = optimizableVariable('Size',[1,20],'Type','integer');
        numPTS = optimizableVariable('numPTS',[1,size(trainingTable,2)-1],'Type','integer'); 
        %minTree = optimizableVariable('minTree',[1,5],'Type','integer');
        hyperparametersRF = [minLS;minSP];
        
        results = bayesopt(@(params)oobErrRF(params,trainingTable),hyperparametersRF,'Verbose',0,'MaxObjectiveEvaluations',40);       
        besthyperparameters = bestPoint(results)       
    end    

    %function to optimize
    function oobErr = oobErrRF(params,X)  
        randomForest = TreeBagger(100,X,'Label','method','classification','OOBPrediction','on','MinLeafSize',params.Leaf,...
        'MaxNumSplits',params.Size); 
        oobErr =oobError(randomForest,'Mode','ensemble');
        %randomForest.OOBPermutedPredictorDeltaError
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
  
    %split data into train and test
    function [train, test ] = splitFeatures (struct) 
    names = fieldnames(struct);   
    rng('default');
    rng(5);
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