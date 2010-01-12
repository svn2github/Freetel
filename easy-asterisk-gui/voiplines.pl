#!/sbin/microperl
# voiplines.pl
# David Rowe 12 Jan 2010
#
# Text processing for the ipphones screen

# slurp up voip trunk details --------------------------------

open SIP, "/etc/asterisk/sip.conf";
$state = "looking for [voip]";

while (<SIP>) { 
    $next_state = $state;

    if ($state eq "looking for [voip]") {

	if (/^\[voip\]/) {
	    $next_state="inside [voip]";
	    #print "$state $next_state\n";
	}

    }

    if ($state eq "inside [voip]") {
	#print $_;
	if (/^\[/) {
	    $next_state = "finished";
	    #print "$next_state\n";
	}
	if (/^;.*\[/) { # commented out next stanza
	    $next_state = "finished";
	}

	if (/^user=(.*)/) {
	    $user = $1;
	}
	if (/^secret=(.*)/) {
	    $pass = $1;
	}
	if (/^host=(.*)/) {
	    $host = $1;
	}
    }

    $state = $next_state;
}
close SIP;

# Determine list of possible VOIP lines to use

open SIP, "/etc/asterisk/sip.conf";

@voiplines=();

while (<SIP>) { 
    if (/^;\[.* \"(.*)\" easy-asterisk/) {
	push (@voiplines, $1);
	#print "$1\n";
    }
    if (/^\[.* \"(.*)\" easy-asterisk/) {
	push (@voiplines, $1);
	$voipline_current = $1;
	#print "$1\n";
    }
}
close SIP;

# Determine if Asterisk can see our Voip Line (SIP trunk) 

my %voip = (); # SIP trunks status keyed on sip.conf names 
               # if no entry we can't see SIP trunk

open SIP, "sipshowpeers.txt";
while (<SIP>) { 
    if (/^(voip[0-9]*)[\s\/].*(OK)/) {
        $voip{$1} = $2;
	#print "'$1' '$2' $voip{$1}\n";
	$e = $1;
	if (/\s([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)\s/) {
	    $ipad{$e} = $1;
	    #print "'$1'\n";
	}
    }
}

close SIP;

# generate form fields -------------------------------------------

print "<tr><td>Provider</td><td>\n";
print "<select name=\"voiplines\">\n";
foreach (@voiplines) {
    if ($_ eq $voipline_current) {
	print "<option selected=\"yes\">$_</option>\n";
    }
    else {
	print "<option>$_</option>\n";
    }
}
print "</select></td></tr>\n";

print "<tr><td>User:</td><td><input type=\"text\" name=\"user\" value=\"$user\"></td></tr>\n";
print "<tr><td>Password:</td><td><input type=\"password\" name=\"pass\" value=\"$pass\"></td></tr>";
print "<tr><td>Host:</td><td><input type=\"text\" name=\"host\" value=\"$host\"></td></tr>";
if ($voip{"voip"} eq "OK") {
    $icon = "<img src=\"tick.png\" alt=\"VOIP Line OK\" />";
}
else {
    $icon = "<img src=\"cross.png\" alt=\"VOIP Line OK\" />";
}
print "<tr><td>Voip Line Status:</td><td>$icon</td></tr>";
