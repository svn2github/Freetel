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
echo "$ARG"

if [ $ARG = "restartstatic" ]; then
	echo "hi there"
fi

# /etc/init.d/network "$ARG"
cat <<EOF
</body>
</html>
EOF
