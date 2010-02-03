#!/bin/sh
# phones.sh
# David Rowe 4 Jan 2010
# Phones screen for Mini Asterisk GUI

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

# See if we have Internet connectivity, first check dns as time outs can be very slow

dns=`cat /etc/resolv.conf | awk '/^nameserver/ {print $2}'`
dns_packet_loss=`ping $dns -c 1 -q | sed -n 's/.*received, \(.*\)%.*/\1/p'`
internet="no";
if [ $dns_packet_loss = "0" ]; then
  packet_loss=`ping google.com -c 1 -q | sed -n 's/.*received, \(.*\)%.*/\1/p'`
  if [ $packet_loss = "0" ]; then
    internet="yes";
  fi
fi

ipaddress=`ifconfig | sed -n 's/.*inet addr:\(.*\)  Bcast.*/\1/p' | head -n 1`

# Construct the web page -------------------------------

cat <<EOF
<script src="prototype.js"></script>
<script type="text/javascript" src="tooltip.js"></script>
<link rel="stylesheet" href="tooltip.css" type="text/css" />
EOF

echo "<script>"
echo 'var init_internet="'$internet'";'
cat phones.js
echo "</script>"

cat << EOF
<html>
<title>Mini Asterisk - Phones</title>
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
      <tr>
	  <td colspan="3" onMouseOver="popUp(event,'network_internet')" onmouseout="popUp(event,'network_internet')">Internet Connection:</td>
	  <td></td><td><div id="internet" >&nbsp;</div></td>
      </tr>
      <tr onMouseOver="popUp(event,'phone_ipaddress')" onmouseout="popUp(event,'phone_ipaddress')">
	  <td colspan="4">Phone System IP Address:</td>
EOF
echo "<td>$ipaddress</td>"
cat <<EOF
      </tr>
      <tr><td>&nbsp</td></tr>
      <tr>
          <td onMouseOver="popUp(event,'phone_phones')" onmouseout="popUp(event,'phone_phones')" 
               colspan="3" align="left" valign="top" ><h2>Phones</h2></td>
      </tr>
EOF

# use perl to construct list of phones and phone lines for us
asterisk "-rx sip show peers" 2>/dev/null > sipshowpeers.txt
perl ./phones.pl

cat<<EOF
    </table>

    </td>

    </tr>

</table>
</body>
</html>
EOF

