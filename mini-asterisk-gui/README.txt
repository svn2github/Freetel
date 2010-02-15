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
  really want to learn Asterisk dial plan syntax and yet another conf
  file format.

* A small office that already has an old analog phone system.  You
  want to keep your current analog lines for incoming calls, but use
  IP Phones for handsets and VOIP for outgoing calls.  You know enough
  to set up a DSL router and don't want to rely on "the Phone Guy" or
  "The Computer Guy" at $100/hr to maintain your phone system.

* You are a "Phone Guy" who doesn't understand Linux and Asterisk but
  you want to install IP-PBXes.

[[why]]
Why Another Asterisk GUI?
-------------------------

There are http://www.voip-info.org/wiki/view/Asterisk+GUI[a lot of
Asterisk GUIs] out there already.  So why do we need another one?

Well I needed an Asterisk GUI that was very easy to use for the
link:ip04.html[IP0X] family.  Something that would lower
the technical skill required to install and maintain an Asterisk Phone
system.  Something my wife and kids could use.

The IP0X can't run a LAMP stack very well so the GUI had to be light
weight.

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
  basic Perl are required (microperl - no CPAN libraries).

* Works directly on extensions.conf and sip.conf, but honors any edits
  you make to these files.  So all the powerful Asterisk features are
  available if after a while you want to dive into Asterisk conf files.

* Doesn't use AJAX or the built-in Asterisk web server or users.conf
  magic.  Plain old HTML, a little Java-script and CGIs written in
  shell script and Perl.

* Doesn't use the Asterisk programming model.  For example you don't
  have to understand what a dial plan is, much less understand how to
  code one.  Plain English terms are used instead, for example "Phones
  and Phone lines".  Terms like Asterisk, Linux, SIP, Zap/1 don't even
  get mentioned.

* Extensive tool tip documentation.  No manual.

* Doesn't require a dedicated PC, not installed from an ISO CD.  So
  you can use it as a GUI for Asterisk on a little SOHO Linux box that
  is also your firewall, server etc.

* Mini Asterisk tells you when something is wrong, for example you get
  a warning if your Phone System can't see the Internet, or if it can't
  see your ITSP.

* Extensive pre-configuration of extensions.conf and sip.conf.  The
  phone numbers of extensions are pre-configured and ITSP
  configurations are selected from a pull-down menu.  Analog ports are
  auto detected, at least on the IP0X.  This makes adding phones and
  ITSPs fast and simple.  It also gives people new to Asterisk a
  working starting point.

[[status]]
Status
------

Alpha:

* Works on IP0X.
* Works on x86 but many features (like upgrades) disabled in the
  interests of security. These need to be implemented on x86 in a safe way.
* Tested on asterisk 1.4.x
* Needs feedback from real users to see how useful the concept is and what
  (un)features need to be added.
* Need a few more (un)features to be added, and Voip Line screen
  populated with more ITSPs.
* But quite useable as it stands, especially as a way to get started
  quickly with the IP0X.

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

Mini Asterisk uses the following technologies: HTML, CSS, Javascript,
CGI, Shell scripting, Perl, Asterisk.  But most of the work is done in
shell script and HTML, with a little Perl for the text processing.

The msh shell on the IP0X is fairly basic (no functions, several
quirks).  The shell script has been coded so that it runs OK on the
IP0X, which is upwards compatible to x86.

You can customise Mini Asterisk by adding your own banner to in
banner.html.

[[install]]
Installation
------------

NOTE: this process may overwrite your Asterisk extensions.conf and
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

You need a web server, Asterisk and some sort of Perl installed
(very basic Perl installation is fine).  Configure your web server to
run CGIs (.sh and .pl) from your server root.  Lighttpd config
instructions are below.

Mini Asterisk expects all files (shell, perl, html etc) to be in the
same directory.  If you find this painful please <<support, let me know>>.

The process below places the web files in /var/www, you may like to
place the files somewhere else.  One of the files is named index.html
so make sure you don't overwrite an existing index.html.

. Login as root.

. If you haven't done so already install Asterisk 1.4.x:
+
-------------------------------------------------------------------
# wget http://downloads.asterisk.org/pub/telephony/asterisk/old-releases/asterisk-1.4.11.tar.gz
# tar xvzf asterisk-1.4.11.tar.gz
# cd asterisk-1.4.11
# ./configure
# make
# make install
# make samples
-------------------------------------------------------------------

. If you have a running Asterisk installation backup your existing
extensions.conf & sip.conf in /etc/asterisk

. Installation of Mini Asterisk is basically copying the cgi-bin and etc/asterisk
files:
+
-------------------------------------------------------------------
# cd ~ 
# svn co https://freetel.svn.sourceforge.net/svnroot/freetel/mini-asterisk-gui
# cd mini-asterisk-gui
# ./update_revision.sh
# cp etc/asterisk/* /etc/asterisk 
# cp cgi-bin/* /var/www
# cd /etc/asterisk
# cp extensions.conf extensions.conf.def
# cp sip.conf sip.conf.def
# mv users.conf users.conf.bak
-------------------------------------------------------------------
+
The .def copies are required by the "reset defaults" feature on the
admin screen.

. It's a good idea to switch off the internal Asterisk web server by editing
/etc/asterisk/http.conf.  Make sure the enabled line reads like this:
+
   enabled=no
+
Then stop and restart Asterisk.
+
-------------------------------------------------------------------
# /etc/init.d/asterisk restart
-------------------------------------------------------------------

. I use lighttpd as the web server.  To enable CGI support:
+
-------------------------------------------------------------------
# lighty-enable-mod cgi
-------------------------------------------------------------------
+
I then modified 10-cgi-conf to enable perl and shell scripts:
+
-------------------------------------------------------------------
cgi.assign      = (
	".pl"  => "/usr/bin/perl",
	".sh"  => "/bin/sh",
)
-------------------------------------------------------------------
+
Make sure lighttpd runs as root, as we need to run Asterisk etc from
shell script CGIs:
+
-------------------------------------------------------------------
## change uid to <uid> (default: don't care)
#server.username            = "wwwrun"

## change uid to <uid> (default: don't care)
#server.groupname           = "wwwrun"
-------------------------------------------------------------------
+
Then restart lighttpd after all the changes:
+
--------------------------------------------------------------------
# /etc/init.d/lighttpd restart
------------------------------------------------------------------
+
For comparison on the IP04 I just modified /etc/lighttpd.conf:
+
-------------------------------------------------------------------
cgi.assign = ( ".sh" => "/bin/sh",".pl" => "/usr/bin/perl" )
-------------------------------------------------------------------

[[contribute]]
Contributions
-------------

. I especially welcome sip.conf entries for your favourite ITSP (VOIP
service) to help populate the Provider field of the
link:mini/voiplines.sh.html[Voip Line Screen].

. Suggestions and patches for new features are also very welcome.

. Help making the web pages look nicer.

. Configuration instructions (even photos) for other IP phones.

[[support]]
Support
-------

Comments, features request, bugs please let me know using Free
Telephony Project
https://lists.sourceforge.net/lists/listinfo/freetel-discuss[Mailing
List]

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

. To download all the GUI scripts to an IP0X machine with the host name "ip04":

   [david@bunny mini-asterisk-gui]$ make HOST=ip04 test
+

However this is a bit slow.  So when editing on an x86 host and testing
on an IP0X I used the M-! command on Emacs that lets you execute a
shell command.  I use this to download just the single the shell file
I am working on, e.g. "scp admin.sh root@ip04".

. A simple cookie is used to indicate logged in status.  To run
  scripts for each screen from the command line (for example to spot
  script syntax errors):
+
IP0X: 
+  
  root:/www> export HTTP_COOKIE="loggedin=1" ; sh admin.sh
+
If you encounter a syntax error when testing a script on an IP0X
then running the same script on an x86 can be useful as it has much
better error reporting:
+
x86:
+
  [david@host cgi-bin]$ export HTTP_COOKIE="loggedin=1" ; sh admin.sh

. If running using the sh command you can also trace execution with 
  the "-x" option:
+
  [david@host cgi-bin]$ sh -x login.sh

. Testing forms from the command line can be achieved by manually
setting up the CGI QUERY_STRING environment variable:
+
  [david@host cgi-bin]$ export QUERY_STRING="pass=uClinux" ; sh login.sh
