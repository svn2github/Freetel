README.txt for foxy
David Rowe
May 25 2013


Introduction
------------

Simple Google Maps application to plot Fox Hunt bearings.

* Screen shot: foxy_screenshot.png

* Based on Google maps V3 API.

* Light weight: On the server we have just a few CGIs and a text file
  database.  A browser cookie is used for storing configuration
  information.  The CGIs are a few lines of shell script, so can run
  on any machine.  Installation involves copying a few files and
  setting a few permissions.


Using Foxy
----------

1/ Point your browser at http://yourserver/foxy.html

2/ Enter the bearing in the text box and left click to add a bearing.

3/ Left click on a bearing to get information.  Right click to delete
a bearing.


Implementation Notes
--------------------

Foxy is implemented in Javascript (foxy.html).  A simple text database
file /var/www/foxy/bearings.txt is used to store bearing
information. Very simple 1 page CGIs written in shell script are used
to access the bearing database.

A browser cookie is used to store config information like the current
map centre.

All the CGI scripts assume a hard coded path of /var/www/bearings for
the bearings.txt database.


Files
-----

  foxy.html        - Javascript code for Foxy
  foxy.css         - style sheet

"cgi-bin" directory:

  addbearing.cgi   - adds a bearing to bearings.txt
  delbearing.cgi   - deletes a bearing from bearings.txt
  getbearings.cgi   - reads bearings.txt database
  cleardb.cgi      - clears database, removing all bearings


Software
--------

You need:

1/ A web server. Apache is assumed in the /usr/lib/cgi-bin path below.
   The paths may be different for other web servers.  Note the path
   to /var/www/foxy is hard coded in the CGI scripts so it's
   best not to change that.


Installation
------------

1/ Server PC

  $ svn co https://freetel.svn.sourceforge.net/svnroot/freetel/foxy
  $ cd foxy
  $ sudo mkdir /var/www/foxy
  $ sudo chmod 777 /var/www/foxy
  $ cp foxy.css foxy.html /var/www/foxy
  $ sudo cp cgi-bin/* /usr/lib/cgi-bin

Tests
-----

1/ Test reading and writing bearings.txt database with your browser:

   http://localhost/cgi-bin/addbearing.cgi?lat=123&lng=456&bearing=90

   $ cat /var/www/foxy/bearings.txt
   123,456,90

   http://localhost/cgi-bin/delbearing.cgi?lat=123&lng=456

   $ cat /var/www/foxy/bearings.txt
   (empty file)

Debugging
---------

1/ Monitor Apache log:

  $ tail -f /var/log/apache2/access.log

2/ Use Firebug on Firefox to single step, set breakpoints etc.

3/ Check bearings.txt database, each line is (lat, lng, IP):

  # cat /var/www/foxy/bearings.txt 

  -34.88548650005714,138.55225324630737,90
  -34.88006501016277,138.55394840240479,180
  -34.87893842193011,138.55278968811035,265
  -34.882511765792316,138.55210304260254,10

4/ The "foxy001" cookie stores our state. On Firefox 3.5 you
   can remove the foxy cookie using Edit-Preferences-Privacy, then
   click on "remove individual cookies".

5/ To manually reset to defaults:
   
   * move to another page (cookie is saved when we exit page)
   * rm -f /var/www/foxy/bearings.txt
   * Delete cookie using step (4) above
