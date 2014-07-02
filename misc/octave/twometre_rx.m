% twometre_rx.m
% David Rowe
% 28 June 2014
%
% Working and simulations for 2m SDR rx

% Input tuned cct and matching from Fig 2-18(D) Borwick ----------------------------
%
%        /----+----\
%        |    |    |
%        C2   |    |
%        |    |    |
%  /-Rs--+    L    Rl
%  |     |    |    |
% (~)    C1   |    |
%  |     |    |    |
%  \-----+----+----/
%
% Purpose of input tuned cct is:
%   + Zmatch 50 ohms to 1500 ohms
%   + some sort of BPF
%   + have realisable component, e.g. L not too small

input_bpf = 0;

if input_bpf
  C1=68E-12;
  C2=15E-12;
  L=100E-9;
  Rs=50;

  Rl = Rs*(1+C1/C2)^2;
  C=1/(1/C1+1/C2);

  f=1/(2*pi*sqrt(L*C));
  Xl = 2*pi*f*L;
  Q = Rl/Xl;
  BW=f/Q;

  printf("C1 %2.0f pF C2 %2.0f pF L %3.0f nH Rs %2.0f Rl %4.0f f %5.2f MHz BW %3.2f MHz\n", ...
       C1*1E12, C2*1E12, L*1E9, Rs, Rl, f/1E6, BW/1E6);
end

% FM simulation ---------------------------------------------------------
%
% RF signal is mixed down to 0 to 500 kHz
% Analog filter 20dB down at 500 kHz
% Sample real signal Fs=1MS/s with single ADC
% test with interferer 25kHz away and 20dB higher
% complex mix down to baseband
% decimate down to Fs2=25kHz, (+/- 12.5kHz, a single 25kHz FM channel)
% FM demod
%
% TODO
%   [X] set deviation
%   [X] hard limiting
%   [X] FM demod
%   [ ] noise
%   [ ] attenuation
%   [ ] effect of ADC
%   [ ] model interferer
%   [X] Audio BPF
%   [ ] pre-emphasis/de-emphasis

% regular natural binary quantiser

function index = quantise_value(value, min_value, max_value, num_levels)
  norm = (value - min_value)/(max_value - min_value);
  index = floor(num_levels*norm + 0.5);
  index(find(index < 0)) = 0;
  index(find(index > (num_levels-1))) = num_levels-1;
endfunction

function value = unquantise_value(index, min_value, max_value, num_levels)
  step = (max_value - min_value)/num_levels;
  value = min_value + step*(index);
endfunction 

more off;

% constants -------------------------

Fs = 1E6;
fc = 250E3; wc = 2*pi*fc/Fs;
fm = 1E3; wm = 2*pi*fm/Fs;

f_max_deviation = 5E3; w_max_deviation = 2*pi*f_max_deviation/Fs;
t = 1:1E6;

fi = fc+50E3; wi = 2*pi*fi/Fs;
Ai = 10;

Fs2 = 50E3;
method = 2;
M=Fs/Fs2;
[b,a] = cheby1(8, 1, 20E3/Fs);

use_adc = 1;
clip = 100;
levels = 2^10;

noise_rms = 0.0;

% simulation -------------------------

% (real) signal at input to ADC

mod = cos(wm*t);
wt = wc*t + w_max_deviation*mod;
tx = cos(wt) + Ai*cos(wi*t) + noise_rms*randn(1,length(t));

% simulate ADC, or use just sign bit

if use_adc
  indexes = quantise_value(tx, -clip, clip, levels);
  rx = unquantise_value(indexes, -clip, clip, levels);
else
  rx = sign(real(tx)) + j*sign(imag(tx));
end

% downconvert to complex baseband and filter/decimate to Fs2

rx_bb   = rx .* exp(-j*wc*t);
rx_filt = filter(b, a, rx_bb);
rx_bb2  = rx_filt(1:M:length(rx_filt));

% limiter

rx_bb2 = sign(real(rx_bb2)) + j*sign(imag(rx_bb2));

% demodulate 

if method == 1

  % atan first, then differentiate.  However we need to unwrap
  % phase to avoid 0 -> 2*pi jumps which all gets a bit messy

  phase = unwrap(atan2(imag(rx_bb2),real(rx_bb2)));
  demod = M*2*pi*diff(phase);
end

% differentiate phase first in rect coords which avoids trig functions
% and nasty 0 -> 2*pi wrap around and allows approximation to avoid
% trig functions altogether.  Much smarter.

if method == 2

  % multiplying by complex conjugate subtracts phases
  %   exp(ja)*exp(jb)' = exp(ja)*exp(jb)'
  %                    = exp(ja)*exp(-jb)
  %                    = exp(j(b-a))

  l = length(rx_bb2);
  rx_bb_diff = rx_bb2(2:l) .* conj(rx_bb2(1:l-1));

  % for x small, tan(x) = sin(x) = x = imag(exp(jx))

  demod = 2*pi*M*imag(rx_bb_diff);

end

% 300-3000Hz BPF on demodualted audio

[b_bpf a_bpf] = cheby1(4, 2, [300/Fs2 3000/Fs2]);
audio_out = filter(b_bpf, a_bpf, demod);

% plot results --------------------------------------

figure(1)
clf
subplot(311)
plot(real(tx(1:100)))
ylabel('ADC input')
subplot(312)
plot(real(rx(1:100)))
ylabel('ADC output')
subplot(313)
plot(audio_out(1:400))
ylabel('Audio Out')
%axis([1 400 -1 1])

figure(2)
subplot(311)
[p,f] = pwelch(tx);
plot(f*1000,10*log10(p));
ylabel('ADC Input');
subplot(312)
[p,f] = pwelch(rx);
plot(f*1000,10*log10(p));
ylabel('ADC Output')
subplot(313)
[p,f] = pwelch(audio_out);
plot(f*50,10*log10(p));
ylabel('Audio Out');
xlabel('Freq (kHz)');

if 0
figure(3)
clf;
H=freqz(b,a,Fs/10);
HdB=20*log10(abs(H));
plot((1:Fs2/10)*10/1000, HdB(1:Fs2/10));
axis([1 Fs2/1000 -40 0]);
grid
end
