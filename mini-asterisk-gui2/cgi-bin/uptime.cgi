#!/bin/sh
# uptime.cgi
#  Ian Roberts
#  May 12 2012
# CGI to execute uptime
# no arguments

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head></head>
<body>
EOF

#arg=`echo "$QUERY_STRING" | sed -n "s/.*arg=//p" | sed "s/,/ /g"`
uptime

cat <<EOF
</body>
</html>
EOF
