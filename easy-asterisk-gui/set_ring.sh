#!/bin/sh
# set_ring.sh
# David Rowe 4 Jan 2010
# CGI to set which extensions ring on uncoming calls

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

# extract extensions to ring and reload extensions.conf

args=`echo "$QUERY_STRING" | sed -n "s/=on*//pg" | sed -n 's,_,/,pg' | sed -n "s/&/\\\\&/p"`
sed -i "s_s,1,Dial(.*) ;; easy-asterisk_s,1,Dial($args) ;; easy-asterisk_" /etc/asterisk/extensions.conf
asterisk -rx "dialplan reload" >> /dev/null

# bounce us back to Phones screen

cat <<EOF
<html>
<head>
<title>Easy Asterisk - Set Ring</title>
<meta http-equiv="REFRESH" content="0;url=http:phones.sh">
<body>
Please wait a few seconds.....
</body>
</head>
</html>
EOF

