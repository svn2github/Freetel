#!/bin/sh
# asterisk.cgi
# David Rowe 3 March 2011 
#
# CGI to return the output from an Asterisk command line

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

arg=`echo "$QUERY_STRING" | sed -n "s/.*arg=//p"`
ifconfig $arg
cat <<EOF
</body>
</html>
EOF
