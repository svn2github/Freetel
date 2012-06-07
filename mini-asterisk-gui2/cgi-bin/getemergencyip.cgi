#!/bin/sh
# getemergency.cgi
# David Rowe 8 March 2011
#
# CGI to return emergency IP details

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

if [ -f /etc/rc.d/S05network-backdoor ]; then
  backdoor=`sed -n 's/IPADDRESS="\(.*\)"/\1/p' /etc/init.d/network-backdoor`
  echo BACKDOOR=\"$backdoor\"
fi

cat <<EOF
</body>
</html>
EOF
