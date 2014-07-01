#!/bin/sh
# setring.cgi
# 
# Ian Roberts 30 August 2012
#
# CGI to set extension to ring on an incoming call
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


# extract extensions to ring 
# must replace underscores with forward slash

echo "Input $QUERY_STRING"

a=`echo "$QUERY_STRING" | sed -n "s/=on*//pg" | sed -n 's,_,/,pg'`

# escape & if present
echo "$a" | grep "&" >> /dev/null
if [ $? -eq 0 ]; then
  a=`echo $a | sed -n "s/&/\\\\&/pg"`
fi

echo "Mods $a"
sed -i "s_s,1,Dial(.*) ;; mini-asterisk_s,1,Dial($a) ;; mini-asterisk_" /etc/asterisk/extensions.conf
#sed -i "s_s,1,Dial(.*) ;; mini-asterisk_s,1,Dial($a) ;; mini-asterisk_" /etc/asterisk/extensions.conf

#reload extensions.conf

# do this from js with a call to asterisk.cgi
#asterisk -rx "dialplan reload" 2>/dev/null 1 > /dev/null

 
cat <<EOF
</body>
</html>
EOF
