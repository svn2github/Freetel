#!/bin/sh
# logout.sh
# David Rowe 4 Jan 2010
# CGI for Easy Asterisk logout GUI

cat <<EOF
Content-type: text/html
Set-Cookie: loggedin=1; expires=Thursday, 01-Jan-98 12:00:00 GMT

<title>Easy Asterisk - Logout</title>
<meta http-equiv="REFRESH" content="0;url=http:login.sh"
EOF
#echo $HTTP_COOKIE
