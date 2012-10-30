#!/bin/sh 
# setlinekey2.cgi
# David Rowe 29 Oct 2012
#
# CGI to set the text in "file" from "this" to "that" on lines with text "key"
#
# Test on host with:
#
#    $ export QUERY_STRING='file=test.txt&this=host=&that=192.168.1.31&key=Jazmin mini'; ./setlinekey2.cgi
#
# using test file:
# 
#   root:/root> cat test.txt 
#   host=192.168.1.28       ; Jazmin mini-asterisk do not remove this comment
#   dtmfmod=rfc2833         ; Jazmin mini-asterisk do not remove this comment
#   host=192.168.1.29       ; SIP mini-asterisk do not remove this comment

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
  # to debug remove -i (write results to file), it will then just print results
  perl -i -pe "s/($this)[A-Za-z0-9\.\<\>]*(.*$key.*)/$this$that\$2/" $file
else
  echo "$file does not exist"
fi
 
cat <<EOF
</body>
</html>
EOF
