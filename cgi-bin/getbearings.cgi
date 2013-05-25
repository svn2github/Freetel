#!/bin/sh
# getbearings.cgi
# David Rowe 25 May 2013
#
# CGI to return list of bearings from the bearings.txt database text file
# We use a CGI rather than fetching the text file directly
# so we can stop Firefox from caching

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

BEARINGS=/var/www/foxy/bearings.txt

if [ -f $BEARINGS ] ; then 
  cat $BEARINGS
fi

cat <<EOF
</body>
</html>
EOF
