#!/sbin/microperl
# voiplines.pl
# David Rowe 12 Jan 2010
#
# Text processing for the voiplines screen

# slurp up list of possible voip line providers --------------------------------
# one of these will be currently selected

open SIP, "/etc/asterisk/sip.conf";
my $provider = ""; # current provider bring parsed in sip.conf
my @providers=();  # list of all providers
my %stanza = ();   # stanza name keyed on provider
my %user = ();     # user keyed on provider
my %pass = ();     # password keyed on provider
my %host = ();     # host name keyed on provider

while (<SIP>) { 

    # start of any stanza switches off parsing.  It may get switched
    # back on below if it contains easy-asterisk keyword. This allows
    # non-easy asterisk SIP devices to be included in sip.conf

    if (/\[/) {
	$provider = "";	
    }

    # currently disabled easy-asterisk provider

    if (/^;\[(.*)\].* \"(.*)\" easy-asterisk/) {
	push (@providers, $2);
	$provider = $2;
	$stanza{$2} = $1;
	#print "'$1' '$2'\n";	
    }

    # current easy-asterisk provider

    if (/^\[(.*)\].* \"(.*)\" easy-asterisk/) {
	push (@providers, $2);
	$provider = $2;
	$provider_current = $2;
	$stanza{$2} = $1;
	#print "'$1' '$2'\n";
    }

    if ($provider ne "") {
	#print $_;

	if (/^;*user=(.*)/) {
	    $user{$provider} = $1;
	}
	if (/^;*secret=(.*)/) {
	    $pass{$provider} = $1;
	}
	if (/^;*host=(.*)/) {
	    $host{$provider} = $1;
	}
    }

    $state = $next_state;
}
close SIP;

# Determine if Asterisk can see current voip line (SIP trunk) 
# sipshowpeers.txt needs to be generated before calling this perl
# script

my %voip = (); # SIP trunks status keyed on sip.conf stanza names 
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

# javascript to handle changing providers

print "<script>\n";
print "var hosts = new Array();\n";
print "var users = new Array();\n";
print "var passwords = new Array();\n";
print "var stanzas = new Array();\n";
foreach (@providers) {
    print "hosts[\'$_\'] = \'$host{$_}\';\n";
    print "users[\'$_\'] = \'$user{$_}\';\n";
    print "passwords[\'$_\'] = \'$pass{$_}\';\n";
    print "stanzas[\'$_\'] = \'$stanza{$_}\';\n";
}
print "</script>\n";

# generate form fields -------------------------------------------

print "<tr><td>Provider</td><td>\n";
print "<select name=\"provider\" id=\"provider\" onchange=\"changeProvider()\">\n";
foreach (@providers) {
    if ($_ eq $provider_current) {
	print "<option selected=\"yes\">$_</option>\n";
    }
    else {
	print "<option>$_</option>\n";
    }
}
print "</select></td></tr>\n";

print "<tr><td>User:</td><td><input type=\"text\" name=\"user\" id=\"user\" value=\"$user{$provider_current}\"></td></tr>\n";
print "<tr><td>Password:</td><td><input type=\"password\" name=\"pass\" id=\"pass\" value=\"$pass{$provider_current}\"></td></tr>";
print "<tr><td>Host:</td><td><input type=\"text\" name=\"host\" id=\"host\" value=\"$host{$provider_current}\"></td></tr>";
if ($voip{$stanza{$provider_current}} eq "OK") {
    $icon = "<img src=\"tick.png\" alt=\"VOIP Line OK\" />";
}
else {
    $icon = "<img src=\"cross.png\" alt=\"VOIP Line OK\" />";
}
print "<tr><td>Voip Line Status:</td><td>$icon</td></tr>";

# hidden field to pass stanza with form

print "<tr><td><input type=\"hidden\" name=\"stanza\" id=\"stanza\" value=\"$stanza{$provider_current}\"></td></tr>";
