#!/bin/sh
# setword.cgi
# Ian Roberts 29 Oct 2012
#
# CGI to replace the text in "file" from "this" to "that"
#
# Test on host with:
#
#    $ export QUERY_STRING='file=test.txt&this=IPADDRESS&that="192.168.1.31"'; ./setword.cgi
#

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head></head>
<body>
EOF

file=`echo $QUERY_STRING | grep -oe "file=[^&?]*" | sed -n "s/file=//p"`
this=`echo $QUERY_STRING | grep -oe "this=[^&?]*" | sed -n "s/this=//p" | sed "s/%22/\"/g"`
that=`echo $QUERY_STRING | grep -oe "that=[^&?]*" | sed -n "s/that=//p" | sed "s/%22/\"/g"`
echo $QUERY_STRING
echo $file
echo $this
echo $that
if [ -f $file ]; then
  sed -i "s/$this/$that/g" $file
else
  echo "$file does not exist"
fi
 
cat <<EOF
</body>
</html>
EOF
