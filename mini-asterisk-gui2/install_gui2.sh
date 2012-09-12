# Script to install GUI2 to IP04
# Developement Edition
# Ian Roberts
# Sept 2012

# check we are on an IP0x box
echo "must be root directory on IP04"
echo ""


# remove old copy

rm gui2.tar.gz


# get the source

wget -O gui2.tar.gz http://freetel.svn.sourceforge.net/viewvc/freetel/mini-asterisk-gui2/?view=tar

#mv index.html?view=tar mag2.tar.gz

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


# check cgi support in lighttpd

#Uncomment cgi support in /etc/lighttpd.conf 
#Restart lighttpd
echo ""
echo "Point your browser at 192.168.1.30/gui2/phones.html"



