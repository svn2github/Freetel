% fsk4.mf
% 4FSK modem attempt from the DMR spec

graphics_toolkit("gnuplot");

fm;

% Frequency response of the DMR raised cosine filter 
% from ETSI TS 102 361-1 V2.2.1 page 111
global fsk4_rcf_resp = @(f) 1.0*(f<=1920) - cos((pi*f)/1920).*1.0.*(f>1920 & f<=2880);

%Maximum positive deviation of amy 4FSK symbol
global fsk4_max_deviation = 1944;

%Deviation of the FSK symbols
global fsk4_symbols = [-1944 -648 648 1944];

function fsk4_states = fsk4_init(fsk4_states,Rs)
    global fsk4_max_deviation;
    global fsk4_symbols;
    global fsk4_rcf_resp;

    Fs = fsk4_states.Fs = 48000;  %Sample rate
    Rs = fsk4_states.Rs = Rs;     %Symbol rate
    M = fsk4_states.M = fsk4_states.Fs/fsk4_states.Rs; %Samples per symbol
    
    %Set up 4FSK raised cosine filter
    rf = (0:(Fs/2));
    tx_filter = fir2(100 ,rf/(Fs/2),fsk4_rcf_resp(rf));
    fsk4_states.tx_filter = tx_filter;
    %Set up the 4FSK symbols
    fsk4_states.symmap = fsk4_symbols / fsk4_max_deviation;
    
    fm_states.Ts = M;
    fm_states.Fs = Fs;
    fm_states.fc = 0;
    fm_states.fm_max = fsk4_max_deviation*2;
    fm_states.fd = fsk4_max_deviation;
    fm_states.pre_emp = fm_states.de_emp = 0;
    fm_states.output_filter = 1;
    fsk4_states.fm_states = analog_fm_init(fm_states);

endfunction 

function d = idmp(data, M, offset)
    d = zeros(1,length(data)/M);
    for i = 1:length(d)
      d(i) = sum(data(1+(i-1)*M:i*M));
    end
endfunction

function [tx, tx_filt] = fsk4_mod(fsk4_states, tx_bits)
  hbits = tx_bits(1:2:length(tx_bits));
  lbits = tx_bits(2:2:length(tx_bits));
  %Pad odd bit lengths
  if(length(hbits)!=length(lbits))
    lbits = [lbits 0]
  end
  tx_symbols = lbits + hbits*2 + 1;
  M = fsk4_states.M;
  nsym = length(tx_symbols);
  nsam = nsym*M;

  tx_stream = zeros(1,nsam);
  for i=1:nsym
    tx_stream(1+(i-1)*M:i*M) = fsk4_states.symmap(tx_symbols(i));
  end
  tx_filt = filter(fsk4_states.tx_filter, 1, tx_stream);
  tx_filt = tx_filt / max(tx_filt);
  tx = analog_fm_mod(fsk4_states.fm_states, tx_filt);
endfunction

%non-coherent demod based on a paper I found on IEEE xplore. Paper claims it is ~1db under coherent.
% I don't think it works
% Paper is titled "ALL- DIGITAL PSEUDO- COHERENT (PC) FSK MODEMS"
function sym = fsk4_demod_thing(fsk4_states, rx)
  global fsk4_symbols;
  Fs = fsk4_states.Fs;
  t = (1:length(rx));
  shiftup = exp(j*2*pi*(1/4)*t);

  rx_up = real(rx.*shiftup);
  symup = fsk4_symbols + Fs/4;
  sym1m = exp(j*2*pi*(symup(1)/Fs)*t).*rx_up;
  sym2m = exp(j*2*pi*(symup(2)/Fs)*t).*rx_up;
  sym3m = exp(j*2*pi*(symup(3)/Fs)*t).*rx_up;
  sym4m = exp(j*2*pi*(symup(4)/Fs)*t).*rx_up;
  sym1m = idmp(sym1m,10); sym1m = (real(sym1m).^2+imag(sym1m).^2);
  sym2m = idmp(sym2m,10); sym2m = (real(sym2m).^2+imag(sym2m).^2);
  sym3m = idmp(sym3m,10); sym3m = (real(sym3m).^2+imag(sym3m).^2);
  sym4m = idmp(sym4m,10); sym4m = (real(sym4m).^2+imag(sym4m).^2);
  sym = sym1m*-3 + sym2m*-1 + sym3m*1 + sym4m*3;
  plot(abs(sym)(1:2000));
  hist(sym(1:2:length(sym)),30);
endfunction

%incoherent demod loosly based on another paper. Works, more or less.
% Paper is titled "Design and Implementation of a Fully Digital 4FSK Demodulator"
function [bits err] = fsk4_demod_fmrid(fsk4_states, rx)
  rxd = analog_fm_demod(fsk4_states.fm_states,rx);
  
  %rx_filt = filter(fsk4_states.tx_filter, 1, rxd); 
  rx_filt=rxd;
  sym = afsym = idmp(rx_filt,fsk4_states.M/2);

  % Demod symbol map. I should probably figure a better way to do this.
  % After integrating, the high symbol tends to be about 7.5
  dmsyms = rot90(fsk4_states.symmap * 7.5);

  oddsyms  = afsym(1:2:length(afsym));
  evensyms = afsym(2:2:length(afsym));
  [errseven,deceven] = min(abs(evensyms - dmsyms));
  [errsodd ,decodd ] = min(abs(oddsyms  - dmsyms));

  terreven = mean(errseven)
  terrodd  = mean(errsodd )

  if terreven < terrodd
    sym = deceven;
    err = errseven;
  else
    sym = decodd;
    err = errsodd;
  end
  bits = zeros(1,length(sym)*2);
  %Translate symbols back into bits
  for i=1:length(sym)
    bits(1+(i-1)*2:i*2) = [[1 1];[1 0];[0 1];[0 0]](sym(i),(1:2));
  end
endfunction

function ber = nfbert(rxoffset)
  bitcnt = 48000;
  offset = 29;
  test_bits = [zeros(1,100) rand(1,bitcnt)>.5]; %Random bits. Pad with zeros to prime the filters
  fsk4_states.M = 1;
  fsk4_states = fsk4_init(fsk4_states,2400);
  txrf = fsk4_mod(fsk4_states,test_bits);
   
  txrf = [txrf(offset:length(txrf)) zeros(1,offset)];
  %add noise here
  
  [rx_bits,rx_err] = fsk4_demod_fmrid(fsk4_states,txrf);
 % for offset = (25:31)
 %  offset
   ber = sum(xor(rx_bits(offset:length(rx_bits)),test_bits(1:length(rx_bits)+1-offset)))/(bitcnt-offset);
 % end
  plot((1:1000),rx_bits(1:1000),(1:1000),rx_err(1:1000));
endfunction

