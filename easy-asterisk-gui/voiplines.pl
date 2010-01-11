#!/sbin/microperl
# voiplines.pl
# David Rowe 12 Jan 2010
#
# Text processing for the ipphones screen

# slurp up voip trunk details --------------------------------

open SIP, "/etc/asterisk/sip.conf";
$state = "looking for [voip]";

while (<SIP>) { 
    if ($state eq "looking for [voip]") {

	if (/^\[voip\]/) {
	    $state="inside [voip]";
	}

    }

    if (($state eq "inside [voip]") {
	if (/^\[/) {
	    $state = "finished";
	}

	if (/user=(.*)/) {
	    $user = $1;
	}
	if (/secret=(.*)/) {
	    $pass = $1;
	}
	if (/host=(.*)/) {
	    $host = $1;
	}
    }
}
close SIP;

# generate form ---------------------------------------------

print "<tr><td>User:</td><td><input type=\"text\" name=\"user\" value=\"$user\"></td></tr>\n";
print "<tr><td>Password:</td><td><input type=\"password\" name=\"pass\" value=\"$pass"></td></tr>";
print "<tr><td>Host:</td><td><input type=\"text\" name=\"host\" value=\"$host"></td></tr>";
