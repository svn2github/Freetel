#!/bin/sh
# upgrade.cgi
# Ian Roberts 31 May 2012
#
# CGI to upgrade mini-asterisk-gui

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

# remove, update and install mini-asterisk-gui

rev_before=`grep -oe "Revision: [0-9]*" /www/about.sh`
echo "<strong>Current $rev_before</strong><br>"
echo "<strong>Removing $ver_before</strong><br>"
ipkg remove mini-asterisk-gui | tr '\n' '#' | sed -n 's/\#/<br>/pg'
echo "<strong>Installing...</strong><br>"
ipkg update | tr '\n' '#' | sed -n 's/\#/<br>/pg'
ipkg install mini-asterisk-gui | tr '\n' '#' | sed -n 's/\#/<br>/pg'


cat <<EOF
</body>
</html>
EOF
