#!/bin/sh
# setline.cgi
# David Rowe 12 Dec 2010
#
# CGI to set the text on a particular line

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head></head>
<body>
EOF

LINE=`echo "$QUERY_STRING" | sed -n "s/.*line=//p"`
echo $LINE

cat <<EOF
</body>
</html>
EOF
