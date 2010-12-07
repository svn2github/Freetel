#!/bin/sh
# getsig.cgi
# David Rowe 6 Dec 2010
#
# CGI to fetch the signal strength from a node
# Requires the node to be running sigstr.sh script as 
# a telnet dameon port 4950

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head></head>
<body>
EOF

ip=`echo "$QUERY_STRING" | sed -n "s/.*ip=//p" | sed "s/,/ /g"`
nc $ip 4950
echo $ip

cat <<EOF
</body>
</html>
EOF
