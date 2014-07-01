% 0.1 ohm current sense resistor 20 June 2009

% Vbatt VbattADC IADC
d = [ 
[  9.2 118 ]
[ 10.0 129 ]
[ 11.1 142 ]
[ 12.2 156 ]
[ 13.2 169 ]
[ 14.0 179 ]
]

d1 = [ 
[ 12.12 157 ]
[ 12.00 155 ]
[ 11.86 153 ]
[ 11.68 151 ]
[ 11.47 148 ]
]

figure(1)
plot(d(:,1), d(:,2))
hold on
plot(d1(:,1), d1(:,2),'g')
hold off
ylabel("ADC V"); xlabel("Vbatt (V)");
grid
axis([11 13 140 170])

