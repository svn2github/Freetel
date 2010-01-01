#!/bin/sh -x
# get_network.sh
#
# Extracts network config into /etc/asterisk/network.conf in a form
# that the AJAM getconfig can read.

F="/etc/asterisk/network.conf"
echo "[network]" > $F

if [ -f /etc/rc.d/S10network ]; then
  echo "dhcp=yes" >> $F  
  tmpipaddr=`ifconfig eth0 | sed -n 's/.*inet addr:\(.*\)\s*Bcast.*/\1/p'`
  tmpnetmask=`ifconfig eth0 | sed -n 's/.*Mask:\(.*\)\s*/\1/p'`
  tmpgateway=`route -n | awk '/^0.0.0.0/ {print $2}'`
  tmpdns=`cat /etc/resolv.conf | awk '/^nameserver/ {print $2}'`
  echo "ipaddress=$tmpipaddr" >> $F
  echo "netmask=$tmpnetmask" >> $F
  echo "gateway=$tmpgateway" >> $F
  echo "dns=$tmpdns" >> $F
fi

if [ -f /etc/rc.d/S10network-static ]
then
  echo "dhcp=no" >> $F
  
  tmpipaddr=`sed -n 's/IPADDRESS="\(.*\)"/\1/p' /etc/init.d/network-static`
  tmpnetmask=`sed -n 's/NETMASK="\(.*\)"/\1/p' /etc/init.d/network-static`
  tmpgateway=`sed -n 's/GATEWAY="\(.*\)"/\1/p' /etc/init.d/network-static`
  tmpdns=`sed -n 's/DNS="\(.*\)"/\1/p' /etc/init.d/network-static`
  echo "ipaddress=$tmpipaddr" >> $F
  echo "netmask=$tmpnetmask" >> $F
  echo "gateway=$tmpgateway" >> $F
  echo "dns=$tmpdns" >> $F          
fi
 
