#!/bin/sh
# node_tune.sh
# David Rowe 13 April 2010
#
# Prints information useful for tuning a mesh node
#
# Useful to run as a telnet login, e.g.
#   telnetd -l /usr/sbin/node_tune.sh -p 4950
# will let you plug a laptop into eth0 and "telnet 192.168.1.20 4950"
# to run this script

  # list of nodes this node can see in IP and MAC order
  
  IP=`cat /proc/net/arp | grep "10.130.1" | awk '{ print $1 }'`
  MAC=`cat /proc/net/arp | grep "10.130.1" | awk '{ print $4 }'`
  
  echo
  echo "sigstr"
  echo $IP
  for m in $MAC
  do
    wlanconfig ath0 list | grep $m | awk '{ printf " %s",$6 }'
  done
  echo




