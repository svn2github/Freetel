#!/bin/sh
# set_network.sh
#
# CGI to set network parameters of an IP0X.

cat <<EOF
<html>
<head>
<title>set_network.sh</title>
<meta http-equiv="REFRESH" content="0;url=http:network.sh">
<body>
Please wait a few seconds.....
</body>
</head>
</html>
EOF

dhcp=`echo "$QUERY_STRING" | grep -oe "dhcp=[^&?]*" | sed -n "s/dhcp=//p"`
ipaddress=`echo "$QUERY_STRING" | grep -oe "ipaddress=[^&?]*" | sed -n "s/ipaddress=//p"`
netmask=`echo "$QUERY_STRING" | grep -oe "netmask=[^&?]*" | sed -n "s/netmask=//p"`
gateway=`echo "$QUERY_STRING" | grep -oe "gateway=[^&?]*" | sed -n "s/gateway=//p"`
dns=`echo "$QUERY_STRING" | grep -oe "dns=[^&?]*" | sed -n "s/dns=//p"`

echo `date` " set_network.sh $dhcp $ipaddress $netmask $gateway $dns" >> /tmp/easy_gui.log

if [ $dhcp == "yes" ]; then

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

if [ $dhcp == "no" ]; then
  
  # Static IP

  if [ -f /etc/rc.d/S10network ]; then
    /etc/init.d/network stop
    /etc/init.d/network disable
    /etc/init.d/network-static enable
  fi
  
  sed -i "s/DHCPD=.*/DHCPD=no/g" /etc/init.d/network-static
  sed -i "s/IPADDRESS=.*/IPADDRESS=\"$ipaddress\"/g" /etc/init.d/network-static
  sed -i "s/NETMASK=.*/NETMASK=\"$netmask\"/g" /etc/init.d/network-static
  sed -i "s/GATEWAY=.*/GATEWAY=\"$gateway\"/g" /etc/init.d/network-static
  sed -i "s/DNS=.*/DNS=\"$dns\"/g" /etc/init.d/network-static
  /etc/init.d/network-static stop
  /etc/init.d/network-static start
fi
