#!/bin/sh -x
# setlinekey2.cgi
# David Rowe 29 Oct 2012
#
# CGI to set the text in "file" from "this" to "that"
#
# Test on host with:
#
#    $ export QUERY_STRING='file=test.txt&this=IPADDRESS=&that=192.168.1.31&key=mykey'; ./setlinekey.cgi
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
key=`echo $QUERY_STRING | grep -oe "key=[^&?]*" | sed -n "s/key=//p" | sed "s/%22/\"/g"`
echo $QUERY_STRING
echo $file
echo $this
echo $that
echo $key
if [ -f $file ]; then
#  sed -i  "s/\($this\)[a-z0-9\.]*\(.*$key.*\)/\1$that\2/"  $file
rhs=`cat $file | grep -oe "host=[a-z0-9\.]*" | sed -n "s/host=//p"`
cat $file | sed "s/$rhs/$that/"
else
  echo "$file does not exist"
fi
 
cat <<EOF
</body>
</html>
EOF
