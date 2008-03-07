README for freepbx-sandbox
David Rowe Feb 2008

Introduction
------------

This directory contains a "sandbox" type development environment for
FreePBX using sqlite3.  It was used to develop patches against FreePBX
2.4.0 to progress sqlite3 support.

The Makefile builds (almost) everything you need for FreePBX (web
server, php etc) from scratch and runs it in a "sandbox" environment.
The notable exception is that it doesn't build Asterisk (yet).

FreePBX is configured to use lighttpd (easier than building Apache)
and and sqlite3.

Most of the files for are confined to this directory, i.e. you mostly
(except for Asterisk) don't need to mess with your PC's configuration.
Used to model and test configuration for Blackfin port, but may be
useful for general FreePBX development.

FreePBX Sqlite3 Status
----------------------

Most of the screens (Admin/Reports/Recording) present OK but have not
been extensively tested.  The Flash Panel isn't working/hasn't been
tested.  Reports/Recording is awaiting sqlite3 CDR support (e.g. in
Asterisk 1.6) for further testing.  The amp_conf installer works OK.

I have run some basic tests such as adding an extension/route/trunk
and made a few calls OK.

This FreePBX/Sqlite3 development has been performed on a laptop
running Ubuntu (no zaptel hardware) with Asterisk 1.4 and FreePBX
2.4.0.

Motivation
----------

1/ When I first tried to manually install FreePBX I had a lot of
problems installing all the apps I needed, especially as I was new to
PHP/SQL/FreePBX.  So I thought about automating the installation (from
source).

2/ It may allow multiple installations of FreePBX, or for example lets
you use a different PHP version to currently installed on your system.
Asterisk would also need to be sandboxed for this to work.

3/ It doesn't mess with your root filesystem.

4/ Let me collect all the code I needed in one dir tree, making source
control easier.

5/ Stepping stone for a Blackfin port.

Building and Running FreePBX-sqlite3 Sandbox
--------------------------------------------

Building:

1/ Install Asterisk 1.4.x and start Asterisk.  

2/ Build freepbx-sandbox:

[your home]$ svn co \
             https://freetel.svn.sourceforge.net/svnroot/freetel/freepbx-sandbox \
             freepbx-sandbox
[your home]$ cd freepbx
[freepbx-sandbox]$ make 

3/ You will eventually get prompted for your root passwd - this is
   required for several stages of the installation.

Running:

1/ Start the web server:
   [freepbx-sandbox]$ sudo ./root/sbin/lighttpd -D -f root/etc/lighttpd.conf
2/ If not already running start Asterisk:
   [freepbx-sandbox]$ sudo asterisk
3/ Point your browser at locahost

Manual Installation of FreePBX with sqlite3
-------------------------------------------

Here is a rough guide for a regular (non-sandbox) installation of
FreePBX with sqlite3.

The Makefile automates FreePBX/sqlite3 installation in a sandbox
environment, use it as a reference for manual installation as I may
have missed a step in the steps below.  See also sqlite.readme in the
FreePBX tar ball.

1/ Install and test a web server with PHP 5.

2/ Install PHP PEAR.  Patch DB/common.php with files/common.patch to
   enable numRows() to work.

3/ Install sqlite3 (shared libs and application).

4/ Install sqlite3-0.5 (PEAR/DB support for sqlite3)
   + cp files/sqlite3.php /your/php/path/DB/sqlite3.php.  This is an updated
     version of the sqlite3.php file that comes with sqlite3-0.5 
   + make sure you install sqlite.so and update php.ini so it can find it.
   + optional: see Test 2 in the next section

5/ tar xvzf freepbx-2.4.0.tar.gz

6/ Edit amportal.conf for sqlite (see sqlite.readme), manually cp to
   /etc (that way installer ask less questions).

7/ Create sqlite database (see sqlite.readme).  Create a dummy cdr
   database file using 'touch /var/asteriskcdr.db' (this last step
   prevents reports/recording screens choking in the absence of
   Asterisk sqlite3 CDR records).

8/ Apply patch/freepbx.patch to freepbx-2.4.0 directory.

9/ Manually edit recordings/includes/main.conf.php:
     ASTERISKCDR_DBENGINE="sqlite3"
     ASTERISKCDR_DBFILE="/var/asteriskcdr.db"
    
   Manually edit: admin/cdr/lib/defines.php:
     DB_TYPE="sqlite3"
     DB_NAME="/var/asteriskcdr.db"

10/ Run freepbx installer

Tests
-----

1/ basic PHP support: run file/test.php on your webserver 
   
2/ PEAR/DB with sqlite3: run files/testdb.php on your webserver
   (after freepbx.db has been created by step 9)

Notes
-----

1/ The Recordings screen needs a sqlite3 CDR backend for Asterisk.
This is available in Asterisk 1.6.  This hasn't been tested yet.

2/ The Makefile automates FreePBX/sqlite3 installation in a sandbox
environment, use it as a reference for manual installation.  See
Makefile for more information.

3/ Changes to FreePBX are in the patch directory.  An updated
sqlite3.php DB/PEAR backend is also required (files dir).

4/ If port 80 is not available kill your existing web server.

5/ Xdebug generates trace files in your /tmp directory.  Useful to
determine what went wrong.  Also keep an eye on the lighttpd log.

6/ I used a laptop running Ubuntu and Asterisk 1.4.4 for these tests -
Asterisk had no analog hardware installed so I didn't install Zaptel.
Asterisk was compiled in another directory in the usual way:
./configure && make && sudo make install.

7/ In files/testdb.php is a simple test script for exercising sqlite3
PEAR/DB support - useful for debugging any sqlite3 problems you might
encounter.

Suggested FreePBX Changes
-------------------------

1/ Consider patch/freepbx.patch (for FreePBX) & patch/common.patch
   (for PEAR/DB).  Patches are explained at the end of Makefile
   (freepbx-make-patch target).

   Please note that some of the patches contain some hard-coded
   information (e.g. changed paths), for example:

     admin/cdr/lib/defines.php (DBNAME & DBTYPE lines)
     install_amp (AMP_CONF)
     retrieve_conf (AMP_CONF)

   This is a consequence of the sandbox method I am using, and in some
   cases current FreePBX sqlite3 support (e.g. the Asterisk CDR
   database is hard coded in several places rather than being set in
   amportal.conf).  If checking into FreePBX source control this part
   of the patch should not be applied.  I will try and work out a
   better way to generate patches in future.

2/ It might be better to store the CDR database parameters in
   amportal.conf rather than hard coding them in
   recordings/includes/main.conf.php ($ASTERISKCDR_DBFILE,
   $ASTERISKCDR_DBNAME) & admin/cdr/lib/defines.php (DBNAME, DB_TYPE).

3/ Include sqlite3.php in FreePBX to support sqlite3 in PEAR/DB for
   PHP 5.

4/ newinstall.sqlite3.sql has a typo on line 327:

   modules_xml -> module_xml

   This correction is included in patch/freepbx.patch

TODO
----

1/ Try to get Asterisk 1.6 CDR working with sqlite3, so we can further
   develop the Reports/recordings menus.

2/ Bunch of warnings from the web server I haven't worked out yet:

   a) when saving a new extension:
   
   <snip>
   2008-03-02 08:14:41: (mod_cgi.c.1231) cgi died ?
 
   b) lots of this sort of thing:
  
   <snip>
   PHP Notice: Undefined offset: 2 in
   /home/david/freepbx-sandbox/root/www/admin/modules/core/functions.inc.php
   on line 2946
