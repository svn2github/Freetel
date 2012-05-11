#!/bin/sh
# ipkg.cgi
#  Ian Roberts
#  May 12 2012
# CGI to execute ipkg <argument>
# script expects only one argument
# valid arguments are list_installed, list etc

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head></head>
<body>
EOF

arg=`echo "$QUERY_STRING" | sed -n "s/.*arg=//p" | sed "s/,/ /g"`
#Todo : Test for one argument
#Todo : Test for valid argument
ipkg $arg

cat <<EOF
</body>
</html>
EOF
