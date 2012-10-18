#!/bin/sh
# asterisk.cgi
# David Rowe 22 Dec 2010
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

CLI=`echo "$QUERY_STRING" | sed -n "s/.*cli=//p" | sed -n "s/%20/ /pg"`
echo "$CLI"
asterisk -rx "$CLI"
echo hello
cat <<EOF
</body>
</html>
EOF
