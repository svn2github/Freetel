#!/bin/sh
# cleardb.cgi
# David Rowe 25 May 2013
#
# CGI to delete entire bearing database

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

P=/var/www/foxy
rm $P/bearings.txt

cat <<EOF
</body>
</html>
EOF
