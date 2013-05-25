#!/bin/sh
# addbearing.cgi
# David Rowe 25 May 2013
#
# CGI to add a new bearing

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
lng=`echo "$QUERY_STRING" | sed -n "s/.*lng=\(.*\)&bearing.*/\1/p"`
bearing=`echo "$QUERY_STRING" | sed -n "s/.*bearing=\(.*\)/\1/p"`

echo $lat,$lng,$bearing >> /var/www/foxy/bearings.txt

echo $QUERY_STRING "<br>"
echo "<br>"
echo $lat $lng $bearing

cat <<EOF
</body>
</html>
EOF
