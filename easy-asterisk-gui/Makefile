# Makefile
# David Rowe 1 Jan 2010
# 
# Makefile for Easy Asterisk GUI
#
# usage: make TESTIP=test.machine.ip|hostname test
#        make TESTIP=192.168.1.150 test
#        make TESTIP=boris test

test:
	rcp cgi-bin/*.html cgi-bin/*.sh cgi-bin/*.pl cgi-bin/*.js cgi-bin/*.css  cgi-bin/*.png root@$(TESTIP):/www
	rcp etc/asterisk/*.conf root@$(TESTIP):/etc/asterisk
