#!/bin/sh
# getdhcp.cgi
# David Rowe 8 March 2011
#
# CGI to return DHCP config details

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1">
</head>
<body>
EOF

if [ -f /etc/rc.d/S10network ]; then
  dhcp=yes
  ipaddress=`ifconfig eth0 | sed -n 's/.*inet addr:\(.*\)  Bcast.*/\1/p'`
  netmask=`ifconfig eth0 | sed -n 's/.*Mask:\(.*\)\s*/\1/p'`
  gateway=`route -n | awk '/^0.0.0.0/ {print $2}'`
  dns=`cat /etc/resolv.conf | awk '/^nameserver/ {print $2}'`
  echo DHCP=\"yes\"
  echo IPADDRESS=\"$ipaddress\"
  echo NETMASK=\"$netmask\"
  echo GATEWAY=\"$gateway\"
  echo DNS=\"$dns\"
else
  echo DHCP=\"no\"
fi

cat <<EOF
</body>
</html>
EOF
