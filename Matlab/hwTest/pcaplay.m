close all
a = [0.8668052 0.919285776  0.9231962  0.9632376];

b = [135.9 884.1 1200 5400];


c = [a; b]';
figure(1);
plot(c(:,2), c(:,1), 'or');
hold on;
plot(c(:,2), c(:,1), '-b');

%dx = 0.1; 
%dy = 0.1;
txt = [{'EML A'},{'EML AG'},{'RAW A'},{'RAW AG'}];

a = [0.8698052 0.924285776  0.9221962  0.9679376];
b = [335.9 284.1 1450 5300];
c = [a; b]';


text(c(:,2) - 80, c(:,1), txt);

xlabel('Power usage [ \muW ]');
ylabel('Accuracy');
grid on;
[coeff,score,latent,explained]  = pca(c);
%biplot(coeff,'scores',score);

PrincipalComponent =  c*coeff; 
%figure(2);
%plot(PrincipalComponent(:,1),PrincipalComponent(:,2),'-r');
%grid on;
figure(2);

a = [0.9632376 0.9231962 0.919285776 0.8668052 ];
b = [2.82 12.43 17.3739 111.0471];


c = [a; b]';

plot(c(:,2), c(:,1), 'or');
hold on;
plot(c(:,2), c(:,1), '-b');

%dx = 0.1; 
%dy = 0.1;
txt = [{'RAW AG'},{'RAW A'},{'EML AG'},{'EML A'}];

a = [0.9632376 0.9271962 0.921285776 0.8748052 ];
b = [2.82 12.43 17.7739 106.0471];

c = [a; b]';


text(c(:,2)+2, c(:,1), txt);

xlabel('Time [ days ]');
ylabel('Accuracy');
grid on;
[coeff,score,latent,explained]  = pca(c);
%biplot(coeff,'scores',score);

PrincipalComponent =  c*coeff; 
%figure(2);
%plot(PrincipalComponent(:,1),PrincipalComponent(:,2),'-r');
%grid on;

i = 42;