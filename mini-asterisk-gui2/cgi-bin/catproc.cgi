#!/bin/sh
# catprocversion.cgi
#  Ian Roberts
#  May 7 2012
# CGI to cat /proc/(argument)

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head></head>
<body>
EOF

what=`echo "$QUERY_STRING" | sed -n "s/.*arg=//p" | sed "s/,/ /g"`
cat /proc/$what

cat <<EOF
</body>
</html>
EOF
