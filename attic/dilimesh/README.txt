README.txt for dilimesh
David Rowe
Dec 7 2010


Introduction
------------

Simple mesh network mapping application (web app) used as a test bed
for link debugging techniques in the Dili Village Telco.

* Screen shot: dilimesh_screenshot.png

* Currently displays packet loss stats as an alternative to batman
  scores.  The purpose of this project is to experiment with the
  display of different information, for example display signal
  strength or even a spectrum of local Wifi activity for each nodes.
  The goal is gather information to debug problem links.

* Based on Google maps V3 API.

* Light weight: On the server we have just a few CGIs and a text file
  database.  A browser cookie is used for storing configuration
  information.  The CGIs are a few lines of shell script, so can run
  on any machine.  Installation involves copying a few files and
  setting a few permissions.

* Loosely coupled design: Can work without any additional software on
  each mesh mode, just needs a Visualisation server somewhere in the
  mesh.  Doesn't affct Mesh Potato operation, or the operation of
  other software on the mesh.


Using Dilimesh
--------------

1/ Point your browser at http://server/dilimesh/dilimesh/html

2/ Set "Visualisation Server IP" on RHS and reload page.

3/ Dilimesh will find new nodes automatically.  Drag bouncing nodes
   to the correct position on map.

4/ Mouse over or click on a node to get IP and packet loss.  If nodes
   are running signal strength daemon signal strengths of adjacent
   nodes will also be displayed.

5/ Click on links to get "Distance" stats on lower right hand side.

5/ Node colours:
     * blue  - packet loss < 10%
     * red   - packet loss between 10% and 90%
     * black - packet loss > 90%

5/ "Update Enable" will update packet loss, network links and signal
    strength automatically.


Status
------

* Alpha
* Tested on Firefox 3.5.3 with Apache2 servers running Ubuntu 9 & 10.
* Tested on small 5 node mesh network, might needs tweaks (e.g. fping
  arguments) for larger networks


Implementation Notes
--------------------

Dilimesh is implemented in Javascript (dilimesh.html).  A simple text
database file /var/www/dilimesh/nodes.txt is used to store node
locations and IPs.  Browser cookies are used to store config
information like our last map location and vis server IP.  Very simple
1 page CGIs written in shell script are used to access the node
database.

The 'Set' buttons on the RHS cause dilimesh.html to reload with CGI
type variables passed to it.  However these variables are not used, we
use onclick() methods to change the state variables which are stored
in a cookie when the page exits.

All the CGI scripts assume a hard coded path of /var/www/dilimesh for
the nodes.txt database.


Directories
-----------

"dilimesh" directory:

  dilimesh.html        - all the Javascript for Dilimesh
  dilimesh.css         - style sheet
  node_good.png        
  node_packet_loss.png 
  node_dead.png        - node icon PNGs, changes with packet loss

"cgi-bin" directory:

  addnode.cgi    - adds a node to nodes.txt
  delnode.cgi    - deletes a node from nodes.txt
  getnodes.cgi   - reads nodes.txt database
  getvis.cgi     - reads vis server data
  fpingnodes.cgi - fires off pings for all nodes, returns results
  getsig.cgi     - reads signal strength of surrounding nodes from one node,
                   requires sigstr.sh telnet daemon running on node.

"scripts" directory:
  
  sigstr.sh      - script that runs on MP as a telnet daemon to return signal
                   strength of each node to getsig.cgi.  Experimental.
  S99sigstr      - /etc/init.d Mesh Potato start upscript for sigstr.sh telnet
                   daemon

Software
--------

You need:

1/ A web server. Apache is assumed in the /usr/lib/cgi-bin path below.
   The paths may be different for other web servers.  Note the path
   to /var/www/dilimesh is hard coded in the CGI scripts so it's
   best not to change that.

2/ netcat (nc) and fping installed on the server.  Note that on Ubuntu
   we found that netcat 1.10.38 was required, netcat-openbsd would not
   work.

3/ A Batman mesh network with the vis server on one of the nodes, this
   may already be running on your supernode.  Batman on each node should
   be configured for the vis server (e.g. -s 10.130.1.1)

4/ A route from the web server PC to the mesh network.  On the Ubuntu
   machine I use as a server I put this script in the file
   /etc/network/if-up.d/potato:

     !/bin/sh
     # Add interface and routes to connect to potato mesh network

     ifconfig eth0 10.30.1.3 netmask 255.255.255.0
     route add -net 10.130.1.0/24 gw 10.30.1.1

   This file runs when the machine boots and connects to the Supernode
   via Ethernet.

Installation
------------

1/ Server PC

  $ svn co https://freetel.svn.sourceforge.net/svnroot/freetel/dilimesh
  $ cd dilimesh
  $ sudo mkdir /var/www/dilimesh
  $ sudo chmod 777 /var/www/dilimesh
  $ cp dilimesh/* /var/www/dilimesh
  $ sudo cp cgi-bin/* /usr/lib/cgi-bin

2/ Supernode

  Configure vis server to work on Wifi and Ethernet interfaces:

    # root@OpenWrt:~# cat /etc/config/vis 
    config vis general
         option 'interface' 'ath0 eth0'

  If it's running OK you should see a process like:

    # ps | grep vis
    711 root      1508 S    vis -j ath0 eth0

3/ Batman nodes

  Configure you nodes to use the visualisation server:
  
    # cat /etc/config/batman  
    config batmand general
        option interface                ath0
        option announce
        option gateway_class
        option originator_interval
        option preferred_gateway
        option routing_class
        option visualisation_srv        10.130.1.1
        option policy_routing_script

  When batman is running with the vis server it should look like:

    # ps | grep batman
    552 root      1564 S    batmand -s 10.130.1.1 ath0

4/ (Optional) Signal strength daemon:

  Copy these scripts to each MP in your mesh that you wish to read sig
  strength from:
  
    $ cd dilimesh/script
    $ scp sigstr.sh root@10.130.1.36:/usr/sbin
    $ scp S99sigstr  root@10.130.1.36:/etc/rc.d
  
  Now start daemon on remote machine:

    $ ssh root@10.130.1.36 '/etc/rc.d/S99sigstr'

  Test if daemon is running

   $ telnet 10.130.1.36 4950

Tests
-----

1/ Server PC can reach node with vis server:

    $ ping 192.168.1.100

2/ Server PC has route to mesh nodes:
 
    $ ping 10.130.1.1
    $ ping 10.130.1.2

3/ Test you have installed fping:

    $ fping 10.130.1.1

4/ Test netcat and vis server:

    $ nc 192.168.1.100 2005

      HTTP/1.0 200 OK
      Content-type: application/json

      [
	{ router : "10.130.1.1", neighbour : "10.130.1.23", label : 1.02 },
	{ router : "10.130.1.14", neighbour : "10.130.1.56", label : 1.76 }
      ]

5/ Test CGIs:
 
   Point your browser at: 

     http://localhost/cgi-bin/fpingnodes.cgi?ip=10.130.1.1

6/ Test reading and writing nodes.txt database with your browser:

   http://localhost/cgi-bin/addnode.cgi?lat=123&lng=456&ip=10.130.1.1

   $ cat /var/www/dilimesh/nodes.txt
   123,456,10.130.1.1

   http://localhost/cgi-bin/delnode.cgi?lat=123&lng=456&ip=10.130.1.1

   $ cat /var/www/dilimesh/nodes.txt
   (empty file)

7/ Test signal strength daemon on each node:

   $ telnet 10.130.1.1 4950
   Trying 10.130.1.36...
   Connected to 10.130.1.36.
   Escape character is '^]'.

   10.130.1.1 10.130.1.56 10.130.1.14
   -64 -90 -75


Debugging
---------

1/ Monitor Apache log:

  $ tail -f /var/log/apache2/access.log

2/ Use Firebug on Firefox to single step, set breakpoints etc.

3/ Check nodes.txt database, each line is (lat, lng, IP):

  # cat /var/www/dilimesh/nodes.txt 

  -34.88548650005714,138.55225324630737,10.130.1.56
  -34.88006501016277,138.55394840240479,10.130.1.1
  -34.87893842193011,138.55278968811035,10.130.1.23
  -34.882511765792316,138.55210304260254,10.130.1.14
  -34.880364257566484,138.5518455505371,10.130.1.36

4/ The "dilimesh" cookie stores a lot of our state. On Firefox 3.5 you
   can remove the dilimesh cookie using Edit-Preferences-Privacy, then
   click on "remove individual cookies".

5/ To reset to defaults:
   
   * move to another page (cookie is svaed when we exit page)
   * rm -f /var/www/dilimesh/nodes.txt
   * Delete cookie using setp (4) above
