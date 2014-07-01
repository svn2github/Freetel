#!/bin/sh 
# uncommentkey.cgi
# David Rowe 30 Oct 2012
#
# CGI to uncomment a chunk of text that has a certain key phrase on each line
#
# Test on host with:
#
#    $ export QUERY_STRING='file=test.txt&key=Jazmin mini'; ./uncommentkey.cgi
#
# try using test file:
# 
#   root:/root> cat test.txt 
#   ;[user]                       ; Jazmin mini-asterisk do not remove this comment
#   ;host=sip.jazmin.net.au       ; Jazmin mini-asterisk do not remove this comment
#   ;secret=<Password>            ; Jazmin mini-asterisk do not remove this comment           

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
key=`echo $QUERY_STRING | grep -oe "key=[^&?]*" | sed -n "s/key=//p" | sed "s/%22/\"/g"`
echo $QUERY_STRING
echo $file
echo $key
if [ -f $file ]; then
  # to debug remove -i (write results to file), it will then just print results
  perl -i -pe "s/^;(.*$key)/\$1/" $file
else
  echo "$file does not exist"
fi
 
cat <<EOF
</body>
</html>
EOF
