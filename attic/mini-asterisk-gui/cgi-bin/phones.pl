#!/usr/bin/perl
# phones.pl
# David Rowe 5 Jan 2010
#
# Text processing for the phones screen.

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
    if (/fxoks=(.*)/) {
        @fxs = split(/,/, $1);
	foreach (@fxs) {
	    $zap{$_} = "fxs";
	}
    }
    if (/fxsks=(.*)/) {
        @fxo = split(/,/, $1);
	foreach (@fxo) {
	    $zap{$_} = "fxo";
	}
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

# Determine which extenions are "Reception", i.e. set to ring on incoming
# calls

my %zap_ring = (); # ring flag keyed on Zap port (1,2...)
my %sip_ring = (); # ring flag keyed on sip.cong ext name (6011,6012 etc)

open EXT, "/etc/asterisk/extensions.conf";
while (<EXT>) { 
    if (/s,1,Dial\((.*)\) ;; mini/) {
        @ring = split(/&/, $1);
	#print "'$1' '@ring'\n foreach:\n";
	foreach (@ring) {
	    #print "    $_\n";
	    if (/Zap\/([0-9]*)/) {
		$zap_ring{$1} = 1;
		#print "'$_' $1 \n";
	    }
	    if (/SIP\/([0-9]*)/) {
		$sip_ring{$1} = 1;
		#print "'$_' $1 \n";
	    }
	}
    }
}
close EXT;


# work out which IP phones are registered -----------------------------------

my %sip = ();  # SIP IP phone status keyed on sip.conf names (6011,6012 etc)
               # if no entry we can't see IP phone device
my %ipad = (); # IP address of SIP device keyed on sip.conf names

open SIP, "sipshowpeers.txt";
while (<SIP>) {

    # just look for 4 digit extensions starting with 6 so we don't
    # confuse with Voip Line
 
    if (/^(6[0-9][0-9][0-9])[\s\/].*(OK)/) {
        $sip{$1} = $2;
	#print "'$1' '$2' $sip{$1}\n";
	$e = $1;
	if (/\s([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)\s/) {
	    $ipad{$e} = $1;
	    #print "'$1'\n";
	}
    }

    if (/^(.*)\/.*(OK)/) {
        #$sip{$1} = $2;
	#print "'$1' '$2' $sip{$1}\n";
	$e = $1;
	if (/\s([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)\s/) {
	    $ipad{$e} = $1;
	    #print "'$1'\n";
	}
    }
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

# start phones ringing form -------------------------------------------

print '<form action="set_ring.sh" method="get">';

# print list of analog phones

$tooltip_anphone = "onMouseOver=\"popUp(event,'phone_anphone')\" onmouseout=\"popUp(event,'phone_anphone')\"";
$tooltip_ext = "onMouseOver=\"popUp(event,'phone_ext')\" onmouseout=\"popUp(event,'phone_ext')\"";
$tooltip_port = "onMouseOver=\"popUp(event,'phone_port_phone')\" onmouseout=\"popUp(event,'phone_port_phone')\"";
$tooltip_reception = "onMouseOver=\"popUp(event,'phone_reception')\" onmouseout=\"popUp(event,'phone_reception')\"";

foreach $a (sort keys %analog) {
    if ($zap{$a} eq "fxs") {
	$icon = "<img src=\"tick.png\" alt=\"Analog Phone OK\" />";
	print "<tr><td $tooltip_ext>$analog{$a}</td><td $tooltip_anphone>Analog Phone</td><td $tooltip_port>Port $a</td>";
	if ($zap_ring{$a} == 1) {
	    $checked = "checked";
	}
	else {
	    $checked = "";
	}
	print "<td $tooltip_reception><input type=\"checkbox\" name=\"Zap_$a\" $checked>Reception</td><td>$icon</td></tr>\n";
    }
}

# print list of IP phones

$tooltip_ipphone = "onMouseOver=\"popUp(event,'phone_ipphone')\" onmouseout=\"popUp(event,'phone_ipphone')\"";
$tooltip_ipphone_ip = "onMouseOver=\"popUp(event,'phone_ipphone_ip')\" onmouseout=\"popUp(event,'phone_ipphone_ip')\"";
$tooltip_reception = "onMouseOver=\"popUp(event,'phone_reception')\" onmouseout=\"popUp(event,'phone_reception')\"";

foreach $s (sort keys %sip) {
    if ($sip{$s} eq "OK") {
	$icon = "<img src=\"tick.png\" alt=\"IP Phone OK\" />";
	print "<tr><td $tooltip_ext>$s</td><td $tooltip_ipphone>IP Phone</td><td $tooltip_ipphone_ip>$ipad{$s}</td>";
	if ($sip_ring{$s} == 1) {
	    $checked = "checked";
	}
	else {
	    $checked = "";
	}
	print "<td $tooltip_reception><input type=\"checkbox\" name=\"SIP_$s\" $checked>Reception</td><td>$icon</td></tr>\n";
    }
}

print '<tr><td>&nbsp;</td></tr>';
print "<tr><td colspan=\"2\" onMouseOver=\"popUp(event,'phone_addipphone')\" onmouseout=\"popUp(event,'phone_addipphone')\">";

$tooltip_update_reception = "onMouseOver=\"popUp(event,'phone_update_reception')\" onmouseout=\"popUp(event,'phone_update_reception')\"";
print "<a href=\"ipphones.sh\">Add IP Phone</a></td><td></td><td $tooltip_update_reception><input type=\"submit\" value=\"Update Reception\"></td></tr>";

$tool_tip = "onMouseOver=\"popUp(event,'phone_lines')\" onmouseout=\"popUp(event,'phone_lines')\"";

print '<tr><td>&nbsp</td></tr>';
print "<tr $tool_tip><td colspan=\"4\" align=\"left\" valign=\"top\" ><h2>Phone Lines</h2></td></tr>
";

print "</form>";

# print list of analog phone lines

$tooltip_phoneline = "onMouseOver=\"popUp(event,'phone_phoneline')\" onmouseout=\"popUp(event,'phone_phoneline')\"";
$tooltip_port = "onMouseOver=\"popUp(event,'phone_port_line')\" onmouseout=\"popUp(event,'phone_port_line')\"";
$tooltip_line_prefix = "onMouseOver=\"popUp(event,'phone_line_prefix')\" onmouseout=\"popUp(event,'phone_line_prefix')\"";

foreach $a (sort keys %analog) {
    if ($zap{$a} eq "fxo") {
	$icon = "<img src=\"tick.png\" alt=\"Phone Line OK\" />";
	print "<tr><td $tooltip_line_prefix>0</td><td $tooltip_phoneline>Analog Line</td><td $tooltip_port>Port $a</td><td></td><td>$icon</td></tr>\n";
    }
}

# print list of SIP VOIP trunks

$tooltip_voipline = "onMouseOver=\"popUp(event,'phone_voipline')\" onmouseout=\"popUp(event,'phone_voipline')\"";
$tooltip_voipline_ip = "onMouseOver=\"popUp(event,'phone_voipline_ip')\" onmouseout=\"popUp(event,'phone_voipline_ip')\"";
$tooltip_voipline_prefix = "onMouseOver=\"popUp(event,'phone_voipline_prefix')\" onmouseout=\"popUp(event,'phone_voipline_prefix')\"";

foreach $s (sort keys %voip) {
    if ($voip{$s} eq "Registered") {
	$icon = "<img src=\"tick.png\" alt=\"VOIP Line OK\" />";
	print "<tr><td $tooltip_voipline_prefix>1</td><td $tooltip_voipline>VOIP Line</td><td $tooltip_voipline_ip>$ipad{$s}</td><td></td><td>$icon</td></tr>\n";
    }
}

print '<tr><td>&nbsp;</td></tr>';
print "<tr><td colspan=2 onMouseOver=\"popUp(event,'phone_addvoipline')\" onmouseout=\"popUp(event,'phone_addvoipline')\">";
print "<a href=\"voiplines.sh\">Set Up VOIP Line</a></td></tr>";
