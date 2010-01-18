#!/bin/sh
# logout.sh
# David Rowe 4 Jan 2010
# CGI for Mini Asterisk logout GUI

cat <<EOF
Content-type: text/html
Set-Cookie: loggedin=1; expires=Thursday, 01-Jan-98 12:00:00 GMT

<title>Mini Asterisk - Logout</title>
<meta http-equiv="REFRESH" content="0;url=login.sh"
EOF
#echo $HTTP_COOKIE
