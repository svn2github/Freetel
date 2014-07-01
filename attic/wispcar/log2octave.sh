#!/bin/sh
# log2octave.sh
# David Rowe 23 June 2009

# strip out time stamp and voltage into four columns so octave batt_test.m 
# can read

cut -f 4,8 -d ' ' $1 | sed -e 's/:/ /g' > $1.oct
