% capmult.m
% David Rowe 17 Dec 2015
%
% Working for capaitance multiplier (that maybe isn't a capacitance multipler)

Vi = 0.1/sqrt(2);
C = 1E-6;
Rs = 1;
Rl = 100;
Rb = 5000;
f = 100;

w = 2*pi*f;
Xc = 1/(w*C);

% Standard circuit, voltage division, ignore Rl as its >> Rs

Vo1 = Vi*j*Xc/(Rs+j*Xc);

% Cap Multiplier circuit, voltage division, ignore Rb as its big compared
% to Xc

Vo2 = Vi*j*Xc/(Rb+j*Xc);

printf("standard: %3.0f mVrms   Cap Mult: %3.0f mVrms\n", abs(Vo1)*1E3, abs(Vo2)*1E3);





