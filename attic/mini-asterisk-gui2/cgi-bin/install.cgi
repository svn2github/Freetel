#!/bin/sh
# install.cgi
# Ian Roberts 31 May 2012
#
# CGI to install new firmware....
# runs a shellscript as root

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
	echo "<html>Sorry, only supported on the IP0X</html>"
	exit
fi

firmwareurl=`echo "$QUERY_STRING" | grep -oe "firmwareurl=[^&?]*" | sed -n "s/firmwareurl=//p"`
# convert URL encoded string, this can just handle : and / so be careful
# with script names
firmwareurl=`echo $firmwareurl | sed -e "s/%3A/:/" -e "s_%2F_/_g"`
wget $firmwareurl
filename=`echo $firmwareurl | sed 's_^.*/__'`
sh $filename
exit

cat <<EOF
</body>
</html>
EOF
