#!/bin/sh
# dashboard.sh
# David Rowe 6 Jan 2010
# Dashboard screen for Mini Asterisk GUI

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

more=`echo "$QUERY_STRING" | grep -oe "more=[^&?]*" | sed -n "s/more=//p"`

# Build list of interface IPs based on what sort of machine we are
# running on.  For IP0X we just print the IP of the first interface.
# For other machines the situation is a bit more complex so we print
# all of them.  I figure anyone with multiple interfaces will know
# enough about what they are doing to figure out the right one to use.

ipaddress=`ifconfig | sed -n 's/.*inet addr:\(.*\)  Bcast.*/\1/p'`

cat /proc/cpuinfo | grep "CPU:.*ADSP" > /dev/null
if [ $? -eq 0 ]; then
    ipaddress=`echo $ipaddress | awk '{ print $1 }'`
    ipaddress_system=$ipaddress
    ipaddress_tooltip=$ipaddress
else
    ipaddress_system=`echo $ipaddress | sed -n 's/ /<br>/p'`

    # make nice tooltips for multiple ip interfaces on some x86 boxes

    ipaddress_tooltip=`echo $ipaddress | sed -n 's/ / or /p'`
fi

# Construct the web page -------------------------------

cat <<EOF
<script src="prototype.js"></script>
<script type="text/javascript" src="tooltip.js"></script>
<link rel="stylesheet" href="tooltip.css" type="text/css" />
EOF

echo "<script>"
echo 'var init_internet="'$internet'";'
cat ipphones.js
echo "</script>"

cat << EOF
<html>
<title>Mini Asterisk - IP Phones</title>
<body onload="localInit()">
EOF

cat tooltips.html
echo '<table align="center" width=800 border=0>'
cat banner.html
echo "    <tr>"
cat menu.html    
cat <<EOF

    <td valign="top">

    <table align="right" width=600 border=0>
      <tr><td  onMouseOver="popUp(event,'ipphones_ipphones')" onmouseout="popUp(event,'ipphones_ipphones')" 
           colspan="4" align="left" valign="top" ><h2>IP Phones</h2></td></tr>
      <tr onMouseOver="popUp(event,'phone_ipaddress')" onmouseout="popUp(event,'phone_ipaddress')">
	  <td colspan="3">Phone System IP Address:</td>
EOF
echo "<td>$ipaddress_system</td>"

    # use perl to construct list of IP phones for us
    asterisk "-rx sip show peers" 2>/dev/null > sipshowpeers.txt
    perl ipphones.pl "$ipaddress_tooltip" $more

cat <<EOF
    </table>

    <h2>&nbsp;<br>How to Configure IP Phones</h2>
    <h3>1. Atcom AT-530</h3>
EOF
cat at-530.html
echo "<h3>2. Atcom AT-620R</h3>"
cat at-620r.html
cat <<EOF
     </td>
    </tr>

</table>
</body>
</html>
EOF

