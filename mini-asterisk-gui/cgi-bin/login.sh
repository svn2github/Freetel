#!/bin/sh
# login.sh
# David Rowe 4 Jan 2010
# CGI for Mini Asterisk login GUI

pass=`echo "$QUERY_STRING" | grep -oe "pass=[^&?]*" | sed -n "s/pass=//p"`

echo $QUERY_STRING | grep pass > /dev/null
if [ $? -eq 1 ]; then
    # Display form -------------------------------

    cat <<EOF
    <script src="prototype.js"></script>
    <script type="text/javascript" src="tooltip.js"></script>
    <link rel="stylesheet" href="tooltip.css" type="text/css" />
EOF
    cat << EOF
    <html>
    <title>Mini Asterisk - Login</title>
    <form action="login.sh" method="get">
    <table align="center" width=600>
    <tr><td colspan="2" align="left"><h2>Login</h2></td>
    <tr>
      <tr><td>Password:</td><td><input type="password" name="pass" ></td></tr>
      <tr><td><input type="submit" value="Login"></td></tr>
    </tr>
    <tr><td></td><td>Default password is uClinux</td>
    </table>
    </form>
    </html>
EOF
else
    
    # If we are a non-blackfin then use hard coded password for now.
    # I don't feel comfortable sending root password through a web
    # GUI for x86 boxes, but this is reasonable approach for embedded
    # systems.  TODO: add a way to store and modify x86 passwds

    cat /proc/cpuinfo | grep "CPU:.*ADSP" > /dev/null
    if [ $? -eq 0 ]; then
	# Blackfin IP0X system
        testuser root "$pass"; res=$?
    else
       [ $pass = "uClinux" ] ; res=$?
    fi 

    if [ $res -eq 0 ]; then

	# login sucessful
        echo "Content-type: text/html"
        echo "Set-Cookie: loggedin=1"
        echo ""
	echo "<head>"
	echo "<title>Mini Asterisk - Login</title>"
	echo '<meta http-equiv="REFRESH" content="0;url=phones.sh">'
	echo "</head>"
	echo "<body>"
	echo "Please wait a few seconds....."
	echo "</body>"

	# load mini asterisk conf files in case this is our first login

	asterisk -rx "sip reload" 2>/dev/null 1 > /dev/null
	asterisk -rx "dialplan reload" 2>/dev/null 1 > /dev/null

    else
	# login failed
	cat <<EOF
	<html>
	<head>
	<title>Mini Asterisk - Login</title>
	<meta http-equiv="REFRESH" content="0;url=login.sh">
	<body>
	Please wait a few seconds.....
	</body>
	</head>
	</html>
EOF
    fi
fi

