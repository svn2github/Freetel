<?php

# test.db.php
# David Rowe Feb 2008
#
# Used to test and debug PEAR/DB sqlite3 support for FreePBX

require_once('DB.php'); // PEAR must be installed

/* 
  datasource in in this style:
  dbengine://username:password@host/database 
*/

# Mysql connection as a starting point ----------------------------
	
/*		

$db_engine = "mysql";
$db_user = "asteriskuser";
$db_pass = "amp109";
$db_host = "localhost";
$db_name = "asterisk";
		
$datasource = $db_engine.'://'.$db_user.':'.$db_pass.'@'.$db_host.'/'.$db_name;
$db = DB::connect($datasource); // attempt connection
print("Connecting to $datasource.....<br>");
if (DB::isError ($db))
	die ("Cannot connect: " . $db->getMessage () . "\n");
print("Disconnecting...<br>");
$db->disconnect ();

*/

# Basic sqlite connection ----------------------------------------

$options = array(
       	'debug'       => 4,
	'portability' => DB_PORTABILITY_NUMROWS
);

$file = "/var/lib/asterisk/freepbx.db";
$datasource = "sqlite3:///" . $file . "?mode=0666";

print("Connecting to $datasource.....<br>");

if (! extension_loaded('sqlite3') ) {
	print("Loading sqlite3.so....<br>");
	dl('sqlite3.so');
}

$db = DB::connect($datasource, $options);
if (DB::isError ($db))
	die ("Cannot connect: " . $db->getMessage () . "\n");

$result = $db->query ("SELECT * FROM admin");
if (DB::isError ($result))
	die ("SELECT failed: " . $result->getMessage () . "\n");

print("admin table rows:<br>");
while ($row =& $result->fetchRow ())
	printf("&nbsp %s, %s<br>", $row[0], $row[1]);
	
# numRows Test --------------------------------------------------

$numRows = $result->numRows();
   	
if (DB::isError ($numRows))
	die ("numRows failed: " . $numRows->getMessage () . "<br>");
print ("result contains: $numRows rows<br>");

# Try inserting a few values -----------------------------------

$result =& $db->query ("INSERT INTO tbl1 VALUES ('AA',30)");

if (DB::isError ($result))
	die ("INSERT failed: " . $result->getMessage () . "\n");
 
$result = $db->query ("SELECT * FROM tbl1");

if (DB::isError ($result))
	die ("SELECT failed: " . $result->getMessage () . "\n");

while ($row =& $result->fetchRow ())
	printf("&nbsp %s<br>", $row[0]);
	
$result->free();
print("Disconnecting...");
$db->disconnect ();
	
?>
