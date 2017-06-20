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

TODO
----

[X] rebuild instructions
[X] link to gnuradio build instruction
[ ] How to find and starta terminal (image)
[ ] what path?
    + lilacsat1/default/prefix too long?
[ ] install emacs as well, in case we need an editor
[ ] way to throttle GR UDP input for testing
    + pipe viewer: cat file | pv -L 512k | nc -u 192.168.x.x 5000
[ ] HackRF test Tx
[ ] way to disable telemetry uploads
    + maybe modify script?
[ ] will it run fast enough on roadkill machines?
    [ ] play samples in real time from a HackRF
    [ ] real satellite pass
[ ] Wenet style start/stop scripts on Desktop
[ ] heartbeat output to show all proceses running
