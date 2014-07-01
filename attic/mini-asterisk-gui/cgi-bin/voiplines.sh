#!/bin/sh
# voiplines.sh
# David Rowe 12 Jan 2010
# VOIP Line screen for Mini Asterisk GUI

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

# See if we have can reach the VOIP Line provider

asterisk "-rx sip show registry" 2>/dev/null > sipshowregistry.txt

# Construct the web page -------------------------------

cat <<EOF
<script src="prototype.js"></script>
<script type="text/javascript" src="tooltip.js"></script>
<link rel="stylesheet" href="tooltip.css" type="text/css" />
<script>
EOF

cat voiplines.js

cat << EOF
</script>
<html>
<title>Mini Asterisk - VOIP Line</title>
EOF

cat tooltips.html
echo '<table align="center" width=800>'
cat banner.html
echo "    <tr>"
cat menu.html    
cat <<EOF

    <td valign="top">

    <table align="right" width=600>
      <tr><td>&nbsp</td></tr>
      <form action="set_voiplines.sh" method="get">
      <tr><td onMouseOver="popUp(event,'voiplines_voiplines')" onmouseout="popUp(event,'voiplines_voiplines')" align="left" valign="top" colspan="3"><h2>VOIP Line</h2></td></tr>

      <tr><td>&nbsp</td></tr>
EOF

perl ./voiplines.pl

cat <<EOF

      <tr><td colspan="2">Sometimes it takes a few seconds to connect
                          your Voip Line.  After clicking OK refresh
                          your browser to update Voip Line status.
                          </td></tr>

      <tr><td>&nbsp</td></tr>
      <tr><td><input type="submit" value="OK"></td></tr>

      </form>

    </table>
    </form>

    </td>

    </tr>

</table>
</body>
</html>
EOF

