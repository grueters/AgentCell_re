#!/bin/bash

######################################################################
#  x2p.sh - A simple bash script for generating an enlarged PPM format
#           image file, from an XPM format image file.
#
#           This script uses "xpmtoppm" and pnmenlarge, which are part
#           of Netpbm.  Netpbm is a toolkit for conversion of images
#           between a variety of different formats, as well as to allow
#           a few basic image operations. Netpbm is available from:
#           "http://netpbm.sourceforge.net/".
#
#  Usage: xpmsplit.pl snapshot_file
#
#  Note:  This script will not work if the filename suffix of the input
#         snapshot file is not ".OUT".
#
######################################################################

for a 
do
    echo "Converting $a to ${a%xpm}ppm..."
    xpmtoppm $a \
    | pnmenlarge 4 \
    > ${a%xpm}ppm
done
