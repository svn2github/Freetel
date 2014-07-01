# Script to install GUI2 to IP04
# Original gui left intact
# New gui at 192.168.1.30/gui2/phones.html
# Developement Edition
# Ian Roberts
# Sept 2012

# check we are on an IP0x box
echo "must be root directory on IP0x"
echo ""
cd /

# remove old copy if exists

rm gui2.tar.gz


# get the source

wget -O gui2.tar.gz http://freetel.svn.sourceforge.net/viewvc/freetel/mini-asterisk-gui2/?view=tar


echo ""

# unpack

tar zxvf gui2.tar.gz


# check root

#testing
#mkdir www
#mkdir www/cgi-bin


#create directories if required

mkdir /www/gui2
mkdir /www/cgi-bin

# copy web files to /www

cp mini-asterisk-gui2/*.html /www/gui2
cp mini-asterisk-gui2/*.js /www/gui2
cp mini-asterisk-gui2/*.png /www/gui2
cp mini-asterisk-gui2/*.css /www/gui2


# copy cgi to /www/cgi-bin

cp mini-asterisk-gui2/cgi-bin/*.* /www/cgi-bin


# check packages installed

ipkg install fping


# check extension.conf 60xx lines have mini_ext tags

cp mini-asterisk-gui2/etc/asterisk/extensions.conf /etc/asterisk/extensions.conf

# check sip.conf has new provider keywords

cp mini-asterisk-gui2/etc/asterisk/sip.conf /etc/asterisk/sip.conf

# check lighttpd installed
# check cgi support in lighttpd

cp mini-asterisk-gui2/etc/lighttpd.conf /etc/lighttpd.conf

# webserver index should point browser at gui2 login

# restart the http server
/etc/init.d/lighttpd stop
/etc/init.d/lighttpd start


echo ""
echo "Point your browser at 192.168.1.30/gui2/phones.html"



