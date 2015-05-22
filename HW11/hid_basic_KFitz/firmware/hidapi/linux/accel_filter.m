
tdfread('accels.txt','\t')


hold all;
t = linspace(0,20,length(z));

% Plot single-sided amplitude spectrum.
figure(1)
subplot(2,1,1)
plotFFT(z)
subplot(2,1,2)
plot(t,z)
xlabel('time(sec)');
ylabel('Acceleration(g)');

figure(2)
subplot(2,1,1)
plotFFT(zmaf)
subplot(2,1,2)
plot(t,zmaf)
xlabel('time(sec)');
ylabel('Acceleration(g)');

figure(3)
subplot(2,1,1)
plotFFT(zfir)
subplot(2,1,2)
plot(t,zfir)
xlabel('time(sec)');
ylabel('Acceleration(g)');

%{
%MAF
windowSize = 5;
b = (1/windowSize)*ones(1,windowSize);
a = 1;
maf = filter(b,a,accels);
figure(2)
subplot(2,1,1)
plotFFT(maf)
subplot(2,1,2)
plot(t,maf)


% Plot FFT of C-code filter data.
figure(3)
subplot(2,1,1)
plotFFT(zfilt)
subplot(2,1,2)
plot(t,zfilt)

%FIR
b2 = [5 5 5 5 5 5 5 5 5 5 4 4]/58
figure(4)
freqz(b2)

fir = filter(b2,1,accels);
figure(2)
subplot(2,1,1)
plotFFT(fir)
subplot(2,1,2)
plot(t,fir)
%}


