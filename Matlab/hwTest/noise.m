function processNoiseData ()
% Defiitions
mc6470Mag=1;
mc6470Acc=2;
mc3672=3;
folder = dir(pwd);
[~,datenumSort]=sort(cell2mat({folder.datenum}));
folderList = {folder(datenumSort).name};
noiseFileStr='noiseVars';
tsComplete=initTimeseries();
outFile='noiseCompleteVars.mat';
fileID = fopen(outFile);
if fileID>0
    load('noiseCompleteVars.mat')
else
    s=initialiseDataStruct();
    figure;
    s=combineSamples(folderList,s);
    s=addNames(s);
    save(outFile,'s');
end
fclose(fileID);
[stCell,stFile]=getStDeviation(s,folderList);
plotFiles(stFile);
%tsComplete=combineSamples(folderList);
%else
%

%tsComplete=addNames(tsComplete);
%S=load('noiseVars07_10.mat');
%tsComplete=addNames(S.ts);
%end


%standardDeviation=stdCalculation(s);
disp('Mission Complete')
    function plotFiles(fileNames)
        figure
        for i=1:length(fileNames)
            subplot(3,1,i)
            hold on;
            zoom xon;
            legendStr={};
            fileID = fopen(char(fileNames{i}));
            if fileID==0
                fclose(fileID);
                continue;
            end
            load(char(fileNames{i}));
            fclose(fileID);
            for j=1:3
                legendStr{end+1}=ts{i}(j).Name;
                histogram(ts{i}(j).Data,256);
            end
            legend(legendStr);
        end
            fileID = fopen(char(fileNames{i}));
            if fileID==0
                fclose(fileID);
                continue;
            end
            load(char(fileNames{i}));
            fclose(fileID);
            for j=1:3
                legendStr{end+1}=ts{i}(j).Name;
                histogram(ts{i}(j).Data,256);
            end
            legend(legendStr);
        end
    end
    function dataStruct=initialiseDataStruct()
        for i=1:3
            for j=1:3
                dataStruct{i}(j).Data=[];
                dataStruct{i}(j).Time=[];
                dataStruct{i}(j).StDeviation=[];
            end
        end
    end
    function [stDeviationCell,stRawDataFile]=getStDeviation(dataStruct,folderNames)
        stDeviationTxt={};
        stDeviationValue={};
        stRawDataFile={};
        for i=1:3
            for j=1:3
                stDeviationTxt{end+1}={dataStruct{i}(j).Name};
                minValue=min(dataStruct{i}(j).StDeviation...
                    (dataStruct{i}(j).StDeviation>0));
                index(j)=find(dataStruct{i}(j).StDeviation==minValue,1);
                stDeviationValue{end+1}=minValue;
            end
            [~,fileIndex]=histc(index,unique(index));
            stRawDataFile{end+1}=folderNames(index(fileIndex(1)));
        end
        
        stDeviationCell=[stDeviationTxt',stDeviationValue'];
    end
    function standardDeviation=stdCalculation(ts)
        figure;
        for i=1:3
            subplot(3,1,i)
            hold on;
            zoom xon;
            legendStr={};
            for j=1:3
                %Remove movement of sensors
                dataPoints=ts{i}(j).Data;
                
                [N,edges]=histcounts(dataPoints);
                [~,I]=max(N);
                lowerLim=edges(max(I-10,1));
                higherLim=edges(min(I+10,length(edges)));
                filteredData=dataPoints((dataPoints<higherLim)&(dataPoints>lowerLim));
                histogram(filteredData);
                legendStr{end+1}=ts{i}(j).Name;
                standardDeviation(i,j)=std(filteredData);
            end
            legend(legendStr);
        end
        
    end
    function ts=addNames(ts)
        xAxisTS=1;
        yAxisTS=2;
        zAxisTS=3;
        tsName{mc6470Mag}='MC6470 Magnetometer';
        tsName{mc6470Acc}='MC6470 Accelerometer';
        tsName{mc3672}='MC3672 Accelerometer';
        axes{xAxisTS}='x';
        axes{yAxisTS}='y';
        axes{zAxisTS}='z';
        %Init timeseries
        for i=1:3
            for j=1:3
                ts{i}(j).Name=[char(tsName{i}),' ',char(axes{j})];
            end
        end
    end
    function ts=initTimeseries
        xAxisTS=1;
        yAxisTS=2;
        zAxisTS=3;
        tsName{mc6470Mag}='MC6470 Magnetometer';
        tsName{mc6470Acc}='MC6470 Accelerometer';
        tsName{mc3672}='MC3672 Accelerometer';
        axes{xAxisTS}='x';
        axes{yAxisTS}='y';
        axes{zAxisTS}='z';
        %Init timeseries
        for i=1:3
            for j=1:3
                ts{i}(j)=timeseries();
                ts{i}(j).TimeInfo.Format='HH:MM:SS.FFF';
                %ts{i}(j).TimeInfo.StartDate=now;
                ts{i}(j).Name=[char(tsName{i}),' ',char(axes{j})];
            end
        end
    end
    function dataStruct=combineSamples(folderContents,dataStruct)
        for i=1:1:length(folderContents)
            fileName=char(folderContents(i));
            filePath=[pwd,'\',fileName];
            [pathstr, name, ext] = fileparts(filePath) ;
            fileID = fopen(filePath);
            try
                if ~strcmp(noiseFileStr,fileName(1:length(noiseFileStr)))
                    continue;
                end
            catch
                continue;
            end
            if fileID==0
                fclose(fileID);
                continue;
            end
            load(filePath);
            fclose(fileID);
            if isempty(ts{1}(1).Data)
                continue;
            end
            
            for tsIndex=1:3
                subplot(3,1,tsIndex)
                hold on;
                zoom xon;
                legendStr={};
                for j=1:3
                    dataStruct{tsIndex}(j).Data =[dataStruct{tsIndex}(j).Data;ts{tsIndex}(j).Data ];
                    dataStruct{tsIndex}(j).Time =[dataStruct{tsIndex}(j).Time;ts{tsIndex}(j).Time ];
                    dataStruct{tsIndex}(j).StDeviation(i)=std(ts{tsIndex}(j));
                    try
                        legendStr{end+1}=[ts{tsIndex}(j).Name,' ' , fileName(length...
                            (noiseFileStr)+1:length(noiseFileStr)+5)];
                        histogram(ts{tsIndex}(j).Data);
                    catch
                    end
                end
                %legend(legendStr);
            end
        end
        
        
    end
