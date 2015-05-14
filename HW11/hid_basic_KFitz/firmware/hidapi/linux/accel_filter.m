fileID = fopen('accels_raw.txt','r');
accels = fscanf(fileID,'%f');
fileID = fopen('accels.txt','r');
zfilt = fscanf(fileID,'%f');
hold all;
t = linspace(0,20,length(accels));

% Plot single-sided amplitude spectrum.
figure(1)
subplot(2,1,1)
plotFFT(accels)
subplot(2,1,2)
plot(t,accels)
%MAF

windowSize = 5;
b = (1/windowSize)*ones(1,windowSize)
a = 1
maf = filter(b,a,accels);
figure(2)
subplot(2,1,1)
plotFFT(maf)
subplot(2,1,2)
plot(t,maf)

% Plot single-sided amplitude spectrum.
figure(3)
subplot(2,1,1)
plotFFT(zfilt)
subplot(2,1,2)
plot(t,zfilt)

%figure(3)
%freqz(b)
