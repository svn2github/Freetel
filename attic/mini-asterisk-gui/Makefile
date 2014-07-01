# Makefile
# David Rowe 1 Jan 2010
# 
# Makefile for Mini Asterisk GUI
#
# 1/ Download files to development box for testing:
#
#    usage: make HOST=test.machine.ip|hostname test
#           make HOST=192.168.1.150 test
#           make HOST=boris test
#
# 2/ Create static copy of web pages from HOST for static demo
#
#    usage: make HOST=test.machine.ip static

test:
	scp cgi-bin/*.html cgi-bin/*.sh cgi-bin/*.pl cgi-bin/*.js cgi-bin/*.css \
	cgi-bin/*.png root@$(HOST):/www
	scp etc/asterisk/*.conf root@$(HOST):/etc/asterisk

.PHONY : static
static: 
	# generate static pages

	rm -Rf static; mkdir static; cd static; \
	wget -rEk --header="Cookie: loggedin=1" http://$(HOST)/phones.sh

	# modify links to non-existent scripts on static demo

	sed -i "s,http://$(HOST)/set_network.sh,network.sh.html," static/$(HOST)/network.sh.html
	sed -i "s,http://$(HOST)/set_ring.sh,phones.sh.html," static/$(HOST)/phones.sh.html
	sed -i "s,http://$(HOST)/set_voiplines.sh,voiplines.sh.html," static/$(HOST)/voiplines.sh.html
	sed -i "s,url=login.sh,url=phones.sh.html," static/$(HOST)/logout.sh.html


