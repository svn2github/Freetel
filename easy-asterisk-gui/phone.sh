#!/bin/sh -x
# phone.sh
# David Rowe 4 Jan 2010
# CGI for Easy Asterisk phones GUI

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
cat phone.js
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
      <tr><td colspan="2" align="left" valign="top" ><h2>Phones</h2></td></tr>
EOF

# use perl to construct list of phones and phone lines for us
asterisk "-rx sip show peers" 2>/dev/null > sipshowpeers.txt
./phone.pl

cat<<EOF
    </table>

    </td>

    </tr>

</table>
</body>
</html>
EOF

