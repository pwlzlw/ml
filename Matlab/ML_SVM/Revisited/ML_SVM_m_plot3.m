%Autohor : Pawel Zalewski, 
%Date : 30/11/2017
%Kernel function plotter, the kernel function only works with standardized
%features. Corrected to interpolate the data from a high resolution grid.
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
FeaturesACC = load('dataACCs.mat');
FeaturesIMA = load('dataIMAs.mat');
gyroPCA = load('dataGYRint.mat');

rng(540); %544 540 533
Features = combineFeatures (FeaturesACC,FeaturesIMA);
[train,test] =  splitFeatures(Features);
kNNModel = trainPlotKNN(train.walking,train.turnR,train.turnL,...
train.running,train.jumping,train.exercising);      
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
    function model = trainPlotKNN(varargin) 
        %combine features
        trainingTable = {};        
        for j=1:nargin
            data2Append = varargin{j};
            trainingTable = [trainingTable;data2Append];           
        end         
        
        labels = trainingTable(:,{'Label_P'});
        trainingTable(:,{'Label','Index'}) = [];   
        trainingTable(:,[1:9,11:18]) = [];
        table = trainingTable;
        trainingTable(:,{'Label_P'}) = [];        
        md = fitcsvm(trainingTable,labels,'KernelFunction','gaussian','KernelScale',0.1,'BoxConstraint',102.86,'Standardize',0);
              
        markers = {'+','*','.','x','s','d','^','v','>','<','p','h'};         
        tblr = table;
        tbl = {};
        %get seperate classes
        tbl{1} = tblr(strcmp(tblr{:,end}, 'Moderate' ),:);
        tbl{2} = tblr(strcmp(tblr{:,end}, 'Rigorous' ),:);   
        a = tbl{1};
        b = tbl{2};
        a= a{:,1:2};
        b= b{:,1:2};    
        %get data from the model
        svs = md.SupportVectors; 
        numberSV = length(svs);
        numberSV
        svClass = md.SupportVectorLabels;        
        shift = md.ModelParameters.KernelOffset;
        scale = md.ModelParameters.KernelScale;
        bias = md.Bias;
        beta= md.Beta;
        alphas = md.Alpha;          
        
        %produce linear spaces for the frid to compute the function
        x = trainingTable{:,1};
        y = trainingTable{:,2};   
              
        x1a = a(:,1);
        x2a = a(:,2);    
        length(x1a)
        
        x11a = b(:,1);
        x21a = b(:,2);        
        length(x11a)
        
        len = 300;
        
        x1all = linspace(min(x), max(x), len);
        x2all = linspace(min(y), max(y), len);            
        
        x1al = linspace(min(x1a), max(x1a), len);
        x2al = linspace(min(x2a), max(x2a), len);
        
        x11al = linspace(min(x11a), max(x11a), len);
        x21al = linspace(min(x21a), max(x21a), len);
        
        %the b parameter
        f = ones(length(x1al), length(x2al))*bias;
        g = ones(length(x1al), length(x2al))*bias;
        u = ones(length(x1all), length(x2all))*bias;      
        
        [x1Grid,x2Grid] = meshgrid(x1all,x2all);
        
        %for every support vector and lagrangian multiplier get the
        %function value on the X Y grid
        for i=1:numberSV
            alphat = alphas(i);
            sv   = svs(i,:);
            sv = [sv(1);sv(2)];
            sign = svClass(i);
            %ze overall grid
            for j=1:length(x1all)
                for k=1:length(x2all)
                    tall = [x1all(j);x2all(k)];                    
                    xmxd2 = tall - sv;                    
                    u(j,k) = u(j,k) + alphat*sign*kernel(scale, tall, sv);                  
                end
            end    
            %ze data split to two classes grid
            for j=1:length(x1al)
                for k=1:length(x2al)                    
                    t = [x1al(j);x2al(k)];                    
                    t2 = [x11al(j);x21al(k)];                    
                    f(j,k) = f(j,k) + alphat*sign*kernel(scale, t, sv);
                    g(j,k) = g(j,k) + alphat*sign*kernel(scale, t2, sv);                                       
                end
            end              
        end         
        figure(1);
        %plot the surfaces, matlab's SVM has flipped co-ordinates
        hold on;
        h(4) = surf(x1Grid,x2Grid,u');
        shading interp;
        lighting phong;
        alpha 0.3;      
        %add a contour
        surfc(x1Grid,x2Grid,u');   
        shading interp;
        lighting phong;
        alpha 0.5;
        %plot support vectors 
        %su = griddata( x1all, x2all, u',svs(:,1),svs(:,2));        
        %plot a grid with less resolution otherwise the surfaces are black 
        sx = 44;
        x1Gridr = resizem(x1Grid,[sx sx]);
        x2Gridr = resizem(x2Grid,[sx sx]);
        u = griddata( x1Grid,x2Grid,u',x1Gridr,x2Gridr);    
        %u = resizem(u,[30 30]);
        h(5) = surf(x1Gridr,x2Gridr,u);
        h(5).FaceColor = 'None';
        alpha 0.5;        
        grid on;      
        %interpolate from high resolution grid into particular data points
        f = griddata( x1al, x2al, f',x1a,x2a);
        g = griddata( x11al, x21al, g',x11a,x21a);
        %plot original data with the f coordiantes 
        h(1) = plot3(x1a,x2a,f,markers{5},'Color','b','LineWidth',3.5);
        h(2) = plot3(x11a,x21a,g,markers{4},'Color',royalr','LineWidth',3.5); 
        %h(6) = plot3(svs(:,1),svs(:,2),su,'o','Color',[0 0 0 0.5],'MarkerSize', 12,'LineWidth',1.5);        
        %produce the contour grid
        d = 0.01;
        [x1Grid,x2Grid] = meshgrid(min(x):d:max(x),...
        min(y):d:max(y));
        xGrid = [x1Grid(:),x2Grid(:)];
        %predict from model the scores
        [~,scores] = predict(md,xGrid);           
        %get the decisiion boundary in f = 0 plane
        [C h(3)] = contour(x1Grid,x2Grid,reshape(scores(:,1),size(x1Grid)),[0 0],'Color',[0 0 0],'LineWidth',1.5);       
        predictorNames = md.PredictorNames;
        classNames = {'Moderate' 'Rigorous'};
        xlabel(predictorNames{1});
        ylabel(predictorNames{2});  
        hold off
        grid on;     
        cd('..')
        cd('epstopdf')        
        %legend(h([1 2 4 3 6]),[classNames,'Kernel function','Decision boundary','Support vector'],'Location','Best');  
        legend(h([1 2 4 3]),[classNames,'Kernel function','Decision boundary'],'Location','Best'); 
        %savefig('svm_kernel001');
        model = md;
    end    
    %feature extraction
    function k = kernel(scale, t, sv)
        k = exp(-(norm((t - sv)./scale))^2);
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
        split_point = round(1001*0.005);
            for i=1:numel(names)
                name = names{i};
                tabl = struct.(name);
                train.(name) = tabl(idx(1:split_point),:); 
                test.(name) = tabl(idx(split_point+1:end),:);            
            end           
    end
end