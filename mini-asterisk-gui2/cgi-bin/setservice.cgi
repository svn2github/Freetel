#!/bin/sh
# setservice.cgi
# Ian Roberts 18 Sept 2012
#
# CGI to return the output from an service command 
#
# valid actions are restartstatic, restartdhcp, restartbackup, enabledhcp, enablestatic
#
cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1">
</head>
<body>
EOF

ARG=`echo "$QUERY_STRING" | sed -n "s/.*action=//p"`
echo "arg is $ARG"

if [ $ARG = "restartstatic" ]; then
	echo "restart static"
	/etc/init.d/network-static stop
	/etc/init.d/network-static start
fi

if [ $ARG = "restartbackup" ]; then
	echo "restartbackup"
	/etc/init.d/network-backup stop
	/etc/init.d/network-backup start
fi

if [ $ARG = "restartdhcp" ]; then
	echo "restartdhcp"
	/etc/init.d/network stop
	/etc/init.d/network start
fi

if [ $ARG = "enabledhcp" ]; then
	echo "enabledhcp"
    /etc/init.d/network-static disable
    /etc/init.d/network-static stop
    /etc/init.d/network enable
    /etc/init.d/network start
fi

if [ $ARG = "enablestatic" ]; then
	echo "enable static"
    /etc/init.d/network stop
    /etc/init.d/network disable
    /etc/init.d/network-static enable
fi

cat <<EOF
</body>
</html>
EOF
