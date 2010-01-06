#!/bin/sh
# dashboard.sh
# David Rowe 6 Jan 2010
# Dashboard screen for Easy Asterisk GUI

# check we are logged in

echo $HTTP_COOKIE | grep "loggedin" > /dev/null
if [ $? -eq 1 ]; then
    echo "<html>"
    echo "<head>"
    echo '<meta http-equiv="REFRESH" content="0;url=http:login.sh">'
    echo "</head>"
    echo "</html>"
    exit
fi

more=`echo "$QUERY_STRING" | grep -oe "more=[^&?]*" | sed -n "s/more=//p"`
ipaddress=`ifconfig eth0 | sed -n 's/.*inet addr:\(.*\)  Bcast.*/\1/p'`

# Construct the web page -------------------------------

cat <<EOF
<script src="prototype.js"></script>
<link href="astman.css" media="all" rel="Stylesheet" type="text/css" />
<script type="text/javascript" src="tooltip.js"></script>
<link rel="stylesheet" href="tooltip.css" type="text/css" />
EOF

echo "<script>"
echo 'var init_internet="'$internet'";'
cat ipphones.js
echo "</script>"

cat << EOF
<html>
<title>Easy Asterisk - IP Phones</title>
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
echo "<td>$ipaddress</td></tr><td>&nbsp;</td><tr></tr>"

    # use perl to construct list of IP phones for us
    asterisk "-rx sip show peers" 2>/dev/null > sipshowpeers.txt
    ./ipphones.pl $ipaddress $more

cat <<EOF
    </table>

    <h2>&nbsp;<br>How to Configure IP Phones</h2>
    <h3>1. Atcom AT-530</h3>
EOF
cat at-530.html
cat <<EOF
     </td>
    </tr>

</table>
</body>
</html>
EOF

