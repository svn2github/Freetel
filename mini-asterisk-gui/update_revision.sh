#!/bin/sh
# update_revision.sh
#
# Sets correct Revision number in the about screen.  Good idea to
# run this before installing

ver=`svn info | grep Revision`
sed -i "s/Revision: [0-9]*/$ver/" cgi-bin/about.sh
