#!/usr/bin/perl -w

###############################################################################
#           TkStochSim, a perl/Tk interface for StochSim                       
# Original author of stochsim (Win32 version) and its MS Windows graphical   
# interface                          1998 : Carl Morton-Firtz                
# Original port to Linux             1999 : Tom Simon Shimizu                
# Corrections and Perl/Tk interface  2000 : Nicolas Le Novère                
#
# File: progbar.pl
#
# Aim:  a progress bar for stochsim
#
# Author: Nicolas Le Novère
#
# Date of last modification: 30/OCT/2000
#
###############################################################################

#   This program is free software; you can redistribute it and/or modify it 
#   under the terms of the GNU General Public License as published by the Free 
#   Software Foundation; either version 2 of the License, or (at your option) 
#     any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
#   for more details.
#
#   You should have received a copy of the GNU General Public License along 
#   with this program; if not, write to the Free Software Foundation, Inc., 59 
#   Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#   Nicolas Le Novère 
#   Department of Zoology, University of Cambridge, Downing street, 
#   CB2 3EJ Cambridge, UK. e-mail: nl223@cus.cam.ac.uk

use 5.004;
use Tk 800.022;
use strict;

use Tk::widgets qw/ProgressBar/;
use vars qw/$VERSION $value $bar $butt/;

$VERSION = 0.001;
$value = 0;
my $output = shift @ARGV or die "no output file specified";

my $mw = MainWindow->new();
$mw->resizable(0,0);
$mw->geometry("-10+10"); 
$mw->title("Simulation in progress");

$bar = &initbar;
$butt = &initbutt;
$mw->Label(-text => "%")->pack(-side => 'left',-anchor => 'w');
$butt->invoke;

MainLoop;

sub initbar {
    my $bar = $mw->ProgressBar(-width    => 20,
			       -length   => 150,
			       -from     => 0,
			       -to       => 100,
			       -gap      => 0,
			       -blocks   => 10,
			       -colors   => [0,'cyan'],
			       -variable => \$value,
			       )->pack(-side => 'left');
    return $bar;
}

sub initbutt {
    my $butt = $mw->Button(-textvariable => \$value,
			   -relief       => 'flat',
			   -command      => sub{ 
			       while ($value != 100){
				   $bar->value(&updatevalue);
				   $bar->update;
				   sleep(1);
			       }
			       exit;
			   } )->pack(-side => 'left');
    return $butt;
}

sub updatevalue {
    
    my $val;
    open(MH,$output) or die "unable to open $output";
    { # no record separator anymore, i.e. whole file in one shot. local because we want it back afterward
	local $/;        
	$_ = <MH>;
	$_ =~ /(\d+)%[^%]+$/sg;
	$val = $1;
    }
    close(MH) or die "unable to close $output";;
    return $val;
}








