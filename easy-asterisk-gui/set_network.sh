#!/bin/sh -x
# set_network.sh
#
# A system call from AJAM/Asterisk sets the network parameters of an IP0X using
# this script
#
# ./set_network yes|no [ipaddress] [netmask] [gateway] [dns]

echo `date` " set_network.sh $1 $2 $3 $4 $5" >> /tmp/easy_gui.log

if [ $1 == "yes" ]; then

  # DHCP

  if [ -f /etc/rc.d/S10network-static ]; then
    /etc/init.d/network-static disable
    /etc/init.d/network-static stop
    /etc/init.d/network enable
    /etc/init.d/network start
  else
    # if already running restart service
    /etc/init.d/network stop
    /etc/init.d/network start
  fi
fi

if [ $1 == "no" ]; then
  
  # Static IP

  # check correct number of arguments

  if [ $# -le 4 ]; then
    exit
  fi

  if [ -f /etc/rc.d/S10network ]; then
    /etc/init.d/network stop
    /etc/init.d/network disable
    /etc/init.d/network-static enable
  fi
  
  sed -i "s/DHCPD=.*/DHCPD=no/g" /etc/init.d/network-static
  sed -i "s/IPADDRESS=.*/IPADDRESS=\"$2\"/g" /etc/init.d/network-static
  sed -i "s/NETMASK=.*/NETMASK=\"$3\"/g" /etc/init.d/network-static
  sed -i "s/GATEWAY=.*/GATEWAY=\"$4\"/g" /etc/init.d/network-static
  sed -i "s/DNS=.*/DNS=\"$5\"/g" /etc/init.d/network-static
  /etc/init.d/network-static stop
  /etc/init.d/network-static start
  exit
fi

