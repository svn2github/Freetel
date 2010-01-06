#!/bin/sh
# about.sh
# David Rowe 7 Jan 2010
# About screen for Easy Asterisk GUI

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

# Construct the web page -------------------------------

cat <<EOF
<script src="prototype.js"></script>
<link href="astman.css" media="all" rel="Stylesheet" type="text/css" />
<script type="text/javascript" src="tooltip.js"></script>
<link rel="stylesheet" href="tooltip.css" type="text/css" />
EOF

cat << EOF
<html>
<title>Easy Asterisk - About</title>
EOF

cat tooltips.html
echo '<table align="center" width=800>'
cat banner.html
echo "    <tr>"
cat menu.html    
cat <<EOF

    <td valign="top">

    <table align="right" width=600>
      <tr><td align="left" valign="top"><h2>About</h2></td></tr>
EOF
echo '      <tr><td>Easy Asterisk $Revision$</td></tr> ' | sed -n 's/\$//pg'

echo "      <tr><td>&nbsp;</td></tr>"
echo "      <tr><td><h3>cat /proc/version</h3></td></tr>"
echo "      <tr><td>"
cat /proc/version
echo "      </td></tr>"

echo "      <tr><td>&nbsp;</td></tr>"
echo "      <tr><td><h3>ipkg list_installed</h3></td></tr>"
echo "      <tr><td>"
ipkg list_installed | tr '\n' '#' | sed -n 's/\#/<br>/pg'
echo "      </td></tr>"

echo "      <tr><td>&nbsp;</td></tr>"
echo "      <tr><td><h3>cat /proc/loadavg</h3></td></tr>"
echo "      <tr><td>"
cat /proc/loadavg
echo "      </td></tr>"

echo "      <tr><td>&nbsp;</td></tr>"
echo "      <tr><td><h3>uptime</h3></td></tr>"
echo "      <tr><td>"
uptime
echo "      </td></tr>"

echo "      <tr><td>&nbsp;</td></tr>"
echo "      <tr><td><h3>cat /proc/meminfo</h3></td></tr>"
echo "      <tr><td>"
cat /proc/meminfo | tr '\n' '#' | sed -n 's/\#/<br>/pg'
echo "      </td></tr>"

echo "      <tr><td>&nbsp;</td></tr>"
echo "      <tr><td><h3>cat /proc/cmdline</h3></td></tr>"
echo "      <tr><td>"
cat /proc/cmdline
echo "      </td></tr>"

echo "      <tr><td>&nbsp;</td></tr>"
echo "      <tr><td><h3>cat /proc/cpuinfo</h3></td></tr>"
echo "      <tr><td>"
cat /proc/cpuinfo
echo "      </td></tr>"

echo "      <tr><td>&nbsp;</td></tr>"
echo "      <tr><td><h3>cat /proc/mtd</h3></td></tr>"
echo "      <tr><td>"
cat /proc/mtd | tr '\n' '#' | sed -n 's/\#/<br>/pg'
echo "      </td></tr>"

echo "      <tr><td>&nbsp;</td></tr>"
echo "      <tr><td><h3>cat /proc/yaffs</h3></td></tr>"
echo "      <tr><td>"
cat /proc/yaffs | tr '\n' '#' | sed -n 's/\#/<br>/pg'
echo "      </td></tr>"

    </table>

    </td>

    </tr>

</table>
</body>
</html>
EOF

