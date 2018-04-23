%Autohor : Pawel Zalewski, 
%Date : 01/02/2018
%Random Forrest: loop.
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
FeaturesACC = load('dataACCintMK2.mat');
FeaturesIMA = load('dataIMA.mat');
gyroPCA = load('dataGYRint.mat');
cd('..');
cd('figures');
cd('Final_Figures');
Features = combineFeatures (FeaturesACC, FeaturesIMA);

[a,s] = getAccuracy();
a
s
    
    function [amu, astd] = getAccuracy()
        
        for j=1:1000
            
            [train,test] =  splitFeatures(Features);
        
            rfmodel = trainPlotRF(train.lying,train.sitting,train.standing,...
            train.walking,train.turnR,train.turnL,...
            train.running,train.jumping,train.exercising);

            losst  = validateRF(test.lying,test.sitting,test.standing,...
            test.walking,test.turnR,test.turnL,...
            test.running,test.jumping,test.exercising, rfmodel);
        
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
        
        trainingTable(:,[1,3:9,11:13,15:18]) = []; %muy,max,miy,IMA

        labels = trainingTable(:,{'Label_P'});
        trainingTable(:,{'Label','Index'}) = [];  
        
        md = varargin{end};        
        losst = error(md,trainingTable,'Mode','Ensemble');
        accuracy = losst(end);       
    end    
    %compute the RF model
    function model = trainPlotRF(varargin)
        %combine features from each class
        trainingTable = {};
        for j=1:nargin
            data2Append = varargin{j};
            trainingTable = [trainingTable;data2Append];           
        end         
        trainingTableMem = trainingTable;
         
        trainingTable(:,[1,3:9,11:13,15:18]) = []; %muy,max,miy,IMA
        
        labels = trainingTable(:,{'Label_P'});
        %get ird of extra info 
        trainingTable(:,{'Label','Index'}) = [];  
        
        md = TreeBagger(10,trainingTable,'Label_P','Method','classification',...
        'MaxNumSplits',5,'MinLeafSize',71,'NumPredictorsToSample',2);     
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