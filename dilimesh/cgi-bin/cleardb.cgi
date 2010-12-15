#!/bin/sh
# cleardb.cgi
# David Rowe 15 Dec 2010
#
# CGI to delete entire node database

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head></head>
<body>
EOF

# path to node database text file

P=/var/www/dilimesh
rm $P/nodes.txt

cat <<EOF
</body>
</html>
EOF
