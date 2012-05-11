#!/bin/sh
# catproc.cgi
#  Ian Roberts
#  May 7 2012
# CGI to execute cat /proc/<argument>
# script expects only one argument
# valid arguments are version, loadavg, cpuinfo, mtd, meminfo, yaffs

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head></head>
<body>
EOF

arg=`echo "$QUERY_STRING" | sed -n "s/.*arg=//p" | sed "s/,/ /g"`
#Todo : Test for one argument
#Todo : Test for valid argument
cat /proc/$arg

cat <<EOF
</body>
</html>
EOF
