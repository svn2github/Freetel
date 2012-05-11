#!/bin/sh
# sipshowpeers.cgi
#  Ian Roberts
#  May 11 2012
# CGI to execute get sip phone info
# no argument

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head></head>
<body>
EOF

#arg=`echo "$QUERY_STRING" | sed -n "s/.*arg=//p" | sed "s/,/ /g"`

#Todo : Test for asterisk running 

asterisk "-rx sip show peers" 2>/dev/null

cat <<EOF
</body>
</html>
EOF
