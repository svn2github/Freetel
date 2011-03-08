#!/bin/sh
# fpingnodes.cgi
# David Rowe 5 Dec 2010
#
# CGI to fping a bunch of nodes

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
fping $ip -a -b1400 -r1 2>/dev/null

cat <<EOF
</body>
</html>
EOF
