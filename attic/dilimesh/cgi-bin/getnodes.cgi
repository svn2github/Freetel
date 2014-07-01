#!/bin/sh
# getnodes.cgi
# David Rowe 5 Dec 2010
#
# CGI to return node list from database text file
# We use a CGI rather than fetching the text file directly
# so we can control Firefox from caching

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

NODES=/var/www/dilimesh/nodes.txt

if [ -f $NODES ] ; then 
  cat $NODES
fi

cat <<EOF
</body>
</html>
EOF
