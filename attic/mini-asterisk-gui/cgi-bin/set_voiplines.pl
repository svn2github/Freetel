#!/usr/bin/perl
# set_voiplines.pl
# David Rowe 12 Jan 2010
#
# Replaces fields in sip.conf, outputs new sip.conf contents to stdout
# Gets run when "OK" is pressed on the Voip Lines screen

$user = $ARGV[0];
$pass = $ARGV[1];
$host = $ARGV[2];
$provider_new = $ARGV[3];

# We need to slurp up the mini asterisk providers and spit them
# back out.  All must be commented out except for the one that
# is selected.  Hopefully non-mini asterisk content of sip.conf
# won't be affected.

open SIP, "/etc/asterisk/sip.conf";
$provider = "";
while (<SIP>) { 

    # start of any new stanza switches off parsing.  It may get
    # switched back on below if it contains mini-asterisk
    # keyword. This allows non-mini asterisk SIP stanzas to be
    # included in sip.conf

    if (/\[/) {
	$stanza = "";	
    }

    # look for commented or uncommented mini asterisk provider stanza

    if (/\[(.*)\].* \"(.*)\" mini-asterisk/) {
	$stanza = $1;
	$provider = $2;
    }

    if ($stanza eq "") {
	# we are not in an mini-asterisk provider stanza
	
        # however we may hit a mini-asterisk register line here

	if (/.*register => (\S*)@(\S*).*;.*(mini-asterisk.*)/) {
	    # the mini-asterisk register line, not only one register supported
	    # at the moment (one VOIP line)

	    #print "XX provider_new='$provider_new' '$1' '$2' '$3'\n";
		
	    # commentout register if "none" selected

	    if ($user eq "none") {
		print ";";
	    }

            # note final /$user seems to be necessary so that incoming VOIP
	    # calls work.  I really don't get the syntax but messed around for
	    # hours until it worked!

	    print "register => $user\@$user/$user; $3\n";
	}
	else {
	    # OK so this is a regular sip.conf line, just echo to stdout
	    print $_;
	}
    }
    else {
	# OK, we are in an mini-asterisk stanza

	# strip off leading ";", except for comments lines which start
	# with ;;

	unless (/^;;/) {
	    $_ =~ s/^\;//;
	}
	
	if ($provider eq $provider_new) {

	    # this stanza should be uncommented

	    if (/\[.*\].* \".*\" mini-asterisk/) {
		# rename stanza name to user name
		# this is required to make incoming calls work
		print "[$user] ; \"$provider\" mini-asterisk do not remove this comment\n";
	    }
	    elsif (/^username=/) {
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
	    # comment out unused mini-asterisk stanzas
	    if (/^;;/) {
		# special csse for comment lines
		print "$_";
	    }
	    else {
		print ";$_";
	    }
	}
    }

}
close SIP;

