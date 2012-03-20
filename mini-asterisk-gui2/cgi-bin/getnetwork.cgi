#!/bin/sh
# getnetwork.cgi
# Ian Roberts 19 March 2012
#
# CGI to return network connection details

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

internet=`fping google.com`;
#packet_loss=`fping google.com -c 1 -q | sed -n 's/.*received, \(.*\)%.*/\1/p'`
#if [ $packet_loss = "0" ]; then
#    internet="yes";
#fi

echo INTERNET=\"$internet\"
#echo PACKETLOSS=\"$packet_loss\"

cat <<EOF
</body>
</html>
EOF
