#!/sbin/microperl
# set_voiplines.pl
# David Rowe 12 Jan 2010
#
# Replaces fields in sip.conf, outputs new sip.conf contents to stdout

$user = $ARGV[0];
$pass = $ARGV[1];
$host = $ARGV[2];
$provider_new = $ARGV[3];

# We need to slurp up the easy asterisk provider and spit them
# back out.  All must be commented out except for the one that
# is selected.  Hopefully non-easy asterisk content of sip.conf
# won't be affected.

open SIP, "/etc/asterisk/sip.conf";
$provider = "";
while (<SIP>) { 

    # start of any new stanza switches off parsing.  It may get
    # switched back on below if it contains easy-asterisk
    # keyword. This allows non-easy asterisk SIP devices to be
    # included in sip.conf

    if (/\[/) {
	$provider = "";	
    }

    # look for commented or uncommented easy asterisk provider stanza

    if (/\[.* \"(.*)\" easy-asterisk/) {
	$provider = $1;
    }

    if ($provider ne "") {
	# OK, we are in an easy-asterisk stanza

	# strip off any leading ";"

	$_ =~ s/^\;//;
	
	if ($provider eq $provider_new) {

	    # this stanza should be uncommented

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
	else {
	    print ";$_";
	}
    }

}
close SIP;

