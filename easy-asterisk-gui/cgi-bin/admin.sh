#!/bin/sh
# admin.sh
# David Rowe 7 Jan 2010
# Admin screen for Easy Asterisk GUI

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

# set password CGI -----------------------------------------------

echo "$QUERY_STRING" | grep -oe "pass=" > /dev/null
if [ $? -eq 0 ]; then
    pass=`echo "$QUERY_STRING" | grep -oe "pass=[^&?]*" | sed -n "s/pass=//p"`
    passwd_cmdline $pass
fi

# restart CGI ----------------------------------------------------

echo "$QUERY_STRING" | grep -oe "restart=1" > /dev/null
if [ $? -eq 0 ]; then

# kill cookie to log out.  This ensures hitting refresh wont run
# the restart process again

cat <<EOF
Content-type: text/html
Set-Cookie: loggedin=1; expires=Thursday, 01-Jan-98 12:00:00 GMT

<head>
<title>Easy Asterisk - Restart</title>
</head>
<body>
<h2>Restarting...come back in 1 minute</h2>
</body>
EOF
reboot
fi

# set defaults CGI ----------------------------------------------------

echo "$QUERY_STRING" | grep -oe "defaults=1" > /dev/null
if [ $? -eq 0 ]; then
    cp /etc/asterisk/extensions.conf.def /etc/asterisk/extensions.conf
    cp /etc/asterisk/sip.conf.def /etc/asterisk/sip.conf
    asterisk -rx "sip reload" 2>/dev/null 1 > /dev/null
    asterisk -rx "dialplan reload" 2>/dev/null 1 > /dev/null
fi

# Construct the web page -------------------------------

cat <<EOF
<script src="prototype.js"></script>
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
      <tr><td colspan="2"><h3>Change Phone System Password</h3></td></tr>
      <tr><td>New Password:</td><td><input type="password" name="pass" ></td>
          <td><input type="submit" value="Set Password"></td>
      </tr>
      <tr><td>&nbsp</td></tr>
      <tr><td></td><td>Default password is uClinux</td>
      </form>

      <tr><td>&nbsp</td></tr>
      <tr><td colspan="2"><h3>Restart Phone System</h3></td>
          <td onMouseOver="popUp(event,'admin_restart')" onmouseout="popUp(event,'admin_restart')">
              <form action="admin.sh" method="get">
              <input type="hidden" name="restart" value="1">
              <input type="submit" value="Restart">
          </form>
          </td>
      </tr>

      <tr><td>&nbsp</td></tr>
      <tr><td colspan="2"><h3>Reset Phone System Defaults</h3></td>
          <td>
              <form action="admin.sh" method="get">
              <input type="hidden" name="defaults" value="1">
              <input type="submit" value="Reset">
	      </form>
	  </td>
      </tr>

      <tr><td>&nbsp</td></tr>
      <tr onMouseOver="popUp(event,'admin_upgrade')" onmouseout="popUp(event,'admin_upgrade')">
          <td colspan="2"><h3>Upgrade Easy Asterisk</h3></td>
          <td>
              <form action="admin.sh" method="get">
              <input type="hidden" name="upgrade" value="1">
              <input type="submit" value="Upgrade">
          </td>
      </tr>

      <tr><td>&nbsp</td></tr>
      <tr><td colspan="2"><h3>Install New Firmware</h3>
      <form action="admin.sh" method="get">
      <tr onMouseOver="popUp(event,'admin_firmware')" onmouseout="popUp(event,'admin_firmware')">
          <td>Firmware URL:</td>
          <td><input type="text" name="firmwareurl" ></td>
          <td><input type="submit" value="Install"></td>
      </tr>
      </form>

    </table>
    </form>

    </td>

    </tr>

</table>
</body>
</html>
EOF

