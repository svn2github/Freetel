#!/sbin/microperl
# set_voiplines.pl
# David Rowe 12 Jan 2010
#
# Replaces fields in sip.conf, outputs new sip.conf contents to stdout

$user = $ARGV[0];
$pass = $ARGV[1];
$host = $ARGV[2];

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

	print $_;
    }

    if ($state eq "inside [voip]") {

	if (/^\[/) {
	    $next_state = "finished";
	}
	if (/^;.*\[/) {
	    $next_state = "finished";
	}

	if (/^user=/) {
	    print "user=$user\n";
	}
	elsif (/^fromuser=/) {
	    print "fromuser=$user\n";
	}
	elsif (/^secret=/) {
	    print "secret=$pass\n";
	}
	elsif (/^host=/) {
	    print "host=$host\n";
	}
	else {
	    print $_;
	}
    }

    if ($state eq "finished") {
	print $_;
    }

    $state = $next_state;
}
close SIP;

