#!/bin/sh
# admin.sh
# David Rowe 7 Jan 2010
# Admin screen for Easy Asterisk GUI

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
<title>Easy Asterisk - Admin</title>
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
      <form action="admin.sh" method="get">
      <tr><td align="left" valign="top"><h2>Admin</h2></td></tr>

      <tr><td>&nbsp</td></tr>
      <tr><td colspan="2"><h3>Change Phone System Password</h2></td></tr>
      <tr><td>User:</td><td><input type="text" name="user" ></td></tr>
      <tr><td>Current Password:</td><td><input type="password" name="oldpass" ></td></tr>
      <tr><td>New Password:</td><td><input type="password" name="newpass" ></td></tr>
      <tr><td>&nbsp</td></tr>
      <tr><td></td><td>Default user/password is root/uClinux</td>
      <tr><td><input type="submit" value="Set Password"></td></tr>
      </form>

      <tr><td>&nbsp</td></tr>
      <tr><td colspan="2"><h3>Restart Phone System</h2></td></tr>
      <form action="admin.sh?reboot=1" method="get">
      <tr><td><input type="submit" value="Restart"></td></tr>
      </form>

      <tr><td>&nbsp</td></tr>
      <tr><td colspan="2"><h3>Reset Phone System Defaults</h2></td></tr>
      <form action="admin.sh?defaults=1" method="get">
      <tr><td><input type="submit" value="Reset"></td></tr>
      </form>

      <tr><td>&nbsp</td></tr>
      <tr><td colspan="2"><h3>Upgrade Software</h2></td></tr>
      <form action="admin.sh?upgrade=1" method="get">
      <tr><td>Software URL:</td><td><input type="text" name="upgrade" ></td></tr>
      <tr><td><input type="submit" value="Upgrade"></td></tr>
      </form>

    </table>
    </form>

    </td>

    </tr>

</table>
</body>
</html>
EOF

