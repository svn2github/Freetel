#!/bin/sh
# delbearing.cgi
# David Rowe 25 May 2012
#
# CGI to delete a bearing

cat <<EOF
Content-type: text/html

<html>
<head>
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="expires" content="-1"></head></head>
<body>
EOF

lat=`echo "$QUERY_STRING" | sed -n "s/lat=\(.*\)&.*/\1/pg"`
lng=`echo "$QUERY_STRING" | sed -n "s/.*lng=\(.*\)/\1/pg"`

# path to node database text file

P=/var/www/foxy

cat $P/bearings.txt | sed "/$lat,$lng.*/ d" > $P/bearings.tmp
cp $P/bearings.tmp $P/bearings.txt
rm $P/bearings.tmp

#echo $QUERY_STRING "<br>"
#echo "<br>"
#echo $lat $lng

cat <<EOF
</body>
</html>
EOF
