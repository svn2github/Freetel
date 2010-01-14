#!/bin/sh
# set_voipline.sh
# # David Rowe 12 Jan 2010
#
# CGI to set voip line parameters in sip.conf.

# check we are logged in

echo $HTTP_COOKIE | grep "loggedin" > /dev/null
if [ $? -eq 1 ]; then
    echo "<html>"
    echo "<head>"
    echo '<meta http-equiv="REFRESH" content="0;url=http:login.sh">'
    echo "</head>"
    echo "</html>"
    exit
fi

user=`echo "$QUERY_STRING" | grep -oe "user=[^&?]*" | sed -n "s/user=//p"`
pass=`echo "$QUERY_STRING" | grep -oe "pass=[^&?]*" | sed -n "s/pass=//p"`
host=`echo "$QUERY_STRING" | grep -oe "host=[^&?]*" | sed -n "s/host=//p"`
stanza=`echo "$QUERY_STRING" | grep -oe "stanza=[^&?]*" | sed -n "s/stanza=//p"`

# create new sip.conf with selected provider uncommented

echo "set_voiplines.sh" $user $pass $host $stanza >> /tmp/log.txt
./set_voiplines.pl $user $pass $host $stanza > /etc/asterisk/sip.conf.new
mv /etc/asterisk/sip.conf /etc/asterisk/sip.conf.bak
mv /etc/asterisk/sip.conf.new /etc/asterisk/sip.conf

# get asterisk to load changes

asterisk -rx "sip reload" 2>/dev/null 1 > /dev/null

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

