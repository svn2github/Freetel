#!/bin/sh
# delnode.cgi
# David Rowe 4 Dec 2010
#
# CGI to delete a node

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

P=/var/www/dilimesh

cat $P/nodes.txt | sed "/$lat,$lng.*/ d" > $P/nodes.tmp
cp $P/nodes.tmp $P/nodes.txt
rm $P/nodes.tmp

#echo $QUERY_STRING "<br>"
#echo "<br>"
#echo $lat $lng

cat <<EOF
</body>
</html>
EOF
