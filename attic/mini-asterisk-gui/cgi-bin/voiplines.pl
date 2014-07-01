#!/usr/bin/perl
# voiplines.pl
# David Rowe 12 Jan 2010
#
# Text processing for the voiplines screen

# slurp up list of possible voip line providers --------------------------------
# one of these will be currently selected

open SIP, "/etc/asterisk/sip.conf";
my $provider = ""; # current provider bring parsed in sip.conf
my @providers=();  # list of all providers
my %user = ();     # user keyed on provider
my %pass = ();     # password keyed on provider
my %host = ();     # host name keyed on provider

my $provider_current = "none";
push (@providers, "none");
$comment{"none"} = "&nbsp;";

while (<SIP>) { 

    # start of any stanza switches off parsing.  It may get switched
    # back on below if it contains mini-asterisk keyword. This allows
    # non-mini asterisk SIP devices to be included in sip.conf

    if (/\[/) {
	$provider = "";	
    }

    # currently disabled mini-asterisk provider

    if (/^;\[.*\].* \"(.*)\" mini-asterisk/) {
	push (@providers, $1);
	$provider = $1;
	#print "'$1'\n";	
    }

    # current mini-asterisk provider

    if (/^\[(.*)\].* \"(.*)\" mini-asterisk/) {
	push (@providers, $2);
	$provider = $2;
	$provider_current = $2;
	$stanza_current = $1;
	#print "'$1' '$2'\n";
    }

    if ($provider ne "") {
	#print $_;

	if (/^;*username=(.*)/) {
	    $user{$provider} = $1;
	}
	if (/^;*secret=(.*)/) {
	    $pass{$provider} = $1;
	}
	if (/^;*host=(.*)/) {
	    $host{$provider} = $1;
	}

	if (/^;; (.*)/) {
	    $comment{$provider} = $1;
	}
    }

    $state = $next_state;
}
close SIP;

# Determine if Asterisk can see current voip line (SIP trunk) 
# sipshowpeers.txt needs to be generated before calling this perl
# script

my %voip = (); # SIP trunks status keyed on sip.conf stanza name/username
               # if no entry we can't see SIP trunk

open SIP, "sipshowregistry.txt";
while (<SIP>) { 
    if (/^(.*):.*(Registered)/) {
        $voip{$1} = $2;
	#print "'$1' '$2' $voip{$1}\n";
    }
}

close SIP;

# javascript to handle changing providers

print "<script>\n";
print "var hosts = new Array();\n";
print "var users = new Array();\n";
print "var passwords = new Array();\n";
print "var stanzas = new Array();\n";
print "var comments = new Array();\n";
foreach (@providers) {
    print "hosts[\'$_\'] = \'$host{$_}\';\n";
    print "users[\'$_\'] = \'$user{$_}\';\n";
    print "passwords[\'$_\'] = \'$pass{$_}\';\n";
    print "comments[\'$_\'] = \'$comment{$_}\';\n";
}
print "</script>\n";

# generate form fields -------------------------------------------

$tt_provider = "onMouseOver=\"popUp(event,'voiplines_provider')\" onmouseout=\"popUp(event,'voiplines_provider')\"";
$tt_user = "onMouseOver=\"popUp(event,'voiplines_user')\" onmouseout=\"popUp(event,'voiplines_user')\"";
$tt_pass = "onMouseOver=\"popUp(event,'voiplines_pass')\" onmouseout=\"popUp(event,'voiplines_pass')\"";
$tt_host = "onMouseOver=\"popUp(event,'voiplines_host')\" onmouseout=\"popUp(event,'voiplines_host')\"";
$tt_status = "onMouseOver=\"popUp(event,'voiplines_status')\" onmouseout=\"popUp(event,'voiplines_status')\"";

print "<tr $tt_provider><td>Provider</td><td>\n";
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

print "<tr $tt_user><td>User:</td><td><input type=\"text\" name=\"user\" id=\"user\" value=\"$user{$provider_current}\"></td></tr>\n";
print "<tr $tt_pass><td >Password:</td><td><input type=\"password\" name=\"pass\" id=\"pass\" value=\"$pass{$provider_current}\"></td></tr>\n";
print "<tr $tt_host><td >Host:</td><td><input type=\"text\" name=\"host\" id=\"host\" value=\"$host{$provider_current}\"></td></tr>\n";

#print "\nXXX $provider_current $stanza_current} $voip{$stanza_current}\n";

if ($voip{$stanza_current} eq "Registered") {
    $icon = "<img src=\"tick.png\" alt=\"OK\" />";
}
else {
    $icon = "<img src=\"cross.png\" alt=\"Not OK\" />";
}
print "<tr $tt_status><td>Voip Line Status:</td><td><div id=\"status\" >$icon</div></td></tr>\n";

print "<tr><td>&nbsp</td></tr>\n";
print "<tr><td colspan=\"2\"><div id=\"comment\">$comment{$provider_current}</div></td></tr>\n";
print "<tr><td>&nbsp</td></tr>\n";
