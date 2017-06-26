README.txt
LilacSat-1 Rx Setup AREG Club Project
Mark Jessop and David Rowe
June 2017

Pre-requisites
---------------

1/ A few hours of your time and an Internet connection.  Script 2 in
particular will take about two hours to run.

2/ Codec 2 is built in ~/codec2-dec/build

3/ SVN is installed

4/ Make sure you have no gnuradio, usrp or related packages
   installed.

Installation
------------

  $ cd ~
  $ svn co https://svn.code.sf.net/p/freetel/code/misc/lilacsat1
  $ cd lilacsat1
  lilacsat1$ ./1-install_packages.sh
  lilacsat1$ ./2-build_gnu_radio.sh
  lilacsat1$ ./3-build_gqrx_gr-satellites.sh

Running
-------

1/ In a Terminal start Codec 2 decoder running in the background:

  $ nc -lu 7000 | ~/codec2-dev/build/src/c2dec 1300 - - | aplay -f S16_LE&

2/ In a new Terminal start GNU Radio LilacSat-1 application running

   $ cd ~/prefix/default
   $ source setup_ev.sh
   $ gnuradio-companion
 
   + File-Open navigate to ~/prefix/default/src/gr-satellites/apps/lilacsat1.grc
   + Manually disable any missing blocks (right click - disable)
   + Click on Green Button, new terminal should start
   + see also lilacsat1/screenshots for other options

3/ In a new Terminal start gqrx

   $ cd ~/prefix/default
   $ source setup_ev.sh
   $ gqrx

   + Set up to talk to your SDR of choice, probably a RTL-SDR
   + To power your LNA from your RTL-SDR see screenshots/enable_bias_t.png 
   + Up top, look for settings icon. Make sure RX port set to 7356
   + Up top, make sure little 'Remote control via TCP' icon is selected (2 computers icon)
   + On 'Input Controls' tab, adjust Freq. Correction value as required.
      + A good idea would be to transmit a CQ signal from a 'known good' transmitter, and adjust the ppm value
        until the carrier is where it should be on the waterfall display.
   + In receiver options tab (on the right) set Mode to USB
   + Drag passband indication above waterfall so filter width is maybe 30 kHz wide.
   + Bottom right, click "..." Network tab, set UDP port to 7355, hostname to localhost
   + Suggest setting the 'main' audio output to a dummy audio device, you don't want to 
     hear the modem signal really.
   + Click 'UDP' to have it start sending samples out via UDP port.
   + Optionally, click 'Rec' to record the modem signal to disk (useful for later debugging)

4/ In a new Terminal start gpredict

   $ gpredict

   + Edit > Update TLE data -> From Network
   + Edit > Update Transponder data -> From Network
   + Edit -> Preferences -> Interfaces
     + Add new interface, radio type RX Only
     + Set port to 7356, hostname of localhost
   + Back on gpredict main window, top right look for down arrow, go to Radio control
     + Choose LilacSat-1 in target dropdown
     + Change downlink frequency from 436.510.000 Hz to 436.498.000 Hz (need a 12 kHz IF signal)
     + Choose gqrx from radio list, set cycle to 5000,
     + Click 'engage' then click 'track'
     + You should now see gqrx's frequency adjusting every 5 seconds.
     + Once you start seeing the downlink signal, you may need to adjust the 'nominal' downlink frequency so it sits at 
       12 kHz within the SSB passband. 

Build/Test Tips
---------------

1/ If the GNU Radio build fails, (e.g. due to a missing package, or
   legacy package) you can restart it from the beginning with:

  $ cd ~/prefix/default
  $ pybombs rebuild

2/ Test the GNU Radio build by checking that gnuradio-companion starts:

  $ source ~/prefix/default/setup_env.sh
  $ gnuradio-companion

3/ Test gqrx build by starting it:

  $ source ~/prefix/default/setup_env.sh
  $ gqrx

Links
-----

https://github.com/gnuradio/pybombs

HackRF Notes
------------

A HackRF can be used to generate a test signal.

1/ If you get persistent :hackrf not found" errors on a laptop, try
   adding the HackRF USB ID to the TLP USB blacklist:

  $ lsusb
  $ vi /etc/default/tlp
  $ /etc/init.d/tlp restart

2/ codec2-dec/octave script to generate Fs = 4MHz .iq file from 48kHz
   wave file

  octave:15> hackrf_uc("/home/david/tmp/misc/lilacsat1/lilacsat1.iq",
                       "/home/david/tmp/misc/lilacsat1/lilacsat1.wav");

3/ There is a 1MHz offset in the .iq file, plus 12kHz in the source wave file
   so tune gqrx to 441.012 MHz, this signal is about -70 dBm:

  $ hackrf_transfer -s 4000000 -t lilacsat1.iq -f 440000000 -a 1 -x 0

TODO
----

[X] rebuild instructions
[X] link to gnuradio build instruction
[X] HackRF test Tx
[X] will it run fast enough on roadkill machines?
    [X] play samples in real time from a HackRF
    [X] uses all 4 < 50% CPUs and runs OK
[ ] real satellite pass
[ ] Wenet style start/stop scripts on Desktop

