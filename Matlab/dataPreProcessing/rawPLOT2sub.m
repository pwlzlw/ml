function iGyro ()
    close all
    clear all
    
    royalb = 1/256*[65,105,225];
    royalr = 1/255*[235,43,54];
    royalg = 1/255*[0,104,87];
    colorsR = {royalb;royalr;royalg};
   
    acc = load('dataACCRAW_2.mat');
    gyro = load('dataGYRRAW_2.mat');
    mag = load('dataMAGRAW_2.mat');
    h = 1;
    fields = fieldnames(acc);   
    i =1;
    figure1=figure('Position', [400, 15, 800, 650]);
    for i=1:numel(fields)
        subplot(3,3,i);   
        %set(gca,'Position',[400   15   560   650] );
        plotRawData(acc.(fields{i}),i,'[ g ]');  
        if(i == 9 ||i == 8 ||i == 7)            
            xlabel('Seconds');
        end 
        set(gca,'YLabel', []);
        if(i == 1 || i ==4 || i==7) 
            ylabel('[ g ]'); 
        end         
        h=findobj(gcf,'type','axes');        
    end
    
    for k=1:9
     a=get(h(k),'xlim');
     xt=linspace(a(1),a(2),10);
     set(h(k),'xtick',xt);
     set(h(k),'XTickLabel',{'0','1','2','3','4','5','6','7','8','9'});
    end
    
    i=42;
    
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
                data = rawData{:,j}.*2048./8;;                       
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
        features.Properties.VariableNames = {'muX','muY','muZ','vaX','vaY','vaZ','siX','siY','siZ',...
            'maX','maY','maZ','miX','miY','miZ','meX','meY','meZ','Label'};
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
        %P=1;              
        %P =  figure(i); 
        hold on;
        theend = length( table{:,1});
        table([1:854,1000:theend],:) = [];
        sLength=length(table{:,1});
        for j=2:4                  
            %P = subplot(3,1,j-1);               
            %minL = min( table{:,j});
            %maxL = max( table{:,j});
            plot(table.Time-table.Time(1),table{:,j},'Color',colorsR{j-1}); 
            axis tight;
            %ylim([(minL-0.2) (maxL+0.2)]);
            dateFormat = 'SS';
            datetick('x',dateFormat);
            %xticks([00 01 02 03 04 05 06 07 08 09]);
            ylabel(ylegend);            
            %xlim([737101.704558750 737101.704727500]);
            %{ 
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
            %}
        end 
        %set( get(P,'XLabel'), 'String', 'Seconds' );
        %set(gca,'XLabel', 'Seconds'); 
        if(i == 3 || i == 6 || i == 9)
            legend('X','Y','Z');
        end
        grid on;
        hold off;
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

    
    function [om,rot] = computeGyro (acc,mag)   
            
            accX = acc{:,2};
            accY = acc{:,3};
            accZ = acc{:,4};
            
            alphaX = 0.0002;                   
            
            gravityR = 0;
            gravityX = [];
            gravityY = [];
            gravityZ = [];
            
            for i=1:length(accX) 
                gravityX(i)= (alphaX).*gravityR  + (1-alphaX).* accX(i); 
                gravityR = gravityX(i);
            end
            gravityR = 0;
            for i=1:length(accX) 
                gravityY(i)= (alphaX).*gravityR  + (1-alphaX).* accY(i); 
                gravityR = gravityY(i);
            end
            gravityR = 0;
            for i=1:length(accX) 
                gravityZ(i)= (alphaX).*gravityR  + (1-alphaX).* accZ(i); 
                gravityR = gravityZ(i);
            end
           
            accX = gravityX;
            accY = gravityY;
            accZ = gravityZ;
            
            magX = mag{:,2};
            magY = mag{:,3};
            magZ = mag{:,4};           
            
            R = [0 0 0;0 0 0;0 0 0];
            I = [0 0 0;0 0 0;0 0 0];
            Rreg = [0 0 0 ;0 0 0;0 0 0];
            Ireg = [0 0 0;0 0 0;0 0 0];
            Rotation = [];           
            
            for i=1:length(accX)             
                %omega{i} = getPitchRoll(accX(i),accY(i),accZ(i),magX(i),magY(i),magZ(i));                     
                [R,I] = getRotationMatrix(accX(i),accY(i),accZ(i),magX(i),magY(i),magZ(i), Rreg, Ireg); 
                %omega{i} =  getAngularVelocity(R,Rreg,1);
                %omega{i} = getAngleChange(R, Rreg); 20                
                Ireg = I;
                omega{i} = R - Rreg;
                Rreg = R;
                %get orientation
                Rotation{i} = getOrientation (R);  
            end
            rot = Rotation;
            om = omega;
    end    
end    