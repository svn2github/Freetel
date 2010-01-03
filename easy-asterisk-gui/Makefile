# Makefile
# David Rowe 1 Jan 2010
# Makefile for easy Asterisk GUI

TESTIP=192.168.1.150

#test:
#	rcp set_network.sh get_network.sh root@$(TESTIP):/var/lib/asterisk/scripts
#	rcp index.html root@$(TESTIP):/var/lib/asterisk/static-http/

test:
	rcp test.sh test1.sh easy.js easy1.js set_network.sh root@$(TESTIP):/www/cgi-bin
