Mini Asterisk
=============

A simple, light weight GUI for Asterisk.

image::/images/ip04/mini.png["Mini Asterisk Demo", link="mini/phones.sh.html"]

Click on the screen shot above for an on line demo.

[[intro]]
Introduction
------------

Mini Asterisk is a simple Web GUI for Asterisk with comprehensive tool
tip documentation and a learning curve of a few minutes.  Analog ports
and IP Phones are automatically detected, and no knowledge of Asterisk
configuration is required.

Mini Asterisk is "unfeatured" - many of the Asterisk options are not
available.  Instead it tries to make very basic, very common Asterisk
installations fast and simple, for example:

* An Asterisk server running on your home gateway/firewall/server.
  You want to connect a few IP Phones and make cheap phones calls
  using VOIP.  An Asterisk distro on a CD is a possibility but you
  don't want to dedicate a full PC just for Asterisk.  You don't
  really want to learn obscure dial plan syntax and yet another conf
  file format.

* A small office that already has an old analog phone system.  You
  want to keep your current analog lines for incoming calls, but
  install 8 IP Phones and use VOIP for outgoing calls.  You know
  enough to set up a DSL router but don't want to rely on "the Phone
  Guy" or "The Computer Guy" at $100/hr to maintain your phone
  system.

* You are a "Phone Guy" who doesn't understand Linux and Asterisk but
  you want to install IP-PBXes.

[[why]]
Why Another Asterisk GUI?
-------------------------

There are http://www.voip-info.org/wiki/view/Asterisk+GUI[a lot of
Asterisk GUIs] out there already.  So why do we need another one?

Well I needed an Asterisk GUI that was very easy to use for the
link:ip04.html[IP0X devices] I sell.  Something that would lower the
technical skill required to install and maintain an Asterisk Phone
system.  Something my wife and kids could use.

I was also interested in exploring the ease-of-use meme, as we have
been discussing it a lot on the http://villagetelco.org[Village Telco]
project.  Just how easy can we make Asterisk to use? So I scratched
the itch.

Mini Asterisk has the following features that make it reasonably
unique.  They may be good or bad features depending on your point of
view!

* Mini Asterisk is "un-featured" - it hides many of the advanced
  Asterisk features in the interest of simple and fast configuration.

* Light weight so it can run on embedded boxes like the IP0X family.
  No SQL database or PHP or LAMP.  Only a basic web server and a very
  basic perl are required (e.g. microperl - no CPAN libraries).

* Works directly on extensions.conf and sip.conf, but honors any edits
  you make to these files.  So all the powerful Asterisk features are
  available in the background.

* Doesn't use AJAX or the built-in Asterisk web server or users.conf
  magic.  Plain old HTML, a little Java-script and CGIs written in
  shell script and Perl.

* Doesn't use the Asterisk programming "model".  For example you don't
  have to understand what a dial-plan is, much less understand how to
  code one.  Plain English terms are used instead, for example "Phones
  and Phone lines".  Terms like Asterisk, Linux, SIP, Zap/1 don't even
  get a mention.

* Extensive tool tip documentation.  No manual.

* Doesn't require a dedicated PC, not installed from an ISO CD.  So
  you can use it as a GUI for Asterisk on a little SOHO Linux box that
  is also your firewall, server etc.

* Mini Asterisk tells you when something is wrong, for example you get
  a warning if your Phone System can't see the Internet.

* Extensive pre-configuration of extensions.conf and sip.conf,
  pre-selected phone numbers, SIP trunks are selected from a pull-down
  menu (except they are called VOIP lines).  Analog ports are auto
  detected, at least on the IP0X.  This makes adding IP phones very
  fast and simple.

[[status]]
Status
------

Alpha:

* Works on IP0X.
* Not tested on x86. Several IP0X features are n/a for x86 and should be
  disabled when the x86 (or non-IP0X) platform is detected.
* Needs feedback from real users to see how useful the concept is and what
  (un)features need to be added.
* Need a few more (un)features to be added, and Voip Line screen
  populated with more ITSPs.
* But quite useable as it stands.

[[notes]]
Implementation Notes
--------------------

Mini Asterisk is written for the
http://rowetel.com/ucasterisk/ip04.html[IP0X embedded Asterisk]
hardware but will also run on x86 and probably many other platforms.

Mini Asterisk has been written to be compatible with regular Asterisk
conf file configuration.  Just leave the conf file lines with
"mini-asterisk" comments alone.  The Asterisk conf files
extensions.conf and sip.conf are directly modified by Mini Asterisk,
but changes are limited to the "mini-asterisk" lines.  

For Auto-detection of Zap ports Mini Asterisk looks at
/etc/zapata.conf, so you may need this set up correctly for your
analog hardware.  On the IP0X this happens automatically.

The scripts need to run as root to run asterisk commands.  This is
probably OK an on IP0X but not a good idea on an x86.  I am not happy
with this security model and would appreciate any suggestions on how
to improve it.

[[install]]
Installation
------------

Note: this process may overwrite your Asterisk extensions.conf and
sip.conf files - back them up if you have an existing Asterisk
installation that you want to keep.  The IP0X ipkg does attempt to
backup the modified conf files to *.bak but no guarantees....

However once mini-asterisk is installed it should honor any manual
changes made to extensions.conf and sip.conf.

IP0X Installation
~~~~~~~~~~~~~~~~~

Installation instructions for IP0X boxes running link:baps.html[BAPS],
with some version of Asterisk and Zaptel installed.

-------------------------------------------------------------------
root~> ipkg install mini-asterisk
-------------------------------------------------------------------

x86 Installation
~~~~~~~~~~~~~~~~

. You need a web server, Asterisk and some sort of Perl installed
(very basic Perl installation is fine).  Configure your web server to
run CGIs (.sh and .pl) from /www (lighttpd config instructions below).
Mini Asterisk expects all files (shell, perl, html etc) to be in the
same directory.  If you find this painful please <<support, let me know>>.

. The process below places the web files in /www, you may like to
place the files somewhere else like /www/asterisk.  One of the files
is named index.html so make sure you don't overwrite an existing
index.html.

. Login as root to install the Mini Asterisk files:
+
Remember to backup your existing extensions.conf & sip.conf in
/etc/asterisk
+
-------------------------------------------------------------------
# cd ~ 
# svn co https://freetel.svn.sourceforge.net/svnroot/freetel/mini-asterisk-gui
# ./update_revision.sh
# cp mini-asterisk-gui/etc/asterisk/* /etc/asterisk 
# cp mini-asterisk-gui/cgi-bin/* /www
# cd /etc/asterisk
# cp extensions.conf extensions.conf.def
# cp sip.conf sip.conf.def
# mv etc/asterisk/users.conf etc/asterisk/users.conf.bak
-------------------------------------------------------------------
+
The final step above may not be required on your machine if you don't
have a users.conf.  The .def copies are required by the "reset
defaults" feature on the admin screen.

. Switch off the internal Asterisk web server by editing
/etc/asterisk.httpd.conf.  Make sure the enabled line reads like this:

   enabled=no
+
Then stop and restart Asterisk.

. I use lighttpd as the web server, the /etc/lighttpd.conf lines
required are:
+
-------------------------------------------------------------------
cgi.assign = ( ".sh" => "/bin/sh",".pl" => "/usr/bin/perl" )
-------------------------------------------------------------------

[[contribute]]
Contributions
-------------

I welcome sip.conf entries for your favourite ITSP (VOIP service) to
help populate the Provider field of the
link:mini/voiplines.sh.html[Voip Line Screen].

[[support]]
Support
-------

Comments, features request, bugs please let me know using Free
Telephony Project http://www.rowetel.com/ucasterisk/#support[Support].

[[source]]
Source Code
-----------

Browse:

http://freetel.svn.sourceforge.net/viewvc/freetel/mini-asterisk-gui/[http://freetel.svn.sourceforge.net/viewvc/freetel/mini-asterisk-gui/]

Check Out:

  $ svn co https://freetel.svn.sourceforge.net/svnroot/freetel/mini-asterisk-gui

[[test]]
Testing and debugging scripts
-----------------------------

. To download all the GUi scripts to an IP0X machine with the host name "ip04":

   [david@bunny mini-asterisk-gui]$ make HOST=ip04 test

. A simple cookie is used to indicate logged in status, so to run
  scripts for each screen from the command line (for example to spot
  script syntax errors):

IP0X: 
  
  root:/www> export HTTP_COOKIE="loggedin=1" ; sh admin.sh

  If you encounter a syntax error when testing a script on an IP0X
  then running the same script on an x86 can be useful as it has much
  better error reporting:

x86:

  [david@host cgi-bin]$ export HTTP_COOKIE="loggedin=1" ; sh admin.sh

. The -x option at the stop of any sheel script can be useful to watch
  execution on an IP0X:

  #!/bin/sh -x

