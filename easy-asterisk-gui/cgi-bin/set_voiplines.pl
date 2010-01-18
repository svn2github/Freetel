#!/usr/bin/perl
# set_voiplines.pl
# David Rowe 12 Jan 2010
#
# Replaces fields in sip.conf, outputs new sip.conf contents to stdout

$user = $ARGV[0];
$pass = $ARGV[1];
$host = $ARGV[2];
$stanza_new = $ARGV[3];

# We need to slurp up the easy asterisk provider and spit them
# back out.  All must be commented out except for the one that
# is selected.  Hopefully non-easy asterisk content of sip.conf
# won't be affected.

open SIP, "/etc/asterisk/sip.conf";
$provider = "";
while (<SIP>) { 

    # start of any new stanza switches off parsing.  It may get
    # switched back on below if it contains easy-asterisk
    # keyword. This allows non-easy asterisk SIP stanzas to be
    # included in sip.conf

    if (/\[/) {
	$stanza = "";	
    }

    # look for commented or uncommented easy asterisk provider stanza

    if (/\[(.*)\] .* easy-asterisk/) {
	$stanza = $1;
    }

    if ($stanza eq "") {
	# we are not in an easy-asterisk provider stanza
	
	if (/;*register => (\S*)@(\S*).*;.*(easy-asterisk.*)/) {
	    # an easy-asterisk register line

	    #print "XX stanza_new='$stanza_new' '$1' '$2' '$3'\n";
		    
	    if ($2 eq $stanza_new) {
		print "register => $user\@$stanza_new; $3\n";
	    }
	    else {
		print ";register => $1\@$2 ; $3\n";
	    }
	}
	else {
	    # OK so this is a regular sip.conf line, just echo to stdout
	    print $_;
	}
    }
    else {
	# OK, we are in an easy-asterisk stanza

	# strip off any leading ";"

	$_ =~ s/^\;//;
	
	if ($stanza eq $stanza_new) {

	    # this stanza should be uncommented

	    if (/^user=/) {
		print "user=$user\n";
	    }
	    if (/^username=/) {
		print "username=$user\n";
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
	    # comment out unused easy-asterisk stanzas
	    print ";$_";
	}
    }

}
close SIP;

