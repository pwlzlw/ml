Ebat = 100*10^(-3)*3.7*3600;
PI = 37.37*10^(-6);

%raw data spi
Espixl = 0.5*10^(-6);
Espigy = 0.3*10^(-6);

%FIFO SPI
Espixl2 = 7.35*10^(-6);
Espigy2 = 4.43*10^(-6);

%machine learning convert to int16
Econvert = 0.48*10^(-6);
%data prepate for the advertisment: gaprole settings etc.
Edata = 12.01*10^(-6);
%energy of stage 1
Es1 = 0.88*10^(-6);
%energy of moderate with the gyroscope
EmoderateG = (1.41*10^(-6))*40; %worst case is the gyro
%without the gyro moderate
EmoderateA = (0.79*10^(-6))*40;
%sedentary
Esedentary = (0.9*10^(-6))*40;
%rigorous
Erigorous  = (0.92*10^(-6))*40;
%Features 
Emu = 0.87*10^(-6);
Eminmax = 0.16*10^(-6);
Emedian = 4.1*10^(-6);
Eima = 8.14*10^(-6);
Evar = 1.21*10^(-6);
Esigma = 1.00*10^(-6);

EfeaturesS1 = (3*Emu + 3*Eminmax*2 + 3*Emedian + 3*Esigma + 3*Evar + Eima);

EfeaturesS = 0;

EfeaturesMA = 0;

EfeaturesMG = (3*Emu + 3*Eminmax*2 + 3*Emedian + 3*Esigma + 3*Evar);

EfeaturesR = 0;

Pxl = 3.7*10^(-6);
Pgy = 3.7*8*10^(-6);
Pgy2 = 3.7*1.07*10^(-3);

Eble_single = 73.16*10^(-6);
fs3=14;
fs1 = 17;
fs2=1/1.15;


%RAW DATA acc 
T_A = Ebat/(PI + Pxl + (Espixl  + Edata + Eble_single)*fs3);
%hours
T_A = T_A/3600;
%days 
T_A = T_A/24;
%T_1


%Embedded machine learning moderate ACC only
T_EA = PI + Pxl + (Espixl2 + Eble_single + Econvert + Edata...
    + Es1 + EmoderateA + EfeaturesMA + EfeaturesS1)*fs2;


%Embedded machine learning moderate ACC + GYRO
T_EAG = PI + Pxl + Pgy2 + (Espixl2 + Espigy2 + Eble_single + Econvert + Edata...
    + Es1 + EmoderateG + EfeaturesMG + EfeaturesS1)*fs2;



%Embedded machine learning sedentary ACC only
T_S = PI + Pxl + (Espixl2 + Eble_single + Econvert + Edata...
    + Es1 + Esedentary + EfeaturesS + EfeaturesS1)*fs2;


%rIGOROUS
T_R = PI + Pxl + (Espixl2 + Eble_single + Econvert + Edata...
    + Es1 + Erigorous + EfeaturesR + EfeaturesS1)*fs2;


PEMLA = 0.808*T_S + 0.188*T_EA + 0.004*T_R
PEMLAG = 0.808*T_S + 0.188*T_EAG + 0.004*T_R


