#!/bin/sh
# node_tune.sh
# David Rowe 13 April 2010
#
# Prints the IPs and signal strengths of nodes we can see.
#
# Run as a telnet login, e.g. on node 10.130.1.100
#   telnetd -l /usr/sbin/sigstr.sh -p 4950
# Test:
#  "telnet 10.130.1.100 4950"

  # list of nodes this node can see in IP and MAC order
  
  IP=`cat /proc/net/arp | grep "10.130.1" | awk '{ print $1 }'`
  MAC=`cat /proc/net/arp | grep "10.130.1" | awk '{ print $4 }'`
  
  echo $IP
  for m in $MAC
  do
    wlanconfig ath0 list | grep $m | awk '{ printf " %s",$6 }'
  done
  echo




