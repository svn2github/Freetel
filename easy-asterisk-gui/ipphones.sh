#!/bin/sh -x
# dashboard.sh
# David Rowe 6 Jan 2010
# Dashboard screen for Easy Asterisk GUI

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

    <form action="/cgi-bin/set_network.sh" onsubmit="return validate_form(this)" method="get">
    <table align="right" width=600 border=0>
      <tr><td colspan="3" align="left" valign="top" ><h2>Dashboard</h2></td></tr>
      <tr onMouseOver="popUp(event,'network_internet')" onmouseout="popUp(event,'network_internet')">
	  <td>Internet Connection:</td>
	  <td><div id="internet" >
	  <span style="margin-left: 4px;font-weight:bold">&nbsp;</span></div></td>
      </tr>
    </table>
    </form>

    </td>

    </tr>

</table>
</body>
</html>
EOF

