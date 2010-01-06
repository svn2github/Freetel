# Makefile
# David Rowe 1 Jan 2010
# Makefile for easy Asterisk GUI

TESTIP=192.168.1.150

#test:
#	rcp set_network.sh get_network.sh root@$(TESTIP):/var/lib/asterisk/scripts
#	rcp index.html root@$(TESTIP):/var/lib/asterisk/static-http/

test:
	rcp at-530.html ipphones.pl ipphones.sh ipphones.js voipline.jpg anphone.png phoneline.jpg ipphone.png phones.pl tooltips.html phones.sh phones.js tick.png cross.png banner.html menu.html check_loggedin.sh logout.sh login.sh network.sh network.js set_network.sh tooltip.js tooltip.css root@$(TESTIP):/www/cgi-bin
	rcp sip.conf extensions.conf root@$(TESTIP):/etc/asterisk
