#!/bin/sh
# reset.cgi
# Ian Roberts 31 May 2012
#
# CGI to reset ipox

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head></head>
<body>
EOF


# check what sort of machine we are running on.  Many operations
# outlawed for x86

cat /proc/cpuinfo | grep "CPU:.*ADSP" > /dev/null
if [ $? -eq 0 ]; then
    mach="ip0x"
else
    mach="x86"
fi



if [ $mach != "ip0x" ] ; then
	echo "Sorry, only supported on the IP0X"
cat <<EOF
</body>
</html>
EOF
	exit
fi

# reset the dialplan files
cp /etc/asterisk/extensions.conf.def /etc/asterisk/extensions.conf
cp /etc/asterisk/sip.conf.def /etc/asterisk/sip.conf
# reload the dialplans in asterisk
asterisk -rx "sip reload" 2>/dev/null 1 > /dev/null
asterisk -rx "dialplan reload" 2>/dev/null 1 > /dev/null


cat <<EOF
</body>
</html>
EOF
