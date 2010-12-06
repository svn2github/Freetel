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
  mesh.


Status
------

* Alpha
* Tested on Firefox 3.5.3 with Apache2 server
* Tested on small 5 node mesh network, might needs tweaks (e.g. fping
  arguments) for larger networks
* Displays nodes, link paths, node IP and packet loss
* Signal strength per link feature under development


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


Software
--------

You need:

1/ A web server. Apache is assumed in the /usr/lib/cgi-bin path below.
   The paths may be different for other web servers.  Note the path
   to /var/www/dilimesh is hard coded in the CGI scripts so it's
   best not to change that.

2/ netcat (nc) and fping installed on the server.

3/ A Batman mesh network with the vis server on one of the nodes, this
   may already be running on your supernode.  Batman on each node should
   be configured for the vis server (e.g. -s 10.130.1.1)

4/ A route from the web server PC to the mesh network.


Installation
------------

1/ Server PC

  # sudo mkdir /var/www/dilimesh
  # sudo chmod 777 /var/www/dilimesh
  # cp dilimesh/* /var/www/dilimesh
  # cp cgi-bin/* /usr/lib/cgi-bin

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
  
    # cat /etc/confog/batman  
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


Using Dilimesh
--------------

1/ Point your browser at http://server/dilimesh/dilimesh/html

2/ Set "Visualisation Server IP" on RHS and reload page.

3/ Dilimesh will find new nodes automatically.  Drag bouncing nodes
   to the correct position on map.

4/ Mouse over or click to get node IP and packet loss.  Right click to
   delete node. Colours:
     * blue  - packet loss < 10%
     * red   - packet loss between 10% and 90%
     * black - packet loss > 90%

5/ "Update Enable" will update packet loss and network links
    automatically.


Debugging
---------

1/ Monitor Apache log:

  $ tail -f /var/log/apache2/access.log

2/ Use Firebug on Firefox.

3/ Check nodes.txt database, each line is (lat, lng, IP):

  # cat /var/www/dilimesh/nodes.txt 

  -34.88548650005714,138.55225324630737,10.130.1.56
  -34.88006501016277,138.55394840240479,10.130.1.1
  -34.87893842193011,138.55278968811035,10.130.1.23
  -34.882511765792316,138.55210304260254,10.130.1.14
  -34.880364257566484,138.5518455505371,10.130.1.36
