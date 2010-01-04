#!/bin/sh
# network.sh
# David Rowe 4 Jan 2010
# CGI for network GUI

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

# See if we have Internet connectivity

packet_loss=`ping google.com -c 1 -q | sed -n 's/.*received, \(.*\)% packet loss/\1/p'`
if [ $packet_loss == "0" ]; then
  internet="Good";
else
  internet="Not Available";
fi

# Construct the web page -------------------------------

cat <<EOF
<script src="prototype.js"></script>
<link href="astman.css" media="all" rel="Stylesheet" type="text/css" />
<script type="text/javascript" src="tooltip.js"></script>
<link rel="stylesheet" href="tooltip.css" type="text/css" />
EOF

echo "<script>"
echo 'var init_dhcp="'$dhcp'";'
echo 'var init_ipaddress="'$ipaddress'";'
echo 'var init_netmask="'$netmask'";'
echo 'var init_gateway="'$gateway'";'
echo 'var init_dns="'$dns'";'
echo 'var init_internet="'$internet'";'
cat network.js
echo "</script>"

cat << EOF
<html>
<title>Network</title>
<body onload="localInit()">
<div id="t1" class="tip">Tells you if I can reach the Internet.  If not "Good" check your network settings, in particular Gateway and DNS.</div>
<form action="/cgi-bin/set_network.sh" onsubmit="return validate_form(this)" method="get">
<table align="center" width=600>
<tr>
  <tr><td colspan="2" align="left"><h2>Easy Asterisk - Network</h2></td>
  <tr>
     <td><input type="radio" id="static" name="dhcp" value="no" onClick="doStatic()">Static</td>
     <td><input type="radio" id="dhcp"   name="dhcp" value="yes" onClick="doDHCP()">DHCP</td>
  </tr>
  <tr><td>IP Address:</td><td><input type="text" name="ipaddress" id="ipaddress" onBlur="isIP(this)"></td></tr>
  <tr><td>Netmask:</td><td><input type="text" name="netmask" id="netmask" onBlur="isIP(this)"></td></tr>
  <tr><td>Gateway:</td><td><input type="text" name="gateway" id="gateway" onBlur="isIP(this)"></td></tr>
  <tr><td>DNS:</td><td><input type="text" name="dns" id="dns" onBlur="isIP(this)"></td></tr>
  <tr onMouseOver="popUp(event,'t1')" onmouseout="popUp(event,'t1')">
      <td>Internet Connection:</td>
      <td><div id="internet" >
      <span style="margin-left: 4px;font-weight:bold">&nbsp;</span></div></td>
      
  </tr>
  <tr><td><input id="networkapply" type="submit" value="Apply"></td></tr>
</tr>
</table>
</form>
</html>
EOF

#<td><input type="text" name="internet" id="internet" disabled=1 onMouseOver="popUp(event,'t1')" ></td></tr>
