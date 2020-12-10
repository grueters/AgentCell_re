#!/usr/bin/perl

######################################################################
#  xpmsplit.pl - A simple Perl script for splitting StochSim array
#                snapshot outputs into separate XPM files.  xpmsplit.pl
#                will create as many output xpm files as there are 
#                snapshot frames in the input file.
#
#  Usage: xpmsplit.pl snapshot_file
#
#  Note:  This script will not work if the filename suffix of the input
#         snapshot file is not ".OUT".
#
######################################################################

$IFNAME = $ARGV[0];

if ($IFNAME !~ /(.+)\.OUT/) {     # check if input file is a .OUT file
    die "Fatal error: Input file must be a .OUT file.\n";
}
$IFSTEM = $1;                     # get stem of filename


open (INFILE,$IFNAME) 
or die "Fatal error: Could not open input file: $IFNAME\n" ;

$count = 0;
$OFNAME = sprintf "%s_%03d.xpm", ($IFSTEM,$count);
warn "Opening file: $OFNAME\n";
open (OUTFILE, ">$OFNAME")
    or die "Fatal error: Could not open output file: $OFNAME\n" ;

$newfile = 0;
while (<INFILE>) {
   

   if ($_ !~ /^\/\* XPM/ &&
       $_ !~ /^static/ &&
       $_ !~ /^\"/)
   { next; }

    if ($newfile == 1){
	$OFNAME = sprintf "%s_%03d.xpm", ($IFSTEM,$count);
	warn "Opening file: $OFNAME\n";
	open (OUTFILE, ">$OFNAME")
	    or die "Fatal error: Could not open output file: $OFNAME\n";
	$newfile = 0;
    }
   print OUTFILE "$_";

   if (/\;/){ 
       close(OUTFILE);
       $count++; 
       $newfile = 1;
   }
}
