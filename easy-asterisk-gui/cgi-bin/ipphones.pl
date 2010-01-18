#!usr/bin/perl
# ipphones.pl
# David Rowe 6 Jan 2010
#
# Text processing for the ipphones screen

$ipaddress = $ARGV[0];
$more = $ARGV[1];

# Slurp up SIP extension (Sip) data from extensions.conf

my %ip = (); # ip extension keyed on sip.conf name

open EXT, "/etc/asterisk/extensions.conf";
while (<EXT>) { 
    if (/.*=>[ ]*([0-9]*),1.*SIP\/([0-9]*)\)/) {
        $ip{$2} = $1;
	#print "'$1' '$2' $ip{$2}\n";
    }
}
close EXT;

my %sip = ();  # SIP IP phone status keyed on sip.conf names 
               # if no entry we can't see IP phone device
my %voip = (); # SIP trunks status keyed on sip.conf names 
               # if no entry we can't see SIP trunk
my %ipad = (); # IP address of SIP device keyed on sip.conf names

open SIP, "sipshowpeers.txt";
while (<SIP>) { 
    if (/^([0-9]*)[\s\/].*(OK)/) {
        $sip{$1} = $2;
	#print "'$1' '$2' $sip{$1}\n";
	$e = $1;
	if (/\s([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)\s/) {
	    $ipad{$e} = $1;
	    #print "'$1'\n";
	}
    }
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

# print list of IP phones with connection status

$unconnected = 0;
foreach $e (sort keys %ip) {
    if ($sip{$e} eq "OK") {
	$icon = "<img src=\"tick.png\" alt=\"Connected\" />";
        $tooltip_status = "onMouseOver=\"popUp(event,'ipphones_connected')\" onmouseout=\"popUp(event,'ipphones_connected')\"";    
        $comment=$ipad{$e};
        $tooltip_ext = "onMouseOver=\"popUp(event,'phone_ext')\" onmouseout=\"popUp(event,'phone_ext')\"";
	$tooltip_ip =  "onMouseOver=\"popUp(event,'phone_ipphone_ip')\" onmouseout=\"popUp(event,'phone_ipphone_ip')\"";
	$unconnected = 0;
    }
    else {
	$unconnected = $unconnected + 1;
	$icon = "<img src=\"cross.png\" alt=\"Not Connected\" />";
        $tooltip_status = "onMouseOver=\"popUp(event,'ipphones_notconnected')\" onmouseout=\"popUp(event,'ipphones_notconnected')\"";
        $comment="Available";
	$tooltip_id = "iphones_$e";
	print "<div id=\"$tooltip_id\" class=\"tip\"> Configure your IP phone with username/password $e/$e, SIP Server IP $ipaddress</div>";
        $tooltip_ext = "onMouseOver=\"popUp(event,'$tooltip_id')\" onmouseout=\"popUp(event,'$tooltip_id')\"";
	$tooltip_ip = "";
    }
    
    if ($more == 1 || ($unconnected < 5)) {
	print "<tr><td $tooltip_ext>$e</td><td></td><td $tooltip_ip>$comment</td><td $tooltip_status>$icon</td></tr>\n";
    }
}

if ($more == 0) {
    print '<tr><td><a href="ipphones.sh?more=1">More</a></td></tr>';
}
else {
    print '<tr><td><a href="ipphones.sh">Less</a></td></tr>';
}


