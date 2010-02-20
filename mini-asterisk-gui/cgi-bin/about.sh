#!/bin/sh
# about.sh
# David Rowe 7 Jan 2010
# About screen for Mini Asterisk GUI

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

# Construct the web page -------------------------------

cat <<EOF
<script src="prototype.js"></script>
<script type="text/javascript" src="tooltip.js"></script>
<link rel="stylesheet" href="tooltip.css" type="text/css" />
EOF

cat << EOF
<html>
<title>Mini Asterisk - About</title>
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
echo '      <tr><td><a href="http://rowetel.com/ucasterisk/mini.html">Mini Asterisk</a> Revision: 156 </td></tr> '
echo '      <tr><td>&nbsp;</td></tr>'
echo '      <tr><td>Brought to you by the <a href="http://rowetel.com/ucasterisk/index.html">Free Telephony Project</a></td></tr>'
echo '      <tr><td><a href="http://rowetel.com/ucasterisk/ip04.html"><img src="http://rowetel.com/images/ip04/ip04_case.jpg" border="0" /></a></td></tr>'

more=`echo "$QUERY_STRING" | grep -oe "more=[^&?]*" | sed -n "s/more=//p"`
if  [ $more -eq 1 ]; then
  echo 
else
    echo "      <tr><td>&nbsp;</td></tr>"
    echo '      <tr><td><a href="about.sh?more=1">More</a></td></tr>';
cat <<EOF
    </table>
    </td>
    </tr>
</table>
</body>
</html>
EOF
exit
fi

echo "      <tr><td>&nbsp;</td></tr>"
echo "      <tr><td><h3>cat /proc/version</h3></td></tr>"
echo "      <tr><td>"
cat /proc/version
echo "      </td></tr>"

which ipkg >> /dev/null
if [ $? -eq 0 ]; then
    echo "      <tr><td>&nbsp;</td></tr>"
    echo "      <tr><td><h3>ipkg list_installed</h3></td></tr>"
    echo "      <tr><td>"
    ipkg list_installed | tr '\n' '#' | sed -n 's/\#/<br>/pg'
    echo "      </td></tr>"
fi

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

if [ -f /proc/mtd ]; then
    echo "      <tr><td>&nbsp;</td></tr>"
    echo "      <tr><td><h3>cat /proc/mtd</h3></td></tr>"
    echo "      <tr><td>"
    cat /proc/mtd | tr '\n' '#' | sed -n 's/\#/<br>/pg'
    echo "      </td></tr>"
fi

if [ -f /proc/yaffs ]; then
    echo "      <tr><td>&nbsp;</td></tr>"
    echo "      <tr><td><h3>cat /proc/yaffs</h3></td></tr>"
    echo "      <tr><td>"
    cat /proc/yaffs | tr '\n' '#' | sed -n 's/\#/<br>/pg'
    echo "      </td></tr>"
fi

echo "      <tr><td>&nbsp;</td></tr>"
echo '      <tr><td><a href="about.sh">Less</a></td></tr>';

cat <<EOF
    </table>

    </td>

    </tr>

</table>
</body>
</html>
EOF

