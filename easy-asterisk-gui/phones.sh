#!/bin/sh -x
# phones.sh
# David Rowe 4 Jan 2010
# Phones screen for Easy Asterisk GUI

# See if we have Internet connectivity, first check dns as time outs can be very slow

dns=`cat /etc/resolv.conf | awk '/^nameserver/ {print $2}'`
dns_packet_loss=`ping $dns -c 1 -q | sed -n 's/.*received, \(.*\)% packet loss/\1/p'`
internet="no";
if [ $dns_packet_loss == "0" ]; then
  packet_loss=`ping google.com -c 1 -q | sed -n 's/.*received, \(.*\)% packet loss/\1/p'`
  if [ $packet_loss == "0" ]; then
    internet="yes";
  fi
fi

ipaddress=`ifconfig eth0 | sed -n 's/.*inet addr:\(.*\)  Bcast.*/\1/p'`

# Construct the web page -------------------------------

sh check_loggedin.sh

cat <<EOF
<script src="prototype.js"></script>
<link href="astman.css" media="all" rel="Stylesheet" type="text/css" />
<script type="text/javascript" src="tooltip.js"></script>
<link rel="stylesheet" href="tooltip.css" type="text/css" />
EOF

echo "<script>"
echo 'var init_internet="'$internet'";'
cat phones.js
echo "</script>"

cat << EOF
<html>
<title>Easy Asterisk - Phones</title>
<body onload="localInit()">
EOF

cat tooltips.html
echo '<table align="center" width=800>'
cat banner.html
echo "    <tr>"
cat menu.html    
cat <<EOF

    <td valign="top">

    <table align="right" width=600>
      <tr>
          <td onMouseOver="popUp(event,'phone_system')" onmouseout="popUp(event,'phone_system')" colspan="4" align="left" valign="top"><h2>Phone System</h2></td>
      </tr>
      <tr onMouseOver="popUp(event,'network_internet')" onmouseout="popUp(event,'network_internet')">
	  <td colspan="3">Internet Connection:</td>
	  <td><div id="internet" >&nbsp;</div></td>
      </tr>
      <tr onMouseOver="popUp(event,'phone_ipaddress')" onmouseout="popUp(event,'phone_ipaddress')">
	  <td colspan="3">Phone System IP Address:</td>
EOF
echo "<td>$ipaddress</td>"
cat <<EOF
      </tr>
      <tr><td>&nbsp</td></tr>
      <tr>
          <td onMouseOver="popUp(event,'phone_phones')" onmouseout="popUp(event,'phone_phones')" 
               colspan="3" align="left" valign="top" ><h2>Phones</h2></td>
	  <td onMouseOver="popUp(event,'phone_addipphone')" onmouseout="popUp(event,'phone_addipphone')">
               <a href="addipphone.sh">Add IP Phone</a></td>
      </tr>
EOF

# use perl to construct list of phones and phone lines for us
asterisk "-rx sip show peers" 2>/dev/null > sipshowpeers.txt
./phones.pl

cat<<EOF
    </table>

    </td>

    </tr>

</table>
</body>
</html>
EOF

