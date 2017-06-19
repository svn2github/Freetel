README.txt
LilacSat-1 Rx Setup for AREG Club Project
Mark Jessop and David Rowe
June 2017

If the Gnu Radio build fails, (e.g. due to a missing package) you can
restart it from the beginning with:

$ cd prefix/default
$ pybombs rebuild

You can test the GNU Radio build by checking that gnuradio-companion starts:

$ source prefix/default/setup_env.sh
$ gnuradio-companion

TODO
----

[X] rebuild instructions
[ ] link to gnuradio build instructions 
[ ] way to throttle GR UDP input for testing
    + pipe viewer: cat file | pv -L 512k | nc -u 192.168.x.x 5000
[ ] HackRF test Tx
[ ] way to disable telemetry uploads
    + maybe modify script?
[ ] will it run fast enough on roadkill machines?
    [ ] play samples in real time from a HackRF
    [ ] real satellite pass
[ ] Wenet style start/stp scripts
[ ] heartbeat output to show all pcoresses running
