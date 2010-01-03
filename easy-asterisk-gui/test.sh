#!/bin/sh

echo `date` " get_network.sh" >> /tmp/easy_gui.log

if [ -f /etc/rc.d/S10network ]; then
  dhcp=yes
  ipaddress=`ifconfig eth0 | sed -n 's/.*inet addr:\(.*\)  Bcast.*/\1/p'`
  netmask=`ifconfig eth0 | sed -n 's/.*Mask:\(.*\)\s*/\1/p'`
  gateway=`route -n | awk '/^0.0.0.0/ {print $2}'`
  dns=`cat /etc/resolv.conf | awk '/^nameserver/ {print $2}'`
fi

if [ -f /etc/rc.d/S10network-static ]
then
  dhcp=no  
  ipaddress=`sed -n 's/IPADDRESS="\(.*\)"/\1/p' /etc/init.d/network-static`
  netmask=`sed -n 's/NETMASK="\(.*\)"/\1/p' /etc/init.d/network-static`
  gateway=`sed -n 's/GATEWAY="\(.*\)"/\1/p' /etc/init.d/network-static`
  dns=`sed -n 's/DNS="\(.*\)"/\1/p' /etc/init.d/network-static`
fi

# Construct the web page -------------------------------

echo "<script src="prototype.js"></script>"
echo "<link href="astman.css" media="all" rel="Stylesheet" type="text/css" />"
echo "<script>"
echo 'var init_dhcp="'$dhcp'";'
echo 'var init_ipaddress="'$ipaddress'";'
echo 'var init_netmask="'$netmask'";'
echo 'var init_gateway="'$gateway'";'
echo 'var init_dns="'$dns'";'
cat easy.js
echo "</script>"

cat << EOF
<html>
<title>Easy Asterisk GUI</title>
<body onload="localInit()">
<form action="/cgi-bin/set_network.sh" method="get">
<table align="center" width=600>
<tr>
  <tr>
     <td><input type="radio" id="static" name="dhcp" value="no" onClick="doStatic()">Static</td>
     <td><input type="radio" id="dhcp"   name="dhcp" value="yes" onClick="doDHCP()">DHCP</td>
  </tr>
  <tr><td>IP Address:</td><td><input type="text" name="ipaddress" id="ipaddress" onBlur="isIP(this)"></td></tr>
  <tr><td>Netmask:</td><td><input type="text" name="netmask" id="netmask" onBlur="isIP(this)"></td></tr>
  <tr><td>Gateway:</td><td><input type="text" name="gateway" id="gateway" onBlur="isIP(this)"></td></tr>
  <tr><td>DNS:</td><td><input type="text" name="dns" id="dns" onBlur="isIP(this)"></td></tr>
  <tr><td><input id="networkapply" type="submit" value="Apply"></td></tr>
</tr>
</table>
</form>
</html>
EOF
