#!/bin/sh
# faq.sh
# David Rowe 7 Jan 2010
# FAQ screen for Mini Asterisk GUI

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
<title>Mini Asterisk - FAQ</title>
EOF

cat tooltips.html
echo '<table align="center" width=800>'
cat banner.html
echo "    <tr>"
cat menu.html    
cat <<EOF

    <td valign="top">

    <table align="right" width=600>
      <tr><td align="left" valign="top"><h2>FAQ</h2></td></tr>
      <tr><td>
EOF
cat faq.html
cat <<EOF
      </td></tr>
    </table>

    </td>

    </tr>

</table>
</body>
</html>
EOF

