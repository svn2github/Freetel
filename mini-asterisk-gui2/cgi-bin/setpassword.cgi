#!/bin/sh
# setpassword.cgi
# Ian Roberts 31 May 2012
#
# CGI to set password on ipox

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

pass=`echo "$QUERY_STRING" | grep -oe "pass=[^&?]*" | sed -n "s/pass=//p"`
passwd_cmdline $pass


cat <<EOF
</body>
</html>
EOF
