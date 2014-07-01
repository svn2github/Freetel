#!/bin/sh
# getvis.cgi
# David Rowe 4 Dec 2010
#
# CGI to fetches vis server data
# I am sure it is possible to do this directly from Javascript
# but I am too much of a noob.

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head>
<body>
EOF

VIS_SERVER_HOST=`echo "$QUERY_STRING" | sed -n "s/.*ip=//p"`

nc $VIS_SERVER_HOST 2005 2>/dev/null >/dev/null 
if [ $? -eq 0 ] ; then
  nc $VIS_SERVER_HOST 2005 | sed -n "/{/ p" 
else
  echo "Can't connect to Visualisation Server on $VIS_SERVER_HOST"
fi

cat <<EOF
</body>
</html>
EOF

