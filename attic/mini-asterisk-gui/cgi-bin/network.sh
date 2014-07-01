#!/bin/sh
# network.sh
# David Rowe 4 Jan 2010
# CGI for Mini Asterisk network GUI

# check we are logged in

echo $HTTP_COOKIE | grep "loggedin" > /dev/null
if [ $? -eq 1 ]; then
    echo "<html>"
    echo "<head>"
    echo '<meta http-equiv="REFRESH" content="0;url=login.sh">'
    echo "</head>"
    echo "</html>"
    exit
fi

# check what sort of machine we are running on.  We only support this screen for
# IP0X

cat /proc/cpuinfo | grep "CPU:.*ADSP" > /dev/null
if [ ! $? -eq 0 ]; then
cat <<EOF
    <html>
    Sorry, Network configuration is only supported on the IP0X.  Anyway you 
    probably have a good way of setting up your network already.
    </html>
EOF
    exit
fi

grok_network=0

if [ -f /etc/rc.d/S10network ]; then
  grok_network=1
  dhcp=yes
  ipaddress=`ifconfig eth0 | sed -n 's/.*inet addr:\(.*\)  Bcast.*/\1/p'`
  netmask=`ifconfig eth0 | sed -n 's/.*Mask:\(.*\)\s*/\1/p'`
  gateway=`route -n | awk '/^0.0.0.0/ {print $2}'`
  dns=`cat /etc/resolv.conf | awk '/^nameserver/ {print $2}'`
fi

if [ -f /etc/rc.d/S10network-static ]
then
  grok_network=1
  dhcp=no  
  ipaddress=`sed -n 's/IPADDRESS="\(.*\)"/\1/p' /etc/init.d/network-static`
  netmask=`sed -n 's/NETMASK="\(.*\)"/\1/p' /etc/init.d/network-static`
  gateway=`sed -n 's/GATEWAY="\(.*\)"/\1/p' /etc/init.d/network-static`
  dns=`sed -n 's/DNS="\(.*\)"/\1/p' /etc/init.d/network-static`
fi

# if we don't understand this machines network config then bail

if [ $groknetwork -eq 0 ]; then
cat << EOF
<html>
<title>Mini Asterisk - Network</title>
EOF

cat tooltips.html
echo '<table align="center" width=800 border=0>'
cat banner.html
echo "    <tr>"
cat menu.html    
cat <<EOF

    <td valign="top">
    <table align="center" width=600 border=0>
      <tr><td colspan="2" align="left" valign="top"><h2>Network</h2></td></tr>
      <tr><td>Sorry - I can't edit the Network configuration on this machine</td></tr>
    </td>
    </table>
    </td>
    </tr>

</table>

</html>
EOF
fi
 
if [ -f /etc/rc.d/S05network-backdoor ]; then
  backdoor=`sed -n 's/IPADDRESS="\(.*\)"/\1/p' /etc/init.d/network-backdoor`
fi

# See if we have Internet connectivity, first check dns as time outs can be very slow

dns_packet_loss=`ping $dns -c 1 -q | sed -n 's/.*received, \(.*\)%.*/\1/p'`
internet="no";
if [ $dns_packet_loss = "0" ]; then
  packet_loss=`ping google.com -c 1 -q | sed -n 's/.*received, \(.*\)%.*/\1/p'`
  if [ $packet_loss = "0" ]; then
    internet="yes";
  fi
fi

# Construct the web page -------------------------------

cat <<EOF
<script src="prototype.js"></script>
<script type="text/javascript" src="tooltip.js"></script>
<link rel="stylesheet" href="tooltip.css" type="text/css" />
EOF

echo "<script>"
echo 'var init_dhcp="'$dhcp'";'
echo 'var init_ipaddress="'$ipaddress'";'
echo 'var init_netmask="'$netmask'";'
echo 'var init_gateway="'$gateway'";'
echo 'var init_dns="'$dns'";'
echo 'var init_backdoor="'$backdoor'";'
echo 'var init_internet="'$internet'";'
cat network.js
echo "</script>"

cat << EOF
<html>
<title>Mini Asterisk - Network</title>
<body onload="localInit()">
EOF

cat tooltips.html
echo '<table align="center" width=800 border=0>'
cat banner.html
echo "    <tr>"
cat menu.html    
cat <<EOF

    <td valign="top">

    <form action="set_network.sh" onsubmit="return validate_form(this)" method="get">
    <table align="center" width=600 border=0>
      <tr><td colspan="2" align="left" valign="top"><h2>Network</h2></td></tr>
      <tr>
	 <td><input type="radio" id="static" name="dhcp" value="no" onClick="doStatic()">Static</td>
	 <td><input type="radio" id="dhcp"   name="dhcp" value="yes" onClick="doDHCP()">DHCP</td>
      </tr>
      <tr><td>IP Address:</td><td><input type="text" name="ipaddress" id="ipaddress" onBlur="isIP(this)"></td></tr>
      <tr><td>Netmask:</td><td><input type="text" name="netmask" id="netmask" onBlur="isIP(this)"></td></tr>
      <tr><td>Gateway:</td><td><input type="text" name="gateway" id="gateway" onBlur="isIP(this)"></td></tr>
      <tr><td>DNS:</td><td><input type="text" name="dns" id="dns" onBlur="isIP(this)"></td></tr>
      <tr onMouseOver="popUp(event,'network_backdoor')" onmouseout="popUp(event,'network_backdoor')">
          <td>Emergency IP:</td><td><input type="text" name="backdoor" id="backdoor" onBlur="isIP(this)"></td>
      </tr>
      <tr onMouseOver="popUp(event,'network_internet')" onmouseout="popUp(event,'network_internet')">
	  <td>Internet Connection:</td>
	  <td><div id="internet" >
	  <span style="margin-left: 4px;font-weight:bold">&nbsp;</span></div></td>

      </tr>
      <tr><td><input id="networkapply" type="submit" value="Apply"></td></tr>
    </table>
    </form>

    </td>

    </tr>

</table>

</html>
EOF

