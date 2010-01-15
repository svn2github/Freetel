# Makefile
# David Rowe 1 Jan 2010
# 
# Makefile for Easy Asterisk GUI
#
# usage: make TESTIP=test.machine.ip|hostname test
#        make TESTIP=192.168.1.150 test
#        make TESTIP=boris test

test:
	rcp restart.sh voiplines.js set_voiplines.pl set_voiplines.sh voiplines.sh voiplines.pl set_ring.sh prototype.js admin.sh index.html faq.sh faq.html about.sh at-530.html ipphones.pl ipphones.sh ipphones.js voipline.jpg anphone.png phoneline.jpg ipphone.png phones.pl tooltips.html phones.sh phones.js tick.png cross.png banner.html menu.html check_loggedin.sh logout.sh login.sh network.sh network.js set_network.sh tooltip.js tooltip.css root@$(TESTIP):/www
	rcp sip.conf extensions.conf root@$(TESTIP):/etc/asterisk
