Easy Asterisk
=============

Introduction
------------

Easy Asterisk is a simple Web GUI for Asterisk with comprehensive tool
tip documentation and a learning curve of a few minutes.  Analog ports
and IP Phones are automatically detected, and no knowledge of Asterisk
configuration is required.

Easy Asterisk is "unfeatured" - most of the Asterisk features are
hidden by the GUI.  Instead it tries to make very basic, very common
Asterisk installations fast and simple, for example:

* An Asterisk server running on your home gateway/firewall/server.
  You want to connect a few IP Phones and make cheap phones calls
  using VOIP.  An CD based Asterisk distro is a possibility but you
  don't want to dedicate a full PC just for Asterisk.  You don't
  really want to learn obscure dial plan syntax and yet another conf
  file format.

* An small office that already has an old analog phone system.  You
  want to keep your current analog lines for incoming calls, but
  install 8 IP Phones and use VOIP for outgoing calls.  You know
  enough to set up a DSL router but don't want to rely on some phone
  or computer guy at $100/hr to maintain your phone system.

Implementation
--------------

Easy Asterisk is written for the
http://rowetel.com/ucasterisk/ip04.html[IP0X embedded Asterisk]
hardware but will also run on x86 and probably many other platforms.
Easy Asterisk does not require a dedicated PC, you do not need to
reformat a hard disk and install an ISO CD image.

It is implemented as a set of CGI scripts (shell, perl, java-script)
and pre-configured extensions.conf and sip.conf files.  Only a basic
web server and microperl support are required, making it suitable for
small embedded platforms.  No Perl libraries or CPAN are required.

Easy Asterisk is "unfeatured" - it hides many of the advanced Asterisk
features in the interest of simple and fast configuration.  However
once you get started and your experience builds these features are of
course available using regular Asterisk configuration techniques.

Easy Asterisk has been written to be compatible with regular Asterisk
conf-file style configuration.  Just leave the conf file lines with
"easy-asterisk" comments alone.  The Asterisk conf files
extensions.conf and sip.conf are directly modified by Easy Asterisk,
but changes are limited to the "easy-asterisk" lines.  

No database is used.  Neither is the users.conf/AJAX style interface
based on the Asterisk built-in web server which should be disabled
when using Easy Asterisk.

For Auto-detection of Zap ports Easy Asterisk looks at
/etc/zapata.conf, so you may need this set up correctly for your
analog hardware.  On the IP0X this happens automatically.

Installation
------------

Note this process will overwrite your Asterisk extensions.conf and
sip.conf files - back them up if you have an existing Asterisk
installation that you want to keep.

IP0X:

   -------------------------------------------------------------------
   root~> ipkg install easy-asterisk
   root~> reboot
   -------------------------------------------------------------------

x86: 

1. You need a web server, Asterisk and some sort of Perl installed
   (very basic Perl installation is fine).  Configure your web server
   to run CGIs (.sh and .pl) from /your/web/server/root (lighttpd
   config instructions below).

2. Login as root to install the Easy Asterisk files:

   -------------------------------------------------------------------
   # cd ~ 
   # svn co https://freetel.svn.sourceforge.net/svnroot/freetel/easy-asterisk-gui
   # cp easy-asterisk-gui/etc/asterisk/* /etc/asterisk 
   # cp easy-asterisk-gui/cgi-bin/* /your/web/server/root 
   # mv etc/asterisk/users.conf etc/asterisk/users.conf.bak
   -------------------------------------------------------------------

   The final step above may not be required on your machine if you
   don't have a users.conf.

3. Switch off the internal Asterisk web server by editing
   /etc/asterisk.httpd.conf.  Make sure the enabled line reads like
   this:

   enabled=no

   Then stop and restart Asterisk.

4. I use lighttpd as the web server, the /etc/lighttpd.conf lines
   required are:

   -------------------------------------------------------------------
   cgi.assign = ( ".sh" => "/bin/sh",".pl" => "/usr/sbin/microperl" )
   -------------------------------------------------------------------
