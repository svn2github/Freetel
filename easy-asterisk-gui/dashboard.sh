#!/bin/sh -x
# dashboard.sh
# David Rowe 4 Jan 2010
# CGI for Easy Asterisk dashboard GUI

# See if we have Internet connectivity

packet_loss=`ping google.com -c 1 -q | sed -n 's/.*received, \(.*\)% packet loss/\1/p'`
if [ $packet_loss == "0" ]; then
  internet="Good";
else
  internet="Not Available";
fi

# Construct the web page -------------------------------

echo $HTTP_COOKIE | grep "loggedin" > /dev/null
if [ $? -eq 1 ]; then
    echo "<html>"
    echo "<head>"
    echo '<meta http-equiv="REFRESH" content="0;url=http:login.sh">'
    echo "</head>"
    echo "</html>"
    exit
fi

cat <<EOF
<script src="prototype.js"></script>
<link href="astman.css" media="all" rel="Stylesheet" type="text/css" />
<script type="text/javascript" src="tooltip.js"></script>
<link rel="stylesheet" href="tooltip.css" type="text/css" />
EOF

echo "<script>"
echo 'var init_internet="'$internet'";'
cat dashboard.js
echo "</script>"

cat << EOF
<html>
<title>Easy Asterisk - Dashboard</title>
<body onload="localInit()">
<div id="t1" class="tip">Tells you if I can reach the Internet.  If not "Good" check your network settings, in particular Gateway and DNS.</div>
<form action="/cgi-bin/set_network.sh" onsubmit="return validate_form(this)" method="get">
<table align="center" width=600>
<tr>
  <tr><td colspan="2" align="left"><h2>Dashboard</h2></td>
  <tr onMouseOver="popUp(event,'t1')" onmouseout="popUp(event,'t1')">
      <td>Internet Connection:</td>
      <td><div id="internet" >
      <span style="margin-left: 4px;font-weight:bold">&nbsp;</span></div></td>
      
  </tr>
</tr>
</table>
</form>
EOF

echo $HTTP_COOKIE
cat<<EOF
</body>
</html>
EOF

