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
FeaturesACC = load('dataACCintMK3.mat');
FeaturesIMA = load('dataIMAMK3.mat');
gyroPCA = load('dataGYRint.mat');
cd('..');
cd('figures');
cd('Final_Figures');
Features = combineFeatures (FeaturesACC, FeaturesIMA);

[a,s] = getAccuracy();
a
s
    
    function [amu, astd] = getAccuracy()
        
        for j=1:100
            
            [train,test] =  splitFeatures(Features);
        
            rfmodel = trainPlotRF(train.walking,train.turnR,train.turnL);

            losst = validateRF(test.walking,test.turnR,test.turnL, rfmodel);
             accuracy(j) = 1 - losst;
        end
        amu = mean(accuracy);
        astd = std(accuracy);        
    end


    function accuracy = validateRF(varargin)
        %combine features
        trainingTable = {};
        for j=1:nargin-1
            data2Append = varargin{j};
            trainingTable = [trainingTable;data2Append];           
        end         
        trainingTableMem = trainingTable;
        
        trainingTable(:,[1:3,7:9,11:13,15,17:18]) = []; %all va, max,miny,mex,ima

        labels = trainingTable(:,{'Label'});
        trainingTable(:,{'Index','Label_P'}) = [];  
        md = varargin{end};        
        losst = error(md,trainingTable,'Mode','Ensemble');
        accuracy = losst(end);      
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
         
        trainingTable(:,[1:3,7:9,11:13,15,17:18]) = []; %all va, max,miny,mex,ima
        
        labels = trainingTable(:,{'Label'});
        trainingTable(:,{'Label_P','Index'}) = [];  
        
        md = TreeBagger(7,trainingTable,'Label','Method','classification',...
        'OOBPredictorImportance','on','SplitCriterion', 'gdi','MinLeafSize',47,'MaxNumSplits',5);       
        model = md;
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