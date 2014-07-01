#!/bin/sh -x
# set_ring.sh
# David Rowe 4 Jan 2010
# CGI to set which extensions ring on uncoming calls

# check we are logged in

echo $HTTP_COOKIE | grep "loggedin" > /dev/null
if [ $? -eq 1 ]; then
    echo "<html>"
    echo "<head>"
    echo '<meta http-equiv="REFRESH" content="0;url=login.sh">'
    echo "</head>"
    echo "</html>"
    exit
fi

# extract extensions to ring and reload extensions.conf

a=`echo "$QUERY_STRING" | sed -n "s/=on*//pg" | sed -n 's,_,/,pg'`

# escape & if present
echo "$a" | grep "&" >> /dev/null
if [ $? -eq 0 ]; then
  a=`echo $a | sed -n "s/&/\\\\&/pg"`
fi

sed -i "s_s,1,Dial(.*) ;; mini-asterisk_s,1,Dial($a) ;; mini-asterisk_" /etc/asterisk/extensions.conf
asterisk -rx "dialplan reload" 2>/dev/null 1 > /dev/null

# bounce us back to Phones screen

cat <<EOF
<html>
<head>
<title>Mini Asterisk - Set Ring</title>
<meta http-equiv="REFRESH" content="0;url=phones.sh">
<body>
Please wait a few seconds.....
EOF
#echo $QUERY_STRING "<br>"
#echo "$QUERY_STRING" | sed -n "s/=on*//pg" | sed -n 's,_,/,pg'
#echo "<br>"
#echo $a

cat <<EOF
</body>
</head>
</html>
EOF

