% 0.1 ohm current sense resistor 20 June 2009

% Vbatt V0.1 I VbattADC IADC
d = [ 
[  8.0 0.121 1.15 36 99]
[  8.9 0.134 1.27 41 109]
[  9.8 0.148 1.40 44 121]
[ 11.1 0.167 1.58 50 136]
[ 12.7 0.191 1.80 58 153]
[ 13.1 0.198 1.87 59 159]  
]

figure(1)
plot(d(:,1), d(:,4))
ylabel("ADC V"); xlabel("Vbatt (V)");

figure(2)
plot(d(:,3), d(:,5))
ylabel('ADC I'); xlabel('I (A)');

figure(3)
plot(d(:,2), d(:,3))
xlabel('Vsense I'); ylabel('I (A)');

figure(4)
plot(d(:,2), d(:,5))
xlabel('Vsense I'); ylabel('ADC I');
grid

