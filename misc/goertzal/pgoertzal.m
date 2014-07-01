% pgoertzal.m
% David Rowe
% 21 June 2014
%
% Helper Octave script to plot Goertzal test data

load pwr.txt; 
max_val = max(pwr(:,2));
max_val_dB = 10*log10(max_val);

figure(1)

subplot(211)
plot(pwr(:,1),10*log10(pwr(:,2)),'b;float;'); 
hold on;
plot(pwr(:,1),10*log10(pwr(:,3)),'g;goertzal3;'); 
hold off;
axis([0 250 max_val_dB-60  max_val_dB])
xlabel("Freq (Hz)");
grid

subplot(212)
st = 85/0.1;
en = 95/0.1;
plot(pwr(st:en,1),10*log10(pwr(st:en,2)),'b;float;'); 
hold on;
plot(pwr(st:en,1),10*log10(pwr(st:en,3)),'g;goertzal3;'); 
plot([89 90.59 90.59 92.42 92.42 94],[0 0 (max_val_dB-3) (max_val_dB-3) 0 0],'r;mask;')
hold off
axis([89 94 max_val_dB-12  max_val_dB])
xlabel("Freq (Hz)");
grid
