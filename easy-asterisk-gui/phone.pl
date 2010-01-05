#!/sbin/microperl
# phone.pl
# David Rowe 5 Jan 2010
#
# Extracts the phone extension infor from /etc/asterisk/extensions.conf
# and generate html.  Perl used as faster than equivalent shell script

$tool_tip = "onMouseOver=\"popUp(event,'network_internet')\" onmouseout=\"popUp(event,'network_internet')\"";

my %analog = (); # analog extension keyed on zap port

open EXT, "/etc/asterisk/extensions.conf";
while (<EXT>) { 
    if (/.*=>[ ]*([0-9]*),1.*Zap\/([0-9]*)\)/) {
        $analog{$2} = $1;
	#print "'$1' '$2' $analog{$2}\n";
    }
}
close EXT;

my %zap = (); # zaptel port type keyed on zap port 
              # (fxs/fxo or no entry if not live)
open ZAP, "/etc/zaptel.conf";
while (<ZAP>) { 
    if (/fxoks=([0-9]*)/) {
        $zap{$1} = "fxs";
    }
    if (/fxsks=([0-9]*)/) {
        $zap{$1} = "fxo";
    }
}
close ZAP;

my %ip = (); # ip extension keyed on sip.conf name

open EXT, "/etc/asterisk/extensions.conf";
while (<EXT>) { 
    if (/.*=>[ ]*([0-9]*),1.*Sip\/([0-9]*)\)/) {
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

# print list of analog phones

$tool_tip = "onMouseOver=\"popUp(event,'phone_phone')\" onmouseout=\"popUp(event,'phone_phone')\"";

foreach $a (sort keys %analog) {
    if ($zap{$a} eq "fxs") {
	$icon = "<img $tool_tip src=\"phone.png\" alt=\"Analog Phone\" />";
	print "<tr><td>$analog{$a}</td><td>Port $a</td><td>$icon</td></tr>\n";
    }
}

# print list of IP phones

$tool_tip = "onMouseOver=\"popUp(event,'phone_ipphone')\" onmouseout=\"popUp(event,'phone_ipphone')\"";

foreach $s (sort keys %sip) {
    if ($sip{$s} eq "OK") {
	$icon = "<img $tool_tip src=\"ipphone.png\" alt=\"IP Phone\" />";
	print "<tr><td>$s</td><td>$ipad{$s}</td><td>$icon</td></tr>\n";
    }
}

print '<tr><td colspan="2" align="left" valign="top" ><h2>Phone Lines</h2></td></tr>
';

# print list of analog phone lines

$tool_tip = "onMouseOver=\"popUp(event,'phone_phoneline')\" onmouseout=\"popUp(event,'phone_phoneline')\"";

foreach $a (sort keys %analog) {
    if ($zap{$a} eq "fxo") {
	$icon = "<img $tool_tip src=\"phoneline.jpg\" alt=\"Phone Line\" />";
	print "<tr><td>$analog{$a}</td><td>Port $a</td><td>$icon</td></tr>\n";
    }
}

# print list of SIP VOIP trunks

$tool_tip = "onMouseOver=\"popUp(event,'phone_voipline')\" onmouseout=\"popUp(event,'phone_voipline')\"";

foreach $s (sort keys %voip) {
    if ($voip{$s} eq "OK") {
	$icon = "<img $tool_tip src=\"voipline.jpg\" alt=\"VOIP Line\" />";
	print "<tr><td>$s</td><td>$ipad{$s}</td><td>$icon</td></tr>\n";
    }
}

