#!/bin/sh
# getconf.cgi
# David Rowe 12 Dec 2010
#
# CGI to return an Asterisk conf file

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

CONF_PATH=/etc/asterisk
FILE=`echo "$QUERY_STRING" | sed -n "s/.*file=//p"`
if [ -f $CONF_PATH/$FILE ] ; then 
  cat $CONF_PATH/$FILE
fi

cat <<EOF
</body>
</html>
EOF
