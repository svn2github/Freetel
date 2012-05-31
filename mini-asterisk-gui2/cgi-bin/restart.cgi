#!/bin/sh
# restart.cgi
# Ian Roberts 31 May 2012
#
# CGI to reboot ipox

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head></head>
<body>
EOF


# check what sort of machine we are running on.  Many operations
# outlawed for x86

cat /proc/cpuinfo | grep "CPU:.*ADSP" > /dev/null
if [ $? -eq 0 ]; then
    mach="ip0x"
else
    mach="x86"
fi



if [ $mach != "ip0x" ] ; then
	echo "<html>Sorry, only supported on the IP0X</html>"
	exit
fi

# kill cookie to log out.  This ensures hitting refresh wont run
# the restart process again

cat <<EOF
Content-type: text/html
Set-Cookie: loggedin=1; expires=Thursday, 01-Jan-98 12:00:00 GMT

<head>
<title>Mini Asterisk - Restart</title>
</head>
<body>
<h2>Restarting...come back in 1 minute</h2>
</body>
EOF
reboot


cat <<EOF
</body>
</html>
EOF
