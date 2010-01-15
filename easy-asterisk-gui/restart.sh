#!/bin/sh
# restart.sh
# David Rowe 15 Jan 2010
# Restart script for Easy Asterisk GUI

cat <<EOF    
<html>
<title>Easy Asterisk - Restart</title>
<meta http-equiv="REFRESH" content="2;url=http:login.sh"
<h2>Restarting...come back in 1 minute</h2>
</html>
EOF
reboot

