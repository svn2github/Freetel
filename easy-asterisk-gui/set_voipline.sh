#!/bin/sh
# set_voipline.sh
# # David Rowe 12 Jan 2010
#
# CGI to set voip line parameters in sip.conf.

cat <<EOF
<html>
<head>
<title>set_network.sh</title>
<meta http-equiv="REFRESH" content="0;url=http:network.sh">
<body>
Please wait a few seconds.....
</body>
</head>
</html>
EOF

user=`echo "$QUERY_STRING" | grep -oe "user=[^&?]*" | sed -n "s/user=//p"`
pass=`echo "$QUERY_STRING" | grep -oe "pass=[^&?]*" | sed -n "s/pass=//p"`
host=`echo "$QUERY_STRING" | grep -oe "host=[^&?]*" | sed -n "s/host=//p"`

./set_voipline.pl $user $pass $host > /etc/asterisk/sip.conf

cat <<EOF
<html>
<head>
<title>set_voiplines.sh</title>
<meta http-equiv="REFRESH" content="0;url=http:voiplines.sh">
<body>
Please wait a few seconds.....
</body>
</head>
</html>
EOF