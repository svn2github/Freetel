README.txt
LilacSat-1 Rx Setup for AREG Club Project
Mark Jessop and David Rowe
June 2017

Build/Test Tips
---------------

1/ If the GNU Radio build fails, (e.g. due to a missing package, or
legacy package) you can restart it from the beginning with:

  $ cd prefix/default
  $ pybombs rebuild

2/ Test the GNU Radio build by checking that gnuradio-companion starts:

  $ source prefix/default/setup_env.sh
  $ gnuradio-companion

3/ Test gqrx build by starting it:

  $ source prefix/default/setup_env.sh
  $ gqrx

Running
-------

# Now you have to make it all talk to each other :-)
# might need to ring me for that.

# GQRX
- Set up to talk to your SDR of choice.
- Up top, look for settings icon.
    - Make sure RX port set to 7356
- Up top, make sure little 'Remote control via TCP' icon is selected (2 computers icon)
- In receiver options tab (on the right) set Mode to USB
- Drag passband indication abover waterfall so filter width is maybe 30 kHz wide.
- Bottom right, click ... 
  - Network tab, set UDP port to 7355, hostname to localhost
- I'd also suggest setting the 'main' audio output to a dummy audio device, you don't want to hear the modem signal really.
- Click 'UDP' to have it start sending samples out via UDP port.

# Gpredict
- Set up as normal, update keps, set location, add lilacsat-1
- Edit -> Preferences -> Interfaces
  - Add new interface, radio type RX Only
  - Set port to 7356, hostname of localhost
- Back on gpredict main window, top right look for down arrow, go to Radio control
  - Choose LilacSat-1 in target dropdown
  - Set downlink freequency to 436.499.000 Hz
  - Choose gqrx from radio list, set cycle to 5000,
  - Click 'engage' then click 'track'
  - You should now see gqrx's frequency adjusting every 5 seconds.


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
[ ] How to find and start a terminal (image)
[ ] what path?
    + lilacsat1/default/prefix too long?
[ ] install emacs as well, in case we need an editor
[ ] way to throttle GR UDP input for testing
    + pipe viewer: cat file | pv -L 512k | nc -u 192.168.x.x 5000
[ ] way to disable telemetry uploads
    + maybe modify script?
[ ] real satellite pass
[ ] Wenet style start/stop scripts on Desktop
[ ] heartbeat output to show all proceses running
[ ] install feh
[ ] work out where rtlsdr is coming from
[ ] determine if any other blocks upload telemetry in classroom situation

