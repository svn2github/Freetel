#!/bin/sh
# addnode.cgi
# David Rowe 4 Dec 2010
#
# CGI to add a new node

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head></head>
<body>
EOF

lat=`echo "$QUERY_STRING" | sed -n "s/.*lat=\(.*\)&lng.*/\1/p"`
lng=`echo "$QUERY_STRING" | sed -n "s/.*lng=\(.*\)&ip.*/\1/p"`
ip=`echo "$QUERY_STRING" | sed -n "s/.*ip=\(.*\)/\1/p"`

echo $lat,$lng,$ip >> /var/www/dilimesh/nodes.txt

echo $QUERY_STRING "<br>"
echo "<br>"
echo $lat $lng $ip

cat <<EOF
</body>
</html>
EOF
