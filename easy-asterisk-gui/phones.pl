#!/sbin/microperl
# phones.pl
# David Rowe 5 Jan 2010
#
# Test processing for the phones screen.

$tool_tip = "onMouseOver=\"popUp(event,'network_internet')\" onmouseout=\"popUp(event,'network_internet')\"";

# Slurp up analog port (Zap) data from extensions.conf

my %analog = (); # analog extension keyed on zap port

open EXT, "/etc/asterisk/extensions.conf";
while (<EXT>) { 
    if (/.*=>[ ]*([0-9]*),1.*Zap\/([0-9]*)\)/) {
        $analog{$2} = $1;
	#print "'$1' '$2' $analog{$2}\n";
    }
}
close EXT;

# Slurp up data on installed zaptel ports from /etc/zaptel.conf

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

# Slurp up SIP extension (Sip) data from extensions.conf

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

$tooltip_anphone = "onMouseOver=\"popUp(event,'phone_anphone')\" onmouseout=\"popUp(event,'phone_anphone')\"";
$tooltip_ext = "onMouseOver=\"popUp(event,'phone_ext')\" onmouseout=\"popUp(event,'phone_ext')\"";
$tooltip_port = "onMouseOver=\"popUp(event,'phone_port_phone')\" onmouseout=\"popUp(event,'phone_port_phone')\"";

foreach $a (sort keys %analog) {
    if ($zap{$a} eq "fxs") {
	$icon = "<img src=\"anphone.png\" alt=\"Analog Phone\" />";
	print "<tr><td $tooltip_ext>$analog{$a}</td><td $tooltip_anphone>Analog Phone</td><td $tooltip_port>Port $a</td><td>$icon</td></tr>\n";
    }
}

# print list of IP phones

$tooltip_ipphone = "onMouseOver=\"popUp(event,'phone_ipphone')\" onmouseout=\"popUp(event,'phone_ipphone')\"";
$tooltip_ipphone_ip = "onMouseOver=\"popUp(event,'phone_ipphone_ip')\" onmouseout=\"popUp(event,'phone_ipphone_ip')\"";

foreach $s (sort keys %sip) {
    if ($sip{$s} eq "OK") {
	$icon = "<img src=\"ipphone.png\" alt=\"IP Phone\" />";
	print "<tr><td $tooltip_ext>$s</td><td $tooltip_ipphone>IP Phone</td<td $tooltip_ipphone_ip>$ipad{$s}</td><td>$icon</td></tr>\n";
    }
}

$tool_tip = "onMouseOver=\"popUp(event,'phone_lines')\" onmouseout=\"popUp(event,'phone_lines')\"";

print '<tr><td>&nbsp</td></tr>';
print "<tr $tool_tip><td colspan=\"4\" align=\"left\" valign=\"top\" ><h2>Phone Lines</h2></td></tr>
";

# print list of analog phone lines

$tooltip_phoneline = "onMouseOver=\"popUp(event,'phone_phoneline')\" onmouseout=\"popUp(event,'phone_phoneline')\"";
$tooltip_port = "onMouseOver=\"popUp(event,'phone_port_line')\" onmouseout=\"popUp(event,'phone_port_line')\"";

foreach $a (sort keys %analog) {
    if ($zap{$a} eq "fxo") {
	$icon = "<img src=\"phoneline.jpg\" alt=\"Phone Line\" />";
	print "<tr><td>$analog{$a}</td><td $tooltip_phoneline>Analog</td><td $tooltip_port>Port $a</td><td>$icon</td></tr>\n";
    }
}

# print list of SIP VOIP trunks

$tooltip_voipline = "onMouseOver=\"popUp(event,'phone_voipline')\" onmouseout=\"popUp(event,'phone_voipline')\"";
$tooltip_voipline_ip = "onMouseOver=\"popUp(event,'phone_voipline_ip')\" onmouseout=\"popUp(event,'phone_voipline_ip')\"";

foreach $s (sort keys %voip) {
    if ($voip{$s} eq "OK") {
	$icon = "<img src=\"voipline.jpg\" alt=\"VOIP Line\" />";
	print "<tr><td>$s</td><td $tooltip_voipline>VOIP</td><td $tooltip_voipline_ip>$ipad{$s}</td><td>$icon</td></tr>\n";
    }
}

