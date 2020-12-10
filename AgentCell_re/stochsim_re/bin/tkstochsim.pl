eval '(exit $?0)' && eval 'exec perl -wS $0 ${1+ "$@"}' && eval 'exec perl -wS $0 $argv:q' if 0;

# The line above is supposed to find perl where it is. Tested under Linux and dec_osf

######################################################################################################
#                       TkStochSim, a perl/Tk interface for StochSim                       
#
# File: tkstochsim.pl
#
# Aim:  Graphical frontend for the STOCHastic SIMulator
#
# Author: Nicolas Le Novère
#
# Date of last modification: 02/MAY/2005
#
######################################################################################################

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
#   Nicolas LE NOVÈRE,  Computational Neurobiology, 
#   EMBL-EBI, Wellcome-Trust Genome Campus, Hinxton, Cambridge, CB10 1SD, UK
#   Tel: +44(0)1223 494 521,  Fax: +44(0)1223 494 468,  Mob: +33(0)689218676
#   http://www.ebi.ac.uk/~lenov


# Notes on nomenclature
#  The suffices have the following meanings:
#  Br -> BrowseEntry, Bt -> Button, Fr -> Frame, L -> Label, Lb -> Listbox, m -> menu, 
#  Nb -> NoteBook, W -> window, mi -> menu item 

use 5.6.0;      # I don't know if I need more than 5.004, but my textbooks are based on that version
use Tk 800.023; # I definitively need that one
use Cwd;
use strict;
use File::Basename;
use Data::Dumper;

use Tk::widgets qw/BrowseEntry LabFrame ROText DragDrop DropSite NoteBook ProgressBar/;
# Tk::Dialog is used to display the error messages
# Tk::BrowseEntry is entry with multiple predefined choices
# Tk::LabFrame is a frame with a label
# Tk::ROText is a text widget which content cannot be altered by users
# Tk::DragDrop and Tk::DropSite are involved in the drag and drop process ;-)
# Tk::ProgressBar ...

# use subs qw//; # FIXME declare functions here 

#variables regarding the operating system and perl installation
use vars qw/$sep/;

# variables regarding stochsim and tkstochsim
use vars qw/$VERSION %limits %messages %look/;

# variables regarding the present user or the present simulation  
use vars qw/$home @previous %total_cplxs $sim @history $stochsim_pid/;       

# widgets
use vars qw/$cpnt_L $cplx_L $react_L $ms_L $ns_L $ar_L $vd_Bt $m_Bt $pl_Bt $run_Bt $stop_Bt $quit_Bt $save_m $Xsbml_m $Xsbml2_m $run_mi $stop_mi/;

##### @previous contains the addresses of the past simulations
##### @history contains refs to the entries of the menubar
##### %total_cplxs contain a list of all complexes. The keys are their symbol,
#####   the values are their type. Currently (Dec 2000) 'simple' or 'multistate' 

$VERSION = "1.037";

######################################################################################################
#         Specification of the distribution tree, allowing to find the necessary files
######################################################################################################

# use vars and not my() to let the variables be accessible to other modules
use vars qw/$DIRBASE $DIRCONF $DIRGUITK $DIRHELP $DIRLIB $DIRBIN/;

# because we want to execute that before le 'use lib $DIRLIB'
BEGIN {
    if ($^O eq "unix" or $^O eq "linux" or $^O eq "dec_osf" or $^O eq "darwin" or $^O eq "aix"){
	$sep = "/";
    } elsif ($^O eq "MSWin32"){
	$sep = "\\";
    } else { die "I do not recognise the operating system: $^O\nPlease contact the developpers of StochSim (See the file AUTHORS).\n" }
    
# Top directory of the distribution, env var STOCHSIM
    $DIRBASE = $ENV{"STOCHSIM"}; 
    $DIRHELP = $DIRBASE.$sep."doc";     # Directory containing the various help files
    $DIRLIB  = $DIRBASE.$sep."lib";     # Directory containing the common packages
    $DIRBIN  = $DIRBASE.$sep."bin";     # Directory containing binaries (e.g. stochsim)
}

use lib $DIRLIB;     # Add $DIRLIB in front of @INC to access the personal librairies

use MyBrowseEntry;   # A modification of BrowseEntry which return the index of the choice
use Balloon;         # A modification of Balloon by Slaven Rezic, which does not care of grabs
use readinifile;
use writeinifile;
use writesbml;
use readsbml;

######################################################################################################
#                    This is a reference to a hash describing a simulation
######################################################################################################
# This is a description of a reference to a hash describing a simulation

$sim = {
    "main_config_dir"   => undef,  
# contains the address of the main directory containing the configuration files

    "general_config"    => {}, 
# hash containing the substance of the file STCHSTC.INI

    "components"        => {}, 
# hash of hashes containing the elementary blocks of the simulation each 
# element possesses three keys 'Name', 'Symbol' and 'Description'.
# { Symbol => { Name        => scalar,
#               Description => scalar } }

    "complexes"         => {}, 
# Hash of hashes containing the objects of the reactions. Each element 
# possesses four mandatory keys 'Symbol', 'InitLevel', Constant and 'Components'. 
# 'Components' points to an array containing the symbols of the components forming 
# the complex. In addition the optional key 'Description' point to a short 
# description of the complex 
# { Symbol => { InitLevel   => scalar,
#               Constant    => scalar (0|1)
#               Description => scalar
#               Components  => [] } }

    "display_variables" => [], 
# array of hashes containing the variables to be displayed (Useful only if 
# DisplayAllLevels = 0). Each element possesses two keys 'Name' and 'Types'.
# 'Types' is an array containing the complexes included in the variable.
# { Name  => scalar,
#   Types => [] }

    "reactions"         => [], 
# array of hashes containing description of reactions in the system. Keys are 
# 'Substrates', 'Products', 'Kf', 'Kr', 'Description'. The two formers are 
# arrays
# { Description => scalar,
#   Kf          => scalar,
#   Kr          => scalar,
#   Substrates  => [],
#   Products    => [] }

    "dynamic_values"       => {}, 
# hash, which keys are the codes of the dynamic values. Each point to a hash 
# which keys are the timepoints and the values are ... the values

# Hash of hashes containing description of the multistates. The keys are 
# 'File', 'Symbol', 'Description', 'Flags', 'InitLevel', 'Rapid_equilibriums', 
# 'Display_variables', 'Reactions'. 'Flags' is an array containing the name of 
# the various flags.
# 'InitLevel' is a hash, which keys are the states (including 
# wildcards). 'Rapid_equilibriums' is an array of hashes which keys are 'Flag',
# which value is the name of the flag affected by this rapid equilibrium and the 
# states, which values are probabilities. 'Display_variables' is an array of hashes 
# which keys are 
# 'Name' and 'States'. States is an array of bitstring representing the states
# handled by this variable. 'Reactions' is an array  of hashes which keys are 
# 'Symbol', 'Effect' and the states. 'Effect' point to an array of values composed
# of + or - followed by one of the flags. 
#  { Symbol => { File               => scalar,
#                Description        => scalar,
#                Flags              => []
#                InitLevel          => { _bitstring_ => scalar },
#                Rapid_equilibriums => [ { Description => scalar,
#                                          Flag        => scalar,
#                                          _bitstring_ => scalar } ],
#                Display_variables  => [ { Name   => scalar,
#                                          States => [] } ]
#                Reactions          => [ { Symbol      => scalar,
#                                          Description => scalar,
#                                          Effect      => [], 
#                                          _bitstring_ => scalar } ] }

    "ns_complexes"      => {}, 
# Hash of hashes containing description of the neighbour sensitive complexes. 
# The keys are 'File','Symbol','Rapid_equilibriums','Reactions'. 'File' is the 
# address of the file containing the specification of this ns_complex. 'Symbol' 
# is the unique identifier of this complex. 'Rapid_equilibriums' is an array of 
# hashes which keys are 'Flag', 'CoupledStates' and a set of bitstrings. 
# 'CoupledStates' is an array. 'Reactions' is an array of hashes which keys
# are 'EffectOnNeighbour', 'ReactNeighbourID' and bitstrings.
# { Symbol => {File               => scalar,
#              NumNeighbours      => scalar,
#              Description        => scalar,
#              Rapid_equilibriums => [ { Description   => scalar,
#                                        Flag          => scalar,
#                                        CoupledStates => [],
#                                        _bitstring_   => [] } ]
#              Reactions          => [ { EffectOnNeighbour => [],
#                                        ReactNeighbourID  => scalar,
#                                        Description       => scalar,
#                                        _bitstring_       => scalar } ] }

    "array_config"         => {}, 
# hash containing the general configuration read in ARRAY.INI
# { DumpInterval       => scalar,
# }

    "arrays"               => {}, 
# hash of hashes containing description of each array. The symbol of the array 
# points to a hash which keys are: Complex, NeighbourSensitive, Geometry,
# XDimension, YDimension, CreateDumpFile, BoundaryCondition,  EquilibrationInterval
# { Symbol => {Description           => scalar,  ...
#              Complex               => scalar,  the symbol of the complex building this array
#              NeighbourSensitive    => scalar,  boolean
#              Geometry              => scalar,  describe the array: Square, Trigonal, Hexagonal 
#              XDimension            => scalar,  integer
#              YDimension            => scalar,  integer 
#              CreateDumpFile        => scalar,  boolean
#              BoundaryCondition     => scalar,  integer: 0 is toroïdal, 1 is real boundaries
#              EquilibrationInterval => scalar   
#              InitialState          => scalar,
#              InitialStateFile      => scalar,
#              InitialStateFromTime  => scalar
#              Snapshot_variables    => { Name => { States          => [],      states shown in the snapshots} 
#                                                   StoreInterval   => scalar,  
#                                                   AveragedOutput  => scalar,
#                                                   AverageInterval => scalar,
#                                                   SampleInterval  => scalar,
#                                                   Description     => scalar } } } }


};

######################################################################################################
#                                 Initialisation of configuration
######################################################################################################
# Initialise some keys of the hash containing the substance of the file STCHSTC.INI

$sim->{"general_config"}{"DisplayInterval"}           = 0.01;
$sim->{"general_config"}{"StoreInterval"}             = 0.001;
$sim->{"general_config"}{"DumpInterval"}              = 0.01;
$sim->{"general_config"}{"TimeIncrement"}             = 0;
$sim->{"general_config"}{"SimulationDuration"}        = 1;
$sim->{"general_config"}{"ReactionVolume"}            = 1e-15;
$sim->{"general_config"}{"MaximumNumberComplexes"}    = 10000;
$sim->{"general_config"}{"AbortOnResolutionErr"}      = 1;    # means yes
$sim->{"general_config"}{"DisplayAllLevels"}          = 1;    # means yes
$sim->{"general_config"}{"OptimiseTimeIncrement"}     = 1;    # means yes
$sim->{"general_config"}{"RandomNumberGenerator"}     = 3;    # Shuffle random number generator
$sim->{"general_config"}{"TimeUnits"}                 = 1;    # means seconds
$sim->{"general_config"}{"CreateDumpFile"}            = 0;    # means no
$sim->{"general_config"}{"UseSpatialExtensions"}      = 0;    # means no
$sim->{"general_config"}{"InitialLevelsFromDumpFile"} = 0;    # means no

######################################################################################################
####################################### Beginning of the main loop ###################################
######################################################################################################
# necessary here because the initialisation requires widgets
 
my $mw = MainWindow->new();

######################################################################################################
#                                        Operating system issues
######################################################################################################
my $stochsim;

if ($^O eq "unix" or $^O eq "linux" or $^O eq "dec_osf" or $^O eq "darwin" or $^O eq "aix") {

# Look for the binary stochsim. First in the PATH, then in $DIRBIN, then ask the
# user where it is. Always test if it is executable
    
    my $path = undef;
    foreach (split /:/, $ENV{PATH}) {
	if (-f $_.$sep."stochsim" and -X $_.$sep."stochsim") {
	    print "I found a StochSim executable in the directory $_\n";
	    $path = $_;
	    last;
	}
    }
    if (defined $path) {
	$path = cwd if $path eq '.';
	$stochsim = $path.$sep."stochsim";
	if (not -X  $stochsim){
	    die "$stochsim is not executable. Check the file permissions.";
	}
    } elsif (-X  $DIRBIN.$sep."stochsim"){ 
	$stochsim = $DIRBIN.$sep."stochsim"; 
    } else { 
	$path = $mw->getOpenFile(-title      => "Choose a StochSim executable",
				 -initialdir => $DIRBIN);
	
	if (defined $path){
	    $stochsim = $path;
	    if (not -X  $stochsim){
		die "$stochsim is not executable. Check the file permissions.";
	    }
	} else {
	    die "I need the StochSim binary. Copy it in the directory\n",
	    "$DIRBIN or in a directory present in your PATH.\n";
	}
    }
    
# Looks for the directory $HOME/.stochsim. If unexistant, create it. 
# Check the permissions and setup $DIRCONF to it.
    
    $home = $ENV{"LOGDIR"} unless $home = $ENV{"HOME"};
    mkdir $home.$sep.".stochsim",0777 unless (-e $home.$sep.".stochsim" and -d $home.$sep.".stochsim");
    $DIRCONF = $home.$sep.".stochsim";
    `cp $DIRLIB${sep}stochsimrc $DIRCONF${sep}stochsimrc` unless (-e $DIRCONF.$sep."stochsimrc");
    
} elsif ($^O eq "MSWin32") {
    $stochsim = $DIRBIN.$sep."stochsim.exe";
    $DIRCONF = $DIRBASE.$sep."config_examples"; # Basal starting directory for config. 
    `copy $DIRLIB${sep}stochsimrc $DIRCONF${sep}stochsimrc` unless (-e $DIRCONF.$sep."stochsimrc");
    
} else {
    die "I do not recognise the operating system";
}

######################################################################################################
# Read the configuration files: limits of StochSim, messages of TkStochSim,
# configuration of TkStochSim. Then read the past simulations
######################################################################################################

eval{
# Hash containing the limits of stochsim which have to be respected by the interface
    %limits = read_gui_conf($DIRLIB.$sep."limits"); 
    $limits{"MAX_COMPLEX_SYMBOL_LENGTH"} = $limits{"MAX_COMPONENT_SYMBOL_LENGTH"} * $limits{"MAX_COMPONENTS_IN_COMPLEX"};
    
# Hash containing the messages to send to the user
    %messages = read_gui_conf($DIRLIB.$sep."messages");       
    
# Hash containing the common rendering parameters
    %look = read_gui_conf($DIRLIB.$sep."stochsimrc");   
    if (-e $DIRCONF.$sep."stochsimrc"){
	
# Hash containing the personal rendering parameters (which overide common parameters)
	# UGLY. SUPRESS THE INTERMEDIARY %perso_config

	my %perso_config = read_gui_conf($DIRCONF.$sep."stochsimrc");
	my ($key,$value);
	while (($key,$value) = each %perso_config){
	    $look{$key} = $value;
	} 
    }
};
message("error","error",$@) if $@;

foreach my $config_key (sort keys %look){
    if ($config_key =~ /^\s*SIM/){          # Read the location of the old simulations 
	push(@previous,$look{$config_key});
    }
}

######################################################################################################
################################################# LET'S GO ###########################################
######################################################################################################
# The user cannot resize the main window?
$mw->resizable(0,0);

# One cannot kill the application via the decorations
$mw->protocol('WM_DELETE_WINDOW' => sub {});

# Activates the help balloons. 
my $balloon = $mw->Balloon(-state => "balloon");

# FIXME: Put everything before the MainLoop in a function;

$mw->geometry("+".$look{"xmain"}."+".$look{"ymain"});
$mw->title("TkStochSim");

$mw->bind("<Control-a>" => sub { save_file_as($sim,\@previous) });
$mw->bind("<Control-b>" => sub { $balloon->configure(-state => "balloon") });
$mw->bind("<Control-i>" => sub { read_sbml($sim) });
$mw->bind("<Control-n>" => sub { $balloon->configure(-state => "none") });
$mw->bind("<Control-o>" => sub { open_file($sim) });
$mw->bind("<Control-p>" => sub { plotsim($sim) });
$mw->bind("<Control-P>" => \&preferences);
$mw->bind("<Control-q>" => \&quittkstochsim);
$mw->bind("<Control-r>" => sub { $stochsim_pid = run_sim($sim) });
$mw->bind("<Control-s>" => sub { save_file($sim) });
$mw->bind("<Control-v>" => sub { view_data($sim->{"main_config_dir"}) });
$mw->bind("<Control-w>" => sub { 
    if (kill 9 => $stochsim_pid){ 
	$stop_mi->configure(-state => 'disabled',); 
    }});
$mw->bind("<Control-x>" => sub { write_sbml1($sim) });
$mw->bind("<Control-z>" => sub { write_sbml2($sim) });

######################################################################################################
#                                              Menu bar
######################################################################################################

# because of the history menu, constructed dynamically, I make the menu in a function.
# I am sure there is a way to update only the "file" cascade with the "update" or idletasks methods
my $menubar = $mw->Menu;
$mw->configure(-menu => $menubar);
constructMenu($sim,\@previous,$menubar);

######################################################################################################
#                              Enter the parameters of the simulation                                
######################################################################################################

my $arg_Fr = $mw->LabFrame(
			   -borderwidth => 1, 
			   -label       => 'Configure the simulation',
			   -labelside   => 'acrosstop'
			   )->pack(-expand => '1', 
				   -fill   => 'both');

#-----------------------------------------------------------------------------------------------------
#                                     General configuration
#-----------------------------------------------------------------------------------------------------

my $conf_Bt = $arg_Fr->Button(-text             => 'General configuration',
			      -width            => '18',
			      -activeforeground => $look{"activeforeground"},
			      -activebackground => $look{"activebackground"},
			      -command          => sub{ general_configuration($sim) }
			      )->grid(-row        => 0,
				      -column     => 0,
				      -columnspan => 3,
#				      -sticky     => 'nsew'
				      );

$balloon->attach($conf_Bt,-msg => $messages{"ConfigButton"});

#-----------------------------------------------------------------------------------------------------
#                                              Components
#-----------------------------------------------------------------------------------------------------

$arg_Fr->Button(-text             => 'new',
		-width            => '5',
		-activeforeground => $look{"activeforeground"},
		-activebackground => $look{"activebackground"},
		-command          => sub{ print_component($sim) }
		)->grid(-row    => 1,
			-column => 2);

my $cpnt_Br = $arg_Fr->BrowseEntry(-variable  => \$cpnt_L,
				   -state     => 'readonly',
				   -listwidth => '25',
				   -browsecmd => sub{ print_component($sim,$cpnt_L) },
				   -width     => '9',
				   )->grid(-row    => 1,
					   -column => 1,
					   -sticky => 'e');

$balloon->attach($cpnt_Br,-msg => $messages{"Components"});

$cpnt_Br->configure(-listcmd => sub{$cpnt_Br->delete(0,'end');
				    $cpnt_Br->insert('end', sort keys %{ $sim->{"components"} });
				}
		    );

my $cpnt_Lb = $cpnt_Br->Subwidget('slistbox');
$cpnt_Lb->configure( -selectbackground => $look{"activeselectionbackground"},
		     -selectforeground => $look{"activeselectionforeground"},
		     );

# BrowseEntry is supposed to have a label method, but it seems not working
$arg_Fr->Label(-text => "components"
	       )->grid(-row    => 1,
			-column => 0);

#-----------------------------------------------------------------------------------------------------
#                                           Complexes
#-----------------------------------------------------------------------------------------------------

$arg_Fr->Button(-text             => 'new',
		-width            => '5',
		-activeforeground => $look{"activeforeground"},
		-activebackground => $look{"activebackground"},
		-command          => sub{ print_complex($sim) }
		)->grid(-row    => 2,
			-column => 2);

my $cplx_Br = $arg_Fr->BrowseEntry(-variable  => \$cplx_L,
				   -state     => 'readonly',
				   -listwidth => '35',
				   -browsecmd => sub{ print_complex($sim,$cplx_L) },
				   -width     => '9',
				    )->grid(-row    => 2,
					    -column => 1,
					    -sticky => 'e');

$balloon->attach($cplx_Br,-msg => $messages{"Complexes"});

$cplx_Br->configure(-listcmd => sub{ $cplx_Br->delete(0,'end');
				     $cplx_Br->insert('end', sort keys %{ $sim->{"complexes"} });
				 }
		    );

my $cplx_Lb = $cplx_Br->Subwidget('slistbox');
$cplx_Lb->configure( -selectbackground => $look{"activeselectionbackground"},
		     -selectforeground => $look{"activeselectionforeground"},
		     );

$arg_Fr->Label(-text => "complexes"
	       )->grid(-row    => 2,
		       -column => 0);

#-----------------------------------------------------------------------------------------------------
#                                        Multistates complexes
#-----------------------------------------------------------------------------------------------------

$arg_Fr->Button(-text             => 'new',
	       -width            => '5',
	       -activeforeground => $look{"activeforeground"},
	       -activebackground => $look{"activebackground"},
	       -command          => sub{ print_multistate($sim) }
	       )->grid(-row    => 3,
		       -column => 2);

my $ms_Br = $arg_Fr->BrowseEntry(#-label     => 'multistates ',
				 -variable  => \$ms_L,
				 -state     => 'readonly',
				 -listwidth => '35',
				 -browsecmd => sub{ print_multistate($sim,$ms_L) },
				 -width     => '9',
				 )->grid(-row    => 3,
					 -column => 1,
					 -sticky => 'e');

$balloon->attach($ms_Br,-msg => $messages{"Multistates"});

$ms_Br->configure(-listcmd => sub{ $ms_Br->delete(0,'end');
				   $ms_Br->insert('end', sort keys %{ $sim->{"ms_complexes"} });
			       }
		  );

my $ms_Lb = $ms_Br->Subwidget('slistbox');
$ms_Lb->configure( -selectbackground => $look{"activeselectionbackground"},
		   -selectforeground => $look{"activeselectionforeground"},
		   );

$arg_Fr->Label(-text => "MS_complexes"
	       )->grid(-row    => 3,
		       -column => 0);

#-----------------------------------------------------------------------------------------------------
#                                        Neighbour sensitive complexes
#-----------------------------------------------------------------------------------------------------

my $ns_Bt = $arg_Fr->Button(-text             => 'new',
			    -width            => '5',
			    -activeforeground => $look{"activeforeground"},
			    -activebackground => $look{"activebackground"},
			    -command          => sub{ 
				if ($sim->{"general_config"}{"UseSpatialExtensions"} == 1){
				    print_neighbour($sim); 
				} else {
				    message("error","error",$messages{"NoSpatialExtensions"})
				}
			    }
			    )->grid(-row    => 4,
				    -column => 2);

my $ns_Br = $arg_Fr->BrowseEntry(#-label     => 'neighbour-sensitives',
				 -variable  => \$ns_L,
				 -listwidth => '35',
				 -browsecmd => sub{ 
				     if ($sim->{"general_config"}{"UseSpatialExtensions"} == 1){
					 print_neighbour($sim,$ns_L);
				     } else {
					 message("error","error",$messages{"NoSpatialExtensions"});
				     }
				 },
				 -width     => '9',
				 )->grid(-row    => 4,
					 -column => 1,
					-sticky => 'e');

$balloon->attach($ns_Br,-msg => $messages{"NeighbourSensitives"});

$ns_Br->configure(-listcmd => sub{ $ns_Br->delete(0,'end');
				   $ns_Br->insert('end', sort keys %{ $sim->{"ns_complexes"} });
			       }
		  );

my $ns_Lb = $ns_Br->Subwidget('slistbox');
$ns_Lb->configure( -selectbackground => $look{"activeselectionbackground"},
		   -selectforeground => $look{"activeselectionforeground"},
		   );

$arg_Fr->Label(-text => "NS_complexes"
	       )->grid(-row    => 4,
		       -column => 0);


#-----------------------------------------------------------------------------------------------------
#                                        Arrays
#-----------------------------------------------------------------------------------------------------

$arg_Fr->Button(-text             => 'new',
		-width            => '5',
		-activeforeground => $look{"activeforeground"},
		-activebackground => $look{"activebackground"},
		-command          => sub{ 
		    if ($sim->{"general_config"}{"UseSpatialExtensions"} == 1){
			print_array($sim); 
		    } else {
			message("error","error",$messages{"NoSpatialExtensions"});
		    }
		}
		)->grid(-row    => 5,
			-column => 2);

my $ar_Br = $arg_Fr->BrowseEntry(#-label     => 'arrays',
				 -variable  => \$ar_L,
				 -listwidth => '35',
				 -browsecmd => sub{ 
				     if ($sim->{"general_config"}{"UseSpatialExtensions"} == 1){
					 print_array($sim,$ar_L);
				     } else {
					 message("error","error",$messages{"NoSpatialExtensions"});
				     }
				 },
				 -width     => '9',
				 )->grid(-row    => 5,
					 -column => 1,
					 -sticky => 'e');

$balloon->attach($ar_Br,-msg => $messages{"Arrays"});

$ar_Br->configure(-listcmd => sub{ $ar_Br->delete(0,'end');
				   $ar_Br->insert('end', sort keys %{ $sim->{"arrays"} });
			       }
		  );

my $ar_Lb = $ar_Br->Subwidget('slistbox');
$ar_Lb->configure( -selectbackground => $look{"activeselectionbackground"},
		   -selectforeground => $look{"activeselectionforeground"},
		   );

$arg_Fr->Label(-text => "arrays"
	       )->grid(-row    => 5,
		       -column => 0);

#-----------------------------------------------------------------------------------------------------
#                                       set reactions and rates
#-----------------------------------------------------------------------------------------------------

$arg_Fr->Button(-text               => 'new',
		-width              => '5',
		-activeforeground   => $look{"activeforeground"},
		-activebackground   => $look{"activebackground"},
		-highlightthickness => $look{"highlightthickness"},
		-command            => sub{ print_reaction($sim) }
		)->grid(-row    => 6,
			-column => 2);

my $react_Br = $arg_Fr->MyBrowseEntry(#-label     => 'reactions',
				      -variable  => \$react_L,
				      -state     => 'readonly',
#				      -listwidth => '65',   # Max Value seems 65 !!!
				      -browsecmd => sub{ print_reaction($sim,$_[2]) },
				      -width     => '9',
				      )->grid(-row    => 6,
					      -column => 1,
					      -sticky => 'e');

# Courrier font for the list of reaction (fixed font is nicer => aligns numbers) 
my $react_Lb = $react_Br->Subwidget('slistbox');
$react_Lb->configure(-font => "-adobe-courier-medium-r-normal--0-0-0-0-m-0-iso8859-1",
#		     -setgrid => 1,
		     -width => 0,
#		     -height => 0,
		     -selectbackground => $look{"activeselectionbackground"},
		     -selectforeground => $look{"activeselectionforeground"},
		     );


$balloon->attach($react_Br,-msg => $messages{"Reactions"});

$react_Br->configure(-listcmd => sub{ 
    $react_Br->delete(0,'end');
    my $num = 1;
    $react_Br->insert('end',
                      # Here I print the number of the reaction, followed by the description
		      map{ sprintf("%2s: ",$num++) . $_->{"Description"}}@{ $sim->{"reactions"} }) 
	              # Here I printed the reaction itself. Not ideal for multistates: several reactions A+B <=> C are possible.
#		      map{ sprintf("%2s: ",$num++) . join('+',@{ $_->{"Substrates"} })."<=>".
#			       join('+',@{ $_->{"Products"} })}@{ $sim->{"reactions"} }) 
    },
		     );

$arg_Fr->Label(-text => "reactions"
	       )->grid(-row    => 6,
		       -column => 0);

#-----------------------------------------------------------------------------------------------------
#                                     Dynamic values
#-----------------------------------------------------------------------------------------------------

my $dyn_Bt = $arg_Fr->Button(-text             => 'Dynamic values',
			      -width            => '18',
			      -activeforeground => $look{"activeforeground"},
			      -activebackground => $look{"activebackground"},
			      -command          => sub{ dynamic_values($sim) }
			      )->grid(-row        => 7,
				      -column     => 0,
				      -columnspan => 3,
#				      -sticky     => 'nsew'
				      );

$balloon->attach($dyn_Bt,-msg => $messages{"DynValueButton"});



######################################################################################################
#                     Run and monitor the simulation                    
######################################################################################################

my $run_Fr = $mw->LabFrame(-borderwidth => 1, 
			   -label       => 'Run the simulation',
			   -labelside   => 'acrosstop'
			   )->pack(-side   => 'top',
				   -expand => '1', 
				   -fill   => 'both');

my $bar_Fr = $run_Fr->Frame(
			   )->pack(-side   => 'bottom',
				   -expand => '1', 
				   -fill   => 'both');
my $prog_value = 0;
my $bar = $bar_Fr->ProgressBar(-width    => 20,
			       -length   => 280,
			       -from     => 0,
			       -to       => 100,
			       -gap      => 0,
			       -blocks   => 10,
			       -colors   => [0,'cyan'],
			       -variable => \$prog_value,
			       )->pack(-side   => 'left',
				       -expand => '1');


$balloon->attach($bar,-msg => $messages{"Monitor"});

my $barId;
$run_Bt = $run_Fr->Button(-text             => 'run',
#			  -width            => '5',
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{ 
			      $stochsim_pid = run_sim($sim);
			      $prog_value = 0;			      
			      $barId = $mw->repeat($look{"refresh"}*1000,\&updatevalue);
			      }
			  )->pack(-side   => 'left',
				  -expand => '1');

$balloon->attach($run_Bt,-msg => $messages{"RunButton"});

$stop_Bt = $run_Fr->Button(-text             => 'stop',
#			   -width            => '5',
			   -state            => 'disabled',
			   -activeforeground => $look{"activeforeground"},
			   -activebackground => $look{"activebackground"},
			   -command          => sub{
			       if (kill 9 => $stochsim_pid){ 
				   $stop_mi->configure(-state => 'disabled');
				   $run_mi->configure(-state => 'normal');
				   $run_Bt->configure(-state => 'normal');	
				   $stop_Bt->configure(-state => 'disabled');	
				   $mw->afterCancel($barId);
			       } else {
				   message("error","error",$messages{"CannotKill"});
			       } }
			   )->pack(-side   => 'left',
				   -expand => '1');

$balloon->attach($stop_Bt,-msg => $messages{"StopButton"});

$pl_Bt = $run_Fr->Button(-text             => 'plot',
#			 -width            => '10',
			 -activeforeground => $look{"activeforeground"},
			 -activebackground => $look{"activebackground"},
			 -state            => 'disabled',
			 -command          => sub{ plotsim($sim) }
			 )->pack(-side   => 'left',
				  -expand => '1');
$balloon->attach($pl_Bt,-msg => $messages{"Plot"});

$vd_Bt = $run_Fr->Button(-text             => 'values',
#			 -width            => '10',
			 -activeforeground => $look{"activeforeground"},
			 -activebackground => $look{"activebackground"},
			 -state            => 'disabled',
			 -command          => sub{ view_data($sim->{"main_config_dir"}) }
			 )->pack(-side   => 'left',
				 -expand => '1');

$balloon->attach($vd_Bt,-msg => $messages{"ViewData"});


# IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
# Note that tk exit function is used here instead of
# -command => [$main => 'destroy']
# The code after the MainLoop is therefore ignored.
# Remind it if you add code after the GUI termination

$quit_Bt = $run_Fr->Button(-text             => "quit",
#			   -width            => '5',
			   -activeforeground => $look{"activeforeground"},
			   -activebackground => $look{"activebackground"},
			   -command          => \&quittkstochsim,
			   )->pack(-side   => 'left',
				   -expand => '1');

$balloon->attach($quit_Bt,-msg => $messages{"QuitButton"});

$quit_Bt->bind("<Control-q>" => \&quittkstochsim);
$quit_Bt->focus;

MainLoop;

######################################################################################################
########################################### End of the main loop #####################################
######################################################################################################

######################################################################################################
# Construct the menubar
######################################################################################################

sub constructMenu{
    my ($sim,$previous,$menubar) = @_;
    

    $menubar->delete(0,'end');

#-----------------------------------------------------------------------------------------------------
#                                           Menu `File'
#-----------------------------------------------------------------------------------------------------

    my $fileMenu = $menubar->cascade(-label => "~File");
    
    $fileMenu->command(-label            => '~Open ...',
		       -command          => sub{ open_file($sim,$previous) },
		       -activeforeground => $look{"activeforeground"},
		       -activebackground => $look{"activebackground"},
		       -accelerator      => "Ctrl-o",
		       );
    
    $save_m = $fileMenu->command(-label            => '~Save',        
				 -command          => sub{ save_file($sim) },
				 -state            => 'disabled',
				 -activeforeground => $look{"activeforeground"},
				 -activebackground => $look{"activebackground"},
				 -accelerator      => "Ctrl-s",
				 );
    
    $fileMenu->command(-label            => 'Save ~As ...', 
		       -command          => sub{ 
			   save_file_as($sim,$previous) 
			   },
		       -activeforeground => $look{"activeforeground"},
		       -activebackground => $look{"activebackground"});
    
    $fileMenu->command(-label            => '~Import SBML', 
		       -command          => sub{ 
			   read_sbml($sim) 
			   },
		       -activeforeground => $look{"activeforeground"},
		       -activebackground => $look{"activebackground"},
		       -accelerator      => "Ctrl-i",
		       );
    
    $Xsbml_m = $fileMenu->command(-label            => 'E~xport SBML1v2', 
				  -state            => 'disabled',
				  -command          => sub{ 
				      write_sbml1($sim) 
				      },
				  -activeforeground => $look{"activeforeground"},
				  -activebackground => $look{"activebackground"},
				  -accelerator      => "Ctrl-x",
				  );
    $Xsbml2_m = $fileMenu->command(-label            => 'E~xport SBML2v1', 
				  -state            => 'disabled',
				  -command          => sub{ 
				      write_sbml2($sim) 
				      },
				  -activeforeground => $look{"activeforeground"},
				  -activebackground => $look{"activebackground"},
				  -accelerator      => "Ctrl-z",
				  );
    
    $fileMenu->separator;
    
#------------------------------------ Construct the "history" menu -----------------------------------
    
    my $i = 0;
    foreach my $simdir (@$previous){
	$history[$i] = $fileMenu->command(-label            => "$simdir", 
					  -command          => sub{ Open_sim($simdir,$sim,$previous) },
					  -activeforeground => $look{"activeforeground"},
					  -activebackground => $look{"activebackground"});
	$i++;
    }
    
    $fileMenu->separator;
    
    $fileMenu->command(-label            => '~Quit',        
		       -command          => \&quittkstochsim,
		       -activeforeground => $look{"activeforeground"},
		       -activebackground => $look{"activebackground"},
		       -accelerator      => "Ctrl-q",
		       );
    
# IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
# Note that tk exit function is used above instead of -command => [$main => 'destroy']
# The code after the MainLoop is therefore ignored.
# Remind it if you add code after the GUI termination
#-----------------------------------------------------------------------------------------------------
#                                         Menu `Simulation'
#-----------------------------------------------------------------------------------------------------
    
    my $SimMenu = $menubar->cascade(-label => "~Simulation"); 
    
    $run_mi = $SimMenu->command(-label            => '~Run',
				 -command          => sub{ run_sim($sim) },
				 -activeforeground => $look{"activeforeground"},
				 -activebackground => $look{"activebackground"},
				-accelerator      => "Ctrl-r",
				);
    
#    $SimMenu->command(-label            => 'Pause', 
#		      -state            => 'disabled',
#		      -command          => sub{ message("error","error",$messages{"NotImplemented"}) },
#		      -activeforeground => $look{"activeforeground"},
#		      -activebackground => $look{"activebackground"});
    
    $stop_mi = $SimMenu->command(-label            => 'Stop',
				 -state            => 'disabled',
				 -command          => sub{ 
				     if (kill 9 => $stochsim_pid){ 
					 $stop_mi->configure(-state => 'disabled');
					 $run_mi->configure(-state => 'normal');
					 $run_Bt->configure(-state => 'normal');					 
				     } else {
					 message("error","error",$messages{"CannotKill"});
				     }
				 },
				 -accelerator      => "Ctrl-w", 
				 -activeforeground => $look{"activeforeground"},
				 -activebackground => $look{"activebackground"});
    
#-----------------------------------------------------------------------------------------------------
#                                            Menu `Option'
#-----------------------------------------------------------------------------------------------------
    
    my $OptionMenu = $menubar->cascade(-label => "~Options");
    
    my $ball_on = $OptionMenu->command
	(-state            => $balloon->cget(-state) eq "balloon" ? "disabled" : "normal",
	 -label            => '~Balloon On', 
	 -activeforeground => $look{"activeforeground"},
	 -activebackground => $look{"activebackground"},
	 -accelerator      => "Ctrl-b",
	 );
    
    my $ball_off = $OptionMenu->command
	(-state            => $balloon->cget(-state) eq "balloon" ? "normal" : "disabled",
	 -label            => 'Balloo~n Off', 
	 -activeforeground => $look{"activeforeground"},
	 -activebackground => $look{"activebackground"},
	 -accelerator      => "Ctrl-n",	 
	 );
    
    $ball_on->configure(-command => sub{ $balloon->configure(-state => "balloon");
					 $ball_off->configure(-state => 'normal');
					 $ball_on->configure(-state => 'disabled');
				     },
			);
    $ball_off->configure(-command => sub{ $balloon->configure(-state => "none");
					  $ball_on->configure(-state => 'normal');
					  $ball_off->configure(-state => 'disabled');
				      },
			 );
    
    if ($look{"balloon"} =~ /^on$/i){
	$balloon->configure(-state => "balloon");
	$ball_off->configure(-state => 'normal');
	$ball_on->configure(-state => 'disabled');
    } elsif ($look{"balloon"} =~ /^off$/i){
	$balloon->configure(-state => "none");
	$ball_on->configure(-state => 'normal');
	$ball_off->configure(-state => 'disabled');
    } else {
	message("error","error",$messages{"UnknownBalloon"});
    }
    
    $OptionMenu->command
	(-label            => 'Preferences', 
	 -command          => \&preferences,
	 -activeforeground => $look{"activeforeground"},
	 -activebackground => $look{"activebackground"}
	 );
    
#-----------------------------------------------------------------------------------------------------
#                                           Menu `Help'
#-----------------------------------------------------------------------------------------------------
    
    my $helpMenu = $menubar->cascade(-label => "~Help"); 
    
    $helpMenu->command(-label            => 'PDF manual',   
		       -command          => sub{ show_pdf("stochsim_manual.pdf") },
		       -activeforeground => $look{"activeforeground"},
		       -activebackground => $look{"activebackground"});
    
    $helpMenu->command(-label            => 'About StochSim', 
		       -command          => sub{ message("info","info",$messages{"AboutStochSim"}) },
		       -activeforeground => $look{"activeforeground"},
		       -activebackground => $look{"activebackground"});
    
    $helpMenu->command(-label            => 'About TkStochSim', 
		       -command          => sub{ 
			   message("info","info", $messages{"AboutTkStochSim"}." - version $VERSION");
		       },
		       -activeforeground => $look{"activeforeground"},
		       -activebackground => $look{"activebackground"});    
}

######################################################################################################
#                     This function reads the configuration files of the GUI
######################################################################################################

sub read_gui_conf{
    my $inputfile = $_[0];
    my %filecontent;
    my ($line,$complete);

    open (INPUTFILE, $inputfile) or die "can't open file $inputfile: $!";
    while ($line = <INPUTFILE>){
	undef $complete;
	if ($line =~ /^\w/){             # remove the white or full comment lines
	    while ($line =~ /\\\s*$/){   # If this is a multiline entry
                # Remove the end backslash. Note the possibility of spaces afterward.
		$line =~ s/\\(\s*)$/$1/; 
		$complete .= $line;      # concatenate with the previous bits
		$line = <INPUTFILE>;     # input the next line
	    }
	    $complete .= $line;          # concatenate with the previous bits
	    $complete =~ s/\s*$//s;      # removes spaces at the end
	    $complete =~ /(.*?)\s*=\s*(.*)/s and $filecontent{$1} = $2;
	    # Note the s option at the end of the regexp: treat the entry as multiline
	}
    }
    close (INPUTFILE);
    return %filecontent;
}

######################################################################################################
#                      Opens a file selection to select the STCHST.INI
######################################################################################################

sub open_file{
    my ($sim,$previous) = @_;
    my $answer;
    my $CONFDIR;

    if ($ARGV[0]){
	print "read confdir from an argument.\n";
	$CONFDIR = $ARGV[0];              # Problem: do not know the upper part of the PATH
    } elsif ($look{"confdir"} =~ /\S+/){
	print "read confdir from the configuration hash.\n";
	$CONFDIR = $look{"confdir"};
    } else {
	print "default confdir.\n";
	$CONFDIR = $DIRCONF;
    }
    print "Initial confdir is:\n  $CONFDIR\n";

    # get the complete pathway of the configuration file.
    $answer = $mw->getOpenFile(-title            => "Open configuration file (STCHSTC.INI)",
			       -defaultextension => "INI",
			       -initialdir       => $CONFDIR);

    print "choosen directory:\n  $answer\n";    
    if ($answer){
        # keep only the main directory. Note that getOpenFile provide Unix-like paths, even 
	# under Windows
	$answer =~ s!/Input/\S*$!!i; 
	print "main directory:\n  $answer\n";
    
	Open_sim($answer,$sim,$previous);

	# we strip one more step in order top record the directory containing the simulations.
	$answer =~ s!/[^/]+$!!;
	print "new confdir:\n  $answer\n";
	$look{"confdir"} = $answer;
    }
}

######################################################################################################
#             Read the configuration of the selected simulation. Reset the interface
######################################################################################################

sub Open_sim{
    my ($maindir,$sim,$previous) = @_;
    
    eval {
	if ($maindir) {	
	    $sim->{"main_config_dir"} = $maindir;
	  readinifile::OpenFile($sim);
	    chdir $sim->{"main_config_dir"}.$sep."Input" 
		or die "can't change current directory to $sim->{main_config_dir}${sep}Input: $!"; 
	}
    };
    if ($@){
	message("error","error",$@);
    } else {
	my $bingo = 0;
	my $rec_idx;
	for ($rec_idx = 0; $rec_idx <= $#{$previous}; $rec_idx++){
	    if ($previous->[$rec_idx] eq $sim->{"main_config_dir"}){
		$bingo = 1; 
		last;
	    }
	}
	if ($bingo){
	    splice(@$previous,$rec_idx,1);
	    unshift(@$previous,$sim->{"main_config_dir"});
	} else {
	    pop @$previous;
	    unshift (@$previous,$sim->{"main_config_dir"});
	}
	constructMenu($sim,$previous,$menubar);

	$Xsbml_m->configure(-state => 'normal');
	$Xsbml2_m->configure(-state => 'normal');
	$save_m->configure(-state => 'normal');

	if (-e "$sim->{main_config_dir}${sep}Output${sep}VAR.OUT"){ 
	    if (defined $vd_Bt){ $vd_Bt->configure(-state => 'normal') }
	    if (defined $m_Bt){ $m_Bt->configure(-state => 'normal') }
	    if (defined $pl_Bt){ $pl_Bt->configure(-state => 'normal') }
	}
	# To allow the exchange of configuration files between different installations
	$sim->{"general_config"}{"MessageINIFile"} = $DIRLIB.$sep."MESSAGE.INI";
	
	($cpnt_L,$cplx_L,$react_L) = (undef,undef,undef);

	# population of the list of complexes and the list of flags
	foreach my $cplx (keys %{ $sim->{"complexes"} }){
	    $total_cplxs{$cplx}{'simple'} = 1;
	}
	foreach my $ms (keys %{ $sim->{"ms_complexes"} }){
	    $total_cplxs{$ms}{'multistate'} = 1;
	}
	foreach my $ns (keys %{ $sim->{"ns_complexes"} }){
	    $total_cplxs{$ns}{'neighboursensitive'} = 1;
	}
    }
}

######################################################################################################
#                          save the initialisation parameters within
######################################################################################################

sub save_file{
    my ($sim) = @_;

    if (not defined $sim->{"main_config_dir"}){
	save_file_as($sim);
    } else {
	$sim->{"general_config"}{"MessageINIFile"}    = $DIRLIB.$sep."MESSAGE.INI";
	$sim->{"general_config"}{"ComplexINIFile"}    = "COMPLEX.INI";
	$sim->{"general_config"}{"DynamicValues"}     = "DYNAMIC.INI";
	$sim->{"general_config"}{"ReactionINIFile"}   = "REACTION.INI";
	$sim->{"general_config"}{"SeedsIn"}           = ""; 
# $sim->{"main_config_dir"}.$sep."Input${sep}SEED.IN"; FIXME - user has to configure that.
	$sim->{"general_config"}{"SeedsOut"}          = "..${sep}Output${sep}SEED.OUT";
	$sim->{"general_config"}{"ReactionMatrixOut"} = "..${sep}Output${sep}R_MATRIX.OUT";
	$sim->{"general_config"}{"DumpOut"}           = "..${sep}Output${sep}DUMP.OUT";
	$sim->{"general_config"}{"VariablesOut"}      = "..${sep}Output${sep}VAR.OUT";
	$sim->{"general_config"}{"LogFile"}           = "..${sep}Output${sep}LOG.OUT";
	if ($sim->{"general_config"}{"UseSpatialExtensions"} == 1){
	    $sim->{"general_config"}{"ArrayINIFile"}      = "ARRAY.INI";
	    $sim->{"general_config"}{"ArrayOutPrefix"}    = "..${sep}Output${sep}";
	}

     	eval {
	  writeinifile::SaveFile($sim);	
	};
	if ($@) {message("error","error",$@)} else {$mw->bell;} 
    }
}

######################################################################################################
#              Gets the name of the file to save the initialisation parameters within
######################################################################################################

sub save_file_as{
    my ($sim,$previous) = @_;
    my $answer;
    my ($initdir);

    # Get the name of the directory where to store configurations, without its path 
    ($initdir = $DIRCONF) =~ s!/$!!;

    # The user has two solutions here:
    # If the main configuration directory exists, he selects the file Input/STCHSTC.INI
    # If this is a brand new simulation, he selects the name of the main configuration directory
    $answer = $mw->getSaveFile(-title       => "Save configuration file as",
			       -initialdir  => $initdir,
			       );    
    
    if ($answer){
	$answer=~ s/\/Input\/.*$//; # because we have to choose one file rather than a directory
	mkdir $answer,0777 unless (-e $answer and -d $answer);
	mkdir $answer.$sep."Input",0777 unless (-e $answer and -d $answer.$sep."Input");
	mkdir $answer.$sep."Output",0777 unless (-e $answer and -d $answer.$sep."Output");

	$sim->{"main_config_dir"} = $answer;
	$sim->{"general_config"}{"MessageINIFile"}    = $DIRLIB.$sep."MESSAGE.INI";
	$sim->{"general_config"}{"ComplexINIFile"}    = "COMPLEX.INI";
	$sim->{"general_config"}{"DynamicValues"}     = "DYNAMIC.INI";
	$sim->{"general_config"}{"ReactionINIFile"}   = "REACTION.INI";
	$sim->{"general_config"}{"ArrayINIFile"}      = "ARRAY.INI";
	$sim->{"general_config"}{"SeedsIn"}           = ""; 
# $sim->{"main_config_dir"}.$sep."Input${sep}SEED.IN"; FIXME - user has to configure that.
	$sim->{"general_config"}{"SeedsOut"}          = "..${sep}Output${sep}SEED.OUT";
	$sim->{"general_config"}{"ReactionMatrixOut"} = "..${sep}Output${sep}R_MATRIX.OUT";
	$sim->{"general_config"}{"DumpOut"}           = "..${sep}Output${sep}DUMP.OUT";
	$sim->{"general_config"}{"VariablesOut"}      = "..${sep}Output${sep}VAR.OUT";
	$sim->{"general_config"}{"ArrayOutPrefix"}    = "..${sep}Output${sep}";
	$sim->{"general_config"}{"LogFile"}           = "..${sep}Output${sep}LOG.OUT";
	eval {
	  writeinifile::SaveFile($sim);
	    chdir $sim->{"main_config_dir"}.$sep."Input" 
		or die "can't change current directory to $sim->{main_config_dir}${sep}Input: $!"; 
	};
	if ($@) {
	    message("error","error",$@);
	} else {
	    $mw->bell;
	    my $bingo = 0;
	    foreach my $record (@$previous){
		$bingo += 1 if $record eq $sim->{"main_config_dir"};
	    }
	    if (not $bingo){
		pop @$previous;
		unshift (@$previous,$sim->{"main_config_dir"});
	    }
	    my $i = 0;
	    foreach my $dir (@{$previous}){
		$history[$i]->configure(-label   => "$dir", 
					-command => sub{ Open_sim($dir,$sim,$previous) });	 
		$i++;
	    }
	    $Xsbml_m->configure(-state => 'normal');
	    $Xsbml2_m->configure(-state => 'normal');
	    $save_m->configure(-state => 'normal');
	}
    }
}

######################################################################################################
#Writes the simulation under the SBML format level 1 
# (see http://www.sbml.org)
######################################################################################################

sub write_sbml1{
    my ($sim) = @_;

    eval {
      writesbml::writesbml1($sim);	
    };
    if ($@) {message("error","error",$@)} else {$mw->bell;} 
} 

######################################################################################################
#Writes the simulation under the SBML format level 2
# (see http://www.sbml.org)
######################################################################################################

sub write_sbml2{
    my ($sim) = @_;

    eval {
      writesbml::writesbml2($sim);	
    };
    if ($@) {message("error","error",$@)} else {$mw->bell;} 
} 

######################################################################################################
# Reads the simulation under the SBML format level 1 
# (see http://www.sbml.org)
######################################################################################################

sub read_sbml{
    my ($sim) = @_;
    my $answer;

    # get the complete pathway of the configuration file.
    $answer = $mw->getOpenFile(-title      => "Load SBML1 model",
			       -initialdir => $DIRCONF);

    if ($answer){
	eval {
	     readsbml::readsbml($answer,$sim);
	};
	if ($@){ 
	    message("error","error",$@); 
	} else { 	
	    $Xsbml_m->configure(-state => 'normal');
	    $Xsbml2_m->configure(-state => 'normal');
	    $save_m->configure(-state => 'normal');
	}
    };
} 

######################################################################################################
#                              This function displays the help 
######################################################################################################

sub show_help{
    my $helpfile = $DIRHELP.$sep.$_[0];
    
    if(!Exists(my $help_W)){

	$help_W = $mw->Toplevel();
	$help_W->title("stochsim help");
	$help_W->Button(-text => "close",
			-command => sub{ $help_W->destroy })->pack;
	
	my $helpText = $help_W->Scrolled
	    ("ROText", 
	     -font => "-adobe-courier-medium-r-normal--0-0-0-0-m-0-iso8859-1"
	     )->pack(-expand => '1',
		     -fill   => 'both'
		     );
	eval {
	    open(FH,$helpfile) || die "unable to open $helpfile";
	    while (<FH>){
		$helpText->insert('end', $_);
	    }
	    close(FH);
	};
	message("error","error",$@) if $@;
    } else {
	$help_W->deiconify();
	$help_W-> raise();
    }
}

######################################################################################################
#                              This function displays the PDF manual 
######################################################################################################

sub show_pdf{
    my $pdffile = $DIRHELP.$sep.$_[0];
    my $answer;

    if (not defined $look{'pdfreader'} or not -X $look{'pdfreader'}){
	# get the complete pathway of the configuration file.
	$answer = $mw->getOpenFile(-title      => "Where is  your favourite PDF reader",
				   -initialdir => $DIRCONF);
	if ($answer){
	    $look{'pdfreader'} = $answer;
	}
    }
    # command.com uses '/' for options, and not for PATHS. 
    # Note the absence of '&' in MSWin32
    if ($^O eq "MSWin32"){
	$look{'pdfreader'} =~ tr/\//\\/;
	print "cannot launch the PDF reader\n" if (system("$look{'pdfreader'} $pdffile") != 0);
    } else {
	print "cannot launch the PDF reader\n" if (system("$look{'pdfreader'} $pdffile &") != 0);
    }
}

######################################################################################################
#                          Setup of the general parameters of the simulation
######################################################################################################
#

sub general_configuration{
    my ($sim)=@_; 
    
    if(!Exists(my $gen_conf_W)){
	
#-----------------------------------------------------------------------------------------------------
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------

	$gen_conf_W = $mw->Toplevel;
	$gen_conf_W->resizable(0,0);
	$gen_conf_W->geometry("+".$look{"xsecond"}."+".$look{"ysecond"}); 
	$gen_conf_W->title("General configuration");
	$gen_conf_W->grab();
	# users cannot kill the window via decorations
	$gen_conf_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$gen_conf_W->transient($gen_conf_W->Parent->toplevel);

#----------------------------------------- close Frame -----------------------------------------------

	my $close_Fr = $gen_conf_W->Frame->pack(-side=>'bottom');
	    
	$close_Fr->Button(-text             => "close",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{ $gen_conf_W->grabRelease();
						    $gen_conf_W->destroy }
			  )->pack(-side => 'right');

#---------------------------------------- parameters Frame -------------------------------------------

	my $par_Fr = $gen_conf_W->LabFrame(-borderwidth => 1, 
					   -label       => 'Simulation parameters',
					   -labelside   => 'acrosstop'
					   )->pack(-side   => 'top',
						   -expand => '1',
						   -fill   => 'both');	
	my $parlegend_Fr = $par_Fr->Frame()->pack(-side => 'left');
	my $parvalue_Fr = $par_Fr->Frame()->pack(-side => 'right');

#	$parlegend_Fr->Label(-text => 'Interval between display')->pack;
#	my $DiI = $parvalue_Fr->Entry(-textvariable => \$sim->{"general_config"}{"DisplayInterval"},
#				     -foreground   => $look{"entryforeground"},
#				     -background   => $look{"entrybackground"},
#				     )->pack;
# 	$balloon->attach($DiI,-msg => $messages{"DisplayInterval"});

#---------------------------
# StoreInterval
#---------------------------

	$parlegend_Fr->Label(-text => 'Interval between storage')->pack;
	my $SI = $parvalue_Fr->Entry(-textvariable => \$sim->{"general_config"}{"StoreInterval"},
				     -foreground   => $look{"entryforeground"},
				     -background   => $look{"entrybackground"},
				     )->pack;
	$balloon->attach($SI,-msg => $messages{"StoreInterval"});

#---------------------------
# DumpInterval
#---------------------------

	$parlegend_Fr->Label(-text => 'Interval between variable dumping')->pack;
	my $DuI = $parvalue_Fr->Entry(-textvariable => \$sim->{"general_config"}{"DumpInterval"},
				     -foreground   => $look{"entryforeground"},
				     -background   => $look{"entrybackground"},
				     )->pack;
	$balloon->attach($DuI,-msg => $messages{"DumpInterval"});

#---------------------------
# ArrayDumpInterval
#---------------------------

	$parlegend_Fr->Label(-text => 'Interval between array dumping')->pack;
	my $ADI = $parvalue_Fr->Entry(-textvariable => \$sim->{"array_config"}{"DumpInterval"},
				     -foreground   => $look{"entryforeground"},
				     -background   => $look{"entrybackground"},
				     )->pack;
	$balloon->attach($ADI,-msg => $messages{"ArrayDumpInterval"});

	
#---------------------------
# TimeIncrement
#---------------------------

	$parlegend_Fr->Label(-text => 'Duration of each iteration')->pack;
	my $TI = $parvalue_Fr->Entry(-textvariable => \$sim->{"general_config"}{"TimeIncrement"},
				     -foreground   => $look{"entryforeground"},
				     -background   => $look{"entrybackground"},
				     )->pack;

#---------------------------
# OptimiseTimeIncrement
#---------------------------

	if ($sim->{"general_config"}{"OptimiseTimeIncrement"}){
	    $TI->configure(-state => 'disabled',
			   -foreground   => $parvalue_Fr->cget(-foreground),
			   -background   => $parvalue_Fr->cget(-background));
	} else {
	    $TI->configure(-state => 'normal',
			   -foreground   => $look{"entryforeground"},
			   -background   => $look{"entrybackground"});
	}

	$balloon->attach($TI,-msg => $messages{"TimeIncrement"});
	
#---------------------------
# SimulationDuration
#---------------------------

	$parlegend_Fr->Label(-text => 'Duration of the simulation')->pack;
	my $SD = $parvalue_Fr->Entry(-textvariable => \$sim->{"general_config"}{"SimulationDuration"},
				     -foreground   => $look{"entryforeground"},
				     -background   => $look{"entrybackground"},
				     )->pack;
	$balloon->attach($SD,-msg => $messages{"SimulationDuration"});

#---------------------------
# ReactionVolume
#---------------------------

	$parlegend_Fr->Label(-text => 'Volume of the simulation')->pack;
	my $RV = $parvalue_Fr->Entry(-textvariable => \$sim->{"general_config"}{"ReactionVolume"},
				     -foreground   => $look{"entryforeground"},
				     -background   => $look{"entrybackground"},
				     )->pack;
	$balloon->attach($RV,-msg => $messages{"ReactionVolume"});

#---------------------------
# MaximumNumberComplexes
#---------------------------

	$parlegend_Fr->Label(-text => 'Maximum number of complexes')->pack;
	
	my $MNC = $parvalue_Fr->Entry
	    (-textvariable => \$sim->{"general_config"}{"MaximumNumberComplexes"},
	     -foreground   => $look{"entryforeground"},
	     -background   => $look{"entrybackground"},
	     )->pack;
	$balloon->attach($MNC,-msg => $messages{"MaximumNumberComplexes"});

#-------------------------------------------- options Frame ------------------------------------------

	my $options_Fr = $gen_conf_W->LabFrame(-borderwidth => 1, 
					       -label       => 'Simulation options',
					       -labelside   => 'acrosstop'
					       )->pack(-expand => '1',
						       -fill   => 'both');	

#---------------------------
# AbortOnResolutionErr
#---------------------------

	$options_Fr->Label(-text => 'Abort if a probability is too small?'
			   )->grid(-column => '0',
				   -row    => '1');
	my $abort_Fr = $options_Fr->Frame->grid(-column => '1',
						-row    => '1',
						-sticky => 'nsew');
	$abort_Fr->Radiobutton(-text     => 'yes', 
			       -value    => '1', 
			       -variable => \$sim->{"general_config"}{"AbortOnResolutionErr"},
			       )->pack(-side   => 'left',
				       -anchor => 'w');
	$abort_Fr->Radiobutton(-text     => 'no', 
			       -value    => '0', 
			       -variable => \$sim->{"general_config"}{"AbortOnResolutionErr"},
			       )->pack(-side => 'right');
	$balloon->attach($abort_Fr,-msg => $messages{"AbortOnResolutionErr"});

#---------------------------
# DisplayAllLevels
#---------------------------

    	$options_Fr->Label(-text => 'Display all variables?'  
			   )->grid(-column => '0', 
				   -row    => '2'); 

	my $displayall_Fr = $options_Fr->Frame->grid(-column => '1', 
						     -row    => '2',
						     -sticky => 'nsew'); 

	my $which_var = $displayall_Fr->Button
	    (-text             => 'Which variables?',
	     -state            => $sim->{"general_config"}{"DisplayAllLevels"} ? 'disabled' : 'normal', 
	     -command          => sub{ setup_variables($sim,$gen_conf_W) },
	     -activeforeground => $look{"activeforeground"},
	     -activebackground => $look{"activebackground"},		
	     )->pack(-side => 'bottom',
		     -fill => 'x');
	
    	$displayall_Fr->Radiobutton(-text     => 'yes', 
				    -value    => '1',
				    -command  => sub{ $which_var->configure(-state => 'disabled') },
				    -variable => \$sim->{"general_config"}{"DisplayAllLevels"},
				    )->pack(-side => 'left');
	$displayall_Fr->Radiobutton(-text     => 'no', 
				    -value    => '0',
				    -command  => sub{ $which_var->configure(-state => 'normal') }, 
				    -variable => \$sim->{"general_config"}{"DisplayAllLevels"},
				    )->pack(-side => 'right');
	$balloon->attach($displayall_Fr,-msg => $messages{DisplayAllLevels});

#---------------------------
# OptimiseTimeIncrement
#---------------------------

    	$options_Fr->Label(-text => 'Optimise time increment?'
			   )->grid(-column => '0',
				   -row    => '3');
	my $optitime_Fr = $options_Fr->Frame->grid(-column => '1',
						   -row    => '3',
						   -sticky => 'nsew');
	$balloon->attach($optitime_Fr,-msg => $messages{"OptimiseTimeIncrement"});
	
	$optitime_Fr->Radiobutton(-text     => 'yes', 
				  -value    => '1', 
				  -command  => sub{ $TI->configure(-state        => 'disabled',
								   -foreground   => $parvalue_Fr->cget(-foreground),
								   -background   => $parvalue_Fr->cget(-background)) 
						    },
				  -variable => \$sim->{"general_config"}{"OptimiseTimeIncrement"},
				  )->pack(-side => 'left');
	$optitime_Fr->Radiobutton(-text     => 'no', 
				  -value    => '0', 
				  -command  => sub{ $TI->configure(-state => 'normal',
								   -foreground   => $look{"entryforeground"},
								   -background   => $look{"entrybackground"}) 
						    }, 
				  -variable => \$sim->{"general_config"}{"OptimiseTimeIncrement"},
				  )->pack(-side => 'right');

#---------------------------
# CreateDumpFile
#---------------------------

    	$options_Fr->Label(-text => 'Create dump files?'
			   )->grid(-column => '0',
				   -row    => '4');

	my $dump_Fr = $options_Fr->Frame->grid(-column   => '1',
					       -row      => '4',
					       -sticky   => 'nsew');
	$balloon->attach($dump_Fr,-msg => $messages{"CreateDumpFile"});

	$dump_Fr->Radiobutton(-text     => 'yes', 
			      -value    => '1', 
			      -variable => \$sim->{"general_config"}{"CreateDumpFile"},
			      )->pack(-side => 'left');
	$dump_Fr->Radiobutton(-text     => 'no', 
			      -value    => '0', 
			      -variable => \$sim->{"general_config"}{"CreateDumpFile"},
			      )->pack(-side => 'right');

#---------------------------
# InitialLevelsFromDumpFile
#---------------------------

	$options_Fr->Label(-text => 'Initial levels from file?'
			   )->grid(-column => '0',
				   -row    => '5');

	my $init_Fr = $options_Fr->Frame->grid(-column   => '1',
					       -row      => '5',
					       -sticky   => 'nsew');

	$options_Fr->Label(-text => 'Which file contains the initial levels?'
			   )->grid(-column => '0',
				   -row    => '6');

	my $initFile_Fr = $options_Fr->Frame->grid(-column   => '1',
						   -row      => '6',
						   -sticky   => 'nsew');
	
	my $initFile_E = $initFile_Fr->Entry(-width        => 10, 
					     -foreground   => $sim->{"general_config"}{"InitialLevelsFromDumpFile"} ? $look{"entryforeground"} : $initFile_Fr->cget(-foreground),
					     -background   => $sim->{"general_config"}{"InitialLevelsFromDumpFile"} ? $look{"entrybackground"} : $initFile_Fr->cget(-background),
					     -state        => $sim->{"general_config"}{"InitialLevelsFromDumpFile"} ? 'normal' : 'disabled', 
					     -textvariable => \$sim->{"general_config"}{"InitialLevelsFile"},
					     )->pack(-side => 'left');
	
	my $initFile_Bt = $initFile_Fr->Button(-text             => 'browse',
					       -activeforeground => $look{"activeforeground"},
					       -activebackground => $look{"activebackground"},	
					       -state            => $sim->{"general_config"}{"InitialLevelsFromDumpFile"} ? 'normal' : 'disabled', 
					       -command          => sub{
						   (my $initdir = $DIRCONF) =~ s!/$!!;
						   $sim->{"general_config"}{"InitialLevelsFile"} = $mw->getOpenFile(-title      => "Choose a dumped file",
														    -initialdir => $initdir);
					       }
					       )->pack(-side => 'right');

	$balloon->attach($initFile_Fr,-msg => $messages{InitialLevelsFile});

	$options_Fr->Label(-text => 'Timepoint when to get the levels'
			   )->grid(-column => '0',
				   -row    => '7');
	
	my $initTime_Fr = $options_Fr->Frame->grid(-column   => '1',
						   -row      => '7',
						   -sticky   => 'nsew');

	my $initTime_Br = $initTime_Fr->BrowseEntry(-variable  => \$sim->{"general_config"}{"InitialLevelsFromTime"},
						    -state     => $sim->{"general_config"}{"InitialLevelsFromDumpFile"} ? 'readonly' : 'disabled',
						    -listwidth => '40',
						    -width     => '15',
						    )->pack(-fill => 'x');
	
	$initTime_Br->configure(-listcmd => sub{
	    my @timepoints;
	    eval {
		open (IF, "$sim->{general_config}{InitialLevelsFile}") 
		    or die "can't open the dumped file $sim->{general_config}{InitialLevelsFile}: $!";
		my $line;
		$line = <IF>;
		$line =~ /^\s*Time/i or die "Does not look like a dumped file to me!"; 
		while (defined($line = <IF>)){
		    my @split_line = split " ",$line;
		    push @timepoints, $split_line[0];
		}
		close (IF) 
		    or die "can't close the dumped file $sim->{general_config}{InitialLevelsFile}: $!";
	};
	    if ($@){
		message("error","error",$@);
	    } else {		
		$initTime_Br->delete(0,'end');
		$initTime_Br->insert('end', @timepoints);
	    }
	}
				);
	
	my $initTime_Lb = $initTime_Br->Subwidget('slistbox');
	$initTime_Lb->configure( -selectbackground => $look{"activeselectionbackground"},
				 -selectforeground => $look{"activeselectionforeground"},
		     );
	
	$balloon->attach($initTime_Fr,-msg => $messages{InitialLevelsFromTime});
	
# Radiobuttons come after the two preceding widgets, in order to know their existence.	

	$init_Fr->Radiobutton(-text     => 'yes', 
			      -value    => '1',
			      -command  => sub{ 
				  $initFile_E->configure(-state      => 'normal',
							 -foreground => $look{"entryforeground"},
							 -background => $look{"entrybackground"});
				  $initFile_Bt->configure(-state      => 'normal');
				  $initTime_Br->configure(-state      => 'normal');
			      },
			      -variable => \$sim->{"general_config"}{"InitialLevelsFromDumpFile"},
			      )->pack(-side => 'left');
	$init_Fr->Radiobutton(-text     => 'no', 
			      -value    => '0',
			      -command  => sub{ 
				  $initFile_E->configure(-state      => 'disabled',
							 -foreground => $initFile_Fr->cget(-foreground),
							 -background => $initFile_Fr->cget(-background));
				  $initTime_Br->configure(-state      => 'disabled');
				  $initFile_Bt->configure(-state      => 'disabled');
				  $sim->{"general_config"}{"InitialLevelsFile"} = undef;
				  $sim->{"general_config"}{"InitialLevelsFromTime"} = undef;
			      },
			      -variable => \$sim->{"general_config"}{"InitialLevelsFromDumpFile"},
			      )->pack(-side => 'right');
	$balloon->attach($init_Fr,-msg => $messages{InitialLevelsFromDumpFile});

#---------------------------
# TimeUnits
#---------------------------

#    	$options_Fr->Label(-text => 'Unit to use for time values ?'
#			   )->grid(-column => '0',
#				   -row    => '6' );
#	my $timeval_Fr = $options_Fr->Frame->grid(-column => '1',
#						  -row    => '6',
#						  -sticky => 'nsew');
#	$balloon->attach($timeval_Fr,-msg => $messages{"TimeUnits"});
#	
#	$timeval_Fr->Radiobutton(-text     => 'seconds', 
#				 -value    => '1', 
#				 -variable => \$sim->{"general_config"}{"TimeUnits"},
#				 )->pack(-side => 'left');
#	$timeval_Fr->Radiobutton(-text     => 'iterations', 
#				 -value    => '0', 
#				 -variable => \$sim->{"general_config"}{"TimeUnits"},
#				 )->pack(-side => 'right');


#---------------------------
# UseSpatialExtensions
#---------------------------

	$options_Fr->Label(-text => 'Use spatial extensions?'
			   )->grid(-column => '0',
				   -row    => '8');

	my $spatialext_Fr = $options_Fr->Frame->grid(-column => '1',
						     -row    => '8',
						     -sticky => 'nsew');
	$balloon->attach($spatialext_Fr,-msg => $messages{"UseSpatialExtensions"});
	
	$spatialext_Fr->Radiobutton(-text     => 'yes', 
				    -value    => '1', 
				    -variable => \$sim->{"general_config"}{"UseSpatialExtensions"},
				    )->pack(-side => 'left');
	$spatialext_Fr->Radiobutton(-text     => 'no', 
				    -value    => '0', 
				    -variable => \$sim->{"general_config"}{"UseSpatialExtensions"},
				    )->pack(-side => 'right');
	
#---------------------------
# RandomNumberGenerator
#---------------------------

    	$options_Fr->Label(-text => 'Type of random generator'
			   )->grid(-column => '0',
				   -row    => '9');

	my $RandType;
      RG:{
	  if ( $sim->{"general_config"}{"RandomNumberGenerator"} == 1 ){ 
	      $RandType = "Dummy"; last RG; }
	  if ( $sim->{"general_config"}{"RandomNumberGenerator"} == 2 ){ 
	      $RandType = "Internal"; last RG; }
	  if ( $sim->{"general_config"}{"RandomNumberGenerator"} == 3 ){ 
	      $RandType = "Shuffle"; last RG; }
	  if ( $sim->{"general_config"}{"RandomNumberGenerator"} == 4 ){ 
	      $RandType = "Bit_string"; last RG; }
	  if ( $sim->{"general_config"}{"RandomNumberGenerator"} == 5 ){ 
	      $RandType = "Quick"; last RG; }
	  $RandType = "Shuffle"; # Default value
      }
        my $randgen_m = $options_Fr->Optionmenu
	    (-options      => [["Dummy" => 1], 
			       ["Internal" => 2], 
			       ["Shuffle" => 3],
			       ["Bit_string" => 4],
			       ["Quick" => 5]],
	     -textvariable => \$RandType,
	     -command      => sub {
		 $sim->{"general_config"}{"RandomNumberGenerator"} = $_[0]
		 },
	     -justify      => 'right',
	     -activeforeground => $look{"activeforeground"},
	     -activebackground => $look{"activebackground"},				 
	     )->grid(-column => '1',
		     -row    => '9',
		     -sticky => 'nsew');
	
	$balloon->attach($randgen_m,-msg => $messages{"RandomNumberGenerator"});
	
    } else {
	$gen_conf_W-> deiconify();
	$gen_conf_W-> raise();
    }
}

######################################################################################################
#                This function allow the configuration of the variables to display 
# structure:
# ----------
# $set_var_W (Toplevel)
#     $close_Fr (Frame)    pack 'bottom'
#         button "accept"
#         button "cancel"
#     $current_Fr (Frame)  pack 'left',
#         $variable_Ls (Listbox)      pack 'left'   : list of the variables to display
#         $new_Bt (Button)            pack 'top'    : creates a new variable
#         $varname_E (Entry)          pack 'right'  : name of the variable currently selected
#     $comp_Fr (Frame)     pack 'right'
#         Label "selected complexes"  grid 1 1  
#         $selected_cplxs (Listbox)   grid 1 2      : complexes included in the variable currently selected
#         $setStates_Bt (Button)      grid 1 3      : setups the states represented by the variable
#         $avail_cplxs (Listbox)      grid 2 1      : all the available complexes
#         Label "available complexes" grid 2 1
#
######################################################################################################

sub setup_variables{
    my ($sim,$parent) = @_;
    my ($i,$dnd_token);
    my @variables;
    my ($var_idx,$cplx_idx);

# variable to display dealing with non multistates
# note that $variable{"Kind"} is undefined
    foreach my $var (@{ $sim->{"display_variables"} }){
	# FIXME: The structure of %variable is silly. cf the loop on ms below
	# variable = ( "Name"   => scalar,    the name of the variable
	#              "Types"  => [],        the complexes represented by the variable
	#              "Kind"   => scalar,    is 'multistate' if one multistate complex
	#              "States" => []         the state(s) of the multistate 
	my %variable;
	$variable{"Name"} = $var->{"Name"};
	for (0 .. $#{ $var->{"Types"} }){
	    $variable{"Types"}[$_] = $var->{"Types"}[$_];
	}
# It is possible that a regular variable to display contains only one multistate
# but in order to display all the possible states (i.e. only ? for all flags).
	if (scalar @{ $variable{"Types"} } == 1 
	    and $total_cplxs{$variable{"Types"}[0]}{'multistate'} == 1){
	    $variable{"Kind"}      = 'multistate';
	    $variable{"States"}[0] = ('?'x scalar @{ $sim->{"ms_complexes"}{$variable{"Types"}[0]}{"Flags"} }); 
	}
	push @variables,\%variable;
    }

# variable to display dealing with multistates complex
    foreach my $ms (keys %{ $sim->{"ms_complexes"} }){ 
	foreach my $dv (@{ $sim->{"ms_complexes"}{$ms}{"Display_variables"} }){ 
	    my %variable; 
	    # test the existence of the following key to distinguish 
	    # from a regular variable to display.  
	    $variable{"Kind"}     = 'multistate';
	    $variable{"Name"}     = $dv->{"Name"};
	    $variable{"Types"}[0] = $ms;
	    for (0 .. $#{ $dv->{"States"} }){
		$variable{"States"}[$_] = $dv->{"States"}[$_]; 
	    }
	    push @variables,\%variable; 
	} 
    }
    
    if(!Exists(my $set_var_W)){
	
#-----------------------------------------------------------------------------------------------------
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------
	
	$set_var_W = $mw->Toplevel();
	$set_var_W->grab();
	$set_var_W->resizable(0,0);
	$set_var_W->title("variables to display");
	$set_var_W->geometry("+".$look{"xthird"}."+".$look{"ythird"}); 
	# users cannot kill the window via decorations
	$set_var_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$set_var_W->transient($set_var_W->Parent->toplevel);
	
#------------------------------------------- close Frame ---------------------------------------------
	
	my $close_Fr = $set_var_W->Frame->pack(-side=>'bottom');
	
	$close_Fr->Button(-text             => "accept",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      
			    TESTCONTENT: {
				foreach my $var (@variables){
				    if (not defined $var->{"Types"} or scalar @{ $var->{"Types"} } == 0){
					message("error","error",$messages{"EmptyVariable"});
					last TESTCONTENT;
				    }
				}
				# Erase everything before to copy the current modifications
				$sim->{"display_variables"} = [];
				
				# Erase all the variable to display in multistates
				foreach my $ms (keys %{ $sim->{"ms_complexes"} }){
				    $sim->{"ms_complexes"}{$ms}{"Display_variables"} = [];
				}
				
				foreach my $var (@variables){
				  # This variable has to represent all the states of a multistate
				    if (defined $var->{"Kind"}
					and $var->{"Kind"} eq 'multistate'
					and scalar @{ $var->{"States"} } == 1
					and $var->{"States"}[0] =~ /^\?+$/){
					delete $var->{"Kind"};
					delete $var->{"States"};
				    }
				    
				    # This variable represents a simple complex
				    if (not defined $var->{"Kind"}){
					push(@{ $sim->{"display_variables"} },$var);
					
					# This variable represents several but not all states of a multistate
				    } elsif ($var->{"Kind"} eq 'multistate'){
					my %variable;
					$variable{"Name"} = $var->{"Name"};
					for (0 .. $#{ $var->{"States"} }){
					    $variable{"States"}[$_] = $var->{"States"}[$_];
					}
					push( @{ $sim->{"ms_complexes"}{$var->{"Types"}[0]}{"Display_variables"} },\%variable );
				    } else {
					# do nothing
				    }
				}
				$set_var_W->grabRelease();
				$parent->grab();
				$set_var_W->destroy;
			    }
			  }
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text             => "cancel",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{ $set_var_W->grabRelease();
						    $parent->grab();
						    $set_var_W->destroy }
			  )->pack(-side => 'left');
	
#-------------------------------- List of the current variable to display ----------------------------
	
	my $current_Fr = $set_var_W->LabFrame(-borderwidth => 1, 
					      -labelside   => 'acrosstop',
					      -label       => 'Current variables',
					      )->pack(-side   => 'left',
						      -expand => '1',
						      -fill   => 'both');
	
	my $new_Bt = $current_Fr->Button(-text             => 'new',
					 -activeforeground => $look{"activeforeground"},
					 -activebackground => $look{"activebackground"},
					 )->pack(-side => 'top',
						 -expand => '1',
						 -fill   => 'x');
	$balloon->attach($new_Bt, -msg => $messages{"NewVariable"});

	my $varname_E = $current_Fr->Entry(-state      => 'disabled',
					   -width      => 10,
					   -foreground => $look{"entryforeground"},
					   )->pack(-side => 'top',
						   -expand => '1',
						   -fill   => 'x');
	
	$balloon->attach($varname_E, -msg => $messages{"EnterVariableName"});
	my $variable_Ls = $current_Fr->Scrolled('Listbox',
						-background   => $look{"entrybackground"},
						-width        => '9',
						-height       => '8', 
						)->pack(-side => 'top',
							);	
	
	$balloon->attach($variable_Ls, -msg => $messages{"SelectRemoveVar"});
	
#-------------------------------------- Composition frame --------------------------------------------
# This frame prints the complexes currently part of the variable selected. We 
# can delete a chosen complex
	
	my $comp_Fr = $set_var_W->LabFrame(-borderwidth => 1, 
					   -labelside   => 'acrosstop',
					   -label       => 'Selected variable',
					   )->pack(-side   => 'right',
						   -expand => '1',
						   -fill   => 'both');
	
	$comp_Fr->Label(-text => "selected\ncomplexes"
			  )->grid(-column => '1',
				  -row    => '1');

	my $selected_cplxs = $comp_Fr->Scrolled('Listbox', 
						-background   => $look{"entrybackground"},
						-width        => '8',
						-height       => '8', 
						-scrollbars => "osoe"
						)->grid(-column => '1',
							-row    => '2');

	my $setStates_Bt = $comp_Fr->Button
	    (-text             => 'setup states',
	     -activeforeground => $look{"activeforeground"},
	     -activebackground => $look{"activebackground"},
	     -width            => '8', 
	     -state            => 'disabled',
	     -command          => sub{
		 if (defined $var_idx){
		     if (defined $variables[$var_idx]{"Kind"}
			 and $variables[$var_idx]{"Kind"} eq 'multistate'){
			 # Dear reader, try to find out what the following line means :-)
			 displ_states($variables[$var_idx],$sim->{"ms_complexes"}{$variables[$var_idx]{"Types"}[0]}{"Flags"},$set_var_W);
		     }
		 }
	     })->grid(-column => '1',
		      -row    => '3');

	$balloon->attach($setStates_Bt, -msg => $messages{"SetStatePresent"});

	$balloon->attach($selected_cplxs, -msg => $messages{"RemoveComp"});

	$comp_Fr->Label(-text => "available\ncomplexes"
			 )->grid(-column => '2',
				 -row    => '1');

	my $avail_cplxs  = $comp_Fr->Scrolled('Listbox', 
					       -width     => '8',
					      -height     => '8',
					      -scrollbars => "osoe"
					      )->grid(-column => '2',
						      -row    => '2');

	$balloon->attach($avail_cplxs, -msg => $messages{"DragComp"});

#-----------------------------------------------------------------------------------------------------
#                                    Functional code
#-----------------------------------------------------------------------------------------------------

# Define the source for drags.
# Drags are started while pressing the left mouse button and moving the
# mouse. Then the StartDrag callback is executed.
	$dnd_token = $avail_cplxs->DragDrop
	    (-event        => '<B1-Motion>',
	     -sitetypes    => ['Local'],
	     -startcommand => sub { StartDrag($dnd_token) },
	     );
# Define the target for drops.
	$selected_cplxs->DropSite
	    (-droptypes     => ['Local'],
	     -dropcommand   => sub{
		 if (defined $var_idx){
		     $selected_cplxs->insert(0, $dnd_token->cget(-text));
		     @{ $variables[$var_idx]{"Types"} } = $selected_cplxs->get('0','end');
		     if (scalar @{ $variables[$var_idx]{"Types"} } == 1 
			 and $total_cplxs{$variables[$var_idx]{"Types"}[0]}{'multistate'} == 1 ){
			 $variables[$var_idx]{"Kind"} = 'multistate';
			 $variables[$var_idx]{"States"}[0] = '?'x scalar @{ $sim->{"ms_complexes"}{$variables[$var_idx]{"Types"}[0]}{"Flags"} }; 
		     } else {
			 delete $variables[$var_idx]{"Kind"};
			 $setStates_Bt->configure(-state => 'disabled');
		     }
		 } else {
		     message("error","error",$messages{"NoVar"});
		 }
	     });
	
	$avail_cplxs->insert("end", keys %{ $sim->{"complexes"} });

	if (@variables){ $variable_Ls->insert('0', map{$_->{"Name"}}@variables);}

	$variable_Ls->bind('<Button-1>' => sub{ 
	    $var_idx = $variable_Ls->curselection;
	    if (defined $var_idx){
		$varname_E->configure(-state        => 'normal',
				      -textvariable => \$variables[$var_idx]{"Name"},
				      -background   => $look{"entrybackground"}
				      );
		$selected_cplxs->delete('0','end');
		$selected_cplxs->insert('0',@{ $variables[$var_idx]{"Types"} });
	    }
	});

	$variable_Ls->bind('<Control-Button-1>' => sub{ 
	    if (defined($variable_Ls->curselection)){
		splice (@variables,$variable_Ls->curselection,1);
		$variable_Ls->delete('active');
		$varname_E->configure(-state        => 'disabled',
				      -textvariable => "",
				      -background   => $current_Fr->cget(-background) ,
				      );
	    }
	});
		
	$new_Bt->configure(-command => sub{ 
	    $avail_cplxs->delete('0','end');
	    $avail_cplxs->insert("end", keys %{ $sim->{"complexes"} });
	    unshift(@variables,{"Name" => undef, "Types" => []});
	    $var_idx = 0;
	    $varname_E->configure(-textvariable => \$variables[$var_idx]{"Name"},
				  -state        => 'normal',
				  -background   => $look{"entrybackground"}
				  );
	    $selected_cplxs->delete('0','end');
	    $selected_cplxs->insert('0',@{$variables[$var_idx]{"Types"}});
	});

#	$varname_E->configure(-validate        => 'all',
#			      -validatecommand => sub{
#				  $variable_Ls->delete('0','end');
#				  $variable_Ls->insert('0', map{$_->{"Name"}}@variables);
#				  return 1;
#			      }
#			      );

############# Fuse
	$varname_E->bind('<Leave>' => sub{ 
	    $variable_Ls->delete('0','end');
	    $variable_Ls->insert('0', map{$_->{"Name"}}@variables);
	});

	$varname_E->bind('<Return>' => sub{ 
	    $variable_Ls->delete('0','end');
	    $variable_Ls->insert('0', map{$_->{"Name"}}@variables);
	});
	
	$varname_E->bind('<FocusOut>' => sub{ 
	    $variable_Ls->delete('0','end');
	    $variable_Ls->insert('0', map{$_->{"Name"}}@variables);
	});

	$selected_cplxs->bind('<Button-1>' => sub{
            if (defined $var_idx){
                if (defined $variables[$var_idx]{"Kind"}
                    and $variables[$var_idx]{"Kind"} eq 'multistate'
		    and scalar @{ $variables[$var_idx]{"Types"} } == 1 ){
		    $setStates_Bt->configure(-state => 'normal');
                }
            }
        });
#############

	$selected_cplxs->bind('<Control-Button-1>' => sub{ 
	    if (defined($selected_cplxs->curselection)){
		$selected_cplxs->delete('active');
		@{ $variables[$var_idx]{"Types"} } = $selected_cplxs->get('0','end');
		if (scalar @{ $variables[$var_idx]{"Types"} } == 1 
		    and $total_cplxs{$variables[$var_idx]{"Types"}[0]}{'multistate'} ){
		    $variables[$var_idx]{"Kind"} = 'multistate';
		    $setStates_Bt->configure(-state => 'normal');
		    @{ $variables[$var_idx]{"States"} } = ('?'x scalar @{ $sim->{"ms_complexes"}{$variables[$var_idx]{"Types"}[0]}{"Flags"} }); 
		}
	    }
	});	
	
    } else {
	$set_var_W->deiconify();
	$set_var_W-> raise();
    }
}

#
# ...
#

sub StartDrag {
    my($token) = @_;
    my $w = $token->parent; # $w is the source listbox
    my $e = $w->XEvent;
    my $idx = $w->nearest($e->y); # get the listbox entry under cursor
    if (defined $idx) {
	# Configure the dnd token to show the listbox entry
	$token->configure(-text => $w->get($idx));
	# Show the token
	my($X, $Y) = ($e->X, $e->Y);
	$token->MoveToplevelWindow($X, $Y);
	$token->raise;
	$token->deiconify;
	$token->FindSite($X, $Y, $e);
    }
}

######################################################################################################
#         Set up the states of a multistate complex to be represented by a variable to display
######################################################################################################

sub displ_states{
    my ($variable,$flags,$parent) = @_;
    my $state_idx;
    my @curr_flags;

    if(!Exists(my $disp_st_W)){

#-----------------------------------------------------------------------------------------------------
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------

	$disp_st_W = $mw->Toplevel();
	$disp_st_W->grab();
	$parent->grabRelease();
	$disp_st_W->resizable(0,0);
	$disp_st_W->title("variable $variable->{Name}");
	$disp_st_W->geometry("+".$look{"xfourth"}."+".$look{"yfourth"}); 
	# users cannot kill the window via decorations
	$disp_st_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$disp_st_W->transient($disp_st_W->Parent->toplevel);


#------------------------------------------- close Frame ---------------------------------------------

	my $close_Fr = $disp_st_W->Frame->pack(-side=>'bottom');
	
	$close_Fr->Button(-text             => "quit",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      $disp_st_W->grabRelease();
			      $parent->grab();
			      $disp_st_W->destroy;}
			     )->pack(-side => 'left');
	
#----------------------------------------- All states Frame ------------------------------------------

	my $states_Fr = $disp_st_W->LabFrame(-borderwidth => 1, 
					     -label       => 'States represented',
					     -labelside   => 'acrosstop'
					     )->pack(-side   => 'top',
						     -expand => '1',
						     -fill   => 'both');

	my $states_Ls = $states_Fr->Scrolled('Listbox',
					     -background   => $look{"entrybackground"},
					     -height => '4',
#					     -width => '15',
					     )->pack(-side   => 'top',
						     -expand => '1',
						     -fill   => 'x');	

	$balloon->attach($states_Ls, -msg => $messages{"RemoveState"});

	my $new_state_Bt = $states_Fr->Button(-text             => 'new',
					      -activeforeground => $look{"activeforeground"},
					      -activebackground => $look{"activebackground"},
					      )->pack(-side   => 'top',
						      -fill   => 'x',
						      -expand => '1');

	$balloon->attach($new_state_Bt, -msg => $messages{"NewState"});

#------------------------------------------ Set state Frame ------------------------------------------

	my $flags_Fr = $states_Fr->Frame->pack(-side => 'bottom');

	$balloon->attach($flags_Fr, -msg => $messages{"SetFlags"});

	$flags_Fr->Label(-text => '0')->grid(-row    => '0',
					     -column => '1');
	$flags_Fr->Label(-text => '?')->grid(-row    => '0',
					     -column => '2');
	$flags_Fr->Label(-text => '1')->grid(-row    => '0',
					     -column => '3');

	my (@rbt0,@rbtQ,@rbt1);
	for my $idx (0..$#$flags){
	    $flags_Fr->Label(-text =>$flags->[$idx]
			     # because first flag is zero and first row is 1 (cause the 0 ? 1)
			     )->grid(-row    => $idx+1, 
				     -column => '0');
	    $rbt0[$idx] = $flags_Fr->Radiobutton(-value    => '0',
						 -variable => \$curr_flags[$idx],
						 -state    => 'disabled',
						 )->grid(-row    => $idx+1,
					   -column => '1');
	    $rbtQ[$idx] = $flags_Fr->Radiobutton(-value    => '?',
						 -variable => \$curr_flags[$idx],
						 -state    => 'disabled',
						 )->grid(-row    => $idx+1,
							 -column => '2');
	    $rbt1[$idx] = $flags_Fr->Radiobutton(-value    => '1',
						 -variable => \$curr_flags[$idx],
						 -state    => 'disabled',
						 )->grid(-row    => $idx+1,
							 -column => '3');
	}
	
#-----------------------------------------------------------------------------------------------------
#                                       Functional code 
#-----------------------------------------------------------------------------------------------------

	$states_Ls->insert('end', @{ $variable->{"States"} });     
	
	$states_Ls->bind('<Button-1>' => sub{ 
	    $state_idx = $states_Ls->curselection; 
	    if (defined $state_idx){
		@curr_flags = split "", $variable->{"States"}[$state_idx];
		for (0..$#$flags){
		    $rbt0[$_]->configure(-variable => \$curr_flags[$_],
					 -state    => 'normal');
		    $rbtQ[$_]->configure(-variable => \$curr_flags[$_],
					 -state    => 'normal');
		    $rbt1[$_]->configure(-variable => \$curr_flags[$_],
					 -state    => 'normal');
		}
	    }
	});
	
	$states_Ls->bind('<Control-Button-1>' => sub{ 
	    # IMPORTANT: test $states_Ls->curselection and NOT $state_idx
	    # Because $state_idx remains defined after usage. Here we require
	    # a new selection before deletion. 
	    if (defined($states_Ls->curselection)){
		splice (@{ $variable->{"States"} },$state_idx,1);
		$states_Ls->delete('active');
	    }
	});

	$new_state_Bt->configure(-command => sub{ 
	    push(@{ $variable->{"States"} },"");
	    $state_idx = $#{ $variable->{"States"} };
	    for (0..$#$flags){
		$curr_flags[$_] = '?';
		$rbt0[$_]->configure(-variable => \$curr_flags[$_],
				     -state    => 'normal');
		$rbtQ[$_]->configure(-variable => \$curr_flags[$_],
				     -state    => 'normal');
		$rbt1[$_]->configure(-variable => \$curr_flags[$_],
				     -state    => 'normal');
	    }
	});
	
	for my $idx (0..$#$flags){
	    $rbt0[$idx]->configure(-command  => sub{
		$variable->{"States"}[$state_idx] = join "",@curr_flags;
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$variable->{"States"}[$state_idx]);
	    });
	    $rbtQ[$idx]->configure(-command  => sub{
		$variable->{"States"}[$state_idx] = join "",@curr_flags;
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$variable->{"States"}[$state_idx]);
	    });
	    $rbt1[$idx]->configure(-command  => sub{
		$variable->{"States"}[$state_idx] = join "",@curr_flags;
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$variable->{"States"}[$state_idx]);
	    });
	}

    } else {
	$disp_st_W->deiconify();
	$disp_st_W->raise();
    }
}

######################################################################################################
# Open a Window where the user can create a complex used in the simulation
#
# Structure:
# ----------
# $print_cplx_W (Toplevel)
#     $close_Fr                      pack(-side => 'bottom')
#         Button "accept"                pack(-side => 'left')
#         Button "cancel"                pack(-side => 'left')
#         Button "delete"                pack(-side => 'left')
#     $descr_Fr  "Description"       pack(-side => 'bottom')
#         Entry "Description"            pack(-side   => 'bottom')
#     $quant_Fr "Quantity"           pack(-side => 'left')
#         Entry "InitLevel"              pack(-side   => 'top')
#         $const (CheckBt)               pack(-side   => 'top')
#     $comp_Fr "Complex composition" pack(-side=> 'right')
#         Label "components included"    grid 1 1  
#         $selcpnts_Ls (Listbox)         grid 1 2      : components included in the complex
#         $availcpnts_Ls (Listbox)       grid 2 1      : all the available complexes
#         Label "available components"   grid 2 1
#
######################################################################################################

sub print_complex{
    my ($sim,$symbol) = @_; # the symbol choosen in the 'complexes' BrowseEntry. $symbol can be undef
    my %complex;            # this complex remains internal until the changes are accepted
    my($dnd_token,$cpnt_idx);

    %complex = ("Description" => undef,
		"Components"  => [],
		"InitLevel"   => undef,
		"Constant"    => 0);
    
    if (defined $symbol){
	# Note that if we enter here, $symbol is obligatory one of the existing complexes
	# Indeed, the user can only choice between them
	$complex{"InitLevel"}       = $sim->{"complexes"}{$symbol}{"InitLevel"};
	$complex{"Constant"}        = $sim->{"complexes"}{$symbol}{"Constant"};
	$complex{"Description"}     = $sim->{"complexes"}{$symbol}{"Description"};
	for (0 .. $#{ $sim->{"complexes"}{$symbol}{"Components"} }){
	    $complex{"Components"}[$_] = $sim->{"complexes"}{$symbol}{"Components"}[$_];
	}
    }
    
    if(!Exists(my $print_cplx_W)){
	
#-----------------------------------------------------------------------------------------------------
#                                           Main Widget 
#-----------------------------------------------------------------------------------------------------

	$print_cplx_W = $mw->Toplevel();
	$print_cplx_W->resizable(0,0);
	$print_cplx_W->geometry("+".$look{"xsecond"}."+".$look{"ysecond"}); 
	$print_cplx_W->title(defined($symbol) ? "Complex $symbol" : "Complex");
	$print_cplx_W->grab();
	# users cannot kill the window via decorations
	$print_cplx_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$print_cplx_W->transient($print_cplx_W->Parent->toplevel);

#------------------------------------------- close Frame ---------------------------------------------
	my $close_Fr = $print_cplx_W->Frame->pack(-side=>'bottom');
	
	$close_Fr->Button(-text             => "accept",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      my $newSymbol = join "",@{ $complex{"Components"} };
			      if ($symbol ne $newSymbol){
				  delete $sim->{"complexes"}{$symbol};
				  delete $total_cplxs{$symbol};
				  $symbol = $newSymbol;
			      }
			      $sim->{"complexes"}{$symbol}{"InitLevel"}   = $complex{"InitLevel"};
			      $sim->{"complexes"}{$symbol}{"Constant"}    = $complex{"Constant"};
			      $sim->{"complexes"}{$symbol}{"Description"} = $complex{"Description"};
			      $sim->{"complexes"}{$symbol}{"Components"}  = [ @{ $complex{"Components"} }];
			      $total_cplxs{$symbol}{'simple'} = 1;
			      $print_cplx_W->grabRelease();
			      $print_cplx_W ->destroy;}
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text             => "cancel",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      $print_cplx_W->grabRelease();
			      $print_cplx_W->destroy}
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text             => "delete",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command => sub{
			      my $used_complex = 0;
			      foreach my $re (@{ $sim->{"reactions"} }){
				  foreach (@{ $re->{"Substrates"} }){ 
				      if ($_ eq $symbol){$used_complex++};
				  }
				  foreach (@{ $re->{"Products"} }){ 
				      if ($_ eq $symbol){$used_complex++};
				  }
			      }
			      if ($total_cplxs{$symbol}{'multistate'} == 1
				  or $total_cplxs{$symbol}{'neighboursensitive'} == 1){
				  $used_complex++;
			      }
			      if ($used_complex){
				  message("error","error",$messages{"UsedComplex"});
			      } else {
				  delete $sim->{"complexes"}{$symbol};
				  delete $total_cplxs{$symbol};
				  $print_cplx_W->grabRelease();
				  $print_cplx_W->destroy;
			      }
			  }
			  )->pack(-side => 'left');
	
	$balloon->attach($close_Fr, -msg => $messages{"CplxClose"});
#--------------------------------------- description Frame -------------------------------------------
# Note that this description would be useful for the GUI (only the GUI since it
# will not be read by stochsim itself. 
     
	my $descr_Fr = $print_cplx_W->LabFrame(-borderwidth => 1, 
					       -label       => 'Description',
					       -labelside   => 'acrosstop'
					       )->pack(-side   => 'bottom',
						       -fill   => 'both',
						       -expand => '1');
	
	$descr_Fr->Entry(-textvariable => \$complex{"Description"},
			 -background   => $look{"entrybackground"},
			 -foreground   => $look{"entryforeground"}
			 )->pack(-side   => 'bottom',
				 -fill   => 'both',
				 -expand => '1');

	$balloon->attach($descr_Fr, -msg => $messages{"CplxDesc"});
	
#---------------------------------------- Quantity Frame ---------------------------------------------
# This frame prints (and reads) the quantity of the complex in the simulation
	
	my $quant_Fr = $print_cplx_W->LabFrame(-borderwidth => 1, 
					       -label       => 'Quantity',
					       -labelside   => 'acrosstop'
					       )->pack(-side => 'left',
						       -fill   => 'both',
						       -expand => '1');

	$quant_Fr->Label(-text => "(# molecules)"
			 )->pack(-side   => 'top',
				 -anchor => 'center');

	$quant_Fr->Entry(-width        => '10',
			 -textvariable => \$complex{"InitLevel"},
			 -background   => $look{"entrybackground"},
			 -foreground   => $look{"entryforeground"}
			 )->pack(-side   => 'top',
				 -anchor => 'center');
	
	$quant_Fr->Checkbutton(-text     => 'Constant?',
                               -variable => \$complex{"Constant"},
                               -onvalue  => 1,
                               -offvalue => 0,
			       )->pack(-side   => 'top',
				       -anchor => 'center');

	$balloon->attach($quant_Fr, -msg => $messages{"CplxQuant"});

#--------------------------------------- Composition frame -------------------------------------------
# This frame prints the components currently part of the complex. We can delete
# a selected component

	my $comp_Fr = $print_cplx_W->LabFrame(-borderwidth => 1, 
					      -labelside   => 'acrosstop',
					      -label       => 'Complex composition',
					      )->pack(-side   => 'right',
						      -expand => '1',
						      -fill   => 'both');
	
	$comp_Fr->Label(-text => "components\nincluded"
			)->grid(-column => '1',
				-row    => '1');

	my $selcpnts_Ls = $comp_Fr->Scrolled('Listbox', 
					     -background   => $look{"entrybackground"},
					     -width        => '8',
					     -height       => '8', 
					     -scrollbars   => "osoe"
					     )->grid(-column => '1',
						     -row    => '2');
	
	$balloon->attach($selcpnts_Ls, -msg => $messages{"RemoveCpnt"});

	$comp_Fr->Label(-text => "available\ncomponents"
			)->grid(-column => '2',
				-row    => '1');
	
	my $availcpnts_Ls  = $comp_Fr->Scrolled('Listbox', 
					      -width      => '8',
					      -height     => '8',
					      -scrollbars => "osoe"
					      )->grid(-column => '2',
						      -row    => '2');

	$balloon->attach($availcpnts_Ls, -msg => $messages{"DragCpnt"});

#-----------------------------------------------------------------------------------------------------
#                                    Functional code
#-----------------------------------------------------------------------------------------------------

	$availcpnts_Ls->insert("end", sort keys %{ $sim->{"components"} });
	$selcpnts_Ls->insert("end", @{ $complex{"Components"} });

# Define the source for drags.
# Drags are started while pressing the left mouse button and moving the
# mouse. Then the StartDrag callback is executed.
	$dnd_token = $availcpnts_Ls->DragDrop
	    (-event        => '<B1-Motion>',
	     -sitetypes    => ['Local'],
	     -startcommand => sub { StartDrag($dnd_token) },
	     );
# Define the target for drops.
	$selcpnts_Ls->DropSite
	    (-droptypes     => ['Local'],
	     -dropcommand   => sub{
		 $selcpnts_Ls->insert("end", $dnd_token->cget(-text));
		 $complex{"Components"} = [ $selcpnts_Ls->get('0','end') ];
	     });

#----------------- selection of a component

	$selcpnts_Ls->bind('<Button-1>' => sub{ 
	    $cpnt_idx = $selcpnts_Ls->curselection;
	});

#--------------------- Removal of a component

	$selcpnts_Ls->bind('<Control-Button-1>' => sub{ 
	    if (defined($cpnt_idx)){
		$selcpnts_Ls->delete('active');
		$complex{"Components"} = [ $selcpnts_Ls->get('0','end') ];
	    }
	});	

    } else {
	$print_cplx_W->deiconify();
	$print_cplx_W->raise();
    }
}

######################################################################################################
# Opens a Window where the user can create basic components of the simulation
######################################################################################################

sub print_component{
    my ($sim,$symbol) = @_; 
# a ref to the simulation and the symbol choosen in the 'components' BrowseEntry. $symbol can be undef
    
    my %component = ("Name"        => undef,
		     "Description" => undef);
    
    if (defined $symbol){
	# Note that if we enter here, $symbol is obligatory one of the existing components
	# Indeed, the user can only choice between them
	%component = %{ $sim->{"components"}{$symbol} };
    } 
    
    if(!Exists(my $print_cpnt_W)){

#-----------------------------------------------------------------------------------------------------
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------

	$print_cpnt_W = $mw->Toplevel();
	$print_cpnt_W->resizable(0,0);
	$print_cpnt_W->geometry("+".$look{"xsecond"}."+".$look{"ysecond"}); 
	$print_cpnt_W->title(defined($symbol) ? "Component $symbol" : "Component");
	$print_cpnt_W->grab();
	# users cannot kill the window via decorations
	$print_cpnt_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$print_cpnt_W->transient($print_cpnt_W->Parent->toplevel);

#---------------------------------------- close Frame ------------------------------------------------
	my $close_Fr = $print_cpnt_W->Frame->pack(-side=>'bottom');
	
	$close_Fr->Button(-text             => "accept",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			    TOTO:{
				foreach (keys %{ $sim->{"components"} }){
				    next if $_ eq $symbol;
				    if (/$symbol/ or $symbol =~ /$_/){
					message("error","error",$messages{"UsedSymbol"});
					last TOTO;
				    }
				}
				$sim->{"components"}{$symbol} = \%component;
				$print_cpnt_W->grabRelease();
				$print_cpnt_W->destroy;
			    }
			  }
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text => "cancel",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command => sub{
			      $print_cpnt_W->grabRelease();
			      $print_cpnt_W->destroy}
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text => "delete",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command => sub{
			      my $used_component = 0;
			      foreach my $complex (keys %{ $sim->{"complexes"} }){
				  foreach my $component (@{ $sim->{"complexes"}{$complex}{"Components"} }){
				      if ($symbol eq $component){$used_component++;}
				  }
			      }
			      if ($used_component){
				  message("error","error",$messages{"UsedComponent"});
			      }else{
				  delete $sim->{"components"}{$symbol};
				  $print_cpnt_W->grabRelease();
				  $print_cpnt_W->destroy;
			      }
			  }
			  )->pack(-side => 'left');
	$balloon->attach($close_Fr, -msg => $messages{"CompClose"});
	
#--------------------------------------- description Frame -------------------------------------------
	my $descr_Fr = $print_cpnt_W->Frame->pack(-side => 'bottom');

	$descr_Fr->Label(-text => "description"
				  )->pack(-side => 'top');
	$descr_Fr->Entry(-width        => '42',
			 -textvariable => \$component{"Description"},
			 -background   => $look{"entrybackground"},
			 -foreground   => $look{"entryforeground"}
			 )->pack();
	$balloon->attach($descr_Fr, -msg => $messages{"CompDesc"});

#---------------------------------------- Symbol Frame -----------------------------------------------
	my $symbol_Fr = $print_cpnt_W->Frame->pack(-side => 'left');

	$symbol_Fr->Label(-text => "symbol"
			     )->pack(-side => 'top');
	$symbol_Fr->Entry(-width        => '8',
			  -textvariable => \$symbol,
			  -background   => $look{"entrybackground"},
			  -foreground   => $look{"entryforeground"}
			  )->pack(-side => 'bottom');
	$balloon->attach($symbol_Fr, -msg => $messages{"CompSymb"});

#---------------------------------------- name Frame -------------------------------------------------
	my $name_Fr = $print_cpnt_W->Frame->pack(-side => 'right');

	$name_Fr->Label(-text => "name"
			)->pack(-side => 'top');
	$name_Fr->Entry(-width        => '32',
			-textvariable => \$component{"Name"},
			-background   => $look{"entrybackground"},
			-foreground   => $look{"entryforeground"}
			)->pack();
	$balloon->attach($name_Fr, -msg => $messages{"CompName"});
	
    } else {
	$print_cpnt_W->deiconify();
	$print_cpnt_W->raise();
    }
}

######################################################################################################
#                 Open a Window where the user can create multi-state complexes 
#
# Structure:
# ----------
# $print_ms_W                      Toplevel
#     $close_Fr           -side =>'bottom'
#         Button "accept"     -side => 'left'
#         Button "cancel"     -side => 'left'
#         Button "delete"     -side => 'left'
#     $desc_Fr (LabFrame) -side => 'bottom'
#         Entry "description" -side   => 'bottom'
#     $base_Fr            -side => 'top'
#         $avail_cplx_Br      -side => 'top'
#     $flags_Fr        -side => 'top'
#         $flag_Ls         -side => 'left'
#         $new_flag_Bt     -side => 'top'
#         Entry $flag      -side => 'top'
#         $ControlledBy_Fr -side => 'bottom'
#             $React_Bt        -side => 'top'
#             $RapEq_Bt        -side => 'top' 
#             $NSREq_Bt        -side => 'top'
#         $SetRE_Bt        -side => 'bottom'
#     $init_Fr         -side => 'top'
#         Button $init_val -side => 'top'        
#
######################################################################################################

sub print_multistate{
    my ($sim,$symbol) = @_; 
    my %ms;                 # this complex remains internal until the changes are accepted
    my $flagindex;
    my $currentFlag;
    my %RapEq;      # Keys are flags, values are:
                    #              0 if driven by a reaction, 
                    #              1 by MS rapid equilibrium, 
                    #              2 by a NS rapid equilibrium 
    my $oldcontrol; # record the type of control for the current flag
    my $oldsymbol;
    my @flag_history; # record all the changes (deletion or creation) of flags. 
                      # a deletion is recorded by its index, a creation by '+'

    %ms = ("File"               => undef,
	   "Description"        => undef,
	   "Flags"              => [],
	   "InitLevel"          => {},
	   "Rapid_equilibriums" => [],
	   "Reactions"          => [],
	   );

    if (not defined $symbol){
	$symbol = "";
	$oldsymbol = $symbol;
    } else {
	$oldsymbol = $symbol; # save the symbol to delete the ms_complex if changed
	# Note that if we enter here, $symbol is obligatory one of the existing $ms_complex
	# Indeed, the user can only choice between them
        # Note: It is useless to copy the $sim->{"ms_complexes"}{$symbol}{"File"}, since
        # it will be set-up automatically

	$ms{"Description"} = $sim->{"ms_complexes"}{$symbol}{"Description"};
	for (0 .. $#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
	    $ms{"Flags"}[$_] = $sim->{"ms_complexes"}{$symbol}{"Flags"}[$_];
	}
	foreach my $fl (@{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
	    $RapEq{$fl} = 0; # All flags are initially considered controlled by reactions
	}
	if ($sim->{"ms_complexes"}{$symbol}{"InitLevel"}){
	    $ms{"InitLevel"} = { %{ $sim->{"ms_complexes"}{$symbol}{"InitLevel"} } };
	}
	for (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Rapid_equilibriums"} }){
	    $ms{"Rapid_equilibriums"}[$_] = { %{ $sim->{"ms_complexes"}{$symbol}{"Rapid_equilibriums"}[$_] } };
	    # This flag is controlled by a rapid equilibrium and not a reaction
	    $RapEq{$ms{"Rapid_equilibriums"}[$_]{"Flag"}} = 1; 
	}
	if (exists $sim->{"ns_complexes"}{$symbol}){
	    for (0..$#{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"} }){
		# This flag is controlled by a neighbour-sensitive rapid equilibrium
		$RapEq{$sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"}[$_]{"Flag"}} = 2;
	    }
	}
    } 

    if(!Exists(my $print_ms_W)){
	
#-----------------------------------------------------------------------------------------------------
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------

	$print_ms_W = $mw->Toplevel();
	$print_ms_W->resizable(0,0);
	$print_ms_W->geometry("+".$look{"xsecond"}."+".$look{"ysecond"}); 
	$print_ms_W->title(defined($symbol) ? "MS-complex $symbol" : "MS-complex");
	$print_ms_W->grab();
	# users cannot kill the window via decorations
	$print_ms_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$print_ms_W->transient($print_ms_W->Parent->toplevel);

#------------------------------------------- close Frame ---------------------------------------------
	my $close_Fr = $print_ms_W->Frame->pack(-side=>'bottom');
	
	$close_Fr->Button(-text             => "accept",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      if (defined $symbol){
				  if ($sim->{"complexes"}{$symbol}{"Constant"}){
				      message("error","error",$messages{"NoConstantMS"}) if ($sim->{"complexes"}{$symbol}{"Constant"});
				  } else {
                                      # Have the flags been modified?
				      my $answer = undef;
				      my $changed = 0;
				      for (0..$#{ $ms{"Flags"} }){
					  $changed++ if $ms{"Flags"}[$_] ne $sim->{"ms_complexes"}{$symbol}{"Flags"}[$_];
				      }
				      if (not $changed){
					  $answer = "Ok";
				      } else {
					  $answer = $print_ms_W->messageBox
					      (-title   => "CAUTION",
					       -icon    => 'warning',
					       -message => $messages{"ModifiedFlags"},
					       -type    => 'OKCancel',
                                          # No default under Unix (would be 'Cancel' and not 'cancel'). However
                                          # better than to omit the option, which would cause Ok to be the default
					       -default => 'cancel',   
					       );
				      }
				      # Let's go. Removal of the flag, and all relative stuff.
				      if ($answer eq "Ok" or $answer eq "ok"){ # Under Windows messageBox returns lowercase strings
					  $sim->{"ms_complexes"}{$symbol}{"Description"} = $ms{"Description"};

					  # Updating the flags
					  @{ $sim->{"ms_complexes"}{$symbol}{"Flags"} } = @{ $ms{"Flags"} };
					  
					  # Updating the InitLevel of each state
					  undef %{ $sim->{"ms_complexes"}{$symbol}{"InitLevel"} };
					  $sim->{"ms_complexes"}{$symbol}{"InitLevel"} = { %{ $ms{"InitLevel"} } };
					  
					  # Updating the multistate rapid equilibriums
					  for (0..$#{ $ms{"Rapid_equilibriums"} }){
					      $sim->{"ms_complexes"}{$symbol}{"Rapid_equilibriums"}[$_] = { %{ $ms{"Rapid_equilibriums"}[$_] } };
					  }
					  
					  # Handle the change of basic complex (who would do that?)
					  $total_cplxs{$symbol}{'multistate'} = 1;
					  delete $sim->{"ms_complexes"}{$oldsymbol} if (defined($oldsymbol) and ($oldsymbol ne $symbol));
					  my $sb_idx = 0;
					  foreach  my $ms_symb (sort keys %{ $sim->{"ms_complexes"} }){
					      $sim->{"ms_complexes"}{$ms_symb}{"File"} = "MS_".($sb_idx+1).".INI";
					      $sb_idx++;
					  }
					  
					  if (exists $sim->{"ns_complexes"}{$symbol}){
					      # Removing a NS rapid equilibrium controlling a disappeared flag (if any)
					      my $re_idx = 0;
					      my $bingo  = 0;
					      foreach my $re (@{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"} }){
						  foreach my $fl (@{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
						      if ($re->{"Flag"} eq $fl){ $bingo++;last};
						      $re_idx++;
						  }
						  if (not $bingo){
						      splice(@{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"} },$re_idx,1);
						      $re_idx--; # because I spliced
						  }
					      }
					  }
					  
					  # modification of the various bitstrings following the history of changes
					  foreach my $event (@flag_history){
					      if ($event eq '+'){ 
						  # Treatment of reactions bitstring. No need to treat the reaction effect.
						  # If the flag has been supressed, there is no reactions modifying it.
						  foreach my $reac (@{ $sim->{"ms_complexes"}{$symbol}{"Reactions"} }){
						      foreach my $string (keys %$reac){
							  if ($string =~ /^[01\?]+$/){
							      my $oldstring = $string;
							      $string =~ s/(.*)/$1\?/;
							      $reac->{$string} = $reac->{$oldstring};
							      delete $reac->{$oldstring};
							  }
						      }
						  }
						  
						  # Treatment of the variable providing the level of various states
						  foreach my $var (@{ $sim->{"ms_complexes"}{$symbol}{"Display_variables"} }){
						      foreach my $string (@{ $var->{"States"} }){
							  $string =~ s/(.*)/$1\?/;
						      }
						  }
						  
						  # Treatment of NS rapid equilibrium bitstrings.
						  if (exists $sim->{"ns_complexes"}{$symbol}){
						      foreach my $re (@{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"}}){
							  foreach my $key (keys %$re){
							      if ($key eq "CoupledStates"){
								  foreach my $string (@{ $re->{$key} }){
								      $string =~ s/(.*)/$1\?/;
								  }
							      } elsif ($key =~ /^[01\?]+$/){
								  my $oldstring = $key;
								  $key =~ s/(.*)/$1\?/;
								  $re->{$key} = $re->{$oldstring};
								  delete $re->{$oldstring};
							      }
							  }
						      }
						  }
						  
					      } else { # Deletion of a flag
						  # Treatment of reactions bitstrings. No need to treat the reaction effect.
						  # If the flag has been supressed, there is no reactions modifying it.
						  foreach my $reac (@{ $sim->{"ms_complexes"}{$symbol}{"Reactions"} }){
						      foreach my $string (keys %$reac){
							  if ($string =~ /^[01\?]+$/){
							      my $oldstring = $string;
							      substr($string,$event,1) = "";
							      $reac->{$string} = $reac->{$oldstring};
							      delete $reac->{$oldstring};
							  }
						      }
						  }
						  
						  # Treatment of the variable providing the level of various states
						  foreach my $var (@{ $sim->{"ms_complexes"}{$symbol}{"Display_variables"} }){
						      foreach my $string (@{ $var->{"States"} }){
							  substr($string,$event,1) = "";
						      }
						  }
						  
						  if (exists $sim->{"ns_complexes"}{$symbol}){
						      # Treatment of NS rapid equilibrium bitstrings.
						      foreach my $re (@{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"}}){
							  foreach my $key (keys %$re){
							      if ($key eq "CoupledStates"){
								  foreach my $string (@{ $re->{$key} }){
								      substr($string,$event,1) = "";
								  }
							      } elsif ($key =~ /^[01\?]+$/){
								  my $oldstring = $key;
								  substr($key,$event,1) = "";
								  $re->{$key} = $re->{$oldstring};
								  delete $re->{$oldstring};
							      }
							  }
						      }
						      
						      # Treatment of NS reaction bitstrings. No need to treat the reaction EffectOn Neighbour.
						      # If the flag has been supressed, there is no reactions modifying it (remember, all 
						      # complexes in an array are currently identical).
						      foreach my $reac (@{ $sim->{"ns_complexes"}{$symbol}{"Reactions"}}){
							  foreach my $key (keys %$reac){
							      my $oldstring = $key;
							      substr($key,$event,1) = "";
							      $reac->{$key} = $reac->{$oldstring};
							      delete $reac->{$oldstring};
							  }
						      }
						  }
					      }
					  }
					  $sb_idx = 0;
					  foreach  my $ms_symb (sort keys %{ $sim->{"ms_complexes"} }){
					      $sim->{"ms_complexes"}{$ms_symb}{"File"} = "MS_".($sb_idx+1).".INI";
					      $sb_idx++;
					  }				      
					  $print_ms_W->grabRelease();
					  $print_ms_W->destroy;
				      }
				  }
			      } else { message("error","error",$messages{"NoComplex"}) 
				       } # beuark: instead disable everything until a complex is chosen			 
			  }
			  )->pack(-side => 'left');
			  
	$close_Fr->Button(-text             => "cancel",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command => sub{
			      $print_ms_W->grabRelease();
			      $print_ms_W->destroy}
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text             => "delete",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command => sub{
			      my $used_complex = 0;
			      if ($total_cplxs{$symbol}{'neigboursensitive'} == 1){
				  message("error","error",$messages{"UsedMSComplex"});
			      } else {
				  delete $sim->{"ms_complexes"}{$symbol};
				  my $sb_idx = 0;
				  foreach  my $ms_symb (sort keys %{ $sim->{"ms_complexes"} }){
				      $sim->{"ms_complexes"}{$ms_symb}{"File"} = "MS_".($sb_idx+1).".INI";
				      $sb_idx++;
				  }	
				  $total_cplxs{$symbol}{'multistate'} = 0;
				  $print_ms_W->grabRelease();
				  $print_ms_W ->destroy;
			      }
			  }
			  )->pack(-side => 'left');
	$balloon->attach($close_Fr, -msg => $messages{"MSClose"});
	
#----------------------------------------- description  ----------------------------------------------
# Note that this description would be useful for the GUI (only the GUI since it
# will not be read by stochsim itself. 

	my $desc_Fr = $print_ms_W->LabFrame(-borderwidth => 1, 
					    -label       => 'Description',
					    -labelside   => 'acrosstop'
					    )->pack(-side => 'bottom',
						    -fill => 'both',
						    -expand => '1');

	$desc_Fr->Entry(-textvariable => \$ms{"Description"},
			-background   => $look{"entrybackground"},
			-foreground   => $look{"entryforeground"}
			)->pack(-side   => 'bottom',
				-fill   => 'both',
				-expand => '1');
	
	$balloon->attach($desc_Fr, -msg => $messages{"MSCplxDesc"});

#------------------------------------------ base complex ---------------------------------------------
	my $base_Fr = $print_ms_W->LabFrame(-borderwidth => 1, 
					    -label       => 'Base complex type',
					    -labelside   => 'acrosstop'
					    )->pack(-side => 'top',
						  -fill => 'both',
						    -expand => '1');
	
        my $avail_cplx_Br = $base_Fr->BrowseEntry(-variable  => \$symbol,
						  -state     => 'readonly',
						  -listwidth => '35',
						  -width     => '9',
						  )->pack(-side => 'top');	
	
	$avail_cplx_Br->configure(-listcmd => sub{
	    $avail_cplx_Br->delete(0,'end');
	    $avail_cplx_Br->insert('end', sort keys %{ $sim->{"complexes"} })
	    });
	
	$balloon->attach($avail_cplx_Br, -msg => $messages{"BaseCplx"});

#---------------------------------------------- Flags ------------------------------------------------

	my $flags_Fr = $print_ms_W->LabFrame(-borderwidth => 1, 
					     -label       => 'Flags',
					     -labelside   => 'acrosstop'
					     )->pack(-side   => 'top',
						     -expand => '1',
						     -fill   => 'both');

	my $SetRE_Bt = $flags_Fr->Button(-text             => "Setup rapid equilibrium",
					 -width            => 20,
					 -activeforeground => $look{"activeforeground"},
					 -activebackground => $look{"activebackground"},
					 -state            => 'disabled',
 					 -command => sub{ 
					     set_MSrap_equilibrium($sim,\%ms, $flagindex, $print_ms_W);}
					 )->pack(-side => 'bottom',
						 );

	$balloon->attach($SetRE_Bt, -msg => $messages{"SetRapEq"});


	my $ControlledBy_Fr = $flags_Fr->Frame->pack(-side => 'bottom');

	$balloon->attach($ControlledBy_Fr,-msg => $messages{"ControlledBy"});
	
	my $React_Bt = $ControlledBy_Fr->Radiobutton(-text     => 'set by regular reaction', 
						     -value    => 0,
						     -state    => 'disabled'
						     )->pack(-side   => 'top',
							     -anchor => 'w');

	my $RapEq_Bt = $ControlledBy_Fr->Radiobutton(-text     => 'set by rapid equilibrium', 
						     -value    => 1,
						     -state    => 'disabled'
						     )->pack(-side   => 'top',
							     -anchor => 'w');
	
	my $NSREq_Bt = $ControlledBy_Fr->Radiobutton(-text     => 'set by NS equilibrium', 
						     -value    => 2,
						     -state    => 'disabled'
						     )->pack(-side   => 'top',
							     -anchor => 'w');

	$balloon->attach($ControlledBy_Fr, -msg => $messages{"ControlledBy"});
	
        my $flag_Ls = $flags_Fr->Scrolled('Listbox',
					  -background => $look{"entrybackground"},
					  -height     => '4',
					  -width      => '10',
					  )->pack(-side => 'left');

	$balloon->attach($flag_Ls, -msg => $messages{"SelectRemoveFlag"});
	
	my $new_flag_Bt = $flags_Fr->Button(-text             => 'new',
					    -activeforeground => $look{"activeforeground"},
					    -activebackground => $look{"activebackground"},
					    )->pack(-side   => 'top',
						    -expand => 1,
						    -fill   => 'both');
	
	$balloon->attach($new_flag_Bt, -msg => $messages{"NewFlag"});

	my $flag = $flags_Fr->Entry(-width      => '8',
				    -state      => 'disabled',
				    -foreground => $look{"entryforeground"},
				    )->pack(-side => 'top',
					    -expand => 1,
					    -fill   => 'x');
	
	$balloon->attach($flag, -msg => $messages{"EnterFlagName"});

#-----------------------------------------------------------------------------------------------------

	my $init_val = $print_ms_W->Button(-text => "Set initial values",
					-width            => 20,
					-activeforeground => $look{"activeforeground"},
					-activebackground => $look{"activebackground"},
					-command          => sub{ 
					    set_ms_initvalues(\%ms,$symbol,$sim->{"complexes"}, $print_ms_W);
					}
					)->pack(-side => 'top',
						  -fill => 'both',
						  -expand => '1');
	
	$balloon->attach($init_val, -msg => $messages{"SetInitVal"});

#-----------------------------------------------------------------------------------------------------
#                                       Functional code
#-----------------------------------------------------------------------------------------------------

	if (scalar @{ $ms{"Flags"} }){ $flag_Ls->insert('0', @{ $ms{"Flags"} }); }

	$avail_cplx_Br->configure(-browsecmd => sub{ 
	    if (($symbol ne $oldsymbol) and exists $sim->{"ms_complexes"}{$symbol}){
		message("warning","warning",$messages{"AlreadyMS"});
		$symbol = $oldsymbol;
	    } else {
		$oldsymbol = $symbol;
	    }
	});
	
	$flag_Ls->bind('<Button-1>' => sub{ 
	    $flagindex = $flag_Ls->curselection;
	    if (defined $flagindex){
		$currentFlag = $ms{"Flags"}[$flagindex];
		$flag->configure(-state        => 'normal',
				 -textvariable => \$ms{"Flags"}[$flagindex],
				 -background   => $look{"entrybackground"}
				 );
		$oldcontrol = $RapEq{$currentFlag};
		$React_Bt->configure(-state    => 'normal',
				     -variable => \$RapEq{$currentFlag},
				     -command  => sub{
                                         # The flag is set by a multistate RE
					 if ($oldcontrol == 1){
					     my $answer = $print_ms_W->messageBox
						 (-title   => "CAUTION",
						  -icon    => 'warning',
						  -message => $messages{"WipeMSRapEq"},
						  -type    => 'OKCancel',
                                          # No default under Unix (would be 'Cancel' and not 'cancel'). However
                                          # better than to omit the option, which would cause Ok to be the default
						  -default => 'cancel',
						  );
# Note that we already activated $React_Bt and then changed $RapEq{$ms{"Flags"}[$flagindex]} to 0.
					     if ($answer eq "Cancel" or $answer eq "cancel"){ # Under Windows messageBox returns lowercase strings
						 $RapEq{$currentFlag} = 1;
						 $oldcontrol = 1;
						 return;
					     } else{ 
						 # Remove the MS rapid equilibrium
						 my $re_idx = 0;
						 foreach my $re (@{ $ms{"Rapid_equilibriums"} }){
						     last if $re->{"Flag"} eq $currentFlag;
						     $re_idx++;
						 }
						 splice(@{ $ms{"Rapid_equilibriums"} },$re_idx,1);
						 $SetRE_Bt->configure(-state => 'disabled');
					     }
                                         # The flag is set by a multistate RE
					 } elsif ($oldcontrol == 2){
					     my $answer = $print_ms_W->messageBox
						 (-title   => "CAUTION",
						  -icon    => 'warning',
						  -message => $messages{"WipeNSRapEq"},
						  -type    => 'OKCancel',
                                          # No default under Unix (would be 'Cancel' and not 'cancel'). However
                                          # better than to omit the option, which would cause Ok to be the default
						  -default => 'cancel',
						  );
# Note that we already activated $React_Bt and then changed $RapEq{$ms{"Flags"}[$flagindex]} to 0.
					     if ($answer eq "Cancel" or $answer eq "cancel"){ # Under Windows messageBox returns lowercase strings
						 $RapEq{$currentFlag} = 2;
						 $oldcontrol = 2;
						 return;
					     } else{ 
						 # Remove the NS rapid equilibrium
						 if (exists $sim->{"ns_complexes"}{$symbol}){
						     my $re_idx = 0;
						     foreach my $re (@{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"} }){
							 last if $re->{"Flag"} eq $currentFlag;
							 $re_idx++;
						     }
						     splice(@{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"} },$re_idx,1);
						     $oldcontrol = 0;
						 }
					     }
					 }
				     });
		$RapEq_Bt->configure(-state    => 'normal',
				     -variable => \$RapEq{$currentFlag},
				     -command  => sub{
					 # The flag is set by a neighbour sensitive RE
					 if ($oldcontrol == 2){
					     my $answer = $print_ms_W->messageBox
						 (-title   => "CAUTION",
						  -icon    => 'warning',
						  -message => $messages{"WipeNSRapEq"},
						  -type    => 'OKCancel',
                                          # No default under Unix (would be 'Cancel' and not 'cancel'). However
                                          # better than to omit the option, which would cause Ok to be the default
						  -default => 'cancel',
						  );
# Note that we already activated $RapEq_Bt and then changed $RapEq{$ms{"Flags"}[$flagindex]} to 1.
					     if ($answer eq "Cancel" or $answer eq "cancel"){ # Under Windows messageBox returns lowercase strings
						 $RapEq{$currentFlag} = 2;
						 $oldcontrol = 2;
						 return;
					     } else {   
						 # remove NS rapid equilibrium
						 if (exists $sim->{"ns_complexes"}{$symbol}){ # This condition is always true if we are here ...
						     my $re_idx = 0;
						     foreach my $re (@{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"} }){
							 last if $re->{"Flag"} eq $ms{"Flags"}[$flagindex];
							 $re_idx++;
						     }
						     splice(@{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"} },$re_idx,1);
						     $oldcontrol = 1;
						     $RapEq{$currentFlag} = 1;
						     # Create a new MS rapid equilibrium
						     $ms{"Rapid_equilibriums"}[$#{ $ms{"Rapid_equilibriums"} }+1]{"Flag"} = $currentFlag;
						     # activate the button to configure the multistate
						     $SetRE_Bt->configure(-state => 'normal');  
						     return;
						 }
					     }
                                         # The flag is set by a regular reaction
					 } elsif ($oldcontrol == 0){
					     # look for reactions involving the flag
######################################################################################################
# An alternative to the following could be a warning message and an automatic removal of all involved 
# reactions if Ok is pressed. The chosen solution is safer but more tedious for the user. 
######################################################################################################
					     my %pos_react;
					     foreach my $react (@{ $ms{"Reactions"} }){
						 if ( join(" ",@{ $react->{"Effect"} }) =~ /$currentFlag/ ){
						     (my $react_num = $react->{"Symbol"}) =~ s/[fr]$//i;
						     $pos_react{$react_num} = 1; # value not important
						 }
					     }
					     if (exists $sim->{"ns_complexes"}{$symbol}){
						 foreach my $react (@{ $sim->{"ns_complexes"}{$symbol}{"Reactions"} }){
						     if ( join(" ",@{ $react->{"EffectOnNeighbour"} }) =~ /$currentFlag/ ){
							 (my $react_num = $react->{"Symbol"}) =~ s/[fr]$//i;
							 $pos_react{$react_num} = 1; # value not important
						     }
						 }
					     }
# if yes, construction of the message (with the reactions numbers)
# if not do nothing ($RapEq{$ms{"Flags"}[$flagindex]} already set to 1 by selecting $RapEq_Bt)
					     if (scalar keys %pos_react){
						 message
						     ("CAUTION",
						      'warning',
						      $messages{"FlagInReact"},
						      "\n",
						      join(", ",sort {$a<=>$b} keys %pos_react));
						 $RapEq{$ms{"Flags"}[$flagindex]} = 0;
						 $oldcontrol = 0;
					     } else {
						 $RapEq{$currentFlag} = 1;
						 $oldcontrol = 1;
						 # Create a new MS rapid equilibrium
						 $ms{"Rapid_equilibriums"}[$#{ $ms{"Rapid_equilibriums"} }+1]{"Flag"} = $currentFlag;
						 $SetRE_Bt->configure(-state => 'normal');  
						 return;
					     }
					 }});
		$NSREq_Bt->configure(-state    => 'disabled',
				     -variable => \$RapEq{$ms{"Flags"}[$flagindex]},
				     );
		if ($RapEq{$ms{"Flags"}[$flagindex]} == 1){
		    $SetRE_Bt->configure(-state => 'normal');  
		} else {
		    $SetRE_Bt->configure(-state => 'disabled');
		};
	    }
	});
	
	$flag_Ls->bind('<Control-Button-1>' => sub{ 
	    if (defined($flag_Ls->curselection)){

		# Test if a reaction control the flag. if yes, do not remove, else ...
		# look for reactions involving the flag
		my %pos_react;
		foreach my $react (@{ $ms{"Reactions"} }){
		    if ( join(" ",@{ $react->{"Effect"} }) =~ /$currentFlag/ ){
			(my $react_num = $react->{"Symbol"}) =~ s/[fr]$//i;
			$pos_react{$react_num} = 1; # value not important
		    }
		}
		if (exists $sim->{"ns_complexes"}{$symbol}){
		    foreach my $react (@{ $sim->{"ns_complexes"}{$symbol}{"Reactions"} }){
			if ( join(" ",@{ $react->{"EffectOnNeighbour"} }) =~ /$currentFlag/ ){
			    (my $react_num = $react->{"Symbol"}) =~ s/[fr]$//i;
			    $pos_react{$react_num} = 1; # value not important
			}
		    }
		}
                # if yes, construction of the message (with the reactions numbers)
		if (scalar keys %pos_react){
		    message
			("CAUTION",
			 'warning',
			 $messages{"FlagInReact"},
			 "\n",
			 join(", ",sort {$a<=>$b} keys %pos_react));
		} else {
		    
		    # Removing the rapid equilibrium controlling the flag
		    my $re_idx = 0;
		    foreach my $re (@{ $ms{"Rapid_equilibriums"} }){
			last if $re->{"Flag"} eq $ms{"Flags"}[$flag_Ls->curselection];
			$re_idx++;
		    }
		    splice(@{ $ms{"Rapid_equilibriums"} },$re_idx,1);
		    		    
		    # Removal of the flag in the local initLevel bitstrings
		    foreach my $string (keys %{ $ms{"InitLevel"} }){
			my $oldstring = $string;                       # because the key IS the bitstring to be modified
			substr($string,$flag_Ls->curselection,1) = "";
			$ms{"InitLevel"}{$string} = $ms{"InitLevel"}{$oldstring};
			delete $ms{"InitLevel"}{$oldstring};
		    }
		    
		    # Removal of the flag from the local rapid_equilibriums bitstrings
		    foreach my $re (@{ $ms{"Rapid_equilibriums"} }){
			foreach my $string (keys %$re){
			    if ($string =~ /^[01\?]+$/){
				my $oldstring = $string;
				substr($string,$flag_Ls->curselection,1) = "";
				$re->{$string} = $re->{$oldstring};
				delete $re->{$oldstring};
			    }
			}
		    }

		    # Removal of the local flag
		    splice (@{ $ms{"Flags"} },$flag_Ls->curselection,1);

		    # Record change
		    push @flag_history, $flag_Ls->curselection;

		    # Removal of the flag from the display
		    $flag_Ls->delete('active');
		}
	    }
	});

############ Fuse
	$flag->bind('<Leave>' => sub{
	    $RapEq{$ms{"Flags"}[$flagindex]} = 0;
	    $flag_Ls->delete('0','end');
	    $flag_Ls->insert('0', @{ $ms{"Flags"} });
	});

	$flag->bind('<Return>' => sub{
	    $RapEq{$ms{"Flags"}[$flagindex]} = 0;
	    $flag_Ls->delete('0','end');
	    $flag_Ls->insert('0', @{ $ms{"Flags"} });
	});

	$flag->bind('<FocusOut>' => sub{
	    $RapEq{$ms{"Flags"}[$flagindex]} = 0;
	    $flag_Ls->delete('0','end');
	    $flag_Ls->insert('0', @{ $ms{"Flags"} });
	});
############

	$new_flag_Bt->configure(-command => sub{ 
	    push(@{ $ms{"Flags"} },"");
	    push @flag_history, '+';
	    $flagindex = $#{ $ms{"Flags"} };
	    $flag->configure(-textvariable => \$ms{"Flags"}[$flagindex],
			     -state        => 'normal',
			     -background   => $look{"entrybackground"}
			     );
	});

#-----------------------------------------------------------------------------------------------------

    } else {
	$print_ms_W->deiconify();
	$print_ms_W->raise();
    }
}

######################################################################################################
#          Open a window which permit to define starting states and their initial populations
######################################################################################################

sub set_ms_initvalues{
    my ($ms,$symbol,$cplxes,$parent) = @_;
    my $state_idx;
    my $total;
    my (@states,@values);
    foreach (sort keys %{ $ms->{"InitLevel"} }){ 
	push @states, $_;
	push @values, $ms->{"InitLevel"}{$_};
    }
    $total = $cplxes->{$symbol}{"InitLevel"};
    my $used = 0;
    foreach (@values){ $used += $_ };
    my $remain = $total - $used;
    my @flags;    # state of each flag (0, 1, ?)

    if(!Exists(my $set_ms_W)){

#-----------------------------------------------------------------------------------------------------
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------

	$set_ms_W = $mw->Toplevel();
	$parent->grabRelease();
	$set_ms_W->grab();
	$set_ms_W->resizable(0,0);
	$set_ms_W->geometry("+".$look{"xthird"}."+".$look{"ythird"}); 
	$set_ms_W->title("Set initial populations");
	# users cannot kill the window via decorations
	$set_ms_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$set_ms_W->transient($set_ms_W->Parent->toplevel);

#-------------------------------------------- close Frame --------------------------------------------

	my $close_Fr = $set_ms_W->Frame->pack(-side=>'bottom');
	
	$close_Fr->Button(-text             => "accept",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      $ms->{"InitLevel"} = {};
			      for (0..$#states){
				  $ms->{"InitLevel"}{$states[$_]} = $values[$_];
			      }
			      $set_ms_W->grabRelease();
			      $parent->grab();
			      $set_ms_W->destroy;}
			     )->pack(-side => 'left');
	
	$close_Fr->Button(-text             => "cancel",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      $set_ms_W->grabRelease();
			      $parent->grab();
			      $set_ms_W->destroy}
			  )->pack(-side => 'left');
	
#	$balloon->attach($close_Fr, -msg => $messages{"CompClose"});

#-------------------------------------- All states Frame ---------------------------------------------

	my $states_Fr = $set_ms_W->LabFrame(-borderwidth => 1, 
					    -label       => 'States',
					    -labelside   => 'acrosstop'
					    )->pack(-side   => 'top',
						    -expand => '1',
						    -fill   => 'both');

	my $states_Ls = $states_Fr->Scrolled('Listbox',
					     -background => $look{"entrybackground"},
					     -height     => '4',
#					     -width      => '15',
					     )->pack(-side   => 'top',
						     -expand => '1',
						     -fill   => 'x');	
	
	$balloon->attach($states_Ls, -msg => $messages{"RemoveState"});

	my $new_state_Bt = $states_Fr->Button(-text             => 'new',
					      -activeforeground => $look{"activeforeground"},
					      -activebackground => $look{"activebackground"},
					      )->pack(-side   => 'top',
						      -fill   => 'x',
						      -expand => '1');

	$balloon->attach($new_state_Bt, -msg => $messages{"NewState"});

#-------------------------------------------- Set state Frame ----------------------------------------

	my $flags_Fr = $states_Fr->Frame->pack(-side => 'left');

	$balloon->attach($flags_Fr, -msg => $messages{"SetFlags"});

	$flags_Fr->Label(-text => '0')->grid(-row    => '0',
					     -column => '1');
	$flags_Fr->Label(-text => '?')->grid(-row    => '0',
					     -column => '2');
	$flags_Fr->Label(-text => '1')->grid(-row    => '0',
					     -column => '3');

	my (@rbt0,@rbtQ,@rbt1);
	for (0..$#{ $ms->{"Flags"} }){
	    $flags_Fr->Label(-text =>$ms->{"Flags"}[$_]
			     # because first flag is zero and first row is 1 (cause the 0 ? 1)
			     )->grid(-row    => $_+1, 
				     -column => '0');
	    $rbt0[$_] = $flags_Fr->Radiobutton(-value    => '0',
					       -variable => \$flags[$_],
					       -state    => 'disabled',
					       )->grid(-row    => $_+1,
						       -column => '1');
	    $rbtQ[$_] = $flags_Fr->Radiobutton(-value    => '?',
					       -variable => \$flags[$_],
					       -state    => 'disabled',
					       )->grid(-row    => $_+1,
							 -column => '2');
	    $rbt1[$_] = $flags_Fr->Radiobutton(-value    => '1',
					       -variable => \$flags[$_],
					       -state    => 'disabled',
					       )->grid(-row    => $_+1,
						       -column => '3');
	}
	
	my $init_val = $states_Fr->Frame->pack(-side => 'right');

	$init_val->Label(-text => "Initial number:"
			 )->pack(-side => 'top');
	my $value_En = $init_val->Entry
	    (-state        => 'disabled',
	     -textvariable => sub{ \$values[$state_idx] if \$values[$state_idx] }, 
	     -foreground   => $look{"entryforeground"},
	     -background   => $look{"entrybackground"},
	     )->pack(-side   => 'top',
		     -expand => 1,
		     -fill   => 'x');

	$balloon->attach($value_En, -msg => $messages{"EnterInit"});

	my $remain_L = $init_val->Label(-textvariable => \$remain
					 )->pack(-side   => 'left',
						 -anchor =>'n'
						 );

	$init_val->Label(-text => "complexes\nare still available",
			 -justify => 'left'
			 )->pack(-side => 'right');
	
#-----------------------------------------------------------------------------------------------------
#                                            Functional code
#-----------------------------------------------------------------------------------------------------
	for (0..$#states){
	    $states_Ls->insert('end', $states[$_].sprintf("%6s",$values[$_]));     
	}

	$states_Ls->bind('<Button-1>' => sub{ 
	    $state_idx = $states_Ls->curselection; 
	    if (defined $state_idx){
		$value_En->configure(-textvariable => \$values[$state_idx],
				     -state        => 'normal');
		@flags = split "",$states[$state_idx];
		for (0..$#{ $ms->{"Flags"} }){
		    $rbt0[$_]->configure(-variable => \$flags[$_],
					 -state    => 'normal');
		    $rbtQ[$_]->configure(-variable => \$flags[$_],
					 -state    => 'normal');
		    $rbt1[$_]->configure(-variable => \$flags[$_],
					 -state    => 'normal');
		}
	    }
	}
			 );

	$states_Ls->bind('<Control-Button-1>' => sub{ 
	    # IMPORTANT: test $states_Ls->curselection and NOT $state_idx
	    # Because $state_idx remains defined after usage. Here we require
	    # a new selection before deletion. 
	    if (defined $states_Ls->curselection){
		splice (@states,$states_Ls->curselection,1);
		$remain += $values[$states_Ls->curselection];
		splice (@values,$states_Ls->curselection,1);
		$states_Ls->delete('active');
		undef $state_idx;
		for (0..$#{ $ms->{"Flags"} }){
		    $rbt0[$_]->configure(-state => 'disabled');
		    $rbt0[$_]->deselect;
		    $rbtQ[$_]->configure(-state => 'disabled');
		    $rbtQ[$_]->deselect;
		    $rbt1[$_]->configure(-state => 'disabled');
		    $rbt1[$_]->deselect;
		}
	    }
	});
	
############ Fuse

	$value_En->bind('<Return>' => sub { 
	    $used = 0;
	    foreach (@values){ $used += $_ if $_ =~ /^\d+$/};
	    my $substr = $total - $used;
	    if ($substr < 0){
		message("error","error",$messages{"NotEnoughCplx"});
	    } else { 
		$remain = $substr;
		if (defined $state_idx){
		    $states[$state_idx] = join "",@flags;
		    $states_Ls->delete($state_idx);
		    $states_Ls->insert($state_idx, $states[$state_idx].sprintf("%6s",$values[$state_idx]));
		}
	    }
	});

	$value_En->bind('<Leave>' => sub {
	    $used = 0;
	    foreach (@values){ $used += $_ if $_ =~ /^\d+$/};
	    my $substr = $total - $used;
	    if ($substr < 0){
		message("error","error",$messages{"NotEnoughCplx"});
	    } else { 
		$remain = $substr;
		if (defined $state_idx){
		    $states[$state_idx] = join "",@flags;
		    $states_Ls->delete($state_idx);
		    $states_Ls->insert($state_idx, $states[$state_idx].sprintf("%6s",$values[$state_idx]));
		}
	    }
	});

	$value_En->bind('<FocusOut>' => sub {
	    $used = 0;
	    foreach (@values){ $used += $_ if $_ =~ /^\d+$/};
	    my $substr = $total - $used;
	    if ($substr < 0){
		message("error","error",$messages{"NotEnoughCplx"});
	    } else { 
		$remain = $substr;
		if (defined $state_idx){
		    $states[$state_idx] = join "",@flags;
		    $states_Ls->delete($state_idx);
		    $states_Ls->insert($state_idx, $states[$state_idx].sprintf("%6s",$values[$state_idx]));
		}
	    }
	});

###########

	$new_state_Bt->configure(-command => sub{ 
	    push(@states,"");
	    push(@values,0);
	    $state_idx = $#states;
	    for (0..$#{ $ms->{"Flags"} }){
		$flags[$_] = '?';
		$rbt0[$_]->configure(-variable => \$flags[$_],
				     -state    => 'normal');
		$rbtQ[$_]->configure(-variable => \$flags[$_],
				     -state    => 'normal');
		$rbt1[$_]->configure(-variable => \$flags[$_],
				     -state    => 'normal');
	    }
	    $states[$state_idx] = join "",@flags;
	    $states_Ls->insert($state_idx, $states[$state_idx].sprintf("%6s",$values[$state_idx]));
	    $value_En->configure(-textvariable => \$values[$state_idx],
				 -state        => 'normal');
	});

	for my $idx (0..$#{ $ms->{"Flags"} }){
	    $rbt0[$idx]->configure(-command  => sub{
		$states[$state_idx] = join "",@flags;
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$states[$state_idx].sprintf("%6s",$values[$state_idx]));
	    });
	    $rbtQ[$idx]->configure(-command  => sub{
		$states[$state_idx] = join "",@flags;
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$states[$state_idx].sprintf("%6s",$values[$state_idx]));
	    });
	    $rbt1[$idx]->configure(-command  => sub{
		$states[$state_idx] = join "",@flags;
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$states[$state_idx].sprintf("%6s",$values[$state_idx]));
	    });
	}

    } else {
	$set_ms_W->deiconify();
	$set_ms_W->raise();
    }
}

######################################################################################################
#                            set the parameters of a MS rapid equilibrium
#
# Structure
# $set_re_W  Toplevel
#     $close_Fr              -side =>'bottom'
#         Button "accept"        -side => 'left'
#         Button "cancel"        -side => 'left'
#     $descr_Fr              -side => 'bottom'
#         Entry                  -side   => 'bottom'
#         Label                  -side => 'left'
#     $states_Fr             -side => 'top'
#         $states_Ls             -side => 'top'
#         $new_state_Bt          -side => 'top'
#         $flags_Fr              -side => 'left'
#             Label 0                grid 0 1
#             Label ?                grid 0 2
#             Label 1                grid 0 3
#             Label "flag"           grid 1 i
#             Radiobutton 0          grid i 1
#             Radiobutton ?          grid i 2
#             Radiobutton 1          grid i 3
#         $prob_Fr               -side => 'right'
#             Label "Probability"    -side => 'top' 
#             Entry                  -side => 'top' 
#             Label "dynamic\nvalue" -side => 'left'
#             $dyn_m                 -side => 'right'
#
######################################################################################################

sub set_MSrap_equilibrium{
    my ($sim, $ms, $flagindex, $parent) = @_;
    my $state_idx;
    my $equ_idx;    # index of the rapid equilibrium within the multistate
    my (@states,@values,@dynvalues);
    my %equilibrium;

    for (0..$#{ $ms->{"Rapid_equilibriums"} }){
	if ($ms->{"Rapid_equilibriums"}[$_]{"Flag"} eq $ms->{"Flags"}[$flagindex]){
	    $equilibrium{"Description"} = $ms->{"Rapid_equilibriums"}[$_]{"Description"};
	    $equilibrium{"Flag"} = $ms->{"Rapid_equilibriums"}[$_]{"Flag"};
	    $equ_idx = $_;
	    last;
	} 
    }

    foreach (sort keys %{ $ms->{"Rapid_equilibriums"}[$equ_idx] }){ 
	if (/^[01\?]+$/){         # We want only the bitstrings, not the flag affected
	    push @states, $_;
	    $ms->{"Rapid_equilibriums"}[$equ_idx]{$_} =~ /([\d\.eE\-\+]+)(@(.+))?/;
	    push @values, $1;
	    push @dynvalues, $3;
	}
    }
    
    my @curr_flags;    # state of each flag (0, 1, ?)

    if(!Exists(my $set_re_W)){

#-----------------------------------------------------------------------------------------------------
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------

	$set_re_W = $mw->Toplevel();
	$parent->grabRelease();
	$set_re_W->grab();
	$set_re_W->resizable(0,0);
	$set_re_W->geometry("+".$look{"xthird"}."+".$look{"ythird"}); 
	$set_re_W->title("Set rapid equilibrium probabilities");
	# users cannot kill the window via decorations
	$set_re_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$set_re_W->transient($set_re_W->Parent->toplevel);

#-------------------------------------------- close Frame --------------------------------------------

	my $close_Fr = $set_re_W->Frame->pack(-side=>'bottom');
	
	$close_Fr->Button(-text             => "accept",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      # erase the old equilibrium before to accept the new one
			      undef %{ $ms->{"Rapid_equilibriums"}[$equ_idx] };
			      $ms->{"Rapid_equilibriums"}[$equ_idx]{"Description"} = $equilibrium{"Description"};
			      $ms->{"Rapid_equilibriums"}[$equ_idx]{"Flag"}        = $equilibrium{"Flag"};
			      for (0..$#states){
				  $ms->{"Rapid_equilibriums"}[$equ_idx]{$states[$_]} = $dynvalues[$_] ? $values[$_].'@'.$dynvalues[$_] : $values[$_];
			      }
			      $set_re_W->grabRelease();
			      $parent->grab();
			      $set_re_W->destroy;}
			     )->pack(-side => 'left');
	
	$close_Fr->Button(-text             => "cancel",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      $set_re_W->grabRelease();
			      $parent->grab();
			      $set_re_W->destroy}
			  )->pack(-side => 'left');
	
#	$balloon->attach($close_Fr, -msg => $messages{"CompClose"});

#--------------------------------------- description Frame -------------------------------------------
# Note that this description would be useful for the GUI (only the GUI since it
# will not be read by stochsim itself. 
     
	my $descr_Fr = $set_re_W->LabFrame(-borderwidth => 1, 
					   -label       => 'description',
					   -labelside   => 'acrosstop'
					   )->pack(-side   => 'bottom',
						   -fill   => 'both',
						   -expand => '1');
	
	$descr_Fr->Entry(-textvariable => \$equilibrium{"Description"},
			 -background   => $look{"entrybackground"},
			 -foreground   => $look{"entryforeground"}
			 )->pack(-side   => 'bottom',
				 -fill   => 'both',
				 -expand => '1');
	
#-------------------------------------- All states Frame ---------------------------------------------

	my $states_Fr = $set_re_W->LabFrame(-borderwidth => 1, 
					    -label       => 'probabilities to set the flag',
					    -labelside   => 'acrosstop'
					    )->pack(-side   => 'top',
						    -expand => '1',
						    -fill   => 'both');

	my $states_Ls = $states_Fr->Scrolled('Listbox',
					     -background => $look{"entrybackground"},
					     -height     => '4',
#					     -width      => '15',
					     )->pack(-side   => 'top',
						     -expand => '1',
						     -fill   => 'x');	
	
	$balloon->attach($states_Ls, -msg => $messages{"RemoveState"});

	my $new_state_Bt = $states_Fr->Button(-text             => 'new',
					      -activeforeground => $look{"activeforeground"},
					      -activebackground => $look{"activebackground"},
					      )->pack(-side   => 'top',
						      -fill   => 'x',
						      -expand => '1');

	$balloon->attach($new_state_Bt, -msg => $messages{"NewState"});

#-------------------------------------------- Set state Frame ----------------------------------------

	my $flags_Fr = $states_Fr->Frame->pack(-side => 'left');

	$balloon->attach($flags_Fr, -msg => $messages{"SetStateModifRE"});

	$flags_Fr->Label(-text => '0')->grid(-row    => '0',
					     -column => '1');
	$flags_Fr->Label(-text => '?')->grid(-row    => '0',
					     -column => '2');
	$flags_Fr->Label(-text => '1')->grid(-row    => '0',
					     -column => '3');

	my (@rbt0,@rbtQ,@rbt1);
	for (0..$#{ $ms->{"Flags"} }){
	    $flags_Fr->Label(-text =>$ms->{"Flags"}[$_]
			     # because first flag is zero and first row is 1 (cause the 0 ? 1)
			     )->grid(-row    => $_+1, 
				     -column => '0');
	    $rbt0[$_] = $flags_Fr->Radiobutton(-value    => '0',
					       -variable => \$curr_flags[$_],
					       -state    => 'disabled',
					       )->grid(-row    => $_+1,
						       -column => '1');
	    $rbtQ[$_] = $flags_Fr->Radiobutton(-value    => '?',
					       -variable => \$curr_flags[$_],
					       -state    => 'disabled',
					       )->grid(-row    => $_+1,
						       -column => '2');
	    $rbt1[$_] = $flags_Fr->Radiobutton(-value    => '1',
					       -variable => \$curr_flags[$_],
					       -state    => 'disabled',
					       )->grid(-row    => $_+1,
						       -column => '3');
	}
	
	# FIXME: set a check here: value is to be between 0 and 1
	my $prob_Fr = $states_Fr->Frame->pack(-side => 'right');

	$prob_Fr->Label(-text => "Probability:"
		     )->pack(-side => 'top');
	my $value_En = $prob_Fr->Entry
	    (-state        => 'disabled',
	     -foreground   => $look{"entryforeground"},
	     -background   => $look{"entrybackground"},
	     )->pack(-side   => 'top',
		     -expand => 1,
		     -fill   => 'x');

	$balloon->attach($value_En, -msg => $messages{"EnterProb"});

	$prob_Fr->Label(-text => "dynamic\nvalue"
		      )->pack(-side => 'left');

	my $dyn_m = $prob_Fr->Optionmenu(-options  => [ "",keys %{ $sim->{"dynamic_values"} }],
					 -justify  => 'right',
					 )->pack(-side => 'right');
	
	$balloon->attach($dyn_m, -msg => $messages{"ChooseDynValue"});	

#-----------------------------------------------------------------------------------------------------
#                                            Functional code
#-----------------------------------------------------------------------------------------------------

	for (0..$#states){
	    $states_Ls->insert('end', $states[$_]." ".sprintf("%6s",$values[$_]));     
	}

	$states_Ls->bind('<Button-1>' => sub{ 
	    $state_idx = $states_Ls->curselection; 
	    if (defined $state_idx){
		$value_En->configure(-textvariable => \$values[$state_idx],
				     -state        => 'normal');
		$dyn_m->configure(-textvariable => \$dynvalues[$state_idx]);
		@curr_flags = split "",$states[$state_idx];
		for (0..$#{ $ms->{"Flags"} }){
		    $rbt0[$_]->configure(-state    => 'normal',
					 -variable => \$curr_flags[$_]);
		    $rbtQ[$_]->configure(-state    => 'normal',
					 -variable => \$curr_flags[$_]);
		    $rbt1[$_]->configure(-state    => 'normal',
					 -variable => \$curr_flags[$_]);
		}
	    }
	}
			 );

	$states_Ls->bind('<Control-Button-1>' => sub{ 
	    # IMPORTANT: test $states_Ls->curselection and NOT $state_idx
	    # Because $state_idx remains defined after usage. Here we require
	    # a new selection before deletion. 
	    if (defined $states_Ls->curselection){
		delete $equilibrium{ $states[$state_idx] };
		splice (@states,$state_idx,1);
		splice (@values,$state_idx,1);
		$states_Ls->delete('active');
		undef $state_idx;
		for (0..$#{ $ms->{"Flags"} }){
		    $rbt0[$_]->configure(-state => 'disabled');
		    $rbt0[$_]->deselect;
		    $rbtQ[$_]->configure(-state => 'disabled');
		    $rbtQ[$_]->deselect;
		    $rbt1[$_]->configure(-state => 'disabled');
		    $rbt1[$_]->deselect;
		}
	    }
	});

	$new_state_Bt->configure(-command => sub{ 
	    $value_En->configure(-state => 'normal');
	    push(@states,"");
	    push(@values,0);
	    $state_idx = $#states;
	    for (0..$#{ $ms->{"Flags"} }){
		$curr_flags[$_] = '?';
		$rbt0[$_]->configure(-state    => 'normal',
				     -variable => \$curr_flags[$_]);
		$rbtQ[$_]->configure(-state    => 'normal',
				     -variable => \$curr_flags[$_]);
		$rbt1[$_]->configure(-state    => 'normal',
				     -variable => \$curr_flags[$_]);
	    }
	    $states[$state_idx] = join "",@curr_flags;
	    $states_Ls->insert($state_idx,$states[$state_idx]." ".sprintf("%6s",$values[$state_idx]));
	    $value_En->configure(-textvariable => \$values[$state_idx]);
	});

########### Fuse

	$value_En->bind('<Leave>' => sub{
	    if (defined $state_idx){
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$states[$state_idx]." ".sprintf("%6s",$values[$state_idx]));
	    }
	});

	$value_En->bind('<Return>' => sub{  
	    if (defined $state_idx){
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$states[$state_idx]." ".sprintf("%6s",$values[$state_idx]));
	    }
	});

	$value_En->bind('<FocusOut>' => sub{  
	    if (defined $state_idx){
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$states[$state_idx]." ".sprintf("%6s",$values[$state_idx]));
	    }
	});

#############

	for my $idx (0..$#{ $ms->{"Flags"} }){
	    $rbt0[$idx]->configure(-command  => sub{
		$states[$state_idx] = join "",@curr_flags;
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$states[$state_idx]." ".sprintf("%6s",$values[$state_idx]));
	    });
	    $rbtQ[$idx]->configure(-command  => sub{
		$states[$state_idx] = join "",@curr_flags;
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$states[$state_idx]." ".sprintf("%6s",$values[$state_idx]));
	    });
	    $rbt1[$idx]->configure(-command  => sub{
		$states[$state_idx] = join "",@curr_flags;
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$states[$state_idx]." ".sprintf("%6s",$values[$state_idx]));
	    });
	}
	
    } else {
	$set_re_W->deiconify();
	$set_re_W->raise();
    }
}

######################################################################################################
#                 Open a Window where the user can create neighbour-sensitive complexes 
#
# Structure:
# ----------
#
# $print_ns_W                                  Toplevel 
#     $close_Fr             -side => 'bottom'
#         Button "accept"       -side => 'left'
#         Button "cancel"       -side => 'left'
#         Button "delete"       -side => 'left'
#     $desc_Fr              -side => 'bottom'
#         Entry 'description'   -side => bottom
#     $base_Fr              -side => 'top'
#         $avail_cplx_Br        -side => 'top'
#     $numNeighb_Fr         -side => 'top'
#         Radiobutton 3         -side   => 'left'
#         Radiobutton 4         -side   => 'left'
#         Radiobutton 6         -side   => 'left'
#     $flags_Fr             -side   => 'top'
#         $flag_Ls              -side   => 'left'
#         Entry $flag           -side => 'top'
#         $ControlledBy_Fr      -side => 'bottom'
#             $React_Bt             -side => 'top'
#             $RapEq_Bt             -side => 'top' 
#             $NSREq_Bt             -side => 'top'
#         $SetRE_Bt             -side => 'bottom'
#
######################################################################################################

sub print_neighbour {
    my ($sim,$symbol) = @_; 
    my %ns;
    my %RapEq;      # Keys are flags, values are:
                    #              0 if driven by a reaction, 
                    #              1 by MS rapid equilibrium, 
                    #              2 by a NS rapid equilibrium 
    my $oldcontrol; # record the type of control for the current flag
    my $oldsymbol;    
    my $flagindex;
    my $currentFlag;

    populateNS($symbol,\%ns,\%RapEq,$sim);
    $oldsymbol = $symbol; # save the symbol to delete the ns_complex if changed    

    if(!Exists(my $print_ns_W)){
	
#-----------------------------------------------------------------------------------------------------
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------

	$print_ns_W = $mw->Toplevel();
	$print_ns_W->resizable(0,0);
	$print_ns_W->geometry("+".$look{"xsecond"}."+".$look{"ysecond"}); 
	$print_ns_W->title(defined($symbol) ? "NS-complex $symbol" : "NS-complex");
	$print_ns_W->grab();
	# users cannot kill the window via decorations
	$print_ns_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$print_ns_W->transient($print_ns_W->Parent->toplevel);

#------------------------------------------- close Frame ---------------------------------------------
	my $close_Fr = $print_ns_W->Frame->pack(-side=>'bottom');
	
	$close_Fr->Button(-text             => "accept",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{    
			      if (defined $symbol){
				  $sim->{"ns_complexes"}{$symbol}{"File"}          = $ns{"File"};
				  $sim->{"ns_complexes"}{$symbol}{"NumNeighbours"} = $ns{"NumNeighbours"};
				  $sim->{"ns_complexes"}{$symbol}{"Description"}   = $ns{"Description"};
				  for (0..$#{ $ns{"Rapid_equilibriums"} }){
				      $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"}[$_] = { %{ $ns{"Rapid_equilibriums"}[$_] }};
				      $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"}[$_]{"CoupledStates"}  = [ @{ $ns{"Rapid_equilibriums"}[$_]{"CoupledStates"} } ];
				  }
				  $total_cplxs{$symbol}{'neighboursensitive'} = 1;
				  if ($oldsymbol and ($oldsymbol ne $symbol)){
				      delete $sim->{"ns_complexes"}{$oldsymbol};				      
				  } 
				  my $sb_idx = 0;
				  foreach  my $ns_symb (sort keys %{ $sim->{"ns_complexes"} }){
				      $sim->{"ns_complexes"}{$ns_symb}{"File"} = "NS_".($sb_idx+1).".INI";
				      $sb_idx++;
				  }
				  $print_ns_W->grabRelease();
				  $print_ns_W->destroy;
			      } else { message("error","error",$messages{"NoComplex"}) } # beuark: instead disable everything until a complex is chosen	
			  })->pack(-side => 'left');
	
	$close_Fr->Button(-text             => "cancel",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      $print_ns_W->grabRelease();
			      $print_ns_W->destroy
			      }
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text             => "delete",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      delete $sim->{"ns_complexes"}{$symbol};
			      my $sb_idx = 0;
			      foreach  my $ns_symb (sort keys %{ $sim->{"ns_complexes"} }){
				  $sim->{"ns_complexes"}{$ns_symb}{"File"} = "NS_".($sb_idx+1).".INI";
				  $sb_idx++;
			      }
			      $total_cplxs{$symbol}{'neighboursensitive'} = 0;
			      $print_ns_W->grabRelease();
			      $print_ns_W ->destroy
			      }
			     )->pack(-side => 'left');

	$balloon->attach($close_Fr, -msg => $messages{"NSClose"});
	
#----------------------------------------- description  ----------------------------------------------
# Note that this description would be useful for the GUI (only the GUI since it
# will not be read by stochsim itself. 
     
	my $desc_Fr = $print_ns_W->LabFrame(-borderwidth => 1, 
					    -label       => 'Description',
					    -labelside   => 'acrosstop'
					    )->pack(-side => 'bottom',
						    -fill => 'both',
						    -expand => '1');
	
	$desc_Fr->Entry(-textvariable => \$ns{"Description"},
			-background   => $look{"entrybackground"},
			-foreground   => $look{"entryforeground"}
			)->pack(-side   => 'bottom',
				-fill   => 'both',
				-expand => '1');
	
	$balloon->attach($desc_Fr, -msg => $messages{"CplxDesc"});
	
#------------------------------------------ base complex ---------------------------------------------

	my $base_Fr = $print_ns_W->LabFrame(-borderwidth => 1, 
					    -label       => 'Base complex type',
					    -labelside   => 'acrosstop'
					    )->pack(-side => 'top',
						    -fill => 'both',
						    -expand => '1');
		
        my $avail_cplx_Br = $base_Fr->BrowseEntry(-variable  => \$symbol,
						  -state     => 'readonly',
						  -listwidth => '35',
						  -width     => '9',
						  )->pack(-side => 'top');	
	
	$avail_cplx_Br->configure(-listcmd => sub{
	    $avail_cplx_Br->delete(0,'end');
	    $avail_cplx_Br->insert('end', keys %{ $sim->{"ms_complexes"} })
	    });
	
	$balloon->attach($avail_cplx_Br, -msg => $messages{"BaseMSCplx"});

#-----------------------------------------Number of reacting neighbours in the array -----------------
	
	my $numNeighb_Fr = $print_ns_W->LabFrame->pack(-side => 'top',
						       -fill => 'both',
						       -expand => '1');
	
	$numNeighb_Fr->Label(-text => "# interacting neighbours",
			     )->pack(-side   => 'left',
				     -fill   => 'both',
				     -expand => '1');
	
	$numNeighb_Fr->Radiobutton(-text     => '3', 
				   -value    => 3,
				   -variable => \$ns{"NumNeighbours"},
				   )->pack(-side   => 'left');
	
	$numNeighb_Fr->Radiobutton(-text     => '4', 
				   -value    => 4,
				   -variable => \$ns{"NumNeighbours"},
				   )->pack(-side   => 'left');
	
	$numNeighb_Fr->Radiobutton(-text     => '6', 
				   -value    => 6,
				   -variable => \$ns{"NumNeighbours"},
				   )->pack(-side   => 'left');
	
	$balloon->attach($numNeighb_Fr, -msg => $messages{"NumNeighbours"});

#---------------------------------------------- Flags ------------------------------------------------

	my $flags_Fr = $print_ns_W->LabFrame(-borderwidth => 1, 
					     -label       => 'Flags',
					     -labelside   => 'acrosstop'
					     )->pack(-side   => 'top',
						     -expand => '1',
						     -fill   => 'both');

	my $SetRE_Bt = $flags_Fr->Button(-text             => "Setup rapid equilibrium",
					 -width            => 20,
					 -activeforeground => $look{"activeforeground"},
					 -activebackground => $look{"activebackground"},
					 -state            => 'disabled',
 					 -command => sub{ 
					     set_NSrap_equilibrium($print_ns_W, \%ns, $sim->{"ms_complexes"}{$symbol}{"Flags"}[$flagindex], $symbol);}
					 )->pack(-side => 'bottom',
						 );

	$balloon->attach($SetRE_Bt, -msg => $messages{"SetRapEq"});	

	my $ControlledBy_Fr = $flags_Fr->Frame->pack(-side => 'bottom');

	$balloon->attach($ControlledBy_Fr,-msg => $messages{"ControlledBy"});
	
	my $React_Bt = $ControlledBy_Fr->Radiobutton(-text     => 'set by regular reaction', 
						     -value    => 0,
						     -state    => 'disabled'
						     )->pack(-side   => 'top',
							     -anchor => 'w');

	my $RapEq_Bt = $ControlledBy_Fr->Radiobutton(-text     => 'set by rapid equilibrium', 
						     -value    => 1,
						     -state    => 'disabled'
						     )->pack(-side   => 'top',
							     -anchor => 'w');
	
	my $NSREq_Bt = $ControlledBy_Fr->Radiobutton(-text     => 'set by NS equilibrium', 
						     -value    => 2,
						     -state    => 'disabled'
						     )->pack(-side   => 'top',
							     -anchor => 'w');

        my $flag_Ls = $flags_Fr->Scrolled('Listbox',
					  -background   => $look{"entrybackground"},
					  -height       => '4',
#					  -width        => '10',
					  )->pack(-side   => 'top',
						  -expand => 1,
						  -fill   => 'x');	

	$balloon->attach($flag_Ls, -msg => $messages{"SelectRemoveFlag"});

#-----------------------------------------------------------------------------------------------------
#                                       Functional code
#-----------------------------------------------------------------------------------------------------
	if (defined $sim->{"ms_complexes"}{$symbol} and @{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
	    $flag_Ls->insert('0', @{ $sim->{"ms_complexes"}{$symbol}{"Flags"} });
	}

	$avail_cplx_Br->configure(-browsecmd => sub{ 
	    if (($symbol ne $oldsymbol) and exists $sim->{"ns_complexes"}{$symbol}){		
		message("warning","warning",$messages{"AlreadyNS"});
		$symbol = $oldsymbol;
	    } else {
		populateNS($symbol,\%ns,\%RapEq,$sim);
		if (defined $sim->{"ms_complexes"}{$symbol} and @{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
		    $flag_Ls->delete(0,'end');
		    $flag_Ls->insert('0', @{ $sim->{"ms_complexes"}{$symbol}{"Flags"} });
		}
	    }
	});
	
	$flag_Ls->bind('<Button-1>' => sub{ 
	    $flagindex = $flag_Ls->curselection;
	    $currentFlag = $sim->{"ms_complexes"}{$symbol}{"Flags"}[$flagindex];
	    if (defined $flagindex){
		$oldcontrol = $RapEq{$currentFlag};
		$React_Bt->configure(-state    => 'normal',
				     -variable => \$RapEq{$currentFlag},
				     -command  => sub{
					 if ($oldcontrol == 1){
					     my $answer = $print_ns_W->messageBox
						 (-title   => "CAUTION",
						  -icon    => 'warning',
						  -message => $messages{"WipeMSRapEq"},
						  -type    => 'OKCancel',
                                          # No default under Unix (would be 'Cancel' and not 'cancel'). However
                                          # better than to omit the option, which would cause Ok to be the default
						  -default => 'cancel',
						  );
					     if ($answer eq "Cancel" or $answer eq "cancel"){ # Under Windows messageBox returns lowercase strings
						 $RapEq{$currentFlag} = 1;
						 $oldcontrol = 1;
						 return;
					     } else{ 
						 # Remove the MS rapid equilibrium
						 my $re_idx = 0;
						 foreach my $re (@{ $sim->{"ms_complexes"}{$symbol}{"Rapid_equilibriums"} }){
						     last if $re->{"Flag"} eq $currentFlag;
						     $re_idx++;
						 }
						 splice(@{ $sim->{"ms_complexes"}{$symbol}{"Rapid_equilibriums"} },$re_idx,1);
						 $SetRE_Bt->configure(-state => 'disabled');
					     }
					 } elsif ($oldcontrol == 2){
					     my $answer = $print_ns_W->messageBox
						 (-title   => "CAUTION",
						  -icon    => 'warning',
						  -message => $messages{"WipeNSRapEq"},
						  -type    => 'OKCancel',
                                          # No default under Unix (would be 'Cancel' and not 'cancel'). However
                                          # better than to omit the option, which would cause Ok to be the default
						  -default => 'cancel',
						  );
					     if ($answer eq "Cancel" or $answer eq "cancel"){ # Under Windows messageBox returns lowercase strings
						 $RapEq{$currentFlag} = 2;
						 $oldcontrol = 2;
						 return;
					     } else{ 
						 # Remove the NS rapid equilibrium
						 my $re_idx = 0;
						 foreach my $re (@{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"} }){
						     last if $re->{"Flag"} eq $currentFlag;
						     $re_idx++;
						 }
						 splice(@{ $ns{"Rapid_equilibriums"} },$re_idx,1);
						 $oldcontrol = 0;
					     }
					 }
					 $RapEq{$currentFlag} = $oldcontrol;
				     });
		$NSREq_Bt->configure(-state    => 'normal',
				     -variable => \$RapEq{$currentFlag},
				     -command  => sub{
					 # The flag is set by a multistate RE
					 if ($oldcontrol == 1){
					     my $answer = $print_ns_W->messageBox
						 (-title   => "CAUTION",
						  -icon    => 'warning',
						  -message => $messages{"WipeMSRapEq"},
						  -type    => 'OKCancel',
                                          # No default under Unix (would be 'Cancel' and not 'cancel'). However
                                          # better than to omit the option, which would cause Ok to be the default
						  -default => 'cancel',
						  );
					     if ($answer eq "Cancel" or $answer eq "cancel"){ # Under Windows messageBox returns lowercase strings
						 $RapEq{$currentFlag} = 1;
						 $oldcontrol = 1;
						 return;
					     } else { 
						 # remove MS rapid equilibrium
						 my $re_idx = 0;
						 foreach my $re (@{ $sim->{"ms_complexes"}{$symbol}{"Rapid_equilibriums"} }){
						     last if $re->{"Flag"} eq $currentFlag;
						     $re_idx++;
						 }
						 splice(@{ $sim->{"ms_complexes"}{$symbol}{"Rapid_equilibriums"} },$re_idx,1);
						 $oldcontrol = 2;
						 $RapEq{$currentFlag} = 2;
						 # Create a new NS rapid equilibrium
						 $ns{"Rapid_equilibriums"}[$#{ $ns{"Rapid_equilibriums"} }+1]{"Flag"} = $currentFlag;
					     }
					 } elsif ($oldcontrol == 0){
					     # look for reactions involving the flag
######################################################################################################
# An alternative to the following could be a warning message and an automatic removal of all involved 
# reactions if Ok is pressed. The chosen solution is safer but more tedious for the user. 
######################################################################################################
					     my %pos_react;
					     foreach my $react (@{ $sim->{"ms_complexes"}{$symbol}{"Reactions"} }){
						 if ( join(" ",@{ $react->{"Effect"} }) =~ /$currentFlag/ ){
						     (my $react_num = $react->{"Symbol"}) =~ s/[fr]$//i;
						     $pos_react{$react_num} = 1; # value not important
						 }
					     }
					     foreach my $react (@{ $sim->{"ns_complexes"}{$symbol}{"Reactions"} }){
						 if ( join(" ",@{ $react->{"EffectOnNeighbour"} }) =~ /$currentFlag/ ){
						     (my $react_num = $react->{"Symbol"}) =~ s/[fr]$//i;
						     $pos_react{$react_num} = 1; # value not important
						 }
					     }
# if yes, construction of the message (with the reactions numbers)
# if not do nothing ($RapEq{$ms{"Flags"}[$flagindex]} already set to 1 by selecting $RapEq_Bt)
					     if (scalar keys %pos_react){
						 message
						     ("CAUTION",
						      'warning',
						      $messages{"FlagInReact"},
						      "\n",
						      join(", ",sort {$a<=>$b} keys %pos_react));
						 $RapEq{$currentFlag} = 0;
						 $oldcontrol = 0;
					     } else {
						 $RapEq{$currentFlag} = 2;
						 $oldcontrol = 2;
						 # Create a new NS rapid equilibrium
						 $ns{"Rapid_equilibriums"}[$#{ $ns{"Rapid_equilibriums"} }+1]{"Flag"} = $currentFlag;
						 return;
					     } 
					 }
				     });
		$RapEq_Bt->configure(-state    => 'disabled',
				     -variable => \$RapEq{$currentFlag},
				     );
		if ($oldcontrol == 2){
		    $SetRE_Bt->configure(-state => 'normal');  
		} else {
		    $SetRE_Bt->configure(-state => 'disabled');
		};
	    }
	});

    } else {
	$print_ns_W->deiconify();
	$print_ns_W->raise();
    }
}

######################################################################################################
#                        Initialise the current neighbour-sensitive complex
######################################################################################################

sub populateNS{
    my ($symbol,$ns,$RapEq,$sim) = @_;

    $ns->{"File"}               = undef;
    $ns->{"Description"}        = undef;
    $ns->{"NumNeighbours"}      = undef;
    $ns->{"Rapid_equilibriums"} = [];
    $ns->{"Reactions"}          = [];
    
    # A complex may be multistate and not neighbour-sensitive. 
    if (defined $sim->{"ms_complexes"}{$symbol}){  
	foreach my $fl (@{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
	    $RapEq->{$fl} = 0; # All flags are initially considered controlled by reactions
	}
	for (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Rapid_equilibriums"} }){
	    # This flag is controlled by a rapid equilibrium and not a reaction
	    $RapEq->{$sim->{"ms_complexes"}{$symbol}{"Rapid_equilibriums"}[$_]{"Flag"}} = 1;
	}
    }
    
    if (defined $sim->{"ns_complexes"}{$symbol}){
	# Note that if we enter here, $symbol is obligatory one of the existing $ns_complexes
	# Indeed, the user can only choice between them
	$ns->{"File"}          = $sim->{"ns_complexes"}{$symbol}{"File"};
	$ns->{"Description"}   = $sim->{"ns_complexes"}{$symbol}{"Description"};
	$ns->{"NumNeighbours"} = $sim->{"ns_complexes"}{$symbol}{"NumNeighbours"};
	for (0 .. $#{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"} } ){
	    $ns->{"Rapid_equilibriums"}[$_] = { %{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"}[$_] } };
	    $ns->{"Rapid_equilibriums"}[$_]{"CoupledStates"} = [ @{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"}[$_]{"CoupledStates"} } ];
	    # This flag is controlled by a rapid equilibrium and not a reaction
	    $RapEq->{$ns->{"Rapid_equilibriums"}[$_]{"Flag"}} = 2;
	    
	    # initialisation of the five probabilities of neighbour-controled states
	    foreach my $key (keys %{$ns->{"Rapid_equilibriums"}[$_]}){
		if ($key =~ /^(01\?)$/){
		    $ns->{"Rapid_equilibriums"}[$_]{$key} = [ @{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"}[$_]{$key} } ];
		}
	    }	
	}
    } 
}

######################################################################################################
#                            set the parameters of a NS rapid equilibrium
#
# Structure
# $set_re_W              Toplevel
#     $close_Fr               -side =>'bottom'
#          Button "accept"        -side => 'left'
#          Button "cancel"        -side => 'left'
#     $descr_Fr               -side => 'bottom'
#          Entry                  -side => 'bottom'
#          Label "Description"    -side => 'left'
#     $self_Fr                -side => 'right'
#          $self_states_Ls        -side => 'top'
#          $new_self_Bt           -side => 'top'
#          $self_flags_Fr         -side => 'left'
#              Label 0                grid 0 1
#              Label ?                grid 0 2
#              Label 1                grid 0 3
#              Label "flag"           grid i+1 0
#              Radiobutton 0          grid i+1 1
#              Radiobutton ?          grid i+1 2
#              Radiobutton 1          grid i+1 3
#          $prob_Fr               -side => 'right'
#              Label "Probability"    grid 0 0
#              Label [i]              grid i+1 0
#              Entry [i]              grid i+1 1
#     $neighb_Fr              -side => 'left'
#          $neighb_states_Ls      -side => 'top'
#          $new_neighb_Bt         -side => 'top'
#          $neighb_flags_Fr       -side => 'top'
#              Label 0                grid 0 1
#              Label ?                grid 0 2
#              Label 1                grid 0 3
#              Label "flag"           grid i+1 0
#              Radiobutton 0          grid i+1 1
#              Radiobutton ?          grid i+1 2
#              Radiobutton 1          grid i+1 3
#
######################################################################################################

sub set_NSrap_equilibrium{
    my ($parent, $ns, $flag, $symbol) = @_;
    my $state_idx;
    my $equ_idx;    # index of the rapid equilibrium within the NS complex
    my (@states,@values);
    my %equilibrium;
    for (0..$#{ $ns->{"Rapid_equilibriums"} }){
	if ($ns->{"Rapid_equilibriums"}[$_]{"Flag"} eq $flag){
	    $equilibrium{"Description"}   = $ns->{"Rapid_equilibriums"}[$_]{"Description"};
	    $equilibrium{"CoupledStates"} = [ @{ $ns->{"Rapid_equilibriums"}[$_]{"CoupledStates"} } ];
	    $equ_idx = $_;
	    last;
	} 
    }

    foreach my $rekey (sort keys %{ $ns->{"Rapid_equilibriums"}[$equ_idx] }){ 
	if ($rekey =~ /^[01\?]+$/){
	    # Note: there are currently no dynamic values on NS rapeq, hence no suffixes 
	    push @states, $rekey;  	    
	    push @values, [];  # CAUTION here $equilibrium{$_} is a reference to an ARRAY
	    for (0 .. $#{ $ns->{"Rapid_equilibriums"}[$equ_idx]{$rekey} }){
		$values[$#values][$_] = $ns->{"Rapid_equilibriums"}[$equ_idx]{$rekey}[$_];
	    }
	}
    }

    my (@self_flags,@neighb_flags);    # state of each flag (0, 1, ?)
    # Initialises all the flags to '?'. Do not use foreach{}, we do not change $ms->{"Flags"}!!!
    for (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
        $self_flags[$_] = undef;
	$neighb_flags[$_] = undef;
    }

    if(!Exists(my $set_re_W)){

#-----------------------------------------------------------------------------------------------------
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------

	$set_re_W = $mw->Toplevel();
	$parent->grabRelease();
	$set_re_W->grab();
	$set_re_W->resizable(0,0);
	$set_re_W->geometry("+".$look{"xthird"}."+".$look{"ythird"}); 
	$set_re_W->title("Set rapid equilibrium probabilities");
	# users cannot kill the window via decorations
	$set_re_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$set_re_W->transient($set_re_W->Parent->toplevel);

#-------------------------------------------- close Frame --------------------------------------------

	my $close_Fr = $set_re_W->Frame->pack(-side=>'bottom');
	
	$close_Fr->Button(-text             => "accept",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      # Delete the old NS equilibrium before to record the modifications
			      undef %{ $ns->{"Rapid_equilibriums"}[$equ_idx] };			     

			      $ns->{"Rapid_equilibriums"}[$equ_idx]{"Flag"} = $flag; 
			      $ns->{"Rapid_equilibriums"}[$equ_idx]{"CoupledStates"} = [ @{ $equilibrium{"CoupledStates"} }];
			      $ns->{"Rapid_equilibriums"}[$equ_idx]{"Description"} = $equilibrium{"Description"};
			      for (0..$#states){
				  $ns->{"Rapid_equilibriums"}[$equ_idx]{$states[$_]} = [ @{ $values[$_] } ];
			      }

			      $set_re_W->grabRelease();
			      $parent->grab();
			      $set_re_W->destroy;}
			     )->pack(-side => 'left');
	
	$close_Fr->Button(-text             => "cancel",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      $set_re_W->grabRelease();
			      $parent->grab();
			      $set_re_W->destroy}
			  )->pack(-side => 'left');
	
#	$balloon->attach($close_Fr, -msg => $messages{"CompClose"});

#--------------------------------------- description Frame -------------------------------------------
# Note that this description would be useful for the GUI (only the GUI since it
# will not be read by stochsim itself. 
     
	my $descr_Fr = $set_re_W->Frame->pack(-side   => 'bottom',
					      -fill   => 'both',
					      -expand => '1');
	$descr_Fr->Entry(-textvariable => \$equilibrium{"Description"},
			 -background   => $look{"entrybackground"},
			 -foreground   => $look{"entryforeground"}
			 )->pack(-side   => 'bottom',
				 -fill   => 'both',
				 -expand => '1');
	
	$descr_Fr->Label(-text => "Description"
			 )->pack(-side => 'left');

#	$balloon->attach($descr_Fr, -msg => $messages{"CplxDesc"});
	
#-------------------------------------- self states Frame --------------------------------------------

	my $self_Fr = $set_re_W->LabFrame(-borderwidth => 1, 
					  -label       => 'Self states',
					  -labelside   => 'acrosstop'
					  )->pack(-side   => 'right',
						  -expand => '1',
					       -fill   => 'both');
	
#---------------------------------------- All states frame -------------------------------------------
 	
	my $self_states_Ls = $self_Fr->Scrolled('Listbox',
						-background   => $look{"entrybackground"},
						-height       => '4',
#						-width        => '35',
						)->pack(-side   => 'top',
							-expand => 1,
							-fill   => 'x');	
	
	$balloon->attach($self_states_Ls, -msg => $messages{"RemoveState"});

	my $new_self_Bt = $self_Fr->Button(-text             => 'new',
					   -activeforeground => $look{"activeforeground"},
					   -activebackground => $look{"activebackground"},
					   )->pack(-side   => 'top',
						   -fill   => 'x',
						   -expand => '1');
	
	$balloon->attach($new_self_Bt, -msg => $messages{"NewState"});
	
#-------------------------------------------- Set state Frame ----------------------------------------
	
	my $self_flags_Fr = $self_Fr->Frame->pack(-side => 'left');
	
	$balloon->attach($self_flags_Fr, -msg => $messages{"SetStateModifRE"});
	
	$self_flags_Fr->Label(-text => '0')->grid(-row    => '0',
						  -column => '1');
	$self_flags_Fr->Label(-text => '?')->grid(-row    => '0',
						  -column => '2');
	$self_flags_Fr->Label(-text => '1')->grid(-row    => '0',
						  -column => '3');
	
	my (@self_rbt0,@self_rbtQ,@self_rbt1);
	for (0..$#self_flags){
	    $self_flags_Fr->Label(-text => $sim->{"ms_complexes"}{$symbol}{"Flags"}[$_]
				  # because first flag is zero and first row is 1 (cause the 0 ? 1)
				  )->grid(-row    => $_+1, 
					  -column => '0');
	    $self_rbt0[$_] = $self_flags_Fr->Radiobutton(-value    => '0',
							 -state    => 'disabled',
							 -variable => \$self_flags[$_],
							 )->grid(-row    => $_+1,
								 -column => '1');
	    $self_rbtQ[$_] = $self_flags_Fr->Radiobutton(-value    => '?',
							 -state    => 'disabled',
							 -variable => \$self_flags[$_],
							 )->grid(-row    => $_+1,
								 -column => '2');
	    $self_rbt1[$_] = $self_flags_Fr->Radiobutton(-value    => '1',
							 -state    => 'disabled',
							 -variable => \$self_flags[$_],
							 )->grid(-row    => $_+1,
								 -column => '3');
	}
	
	# FIXME: set a check here: value is to be between 0 and 1
	my $prob_Fr = $self_Fr->Frame->pack(-side => 'right');
	
	$prob_Fr->Label(-text => "Probability\n(according to the # of neighbours)"
			)->grid(-row        => 0,
				-colum      => 0,
				-columnspan => 2);
	
	my (@value_En,@value_L);
	for (0..$ns->{"NumNeighbours"}){ 
	    # I do not use LabEntry, because the option labelPack, which control the position
	    # of the label does not work (or rarely)

	    $value_L[$_] = $prob_Fr->Label(-text => $_
					   )->grid(-row    => $_+1,
						   -column => 0);
	    $value_En[$_] = $prob_Fr->Entry
		(-width        => '10',
		 -state        => 'disabled',
		 -textvariable => sub{ \$values[$state_idx][$_] if \$values[$state_idx][$_] }, 
		 -foreground   => $look{"entryforeground"},
		 -background   => $look{"entrybackground"},
		 )->grid(-row    => $_+1,
			 -column => 1);
	}
	$balloon->attach($prob_Fr, -msg => $messages{"EnterProbNeighb"});
	
#-----------------------------------------------------------------------------------------------------
#                                            Functional code
#-----------------------------------------------------------------------------------------------------

        # I use $i here and not $_ because there is another $_ for the map function. Confusing
	for my $i (0..$#states){
	    $self_states_Ls->insert('end', $states[$i]."  ".join "  ", map{sprintf "%5.2f",$_ }@{ $values[$i] });     
	}

	$self_states_Ls->bind('<Button-1>' => sub{ 
	    $state_idx = $self_states_Ls->curselection; 
	    if (defined $state_idx){
		for (0..$ns->{"NumNeighbours"}){
		    $value_En[$_]->configure(-textvariable => \$values[$state_idx][$_],
					     -state        => 'normal');
		}
		@self_flags = split "",$states[$state_idx];
		for (0..$#self_flags){
		    $self_rbt0[$_]->configure(-variable => \$self_flags[$_],
					      -state    => 'normal'
					      );
		    $self_rbtQ[$_]->configure(-variable => \$self_flags[$_],
					      -state    => 'normal'
					      );
		    $self_rbt1[$_]->configure(-variable => \$self_flags[$_],
					      -state    => 'normal'
					      );
		}
	    }
	}
			 );

	$self_states_Ls->bind('<Control-Button-1>' => sub{ 
	    # IMPORTANT: test $states_Ls->curselection and NOT $state_idx
	    # Because $state_idx remains defined after usage. Here we require
	    # a new selection before deletion. 
	    if (defined $self_states_Ls->curselection){
		delete $equilibrium{ $states[$state_idx] };
		splice (@states,$state_idx,1);
		splice (@values,$state_idx,1);
		$self_states_Ls->delete('active');
		undef $state_idx;
		for (0..$#self_flags){
		    $self_rbt0[$_]->configure(-state => 'disabled');
		    $self_rbt0[$_]->deselect;
		    $self_rbtQ[$_]->configure(-state => 'disabled');
		    $self_rbtQ[$_]->deselect;
		    $self_rbt1[$_]->configure(-state => 'disabled');
		    $self_rbt1[$_]->deselect;
		}
	    }
	});

	$new_self_Bt->configure(-command => sub{
	    push(@states,"");
	    push(@values,[]);
	    $state_idx = $#states;
	    for (0..$#self_flags){
		$self_flags[$_] = '?';
		$self_rbt0[$_]->configure(-variable => \$self_flags[$_],
					      -state    => 'normal');
		$self_rbtQ[$_]->configure(-variable => \$self_flags[$_],
					      -state    => 'normal');
		$self_rbt1[$_]->configure(-variable => \$self_flags[$_],
					      -state    => 'normal');
	    }
	    for (0..$ns->{"NumNeighbours"}){
		$values[$state_idx][$_] = 0;
		$value_En[$_]->configure(-textvariable => \$values[$state_idx][$_],
					 -state        => 'normal');
	    }
	    $states[$state_idx] = join "",@self_flags;
	    $self_states_Ls->delete($state_idx);
	    $self_states_Ls->insert($state_idx, $states[$state_idx]."  ".join "  ", map{sprintf "%5.2f",$_ }@{ $values[$state_idx] });

	});

	for (0..$ns->{"NumNeighbours"}){ 

################ Fuse

	    $value_En[$_]->bind('<Return>' => sub{
		if (defined $state_idx){
		    $self_states_Ls->delete($state_idx);
		    $self_states_Ls->insert($state_idx, $states[$state_idx]."  ".join "  ", map{sprintf "%5.2f",$_ }@{ $values[$state_idx] });
		}
	    });
	    
	    $value_En[$_]->bind('<Leave>' => sub{
		if (defined $state_idx){
		    $self_states_Ls->delete($state_idx);
		    $self_states_Ls->insert($state_idx, $states[$state_idx]."  ".join "  ", map{sprintf "%5.2f",$_ }@{ $values[$state_idx] });
		}
	    });

	    $value_En[$_]->bind('<FocusOut>' => sub{
		if (defined $state_idx){
		    $self_states_Ls->delete($state_idx);
		    $self_states_Ls->insert($state_idx, $states[$state_idx]."  ".join "  ", map{sprintf "%5.2f",$_ }@{ $values[$state_idx] });
		}
	    });

##################
	}

	for my $idx (0..$#self_flags){
	    $self_rbt0[$idx]->configure(-command  => sub{
		$states[$state_idx] = join "",@self_flags;
		$self_states_Ls->delete($state_idx);
		$self_states_Ls->insert($state_idx, $states[$state_idx]."  ".join "  ", map{sprintf "%5.2f",$_ }@{ $values[$state_idx] });
	    });
	    $self_rbtQ[$idx]->configure(-command  => sub{
		$states[$state_idx] = join "",@self_flags;
		$self_states_Ls->delete($state_idx);
		$self_states_Ls->insert($state_idx, $states[$state_idx]."  ".join "  ", map{sprintf "%5.2f",$_ }@{ $values[$state_idx] });
	    });
	    $self_rbt1[$idx]->configure(-command  => sub{
		$states[$state_idx] = join "",@self_flags;
		$self_states_Ls->delete($state_idx);
		$self_states_Ls->insert($state_idx, $states[$state_idx]."  ".join "  ", map{sprintf "%5.2f",$_ }@{ $values[$state_idx] });
	    });
	}

#-------------------------------------- neighbour states Frame ---------------------------------------
	
	my $neighb_Fr = $set_re_W->LabFrame(-borderwidth => 1, 
					    -label       => 'neighbour states',
					    -labelside   => 'acrosstop'
					    )->pack(-side   => 'left',
						    -expand => '1',
						    -fill   => 'both');
	
	my $neighb_states_Ls = $neighb_Fr->Scrolled('Listbox',
						    -background   => $look{"entrybackground"},
						    -height       => '4',
#						    -width        => '20',
						    )->pack(-side   => 'top',
							    -expand => 1,
							    -fill   => 'x');	
	
	$balloon->attach($neighb_states_Ls, -msg => $messages{"RemoveState"});
	
	my $new_neighb_Bt = $neighb_Fr->Button(-text             => 'new',
					       -activeforeground => $look{"activeforeground"},
					       -activebackground => $look{"activebackground"},
					       )->pack(-side   => 'top',
						       -fill   => 'x',
						       -expand => '1');
	
	$balloon->attach($new_neighb_Bt, -msg => $messages{"NewState"});
	
#-------------------------------------------- Set state Frame ----------------------------------------
	
	my $neighb_flags_Fr = $neighb_Fr->Frame->pack(-side => 'top');
	
	$balloon->attach($neighb_flags_Fr, -msg => $messages{"SetNeighbStateModifRE"});
	
	$neighb_flags_Fr->Label(-text => '0')->grid(-row    => '0',
						  -column => '1');
	$neighb_flags_Fr->Label(-text => '?')->grid(-row    => '0',
						  -column => '2');
	$neighb_flags_Fr->Label(-text => '1')->grid(-row    => '0',
						  -column => '3');
	
	my (@neighb_rbt0,@neighb_rbtQ,@neighb_rbt1);
	for (0..$#neighb_flags){
	    $neighb_flags_Fr->Label(-text => $sim->{"ms_complexes"}{$symbol}{"Flags"}[$_]
				  # because first flag is zero and first row is 1 (cause the 0 ? 1)
				  )->grid(-row    => $_+1, 
					  -column => '0');
	    $neighb_rbt0[$_] = $neighb_flags_Fr->Radiobutton(-value    => '0',
							     -state    => 'disabled',
							     -variable => \$neighb_flags[$_],
							     )->grid(-row    => $_+1,
								     -column => '1');
	    $neighb_rbtQ[$_] = $neighb_flags_Fr->Radiobutton(-value    => '?',
							     -state    => 'disabled',
							     -variable => \$neighb_flags[$_],
							     )->grid(-row    => $_+1,
								     -column => '2');
	    $neighb_rbt1[$_] = $neighb_flags_Fr->Radiobutton(-value    => '1',
							     -state    => 'disabled',
							     -variable => \$neighb_flags[$_],
							     )->grid(-row    => $_+1,
								     -column => '3');
	}
	
#-----------------------------------------------------------------------------------------------------
#                                            Functional code
#-----------------------------------------------------------------------------------------------------

        # I use $i here and not $_ because there is another $_ for the map function. Confusing
	$neighb_states_Ls->insert('end', @{ $equilibrium{"CoupledStates"} });     
	
	$neighb_states_Ls->bind('<Button-1>' => sub{ 
	    $state_idx = $neighb_states_Ls->curselection; 
	    if (defined $state_idx){
		@neighb_flags = split "",$equilibrium{"CoupledStates"}[$state_idx];
		for (0..$#neighb_flags){
		    $neighb_rbt0[$_]->configure(-variable => \$neighb_flags[$_],
						-state    => 'normal');
		    $neighb_rbtQ[$_]->configure(-variable => \$neighb_flags[$_],
						-state    => 'normal');
		    $neighb_rbt1[$_]->configure(-variable => \$neighb_flags[$_],
						-state    => 'normal');
		}
	    }
	}
			 );

	$neighb_states_Ls->bind('<Control-Button-1>' => sub{ 
	    # IMPORTANT: test $states_Ls->curselection and NOT $state_idx
	    # Because $state_idx remains defined after usage. Here we require
	    # a new selection before deletion. 
	    if (defined $neighb_states_Ls->curselection){
		splice(@{ $equilibrium{"CoupledStates"} },[$state_idx],1);
		$neighb_states_Ls->delete('active');
		undef $state_idx;
		for (0..$#neighb_flags){
		    $neighb_rbt0[$_]->configure(-state => 'disabled');
		    $neighb_rbt0[$_]->deselect;
		    $neighb_rbtQ[$_]->configure(-state => 'disabled');
		    $neighb_rbtQ[$_]->deselect;
		    $neighb_rbt1[$_]->configure(-state => 'disabled');
		    $neighb_rbt1[$_]->deselect;
		}
	    }
	});

	$new_neighb_Bt->configure(-command => sub{ 
	    push(@{ $equilibrium{"CoupledStates"} },"");
	    $state_idx = $#{ $equilibrium{"CoupledStates"} };
	    for (0..$#neighb_flags){
		$neighb_flags[$_] = '?';
		$neighb_rbt0[$_]->configure(-variable => \$neighb_flags[$_],
					    -state    => 'normal');
		$neighb_rbtQ[$_]->configure(-variable => \$neighb_flags[$_],
					    -state    => 'normal');
		$neighb_rbt1[$_]->configure(-variable => \$neighb_flags[$_],
					    -state    => 'normal');
	    }
	    $equilibrium{"CoupledStates"}[$state_idx] = join "",@neighb_flags;
	    $neighb_states_Ls->delete($state_idx);
	    $neighb_states_Ls->insert($state_idx, $equilibrium{"CoupledStates"}[$state_idx]);
	});

	for my $idx (0..$#self_flags){
	    $neighb_rbt0[$idx]->configure(-command  => sub{
		$equilibrium{"CoupledStates"}[$state_idx] = join "",@neighb_flags;
		$neighb_states_Ls->delete($state_idx);
		$neighb_states_Ls->insert($state_idx, $equilibrium{"CoupledStates"}[$state_idx]);
	    });
	    $neighb_rbtQ[$idx]->configure(-command  => sub{
		$equilibrium{"CoupledStates"}[$state_idx] = join "",@neighb_flags;
		$neighb_states_Ls->delete($state_idx);
		$neighb_states_Ls->insert($state_idx, $equilibrium{"CoupledStates"}[$state_idx]);
	    });
	    $neighb_rbt1[$idx]->configure(-command  => sub{
		$equilibrium{"CoupledStates"}[$state_idx] = join "",@neighb_flags;
		$neighb_states_Ls->delete($state_idx);
		$neighb_states_Ls->insert($state_idx, $equilibrium{"CoupledStates"}[$state_idx]);
	    });
	}

    } else {
	$set_re_W->deiconify();
	$set_re_W->raise();
    }
}

######################################################################################################
# Opens a Window where the user can create and configure an array of complexes
#
# Structure:
# ----------
# $print_ar_W        toplevel
#     $close_Fr          -side=>'bottom'
#         Button "accept"        -side => 'left'
#         Button "delete"        -side => 'left'
#         Button "cancel"        -side => 'left'
#     $param_Fr              -side => 'bottom'
#         $ident_Fr              grid 0 0
#             Label "symbol"         grid 0 0 
#             Entry "symbol"         grid 0 1
#             Label "complex"        grid 1 0
#             $avail_cplx_Br         grid 1 1
#         $snaps_Fr              grid 0 1
#             $snaps_Br              grid 0 0
#             Button "new"           grid 0 1
#         $geom_Fr               grid 1 0
#             Optionmenu "type"      grid 0 0
#             Label "X"              grid 1 0
#             Entry "X"              grid 1 1
#             Label "Y"              grid 2 0
#             Entry "Y"              grid 2 1    
#         $misc_Fr               grid 1 1
#             Label "dump"           grid 0 0
#             Radiobutton "yes"      grid 0 1
#             Radiobutton "no"       grid 0 2
#             Label "boundary"       grid 1 0
#             Radiobutton "regular"  grid 1 1
#             Radiobutton "toroidal" grid 1 2 
#             Label "equilibration"  grid 2 0
#             Entry "equilibration"  grid 2 1
#         $descr_Fr              grid 2 0
#             Entry description      -side => 'top'
#
######################################################################################################

sub print_array{
    my ($sim,$symbol) = @_; 
# a ref to the simulation and the symbol of the array choosen in the 'arrays' BrowseEntry. $symbol can be undef
    my $oldsymbol; # used to take care when changing the name of an array (checks if no other array with the same name)

    my %array = ("Description"           => undef,
		 "Complex"               => undef,
		 "NeighbourSensitive"    => 1,
		 "Geometry"              => undef,
		 "XDimension"            => undef,
		 "YDimension"            => undef,
		 "CreateDumpFile"        => 0,
		 "BoundaryCondition"     => undef,
		 "EquilibrationInterval" => undef,
		 "Snapshot_variables"    => {},
		 "InitialState"          => "FromConcentration",
		 "InitialStateFile"      => undef,
		 "InitialStateFromTime"  => undef);
    
    if (defined $symbol){
	$oldsymbol = $symbol;
	$array{"Description"}           = $sim->{"arrays"}{$symbol}{"Description"};
	$array{"Complex"}               = $sim->{"arrays"}{$symbol}{"Complex"};
	$array{"NeighbourSensitive"}    = $sim->{"arrays"}{$symbol}{"NeighbourSensitive"};
	$array{"Geometry"}              = $sim->{"arrays"}{$symbol}{"Geometry"};
	$array{"XDimension"}            = $sim->{"arrays"}{$symbol}{"XDimension"};
	$array{"YDimension"}            = $sim->{"arrays"}{$symbol}{"YDimension"};
	$array{"CreateDumpFile"}        = $sim->{"arrays"}{$symbol}{"CreateDumpFile"};
	$array{"BoundaryCondition"}     = $sim->{"arrays"}{$symbol}{"BoundaryCondition"};
	$array{"EquilibrationInterval"} = $sim->{"arrays"}{$symbol}{"EquilibrationInterval"};
	$array{"InitialState"}          = $sim->{"arrays"}{$symbol}{"InitialState"};
	$array{"InitialStateFile"}      = $sim->{"arrays"}{$symbol}{"InitialStateFile"};
	$array{"InitialStateFromTime"}  = $sim->{"arrays"}{$symbol}{"InitialStateFromTime"};
	foreach my $snaps (keys %{ $sim->{"arrays"}{$symbol}{"Snapshot_variables"} }){
	    $array{"Snapshot_variables"}{$snaps}{"Description"}     = $sim->{"arrays"}{$symbol}{"Snapshot_variables"}{$snaps}{"Description"};
	    $array{"Snapshot_variables"}{$snaps}{"StoreInterval"}   = $sim->{"arrays"}{$symbol}{"Snapshot_variables"}{$snaps}{"StoreInterval"};
	    $array{"Snapshot_variables"}{$snaps}{"AveragedOutput"}  = $sim->{"arrays"}{$symbol}{"Snapshot_variables"}{$snaps}{"AveragedOutput"};
	    $array{"Snapshot_variables"}{$snaps}{"AverageInterval"} = $sim->{"arrays"}{$symbol}{"Snapshot_variables"}{$snaps}{"AverageInterval"};
	    $array{"Snapshot_variables"}{$snaps}{"SampleInterval"}  = $sim->{"arrays"}{$symbol}{"Snapshot_variables"}{$snaps}{"SampleInterval"};
	    $array{"Snapshot_variables"}{$snaps}{"States"}          = [ @{ $sim->{"arrays"}{$symbol}{"Snapshot_variables"}{$snaps}{"States"} } ];
	}
    } 
    
    if(!Exists(my $print_ar_W)){

#-----------------------------------------------------------------------------------------------------
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------

	$print_ar_W = $mw->Toplevel();
	$print_ar_W->resizable(0,0);
	$print_ar_W->geometry("+".$look{"xsecond"}."+".$look{"ysecond"}); 
	$print_ar_W->title(defined($symbol) ? "Array $symbol" : "new array");
	$print_ar_W->grab();
	# users cannot kill the window via decorations
	$print_ar_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$print_ar_W->transient($print_ar_W->Parent->toplevel);

#---------------------------------------- close Frame ------------------------------------------------
	my $close_Fr = $print_ar_W->Frame->pack(-side=>'bottom');
	
	$close_Fr->Button(-text             => "accept",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      
			    TOTO: {
				if ($symbol ne $oldsymbol){
				    if (exists $sim->{"arrays"}{$symbol}){
					message("error","error",$messages{"AlreadyArray"});
					last TOTO;
				    }
				}
				# erase the previous state of the array before to save the current setup.
				delete $sim->{"arrays"}{$oldsymbol};
				
				%{ $sim->{"arrays"}{$symbol} } = %array;
				foreach my $snaps (keys %{ $sim->{"arrays"}{$symbol}{"Snapshot_variables"} }){
				    %{ $sim->{"arrays"}{$symbol}{"Snapshot_variables"}{$snaps} } = %{ $array{"Snapshot_variables"}{$snaps} };
				    $sim->{"arrays"}{$symbol}{"Snapshot_variables"}{$snaps}{"States"} = [ @{ $array{"Snapshot_variables"}{$snaps}{"States"} } ];
				}				
				$print_ar_W->grabRelease();
				$print_ar_W->destroy;
			    }
			  }
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text => "cancel",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command => sub{
			      $print_ar_W->grabRelease();
			      $print_ar_W->destroy}
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text => "delete",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command => sub{
			    TOTO: {
				if ($symbol ne $oldsymbol){
				    if (exists $sim->{"arrays"}{$symbol}){
					message("error","error",$messages{"AlreadyArray"});
					last TOTO;
				    }
				}
				# erase the previous state of the array
				delete $sim->{"arrays"}{$oldsymbol};
				$print_ar_W->grabRelease();
				$print_ar_W->destroy;
			      }
			  }
			  )->pack(-side => 'left');
	$balloon->attach($close_Fr, -msg => $messages{"ArrayClose"});
	
#--------------------------------------- parameter frame ---------------------------------------------

	my $param_Fr = $print_ar_W->Frame->pack(-side => 'bottom');


#----------------------------------------- identity Frame --------------------------------------------

	my $ident_Fr = $param_Fr->LabFrame(-borderwidth => 1, 
					   -label       => 'Identity',
					   -labelside   => 'acrosstop'
					   )->grid(-row    => '0',
						   -column => '0',
						   -sticky => 'nsew');

	$ident_Fr->Label(-text => "symbol",
			 )->grid(-row    => '0',
				 -column => '0',
				 -sticky => 'nsew');

	my $symbol_En = $ident_Fr->Entry(-width        => 10,
					 -textvariable => \$symbol,
					 -foreground   => $look{"entryforeground"},
					 -background   => $look{"entrybackground"},
					 )->grid(-row    => '0',
						 -column => '1',
						 -sticky => 'nsew');

	$balloon->attach($symbol_En, -msg => $messages{"SymbolArray"});

	$ident_Fr->Label(-text => "complex",
			 )->grid(-row    => '1',
				 -column => '0',
				 -sticky => 'nsew');

        my $avail_cplx_Br = $ident_Fr->BrowseEntry(-variable  => \$array{"Complex"},
						   -state     => 'readonly',
						   -listwidth => '35',
						   -width     => '9',
						   )->grid(-row    => '1',
							   -column => '1',
							   -sticky => 'nsew');
	
	$avail_cplx_Br->configure(-listcmd => sub{
	    $avail_cplx_Br->delete(0,'end');
	    # CAUTION: Here I assume that the array contains only neighbour-sensitive complexes
	    $avail_cplx_Br->insert('end', keys %{ $sim->{"ns_complexes"} })
	    });
	
	$balloon->attach($avail_cplx_Br, -msg => $messages{"ElemCplx"});
	
#----------------------------------------- snapshots Frame -------------------------------------------

	my $snaps_Fr = $param_Fr->LabFrame(-borderwidth => 1, 
					   -label       => 'Snapshot variables',
					   -labelside   => 'acrosstop'
					   )->grid(-row    => '0',
						   -column => '1',
						   -sticky => 'nsew');

	$snaps_Fr->Button(-text             => 'new',
			  -width            => '5',
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{ print_snaps($print_ar_W,$sim,\%array) }
			  )->grid(-row    => 0,
				  -column => 1,
				  -sticky => 'w');
	
	my $snaps_L = undef;

	my $snaps_Br = $snaps_Fr->BrowseEntry(-variable  => \$snaps_L,
					      -state     => 'readonly',
					      -listwidth => '25',
					      -browsecmd => sub{ print_snaps($print_ar_W,$sim,\%array,$snaps_L) },
					      -width     => '9',
					      )->grid(-row    => 0,
						      -column => 0,
						      -sticky => 'e');
	
	$balloon->attach($snaps_Br,-msg => $messages{"SnapshotVariables"});
	
	$snaps_Br->configure(-listcmd => sub{ $snaps_Br->delete(0,'end');
					      $snaps_Br->insert('end', keys %{ $array{"Snapshot_variables"} });
					}
			    );

#----------------------------------------- geometry Frame --------------------------------------------

	my $geom_Fr = $param_Fr->LabFrame(-borderwidth => 1, 
					   -label       => 'Array geometry',
					   -labelside   => 'acrosstop'
					   )->grid(-row    => '1',
						   -column => '0',
						   -sticky => 'nsew');

	$balloon->attach($geom_Fr, -msg => $messages{"ArrayGeom"});

        my $geomtype_m = $geom_Fr->Optionmenu
	    (-options      => ["Triangle", "Square","Hexagon"],
	     -textvariable => \$array{"Geometry"},
	     -justify      => 'right',
	     )->grid(-row        => 0,
		     -column     => 0,
		     -columnspan => 2);

	$geom_Fr->Label(-text => 'X'
			)->grid(-row     => 1,
				-column  => 0,
				);
	
	$geom_Fr->Entry(-textvariable => \$array{"XDimension"},
			-foreground   => $look{"entryforeground"},
			-background   => $look{"entrybackground"},
			-width        => 8 
			)->grid(-row     => 1,
				-column  => 1,
				);

	$geom_Fr->Label(-text => 'Y',
			)->grid(-row     => 2,
				-column  => 0,
				);

	$geom_Fr->Entry(-textvariable => \$array{"YDimension"},
			-foreground => $look{"entryforeground"},
			-background => $look{"entrybackground"},
			-width        => 8 
			)->grid(-row     => 2,
				-column  => 1,
				);

#----------------------------------------- miscellaneous Frame ---------------------------------------

	my $misc_Fr = $param_Fr->LabFrame(-borderwidth => 1, 
					   -label       => 'Miscellaneous parameters',
					   -labelside   => 'acrosstop'
					   )->grid(-row    => '1',
						   -column => '1',
						   -sticky => 'nsew');

#-----------------
# DumpArray
#-----------------

	my $dump_L = $misc_Fr->Label(-text => "create dump file?"
				     )->grid(-row    => 0,
					     -column => 0);

	$balloon->attach($dump_L, -msg => $messages{"DumpArray"});

	$misc_Fr->Radiobutton(-text     => 'yes', 
			      -value    => '1', 
			      -variable => \$array{"CreateDumpFile"},
			      )->grid(-row    => 0,
				      -column => 1,
				      -sticky => 'w');
	$misc_Fr->Radiobutton(-text     => 'no', 
			       -value    => '0', 
			       -variable => \$array{"CreateDumpFile"},
			       )->grid(-row    => 0,
				       -column => 2,
				       -sticky => 'w');

#---------------------------
# InitialStateFile
#---------------------------
	
	$misc_Fr->Label(-text => 'Which file contains the initial levels?'
			)->grid(-column => '0',
				-row    => '2');
	
	my $initFile_E = $misc_Fr->Entry(-width        => 10, 
					 -foreground   => $array{"InitialState"} eq "FromDumpFile" ? $look{"entryforeground"} : $misc_Fr->cget(-foreground),
					 -background   => $array{"InitialState"} eq "FromDumpFile" ? $look{"entrybackground"} : $misc_Fr->cget(-background),
					 -state        => $array{"InitialState"} eq "FromDumpFile" ? 'normal' : 'disabled', 
					 -textvariable => \$array{"InitialStateFile"},
					 )->grid(-row    => '2',
						 -column => '1');
	
	my $initFile_Bt = $misc_Fr->Button(-text             => 'browse',
					   -activeforeground => $look{"activeforeground"},
					   -activebackground => $look{"activebackground"},	
					   -state            => $array{"InitialState"} eq "FromDumpFile" ? 'normal' : 'disabled', 
					   -command          => sub{
					       (my $initdir = $DIRCONF) =~ s!/$!!;
					       $array{"InitialStateFile"} = $mw->getOpenFile(-title      => "Choose a dumped array file",
											     -initialdir => $initdir);
					   }
					   )->grid(-row    => '2',
						   -column => '2');
	
	$balloon->attach($initFile_Bt,-msg => $messages{InitialStateFile});
	$balloon->attach($initFile_E,-msg => $messages{InitialStateFile});
	    
#-----------------------
# InitialStateFromTime
#-----------------------

	$misc_Fr->Label(-text => 'Timepoint when to get the state'
			   )->grid(-column => '0',
				   -row    => '3');
	
	my $initTime_Br = $misc_Fr->BrowseEntry(-variable  => \$array{"InitialStateFromTime"},
						-state     => $array{"InitialState"} eq "FromDumpFile" ? 'readonly' : 'disabled',
						-listwidth => '40',
						-width     => '15',
						)->grid(-column     => '1',
							-columnspan => '2',
							-row        => '3');
	
	$initTime_Br->configure(-listcmd => sub{
	    my @timepoints;
	    eval {
		open (IF, "$array{InitialStateFile}") 
		    or die "can't open the dumped file $array{InitialStateFile}: $!";
		my $line;
		$line = <IF>;
		$line =~ /^\s*TIME/i or die "Does not look like a dumped file to me!"; 
		while (defined($line = <IF>)){
		    my @split_line = split " ",$line;
		    push @timepoints, $split_line[0];
		}
		close (IF) 
		    or die "can't close the dumped file $array{InitialStateFile}: $!";
	};
	    if ($@){
		message("error","error",$@);
	    } else {		
		$initTime_Br->delete(0,'end');
		$initTime_Br->insert('end', @timepoints);
	    }
	}
				);
	
	my $initTime_Lb = $initTime_Br->Subwidget('slistbox');
	$initTime_Lb->configure( -selectbackground => $look{"activeselectionbackground"},
				 -selectforeground => $look{"activeselectionforeground"},
		     );
	
	$balloon->attach($initTime_Br,-msg => $messages{InitialStateFromTime});

#-------------------
# InitialState. COME AFTER THE TWO PREVIOUS LINE BECAUSE THE OPTIONMENU CONTROL THEIR STATE
#-------------------
	
	my $initial_L = $misc_Fr->Label(-text => "Initial state from file?"
					)->grid(-row    => 1,
						-column => 0);
	
	$balloon->attach($initial_L, -msg => $messages{"InitialState"});
	
	$misc_Fr->Optionmenu(
			     -options      => ["FromConcentration", "FromDumpFile"],
			     -textvariable => \$array{"InitialState"},
			     -justify      => 'right',
			     -command      => sub {
				 if ( $array{"InitialState"} eq "FromConcentration" ){
				     $initFile_E->configure(-state      => 'disabled',
							    -foreground => $misc_Fr->cget(-foreground),
							    -background => $misc_Fr->cget(-background),
							    );
				     $initFile_Bt->configure(-state      => 'disabled');
				     $initTime_Br->configure(-state      => 'disabled');
				     $array{"InitialStateFile"} = undef,
				     $array{"InitialStateFromTime"} = undef,
				 } elsif ( $array{"InitialState"} eq "FromDumpFile" ){
				     $initFile_E->configure(-state      => 'normal',
							    -foreground => $look{"entryforeground"},
							    -background => $look{"entrybackground"},
							    );
				     $initFile_Bt->configure(-state     => 'normal');
				     $initTime_Br->configure(-state     => 'normal');
				 }
			     }
			     )->grid(-row        => 1,
				     -column     => 1,
				     -columnspan => 2);

#-------------------
# BoundaryCondition
#-------------------
	    
	my $bound_L = $misc_Fr->Label(-text => "boundary condition:"
				      )->grid(-row    => 4,
					      -column => 0);
	
	$balloon->attach($bound_L, -msg => $messages{"BoundCondition"});

	$misc_Fr->Radiobutton(-text     => 'regular', 
			      -value    => '1', 
			      -variable => \$array{"BoundaryCondition"},
			      )->grid(-row    => 4,
				      -column => 1,
				      -sticky => 'w');

	$misc_Fr->Radiobutton(-text     => 'toroidal', 
			      -value    => '0', 
			      -variable => \$array{"BoundaryCondition"},
			      )->grid(-row    => 4,
				      -column => 2,
				      -sticky => 'w');

#-----------------------
# EquilibrationInterval
#-----------------------

	my $equint_L = $misc_Fr->Label(-text => "equilibration interval"
				     )->grid(-row    => 4,
					     -column => 0);

	$misc_Fr->Entry(-textvariable => \$array{"EquilibrationInterval"},
			-foreground   => $look{"entryforeground"},
			-background   => $look{"entrybackground"},
			-width        => 8 
			)->grid(-row        => 4,
				-column     => 1,
				-columnspan => 2,
				-sticky     => 'nswe',
				);

	$balloon->attach($equint_L, -msg => $messages{"ArrayEquilInterval"});

#--------------------------------------- description Frame -------------------------------------------
	my $descr_Fr = $param_Fr->LabFrame(-borderwidth => 1, 
					   -label       => 'Description',
					   -labelside   => 'acrosstop'
					   )->grid(-row        => '2',
						   -column     => '0',
						   -columnspan => '2',
						   -sticky     => 'nsew');

	$descr_Fr->Entry(-textvariable => \$array{"Description"},
			 -background   => $look{"entrybackground"},
			 -foreground   => $look{"entryforeground"}
			 )->pack(-side   => 'top',
				 -expand => 1,
				 -fill   => 'both');
	$balloon->attach($descr_Fr, -msg => $messages{"ArrayDesc"});

    } else {
	$print_ar_W->deiconify();
	$print_ar_W->raise();
    }
}

######################################################################################################
# Open a window to configure the snapshots of the arrays
# 
# Structure:
# ----------
# $snaps_W                    Toplevel
#     $close_Fr                   -side =>'bottom'
#         Button "accept"             -side => 'left'
#         Button "cancel"             -side => 'left'
#         Button "delete"             -side => 'left'
#     $descr_Fr                   -side =>'bottom'
#         Entry "description"         -side => 'top'
#     $param_Fr                   -side => 'bottom'
#         LabFrame 'name'             grid 0 0
#             Entry 'name'                -side   => 'top',
#         LabFrame 'states'           grid 0 1
#         LabFrame 'miscelleaneous'   grid 1 0
#             Label 'StoreInterval'       grid 0 0
#             Entry 'StoreInterval'       grid 0 2
#             Label 'AveragedOutput'      grid 1 0
#             Radiobutton 'instantaneous' grid 1 1
#             Radiobutton 'time-lapse'    grid 1 2
#             Label 'AverageInterval'     grid 2 0
#             $AvInt_E                    grid 2 2
#             Label 'SampleInterval'      grid 3 0
#             $SampInt_E                  grid 3 2
#
######################################################################################################

sub print_snaps{
    my ($parent,$sim,$array,$symbol) = @_;
    my ($oldsymbol);
    my $state_idx;
    my (@flags,@curr_flags);

    for (0..$#{ $sim->{"ms_complexes"}{$array->{"Complex"}}{"Flags"} }){
	$flags[$_] = $sim->{"ms_complexes"}{$array->{"Complex"}}{"Flags"}[$_];
    }

    my %snaps = ("Description"     => undef,
		 "SampleInterval"  => undef,
		 "AverageInterval" => undef,
		 "AveragedOutput"  => undef,
		 "StoreInterval"   => undef,
		 "States"          => []
		 );
    
    if (defined $symbol){
	$oldsymbol = $symbol;
	$snaps{"Description"}     = $array->{"Snapshot_variables"}{$symbol}{"Description"};
	$snaps{"SampleInterval"}  = $array->{"Snapshot_variables"}{$symbol}{"SampleInterval"};
	$snaps{"AverageInterval"} = $array->{"Snapshot_variables"}{$symbol}{"AverageInterval"};
	$snaps{"AveragedOutput"}  = $array->{"Snapshot_variables"}{$symbol}{"AveragedOutput"};
	$snaps{"StoreInterval"}   = $array->{"Snapshot_variables"}{$symbol}{"StoreInterval"};
	$snaps{"States"}          = [ @{ $array->{"Snapshot_variables"}{$symbol}{"States"} } ];
    }

    if(!Exists(my $snaps_W)){

#-----------------------------------------------------------------------------------------------------
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------

	$snaps_W = $mw->Toplevel();
	$snaps_W->grab();
	$parent->grabRelease();
	$snaps_W->resizable(0,0);
	$snaps_W->geometry("+".$look{"xthird"}."+".$look{"ythird"}); 
	$snaps_W->title(defined($symbol) ? "Snapshot variable $symbol" : "new snapshot variable");
	$snaps_W->grab();
	# users cannot kill the window via decorations
	$snaps_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$snaps_W->transient($snaps_W->Parent->toplevel);

#---------------------------------------- close Frame ------------------------------------------------
	my $close_Fr = $snaps_W->Frame->pack(-side =>'bottom');
	
	$close_Fr->Button(-text             => "accept",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{

			    TOTO: {
				if ($symbol ne $oldsymbol){
				    if (exists $array->{"Snapshot_variables"}{$symbol}){
					message("error","error",$messages{"AlreadySnaps"});
					last TOTO;
				    }
				}
				# erase the previous state of the array before to save the current setup.
				delete $array->{"Snapshot_variables"}{$oldsymbol};
				

				$array->{"Snapshot_variables"}{$symbol}{"Description"}     = $snaps{"Description"};
				$array->{"Snapshot_variables"}{$symbol}{"SampleInterval"}  = $snaps{"SampleInterval"};
				$array->{"Snapshot_variables"}{$symbol}{"AverageInterval"} = $snaps{"AverageInterval"};
				$array->{"Snapshot_variables"}{$symbol}{"AveragedOutput"}  = $snaps{"AveragedOutput"};
				$array->{"Snapshot_variables"}{$symbol}{"StoreInterval"}   = $snaps{"StoreInterval"};
				$array->{"Snapshot_variables"}{$symbol}{"States"}          = [ @{ $snaps{"States"} } ];
				
				$snaps_W->grabRelease();
				$parent->grab();
				$snaps_W->destroy;		
			    }	    
			  }
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text => "cancel",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command => sub{
			      $snaps_W->grabRelease();
			      $parent->grab();
			      $snaps_W->destroy}
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text => "delete",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command => sub{
			    TOTO: {
				if ($symbol ne $oldsymbol){
				    if (exists $array->{"Snapshot_variables"}{$symbol}){
					message("error","error",$messages{"AlreadySnaps"});
					last TOTO;
				    }
				}
				# erase the previous state of the array
				delete $array->{"Snapshot_variables"}{$oldsymbol};
				$snaps_W->grabRelease();
				$parent->grab();
				$snaps_W->destroy;
			    }
			    }
			  )->pack(-side => 'left');

	$balloon->attach($close_Fr, -msg => $messages{"SnapsClose"});

#-------------------------------------- parameter frame ----------------------------------------------

	my $param_Fr = $snaps_W->Frame->pack(-side =>'bottom');

#--------------------------------------- description Frame -------------------------------------------

	my $descr_Fr = $param_Fr->LabFrame(-borderwidth => 1, 
					   -label       => 'Description',
					   -labelside   => 'acrosstop'
					   )->grid(-row        => '2',
						   -column     => '0',
						   -columnspan => '2',
						   -sticky     => 'nsew');

	$descr_Fr->Entry(-textvariable => \$snaps{"Description"},
			 -background   => $look{"entrybackground"},
			 -foreground   => $look{"entryforeground"}
			 )->pack(-side   => 'top',
				 -expand => 1,
				 -fill   => 'both');
	$balloon->attach($descr_Fr, -msg => $messages{"SnapshotDesc"});

#----------------------------------------- name ------------------------------------------------------

	my $name_Fr = $param_Fr->LabFrame(-borderwidth => 1, 
					 -label       => 'Name',
					 -labelside   => 'acrosstop'
					 )->grid(-row        => '0',
						 -column     => '0',
						 -sticky     => 'nsew');
	
	$name_Fr->Entry(-textvariable => \$symbol,
			-width        => '9',
			-background   => $look{"entrybackground"},
			-foreground   => $look{"entryforeground"}
			)->pack(-side   => 'top',
				-expand => 1,
				-fill   => 'x');

	$balloon->attach($name_Fr, -msg => $messages{"SnapshotName"});

#--------------------------------------- states ------------------------------------------------------

	my $states_Fr = $param_Fr->LabFrame(-borderwidth => 1, 
					    -label       => 'States',
					    -labelside   => 'acrosstop'
					    )->grid(-row     => '0',
						    -column  => '1',
						    -rowspan => '2',
						    -sticky  => 'nsew');	


#------------------------------------------ Set state Frame ------------------------------------------

	my $set_Fr = $states_Fr->Frame()->pack(-side   => 'bottom',
					       -expand => '1',
					       -fill   => 'both');

	$balloon->attach($set_Fr, -msg => $messages{"SetFlags"});
	
	$set_Fr->Label(-text => '0')->grid(-row    => '0',
					     -column => '1');
	$set_Fr->Label(-text => '?')->grid(-row    => '0',
					     -column => '2');
	$set_Fr->Label(-text => '1')->grid(-row    => '0',
					     -column => '3');

	my (@rbt0,@rbtQ,@rbt1);
	for (0..$#flags){
	    $set_Fr->Label(-text => $flags[$_]
			     # because first flag is zero and first row is 1 (cause the 0 ? 1)
			     )->grid(-row    => $_+1, 
				     -column => '0');
	    $rbt0[$_] = $set_Fr->Radiobutton(-value    => '0',
					     -variable => \$curr_flags[$_],
					     -state    => 'disabled',
					       )->grid(-row    => $_+1,
						       -column => '1');
	    $rbtQ[$_] = $set_Fr->Radiobutton(-value    => '?',
					     -variable => \$curr_flags[$_],
					     -state    => 'disabled',
					     )->grid(-row    => $_+1,
						     -column => '2');
	    $rbt1[$_] = $set_Fr->Radiobutton(-value    => '1',
					     -variable => \$curr_flags[$_],
					     -state    => 'disabled',
					     )->grid(-row    => $_+1,
						     -column => '3');
	}
	
#----------------------------------------- All states Frame ------------------------------------------

	my $states_Ls = $states_Fr->Scrolled('Listbox',
					     -background   => $look{"entrybackground"},
					     -height => '4',
					     -width => '15',
					     )->pack(-side => 'top');	
	
	$balloon->attach($states_Ls, -msg => $messages{"RemoveState"});
	
	
	my $new_state_Bt = $states_Fr->Button(-text             => 'new',
					      -activeforeground => $look{"activeforeground"},
					      -activebackground => $look{"activebackground"},
					      )->pack(-side   => 'top',
						      -fill   => 'x',
						      -expand => '1');

#--------------------------------------------- miscellaneous -----------------------------------------

	my $misc_Fr = $param_Fr->LabFrame(-borderwidth => 1, 
					  -label       => 'Output characteristics',
					  -labelside   => 'acrosstop'
					  )->grid(-row        => '1',
						  -column     => '0',
						  -sticky     => 'nsew');
	
	my $SnapsInt_L = $misc_Fr->Label(-text => "Interval between storage"
					 )->grid(-row        => 0,
						 -column     => 0,
						 -columnspan => 2
						 );
	
	$misc_Fr->Entry(-textvariable => \$snaps{"StoreInterval"},
			-foreground   => $look{"entryforeground"},
			-background   => $look{"entrybackground"},
			-width        => 8 
			)->grid(-row    => 0,
				-column => 2,
				);
	
	$balloon->attach($SnapsInt_L, -msg => $messages{"SnapshotInterval"});
	
	my $Output_L = $misc_Fr->Label(-text => "output type"
				       )->grid(-row    => 1,
					       -column => 0,
					       );
	
	my $instant_Bt = $misc_Fr->Radiobutton(-text     => 'instantaneous', 
					       -value    => '0', 
					       -variable => \$snaps{"AveragedOutput"},
					       )->grid(-row    => 1,
						       -column => 1,
						       );
	
	my $timelapse_Bt = $misc_Fr->Radiobutton(-text     => 'time-lapse', 
						 -value    => '1', 
						 -variable => \$snaps{"AveragedOutput"},
						 )->grid(-row    => 1,
							 -column => 2);
	
	$balloon->attach($Output_L, -msg => $messages{"SnapshotType"});
	
# The following is conditioned by the type of output
	
	my $RecordDur_L = $misc_Fr->Label(-text => "recording duration"
					  )->grid(-row        => 2,
						  -column     => 0,
						  -columnspan => 2
						  );
	
	my $AvInt_E = $misc_Fr->Entry(-textvariable => \$snaps{"AverageInterval"},
				      -foreground   => $look{"entryforeground"},
				      -background   => $look{"entrybackground"},
				      -width        => 8
				      )->grid(-row    => 2,
					      -column => 2,
					      );
	
	$balloon->attach($RecordDur_L, -msg => $messages{"AverageInterval"});
	
	my $SampleInt_L = $misc_Fr->Label(-text => "interval between samples"
			     )->grid(-row        => 3,
				     -column     => 0,
				     -columnspan => 2
				     );
	
	my $SampInt_E = $misc_Fr->Entry(-textvariable => \$snaps{"SampleInterval"},
					-foreground   => $look{"entryforeground"},
					-background   => $look{"entrybackground"},
					-width        => 8 
					)->grid(-row    => 3,
						-column => 2,
						);
	
	$balloon->attach($SampleInt_L, -msg => $messages{"SampleInterval"});

#-----------------------------------------------------------------------------------------------------
#                                       Functional code 
#-----------------------------------------------------------------------------------------------------

	$instant_Bt->configure(-command  => sub{ 
	    $AvInt_E->configure(-state => 'disabled',
				-background => $misc_Fr->cget(-background));
	    $SampInt_E->configure(-state => 'disabled',
				  -background => $misc_Fr->cget(-background));
	});

	$timelapse_Bt->configure(-command  => sub{ 
	    $AvInt_E->configure(-state => 'normal',
				-background => $look{"entrybackground"});
	    $SampInt_E->configure(-state => 'normal',
				  -background => $look{"entrybackground"});				  
	});						   
	
	$states_Ls->insert('end', @{ $snaps{"States"} });     
	
	$states_Ls->bind('<Button-1>' => sub{ 
	    $state_idx = $states_Ls->curselection; 
	    if (defined $state_idx){
		@curr_flags = split "", $snaps{"States"}[$state_idx];
		for (0..$#flags){
		    $rbt0[$_]->configure(-variable => \$curr_flags[$_],
					 -state    => 'normal');
		    $rbtQ[$_]->configure(-variable => \$curr_flags[$_],
					 -state    => 'normal');
		    $rbt1[$_]->configure(-variable => \$curr_flags[$_],
					 -state    => 'normal');
		}
	    }
	});
	
	$states_Ls->bind('<Control-Button-1>' => sub{ 
	    # IMPORTANT: test $states_Ls->curselection and NOT $state_idx
	    # Because $state_idx remains defined after usage. Here we require
	    # a new selection before deletion. 
	    if (defined($states_Ls->curselection)){
		splice (@{ $snaps{"States"} },$state_idx,1);
		$states_Ls->delete('active');
		for (0..$#flags){
		    $rbt0[$_]->configure(-state => 'disabled');
		    $rbt0[$_]->deselect;
		    $rbtQ[$_]->configure(-state => 'disabled');
		    $rbtQ[$_]->deselect;
		    $rbt1[$_]->configure(-state => 'disabled');
		    $rbt1[$_]->deselect;
		}
	    }
	});

	$new_state_Bt->configure(-command => sub{ 
	    push(@{ $snaps{"States"} },"");
	    $state_idx = $#{ $snaps{"States"} };
	    for my $i (0..$#flags){
		$curr_flags[$i] = '?';
		$rbt0[$i]->configure(-variable => \$curr_flags[$i],
				     -state    => 'normal');
		$rbtQ[$i]->configure(-variable => \$curr_flags[$i],
				     -state    => 'normal');
		$rbt1[$i]->configure(-variable => \$curr_flags[$i],
				     -state    => 'normal');
	    }
	});

	for my $idx (0..$#flags){
	    $rbt0[$idx]->configure(-command  => sub{
		$snaps{"States"}[$state_idx] = join "",@curr_flags;
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$snaps{"States"}[$state_idx]);
	    });
	    $rbtQ[$idx]->configure(-command  => sub{
		$snaps{"States"}[$state_idx] = join "",@curr_flags;
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$snaps{"States"}[$state_idx]);
	    });
	    $rbt1[$idx]->configure(-command  => sub{
		$snaps{"States"}[$state_idx] = join "",@curr_flags;
		$states_Ls->delete($state_idx);
		$states_Ls->insert($state_idx,$snaps{"States"}[$state_idx]);
	    });
	}

    } else {
	$snaps_W->deiconify();
	$snaps_W->raise();
    }

}

######################################################################################################
#                                      set reactions and rates
#
# Structure:
# ----------
# $print_react_W (Toplevel)
#     $close_Fr                    pack(-side => 'bottom')
#         Button "accept"              pack(-side => 'left')
#         Button "cancel"              pack(-side => 'left')
#         Button "delete"              pack(-side => 'left')
#     $descr_Fr (Frame)            pack(-side => 'bottom');
#         Label "Description"          pack(-side => 'top')
#         Entry "Description"          pack(-side => 'top')
#     $reaction_Fr                     pack(-side => 'left')
#         $subst_Fr                    pack(-side => 'top')
#             Label "Substrates"           pack(-side => 'top')
#             $subst_Ls (Listbox)          pack(-side => 'top')
#             $setSubstStates_Bt (Button)      pack(-side => 'top')
#         $prod_Fr                     pack(-side => 'bottom')
#             Label "Products"             pack(-side => 'top')
#             $prod_Ls (Listbox)           pack(-side => 'top')
#             $setProdStates_Bt (Button)       pack(-side => 'top')
#         $kf_Fr                       pack(-side => 'left')
#             Label "kf"                   pack(-side => 'top')
#             Entry "Kf"                   pack(-side => 'top')
#             Label "dynamic value"        pack(-side => 'left')
#             OptionMenu                   pack(-side => 'right')
#         $arrow_L (Label)             pack(-side => 'left')
#         $kr_Fr                       pack(-side   => 'right')
#             Label "kr"                   pack(-side => 'top')
#             Entry "Kr"                   pack(-side => 'top')
#             Label "dynamic value"        pack(-side => 'left')
#             OptionMenu                   pack(-side => 'right')
#     $avail_cplxs_Fr              pack(-side => 'right')
#         $avail_cplxs_Ls (Listbox)    pack(-side => 'top')
#
######################################################################################################

sub print_reaction{
    my ($sim,$index) = @_;   # $index can be undef
    my ($dnd_token,$subst_idx,$prod_idx);
    my ($Fdynvalue,$Rdynvalue);

   my %reaction = ("Description" => undef, 
		   "Substrates"  => [],
		   "Products"    => [],
		   "Kf"          => 0,
		   "Kr"          => 0
		   );

    if (defined $index){
	$reaction{"Description"} = $sim->{"reactions"}[$index]{"Description"};
        $reaction{"Substrates"}  = [ @{ $sim->{"reactions"}[$index]{"Substrates"} } ];
        $reaction{"Products"}    = [ @{ $sim->{"reactions"}[$index]{"Products"} } ];
        $sim->{"reactions"}[$index]{"Kf"} =~ /([\d\.eE\-\+]+)(@(\S+))?/;
	$reaction{"Kf"}          = $1;
	$reaction{"Fdynvalue"}   = $3;

        $sim->{"reactions"}[$index]{"Kr"} =~ /([\d\.eE\-\+]+)(@(\S+))?/;
	$reaction{"Kr"}          = $1;
	$reaction{"Rdynvalue"}   = $3;
    } else {                   # the reaction does not exist. Let's create a new one 
	$index = @{ $sim->{"reactions"} };   # i.e. $#{ $sim->{"reactions"} } +1
    }
    
    if(!Exists(my $print_react_W)){
	
#-----------------------------------------------------------------------------------------------------
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------

	$print_react_W = $mw->Toplevel();
	$print_react_W->resizable(0,0);
	$print_react_W->geometry("+".$look{"xsecond"}."+".$look{"ysecond"}); 
	$print_react_W->title(defined($index) ? ("Reaction ".($index+1)) : "Reaction");
	$print_react_W->grab();
	# users cannot kill the window via decorations
	$print_react_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$print_react_W->transient($print_react_W->Parent->toplevel);

#------------------------------------------ close Frame ----------------------------------------------

	my $close_Fr = $print_react_W->Frame->pack(-side=>'bottom');
	
	$close_Fr->Button(-text             => "accept",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      my ($substOK,$prodOK) = (0,0); # mistake if the contrary not proven  
			      # check the substrates
			      foreach my $subst (@{ $reaction{"Substrates"} }){
				  $substOK++ if (not $sim->{"complexes"}{$subst}{"Constant"});
			      }
			      # check the products
			      foreach my $prod (@{ $reaction{"Products"} }){
				  $prodOK++ if (not $sim->{"complexes"}{$prod}{"Constant"});
			      }
			      if ( $substOK and $prodOK) {
				  if ($reaction{"Fdynvalue"} eq "none"){ $reaction{"Fdynvalue"} = ""};
				  if ($reaction{"Rdynvalue"} eq "none"){ $reaction{"Rdynvalue"} = ""};
				  $sim->{"reactions"}[$index]{"Kf"} = $reaction{"Fdynvalue"} ? $reaction{"Kf"}.'@'.$reaction{"Fdynvalue"} : $reaction{"Kf"};
				  $sim->{"reactions"}[$index]{"Kr"} = $reaction{"Rdynvalue"} ? $reaction{"Kr"}.'@'.$reaction{"Rdynvalue"} : $reaction{"Kr"};
				  $sim->{"reactions"}[$index]{"Substrates"}  = [ @{ $reaction{"Substrates"} } ];
				  $sim->{"reactions"}[$index]{"Products"}    = [ @{ $reaction{"Products"} } ];
				  $sim->{"reactions"}[$index]{"Description"} = $reaction{"Description"};
				  $print_react_W->grabRelease();
				  $print_react_W->destroy;
			      } else {
				  message("error","error",$messages{"CstSubstrates"}) if (not $substOK);
				  message("error","error",$messages{"CstProducts"}) if (not $prodOK);
			      }
			  }
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text             => "cancel",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{ 
			      $print_react_W->grabRelease();
			      $print_react_W->destroy}
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text             => "delete",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      # suppress the reaction itself (described in REACTION.INI)
			      splice (@{$sim->{"reactions"}},$index,1);
			      my $num = $index+1; # number of the reaction (remember, start at 1)

			      foreach my $ms (keys %{ $sim->{"ms_complexes"} }){
				  # explore the multistate one by one
				  my $react_idx;
				  for ($react_idx = 0; $react_idx < @{ $sim->{"ms_complexes"}{$ms}{"Reactions"} }; $react_idx++){
				      # explore the section reactions of the multistate, to reflect the change in 
				      # the general reactions.
				      if ($sim->{"ms_complexes"}{$ms}{"Reactions"}[$react_idx]{"Symbol"} =~ /^(\d+)([RF])/){
					  # The reaction is the right one, let's erase 
					  if ($1 == $num){
					      splice (@{ $sim->{"ms_complexes"}{$ms}{"Reactions"} },$react_idx,1);
					      $react_idx--; # ARRRGHHH. necessary cause I spliced ...
					  # The reaction had a higher index. We need to decremente it to reflect
					  # the suppression.
					  } elsif ($1 > $num){
					      $sim->{"ms_complexes"}{$ms}{"Reactions"}[$react_idx]{"Symbol"} = join("",$1 - 1,$2);
					  }
				      } else {print "something is seriously wrong function print_reaction\n" }
				  } 
			      }

			      foreach my $ns (keys %{ $sim->{"ns_complexes"} }){
				  # explore the multistate one by one
				  my $react_idx;
				  for ($react_idx = 0; $react_idx < @{ $sim->{"ns_complexes"}{$ns}{"Reactions"} }; $react_idx++){
				      # explore the section reactions of the multistate, to reflect the change in 
				      # the general reactions.
				      if ($sim->{"ns_complexes"}{$ns}{"Reactions"}[$react_idx]{"Symbol"} =~ /^(\d+)([RF])/){
					  # The reaction is the right one, let's erase 
					  if ($1 == $num){
					      splice (@{ $sim->{"ns_complexes"}{$ns}{"Reactions"} },$react_idx,1);
					      $react_idx--; # ARRRGHHH. necessary cause I spliced ...
					  # The reaction had a higher index. We need to decremente it to reflect
					  # the suppression.
					  } elsif ($1 > $num){
					      $sim->{"ns_complexes"}{$ns}{"Reactions"}[$react_idx]{"Symbol"} = join("",$1 - 1,$2);
					  }
				      } else {print "something is seriously wrong function print_reaction\n" }
				  } 
			      }

			      $print_react_W->grabRelease();
			      $print_react_W->destroy}
			  )->pack(-side => 'left');
	
	$balloon->attach($close_Fr, -msg => $messages{"ReacClose"});
	
#---------------------------------------- description Frame ------------------------------------------
# Note that this description is intended to be useful for the GUI only. Indeed 
# it will not be printed in the file COMPLEX.INI
	
	my $descr_Fr = $print_react_W->LabFrame(-borderwidth => 1, 
						-label       => 'Description',
						-labelside   => 'acrosstop'
						)->pack(-expand => '1',
							-fill   => 'both',
							-side   => 'bottom');
	
	$descr_Fr->Entry(-textvariable => \$reaction{"Description"},
			 -background   => $look{"entrybackground"},
			 -foreground   => $look{"entryforeground"}
			 )->pack(-expand => '1',
				 -fill   => 'both',
				 -side   => 'top');
	
	$balloon->attach($descr_Fr, -msg => $messages{"ReacDesc"});
	
#-----------------------------------------------------------------------------------------------------
#                              Description of the reaction 
#-----------------------------------------------------------------------------------------------------

	my $reaction_Fr = $print_react_W->LabFrame(-borderwidth => 1, 
						   -label       => 'Reaction',
						   -labelside   => 'acrosstop'
						   )->pack(-side => 'left',
							   -expand => 1,
							   -fill   => 'both');
	
#---------------------------------------- Substrates frame -------------------------------------------
# This frame prints the components currently part of the complex. We can delete
# a selected component
	
	my $subst_Fr = $reaction_Fr->Frame->pack(-side   => 'top',
						 -expand => 1,
						 -fill   => 'both');

	$subst_Fr->Label(-text => "Substrates",
			 )->pack(-side => 'top');
	
	my $subst_Ls  = $subst_Fr->Scrolled('Listbox', 
					    -background => $look{"entrybackground"},
#					    -width      => '8',
					    -height     => '2',
					    -scrollbars => "osoe"
					    )->pack(-side   => 'top',
						    -expand => 1,
						    -fill   => 'x');

	$balloon->attach($subst_Ls, -msg => $messages{"SelectSubst"});

	my $setSubstStates_Bt = $subst_Fr->Button
	    (-text    => "set states",
	     -state   => 'disabled',
	     -command => sub{ 
		if ($total_cplxs{$reaction{"Substrates"}[$subst_idx]}{'multistate'} == 1){
		    react_states( $print_react_W,
				  "F",
				  $index,
				  $reaction{"Substrates"}[$subst_idx],
				  );
		}
	    })->pack(-side   => 'top',
		     -expand => 1,
		     -fill   => 'x');

	$balloon->attach($setSubstStates_Bt, -msg => $messages{"SetFlagEffect"});

#----------------------------------------- Products frame --------------------------------------------
# This frame prints the components currently part of the complex. We can delete
# a selected component

	my $prod_Fr = $reaction_Fr->Frame->pack(-side   => 'bottom',
						-expand => 1,
						-fill   => 'both');
	
	$prod_Fr->Label(-text => "Products",
			)->pack(-side => 'top');
	
	my $prod_Ls  = $prod_Fr->Scrolled('Listbox', 
					  -background => $look{"entrybackground"},
#					  -width      => '8',
					  -height     => '2',
					  -scrollbars => "osoe"
					  )->pack(-side   => 'top',
						  -expand => 1,
						  -fill   => 'x');

	$balloon->attach($prod_Ls, -msg => $messages{"SelectProd"});

	my $setProdStates_Bt = $prod_Fr->Button
	    (-text    => "set states",
	     -state   => 'disabled',
	     -command => sub{ 
		 if ($total_cplxs{$reaction{"Products"}[$prod_idx]}{'multistate'} == 1){
		     react_states($print_react_W,
				  "R",
				  $index,
				  $reaction{"Products"}[$prod_idx],
				  );
		 }
	     })->pack(-side   => 'top',
		      -expand => 1,
		      -fill   => 'x');
		 
	$balloon->attach($setProdStates_Bt, -msg => $messages{"SetFlagEffect"});

#--------------------------------------------- Kf ----------------------------------------------------

	my $kf_Fr = $reaction_Fr->Frame->pack(-side => 'left',
					  -expand => '1');
	
	$kf_Fr->Label(-text => "kf"
		      )->pack(-side => 'top');

	my $kf_En = $kf_Fr->Entry(-textvariable => \$reaction{"Kf"},
				  -background   => $look{"entrybackground"},
				  -foreground   => $look{"entryforeground"},
				  -width        => '10',
				  )->pack(-side   => 'top',
					  -expand => 1,
					  -fill   => 'x');

	$balloon->attach($kf_En, -msg => $messages{"Kf"});

	$kf_Fr->Label(-text => "dynamic\nvalue"
		      )->pack(-side => 'left');

	my $kf_m = $kf_Fr->Optionmenu(-options  => [ "",keys %{ $sim->{"dynamic_values"} }],
			   -textvariable => \$reaction{"Fdynvalue"},
			   -justify      => 'right',
			   )->pack(-side => 'right');

	$balloon->attach($kf_m, -msg => $messages{"ChooseDynValue"});

#---------------------------------------- Double arrows ----------------------------------------------

	my $arrow_L = $reaction_Fr->Label->pack(-side => 'left',
						-anchor => 'c');

	my $dblarrow = $arrow_L->Pixmap(-file => $DIRLIB.$sep."dblarrow.xpm");
	$arrow_L->configure(-image => $dblarrow);

#--------------------------------------------- Kr ----------------------------------------------------

	my $kr_Fr = $reaction_Fr->Frame->pack(-side   => 'right',
					  -expand => '1');
	
	$kr_Fr->Label(-text => "kr"
			 )->pack(-side => 'top');

	my $kr_En = $kr_Fr->Entry(-textvariable => \$reaction{"Kr"},
				  -background   => $look{"entrybackground"},
				  -foreground   => $look{"entryforeground"},
				  -width        => '10',
				  )->pack(-side   => 'top',
					  -expand => 1,
					  -fill   => 'x');

	$balloon->attach($kr_En, -msg => $messages{"Kr"});

	$kr_Fr->Label(-text => "dynamic\nvalue"
		      )->pack(-side => 'left');

	my $kr_m = $kr_Fr->Optionmenu(-options  => [ "",keys %{ $sim->{"dynamic_values"} }],
			   -textvariable => \$reaction{"Rdynvalue"},
			   -justify      => 'right',
			   )->pack(-side => 'right');

	$balloon->attach($kr_m, -msg => $messages{"ChooseDynValue"});
	
#---------------------------------- Available complexes frame ----------------------------------------
# This frame prints the components currently available. We can add a selected 
# component to the complex 

	my $avail_cplxs_Fr = $print_react_W->LabFrame(-borderwidth => 1, 
						      -label       => "Available complexes",
						      -labelside   => 'acrosstop'
						      )->pack(-side   => 'right',
							      -expand => 1,
							      -fill   => 'both');

	my $avail_cplxs_Ls = $avail_cplxs_Fr->Scrolled('Listbox', 
						       -height     => 0,
						       -scrollbars => "osoe"
						       )->grid(-row    => 0,
							       -column => 0,
							       -sticky => 'nswe'
);
	
	$balloon->attach($avail_cplxs_Ls, -msg => $messages{"DragComp"});

#-----------------------------------------------------------------------------------------------------
#                                         Functional code 
#-----------------------------------------------------------------------------------------------------
	
	$avail_cplxs_Ls->insert("end", sort keys %{ $sim->{"complexes"} });
	$subst_Ls->insert("end",@{ $reaction{"Substrates"} });
	$prod_Ls->insert("end",@{ $reaction{"Products"} });

# Define the source for drags.
# Drags are started while pressing the left mouse button and moving the
# mouse. Then the StartDrag callback is executed.
	$dnd_token = $avail_cplxs_Ls->DragDrop
	    (-event        => '<B1-Motion>',
	     -sitetypes    => ['Local'],
	     -startcommand => sub { StartDrag($dnd_token) },
	     );

# Define the targets for drops.

	$subst_Ls->DropSite
	    (-droptypes     => ['Local'],
	     -dropcommand   => sub{
		 $subst_Ls->insert("end", $dnd_token->cget(-text));
		 $subst_Ls->see("end");
		 @{ $reaction{"Substrates"} } = $subst_Ls->get('0','end');
	     });

	$prod_Ls->DropSite
	    (-droptypes     => ['Local'],
	     -dropcommand   => sub{
		 $prod_Ls->insert("end", $dnd_token->cget(-text));
		 $prod_Ls->see("end");
		 @{ $reaction{"Products"} } = $prod_Ls->get('0','end');
	     });

#----------------- selection of a complex

	$subst_Ls->bind('<Button-1>' => sub{ 
	    $subst_idx = $subst_Ls->curselection;
	    if (defined $subst_idx){
		if ($total_cplxs{$reaction{"Substrates"}[$subst_idx]}{'multistate'} == 1){
		    $setSubstStates_Bt->configure(-state => 'normal');
		} else {
		    $setSubstStates_Bt->configure(-state => 'disabled');
		}
	    }
	});

	$prod_Ls->bind('<Button-1>' => sub{ 
	    $prod_idx = $prod_Ls->curselection;
	    if (defined $prod_idx){
		if ($total_cplxs{$reaction{"Products"}[$prod_idx]}{'multistate'} == 1){
		    $setProdStates_Bt->configure(-state => 'normal');
		} else {
		    $setProdStates_Bt->configure(-state => 'disabled');
		}
	    }
	});

#--------------------- Removal of a complex

	$subst_Ls->bind('<Control-Button-1>' => sub{ 
	    if (defined($subst_idx)){
		$subst_Ls->delete('active');
		@{ $reaction{"Substrates"} } = $subst_Ls->get('0','end');
	    }
	});	

	$prod_Ls->bind('<Control-Button-1>' => sub{ 
	    if (defined($prod_idx)){
		$prod_Ls->delete('active');
		@{ $reaction{"Products"} } = $prod_Ls->get('0','end');
	    }
	});	

    } else {
	$print_react_W->deiconify();
	$print_react_W->raise();
    }
}

######################################################################################################
# This setup the effect of a reaction on a multistate complex, as well as 
# the relative rate of the reaction according to the state
#
# Structure:
# ----------
# $react_states_W      Toplevel
#     $close_Fr             -side=>'bottom'
#        Button "accept"        -side => 'left'
#        Button "cancel"        -side => 'left'
#     $states_Nb            -side => 'top'
#        $self    (page)
#            LabEntry "description"      -side   => 'bottom'
#            $self_rate_Fr               -side   => 'right'
#                $self_states_Fr             -side   => 'top'
#                    $self_states_Ls             -side   => 'top'
#                    $self_update_states_Bt      -side   => 'right'
#                    $self_new_state_Bt          -side   => 'right'
#                $self_set_Fr                -side   => 'top'
#                    $self_flags_Fr              -side   => 'top'
#                        Label "0"                   grid 0 1
#                        Label "?"                   grid 0 2
#                        Label "1"                   grid 0 3
#                        Label "flag"                grid i 0
#                        self_rbt0[i]                grid i 1
#                        self_rbtQ[i]                grid i 2
#                        self_rbt1[i]                grid i 3
#                    $self_rate_mod 
#                        label "Relative rate:"      -side => 'right'
#                        $self_modificator (Entry)   -side => 'top'
#            $self_setclear_Fr           -side   => 'left'
#               Label "set"                  grid 0 1
#               Label "clear"                grid 0 2
#               Label "none"                 grid 0 3
#               Label "flagi"                grid i+1 0
#               Radiobutton +                grid i+1 1
#               Radiobutton -                grid i+1 2
#               Radiobutton ""               grid i+1 3
#
#        $neighbour (page)
#            LabEntry "description"      -side   => 'bottom'
#            $neighb_rate_Fr               -side   => 'right'
#                $neighb_states_Fr             -side   => 'top'
#                    $neighb_states_Ls             -side   => 'top'
#                    $neighb_update_states_Bt      -side   => 'right'
#                    $neighb_new_state_Bt          -side   => 'right'
#                $neighb_set_Fr                -side   => 'top'
#                    $neighb_flags_Fr              -side   => 'top'
#                        Label "0"                     grid 0 1
#                        Label "?"                     grid 0 2
#                        Label "1"                     grid 0 3
#                        Label "flag"                  grid i 0
#                        neighb_rbt0[i]                grid i 1
#                        neighb_rbtQ[i]                grid i 2
#                        neighb_rbt1[i]                grid i 3
#                    $neighb_rate_mod 
#                        label "Relative rate:"        -side => 'right'
#                        $neighb_modificator (Entry)   -side => 'top'
#            $neighb_setclear_Fr            -side   => 'left'
#               Label "set"                  grid 0 1
#               Label "clear"                grid 0 2
#               Label "none"                 grid 0 3
#               Label "flagi"                grid i+1 0
#               Radiobutton +                grid i+1 1
#               Radiobutton -                grid i+1 2
#               Radiobutton ""               grid i+1 3
#
######################################################################################################

sub react_states{
    my ($parent,$sense,$index,$symbol) = @_; 
    # sense is 'F' for forward and 'R' for reverse
    # $index is the reaction index in $sim->{"reactions"}, not in $ms->{"Reactions"}
    # CAUTION: the symbol of reaction will be $index+1 because the first in indexed 1 in INI files
    # $symbol is the symbol of a complex
    
    my %react;               # local copy of the reaction parameters. Avoid unwanted changes.
                             # Two keys:  'myself' contains all the modifications of the multistae itself
                             #            'neighbour' contains all the modifications of the neighbour
    my @self_flags_effect;   # value of flag changes for the multistate itself (+, - or "")
    my @neighb_flags_effect; # value of flag changes for the neighbour (+, - or "")        
    my @self_curr_flags;     # current state of the flags of the multistate itself         
    my @neighb_curr_flags;   # current state of the flags of the neighbour                 
    my ($self_state_idx,$neighb_state_idx);
    my (%states,%values,%dynvalues);

# IMPORTANT:
# In the multistate complex file, the specified changes affect the multistate itself.
# In the neighbour sensitive file, the specified changes affect one of the neighbours.
# Note that the flags of the ms complex and its affected neighbours are not obligatory 
# the same (name and numbers) even if in the current version of StochSim (17/Jul/2001)
# an array can contain only one kind of multistate.

    # In StochSim the numbering of items begin at 1.
    # It is important to keep the symbol in both hashes
    $react{"myself"}{"Symbol"} = ($index+1).$sense;
    $react{"neighbour"}{"Symbol"} = $react{"myself"}{"Symbol"};

    # Screen the multistate complex for the current reaction
    my $issetup = 0;
    foreach my $reaction (@{ $sim->{"ms_complexes"}{$symbol}{"Reactions"} }){
	if ($reaction->{"Symbol"} eq $react{"myself"}{"Symbol"}){
	    $issetup = 1;
	    $react{"myself"}{"Description"} = $reaction->{"Description"};
	    $react{"myself"}{"Effect"}      = [ @{ $reaction->{"Effect"} } ];
	    foreach (sort keys %{ $reaction }){
		# One does not populate the react hash with the states and their values
		# otherwise, we can't remove them easely later on.
		if (/^[01\?]+$/){ 
		    push(@{ $states{"myself"} }, $_);
		    $reaction->{$_} =~ /([\d\.eE\-\+]+)(@(.+))?/;
		    push(@{ $values{"myself"} }, $1);
		    push(@{ $dynvalues{"myself"} }, $3);
		}
	    }
	    last;
	}
    }
    
    for my $flg_idx (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
	foreach my $effect (@{ $react{"myself"}{"Effect"} }){
	    $effect =~ /(\+|-)(\w+)/;
	    if ($2 eq $sim->{"ms_complexes"}{$symbol}{"Flags"}[$flg_idx]){
		$self_flags_effect[$flg_idx] = $1;
	    }
	}
    }
    
    # OK because current arrays contain only one type of complex ...
    for (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
	$self_curr_flags[$_] = "";
    }

    if ($sim->{"general_config"}{"UseSpatialExtensions"}){
	# Screen the neighbour sensitive complex for the current reaction
	$issetup = 0;
	foreach my $reaction (@{ $sim->{"ns_complexes"}{$symbol}{"Reactions"} }){
	    
	    if ($reaction->{"Symbol"} eq $react{"neighbour"}{"Symbol"}){
		$issetup = 1;
		$react{"neighbour"}{"ReactNeighbourID"}  = $reaction->{"ReactNeighbourID"};
		$react{"neighbour"}{"EffectOnNeighbour"} = [ @{ $reaction->{"EffectOnNeighbour"} } ];
		while (my ($key,$value) = each %{ $reaction }){
		    # One does not populate the react hash with the states and their values
		    # otherwise, we can't remove them easely later on.
		    if ($key =~ /^(0|1|\?)+$/){ 
			push(@{ $states{"neighbour"} }, $key);
			push(@{ $values{"neighbour"} }, $value);
		    }
		}
		last;
	    }
	}

	# Currently, all NS are the same. In futur the flags have to be determined from the neighbour
	for my $flg_idx (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
	    foreach my $effect (@{ $react{"neighbour"}{"EffectOnNeighbour"} }){
		$effect =~ /(\+|-)(\w+)/;
		if ($2 eq $sim->{"ms_complexes"}{$symbol}{"Flags"}[$flg_idx]){
		    $neighb_flags_effect[$flg_idx] = $1;
		}
	    }
	}

	# OK because current arrays contain only one type of complex ...
	for (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
	    $neighb_curr_flags[$_] = "";
	}
    }    

    if(!Exists(my $react_states_W)){
	
#---------------------------------------------------------------------------------------------------- 
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------
	
	$react_states_W = $mw->Toplevel();
	$react_states_W->resizable(0,0);
	$react_states_W->geometry("+".$look{"xthird"}."+".$look{"ythird"}); 
	$react_states_W->title("Reciprocal effects between reaction and states");
	$react_states_W->grab();
	$parent->grabRelease();
	# users cannot kill the window via decorations
	$react_states_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$react_states_W->transient($react_states_W->Parent->toplevel);

#---------------------------------------------- close Frame ------------------------------------------

	my $close_Fr = $react_states_W->Frame->pack(-side=>'bottom');

	$close_Fr->Button(-text             => "accept",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{
			      
			      #---------------------------------------------- The multistate itself -------------------------------------------
			      $react{"myself"}{"Effect"} = [];
			    SWITCH: {
				my $effects = join("",@self_flags_effect);
				if ($effects !~ /(\+|-)/ and scalar @{ $states{"myself"} } == 0){
				    deleteMS($sim,$react{"myself"}{"Symbol"},$symbol);
				    last SWITCH;         # removal of the MS reaction and bye bye. The following is ignored
				}
				
				if (scalar @{ $states{"myself"} } == 0){
				    $states{"myself"}[0] =  '?'x scalar @{ $sim->{"ms_complexes"}{$symbol}{"Flags"} };
				    $values{"myself"}[0] = 1;
				}
				
				for (0..$#self_flags_effect){
				    if (defined $self_flags_effect[$_] and $self_flags_effect[$_] =~ /^(\+|-)$/){
					push(@{ $react{"myself"}{"Effect"} },$self_flags_effect[$_].$sim->{"ms_complexes"}{$symbol}{"Flags"}[$_] );
				    }
				}	
				
				foreach (0..$#{ $states{"myself"} }){ 
				    $react{"myself"}{$states{"myself"}[$_]} = $dynvalues{"myself"}[$_] ? $values{"myself"}[$_].'@'.$dynvalues{"myself"}[$_] : $values{"myself"}[$_]; 
				}

				my $found = 0; 
				for (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Reactions"} }){
				    if ( $react{"myself"}{"Symbol"} eq $sim->{"ms_complexes"}{$symbol}{"Reactions"}[$_]{"Symbol"} ){
					$found = 1;      # The reaction is already known
					undef %{ $sim->{"ms_complexes"}{$symbol}{"Reactions"}[$_] };   # necessary to erase the old states				      
					while (my ($key,$value) = each %{ $react{"myself"} }){
					    $sim->{"ms_complexes"}{$symbol}{"Reactions"}[$_]{$key} = $value;
					}
					$sim->{"ms_complexes"}{$symbol}{"Reactions"}[$_]{"Effect"} = [ @{ $react{"myself"}{"Effect"} } ];
					last;
				    }
				}
				if (not $found){ # the modification of the reaction is a new one
				    my $reac_idx = $#{ $sim->{"ms_complexes"}{$symbol}{"Reactions"} } +1;  # Addition of a reaction after the last one
				    $sim->{"ms_complexes"}{$symbol}{"Reactions"}[$reac_idx]{"Symbol"} = $react{"myself"}{"Symbol"};
				    while (my ($key,$value) = each %{ $react{"myself"} }){
					$sim->{"ms_complexes"}{$symbol}{"Reactions"}[$reac_idx]{$key} = $value;
				    }
				    $sim->{"ms_complexes"}{$symbol}{"Reactions"}[$reac_idx]{"Effect"} = [ @{ $react{"myself"}{"Effect"} } ];
				}
			    }

			      #---------------------------------------------- The neighbour -------------------------------------------
			      
			      if ($sim->{"general_config"}{"UseSpatialExtensions"} and $total_cplxs{$symbol}{'neighboursensitive'} == 1){
				  $react{"neighbour"}{"EffectOnNeighbour"} = [];

				  my $effectsonneighb = join("",@neighb_flags_effect);
				  if ($effectsonneighb !~ /(\+|-)/ and scalar @{ $states{"neighbour"} } == 0){
				      deleteNS($sim,$react{"neighbour"}{"Symbol"},$symbol);
				      last SWITCH;         # removal of the MS reaction and bye bye. The following is ignored
				  }

				  if (scalar @{ $states{"neighbour"} } == 0){
				      $states{"neighbour"}[0] =  '?'x scalar @{ $sim->{"ms_complexes"}{$symbol}{"Flags"} };
				      $values{"neighbour"}[0] = 1;
				  }
				  
				  for (0..$#neighb_flags_effect){
				      if (defined $neighb_flags_effect[$_] and $neighb_flags_effect[$_] =~ /^(\+|-)$/){
					  push(@{ $react{"neighbour"}{"EffectOnNeighbour"} },$neighb_flags_effect[$_].$sim->{"ms_complexes"}{$symbol}{"Flags"}[$_] );
				      }
				  }
				  for (0..$#{ $states{"neighbour"} }){ 
				      $react{"neighbour"}{$states{"neighbour"}[$_]} = $values{"neighbour"}[$_]; 
				  }
				  if (scalar @{ $react{"neighbour"}{"EffectOnNeighbour"} }){ # OK, some flags are affected by the reaction
				      my $found = 0; 
				      for (0..$#{ $sim->{"ns_complexes"}{$symbol}{"Reactions"} }){
					  if ( $react{"neighbour"}{"Symbol"} eq $sim->{"ns_complexes"}{$symbol}{"Reactions"}[$_]{"Symbol"} ){
					      $found = 1; # The reaction is already known
					      undef %{ $sim->{"ns_complexes"}{$symbol}{"Reactions"}[$_] };
					      while (my ($key,$value) = each %{ $react{"neighbour"} }){					      
						  $sim->{"ns_complexes"}{$symbol}{"Reactions"}[$_]{$key} = $value;
					      }
					      $sim->{"ns_complexes"}{$symbol}{"Reactions"}[$_]{"EffectOnNeighbour"} = [ @{ $react{"neighbour"}{"EffectOnNeighbour"} } ];
					      last;
					  }
				      }
				      if (not $found){ # the modification of the reaction is a new one
					  my $reac_idx = $#{ $sim->{"ns_complexes"}{$symbol}{"Reactions"} } +1;  # Addition of a reaction after the last one
					  $sim->{"ns_complexes"}{$symbol}{"Reactions"}[$reac_idx]{"Symbol"} = $react{"neighbour"}{"Symbol"};
					  while (my ($key,$value) = each %{ $react{"neighbour"} }){
					      $sim->{"ns_complexes"}{$symbol}{"Reactions"}[$reac_idx]{$key} = $value;
					  }
					  $sim->{"ns_complexes"}{$symbol}{"Reactions"}[$reac_idx]{"EffectOnNeighbour"} = [ @{ $react{"neighbour"}{"EffectOnNeighbour"} } ];
				      }
				  }		
			      }	      
			      $react_states_W->grabRelease();
			      $parent->grab();
			      $react_states_W->destroy;}
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text             => "cancel",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{ 
			      $react_states_W->grabRelease();
			      $parent->grab();
			      $react_states_W->destroy}
			  )->pack(-side => 'left');
	
	$close_Fr->Button(-text => "delete MS",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command => sub{
			      deleteMS($sim,$react{"myself"}{"Symbol"},$symbol);
			      $react_states_W->grabRelease();
			      $parent->grab();
			      $react_states_W->destroy;
			  }
			  )->pack(-side => 'left');

	$close_Fr->Button(-text => "delete NS",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command => sub{
			      deleteNS($sim,$react{"neighbour"}{"Symbol"},$symbol);
			      $react_states_W->grabRelease();
			      $parent->grab();
			      $react_states_W->destroy;
			  }
			  )->pack(-side => 'left');
#------------------------------------------- The NoteBook ------------------------------------------- 
	
	my $states_Nb = $react_states_W->NoteBook()->pack(-side => 'top');
	my $self      = $states_Nb->add("self", -label => "self");
	
#----------------------------------------------------------------------------------------------------
#                                             Page "self" 
#----------------------------------------------------------------------------------------------------
	
	my $self_descr = $self->LabEntry(-label        => "Description",
					 -textvariable => \$react{"myself"}{"Description"},
					 -background   => $look{"entrybackground"},
					 -foreground   => $look{"entryforeground"}
					 )->pack(-side   => 'bottom',
						 -fill   => 'both',
						 -expand => '1');
	
	my $self_desc_L = $self_descr->Subwidget('label');
	$self_desc_L->configure(-background => $self->cget(-background));

#------------------------------------------- Relative rates ------------------------------------------

	my $self_rate_Fr = $self->LabFrame(-borderwidth    => 1, 
					   -label          => 'Relative rates',
					   -labelside      => 'acrosstop'
					   )->pack(-side   => 'right',
						   -expand => '1', 
						   -fill   => 'both');
	
#------------------------------------------- All states Frame ----------------------------------------
	
	my $self_states_Fr = $self_rate_Fr->Frame()->pack(-side   => 'top',
							  -expand => '1',
							  -fill   => 'both');
	
	my $self_states_Ls = $self_states_Fr->Scrolled('Listbox',
						       -background => $look{"entrybackground"},
						       -height     => '4',
#						       -width      => '15',
						       )->pack(-side   => 'top',
							       -expand => '1',
							       -fill   => 'x');	
	
	$balloon->attach($self_states_Ls, -msg => $messages{"RemoveState"});
	
	
	my $self_new_state_Bt = $self_states_Fr->Button(-text             => 'new',
						       -activeforeground => $look{"activeforeground"},
						       -activebackground => $look{"activebackground"},
						       )->pack(-side   => 'right',
							       -fill   => 'x',
							       -expand => '1',
							       -anchor => 'w');
	
	$balloon->attach($self_new_state_Bt, -msg => $messages{"NewState"});
	
#--------------------------------------------- Set state Frame ---------------------------------------
	
	my $self_set_Fr = $self_rate_Fr->Frame()->pack(-side   => 'top',
						       -expand => '1',
						       -fill   => 'both');
	
	
	my $self_flags_Fr = $self_set_Fr->Frame->pack(-side => 'left');
	
	$balloon->attach($self_flags_Fr, -msg => $messages{"SetStateModifReac"});
	
	$self_flags_Fr->Label(-text => '0')->grid(-row    => '0',
						  -column => '1');
	$self_flags_Fr->Label(-text => '?')->grid(-row    => '0',
						 -column => '2');
	$self_flags_Fr->Label(-text => '1')->grid(-row    => '0',
						  -column => '3');
	
	my (@self_rbt0,@self_rbtQ,@self_rbt1);
	for (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
	    $self_flags_Fr->Label(-text =>$sim->{"ms_complexes"}{$symbol}{"Flags"}[$_]
				  # because first flag is zero and first row is 1 (cause the 0 ? 1)
				  )->grid(-row    => $_+1, 
					  -column => '0');
	    $self_rbt0[$_] = $self_flags_Fr->Radiobutton(-value    => '0',
							 -variable => \$self_curr_flags[$_],
							 -state    => 'disabled',
							 )->grid(-row    => $_+1,
								 -column => '1');
	    $self_rbtQ[$_] = $self_flags_Fr->Radiobutton(-value    => '?',
							 -variable => \$self_curr_flags[$_],
							 -state    => 'disabled',
							 )->grid(-row    => $_+1,
								 -column => '2');
	    $self_rbt1[$_] = $self_flags_Fr->Radiobutton(-value    => '1',
							 -variable => \$self_curr_flags[$_],
							 -state    => 'disabled',
							 )->grid(-row    => $_+1,
								 -column => '3');
	    
	}
	my $self_rate_mod = $self_set_Fr->Frame->pack(-side => 'right');
	
	$self_rate_mod->Label(-text => "Relative rate:"
			      )->pack(-side => 'top');
	
	my $self_modificator = $self_rate_mod->Entry(-state        => 'disabled',
						     -foreground   => $look{"entryforeground"},
						     -background   => $look{"entrybackground"},
						     )->pack(-side   => 'top',
							     -expand => 1,
							     -fill   => 'x');
	
	$balloon->attach($self_modificator, -msg => $messages{"Modificator"});
	$self_rate_mod->Label(-text => "dynamic\nvalue"
			      )->pack(-side => 'left');
	
	my $dyn_m = $self_rate_mod->Optionmenu(-options  => [ "",keys %{ $sim->{"dynamic_values"} }],
					       -justify  => 'right',
					       )->pack(-side => 'right');
	
	$balloon->attach($dyn_m, -msg => $messages{"ChooseDynValue"});
	
#-------------------------------------------- Clear/Set flags ----------------------------------------
	
	my $self_setclear_Fr = $self->LabFrame(-borderwidth    => 1, 
					      -label          => 'Set/Clear flags',
					      -labelside      => 'acrosstop'
					      )->pack(-side   => 'left', 
						      -expand => '1', 
						      -fill   => 'both');
	
	$balloon->attach($self_setclear_Fr, -msg => $messages{"SetClear"});
	
	$self_setclear_Fr->Label(-text => 'set')->grid(-row    => '0',
						       -column => '1');
	$self_setclear_Fr->Label(-text => 'clear')->grid(-row    => '0',
							 -column => '2');
	$self_setclear_Fr->Label(-text => 'none')->grid(-row    => '0',
							-column => '3');
	
	for my $flag_idx (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
	    $self_setclear_Fr->Label(-text => $sim->{"ms_complexes"}{$symbol}{"Flags"}[$flag_idx]
				     )->grid(-row    => $flag_idx+1, 
					     -column => '0');
	    
	    # If the flag is controlled by a rapid equilibrium, we disable the possible control 
	    my $bingo1 = 0;
	    foreach my $re (@{ $sim->{"ms_complexes"}{$symbol}{"Rapid_equilibriums"} }){
		$bingo1++ if ($re->{"Flag"} eq $sim->{"ms_complexes"}{$symbol}{"Flags"}[$flag_idx]);
	    }
	    if ($sim->{"general_config"}{"UseSpatialExtensions"} and defined $sim->{"ns_complexes"}{$symbol}){
		foreach my $re (@{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"} }){
		    $bingo1++ if ($re->{"Flag"} eq $sim->{"ms_complexes"}{$symbol}{"Flags"}[$flag_idx]);
		}
	    }
	    if ($bingo1){
		$self_setclear_Fr->Radiobutton(-value    => "",
					       -variable => \$self_flags_effect[$flag_idx],
					      )->grid(-row    => $flag_idx+1,
						      -column => '3');
	    } else {
		$self_setclear_Fr->Radiobutton(-value    => '+',
					      -variable => \$self_flags_effect[$flag_idx],
					      )->grid(-row    => $flag_idx+1,
						      -column => '1');
		$self_setclear_Fr->Radiobutton(-value    => '-',
					      -variable => \$self_flags_effect[$flag_idx],
					      )->grid(-row    => $flag_idx+1,
						      -column => '2');
		$self_setclear_Fr->Radiobutton(-value    => "",
					      -variable => \$self_flags_effect[$flag_idx],
					      )->grid(-row    => $flag_idx+1,
						      -column => '3');
	    }	
	}

#-----------------------------------------------------------------------------------------------------
#                                           Functionnal code
#-----------------------------------------------------------------------------------------------------
	
	for my $i (0..$#{ $states{"myself"} }){
	    $self_states_Ls->insert('end', $states{"myself"}[$i]." ".sprintf("%6s",$values{"myself"}[$i]));
	}
	
	$self_states_Ls->bind('<Button-1>' => sub{ 
	    $self_state_idx = $self_states_Ls->curselection; 
	    if (defined $self_state_idx){
		$self_modificator->configure(-textvariable => \$values{"myself"}[$self_state_idx],
					     -state        => 'normal');
		$dyn_m->configure(-textvariable => \$dynvalues{"myself"}[$self_state_idx]);
		@self_curr_flags = split "",$states{"myself"}[$self_state_idx];
		for (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
		    $self_rbt0[$_]->configure(-variable => \$self_curr_flags[$_],
					      -state    => 'normal');
		    $self_rbtQ[$_]->configure(-variable => \$self_curr_flags[$_],
					      -state    => 'normal');
		    $self_rbt1[$_]->configure(-variable => \$self_curr_flags[$_],
					      -state    => 'normal');
		}
	    }
	});
	
	$self_states_Ls->bind('<Control-Button-1>' => sub{ 
	    # IMPORTANT: test $states_Ls->curselection and NOT $self_state_idx
	    # Because $self_state_idx remains defined after usage. Here we require
	    # a new selection before deletion. 
	    if (defined $self_states_Ls->curselection){
		splice (@{ $states{"myself"} },$self_state_idx,1);
		splice (@{ $values{"myself"} },$self_state_idx,1);
		$self_states_Ls->delete('active');
		undef $self_state_idx;
		for (0..$#self_curr_flags){
		    $self_rbt0[$_]->configure(-state => 'disabled');
		    $self_rbt0[$_]->deselect;
		    $self_rbtQ[$_]->configure(-state => 'disabled');
		    $self_rbtQ[$_]->deselect;
		    $self_rbt1[$_]->configure(-state => 'disabled');
		    $self_rbt1[$_]->deselect;
		}
	    }
	});
	
############## Fuse

	$self_modificator->bind('<Leave>' => sub{
	    if (defined $self_state_idx){
		$states{"myself"}[$self_state_idx] = join "", @self_curr_flags;
		$self_states_Ls->delete($self_state_idx);
		$self_states_Ls->insert($self_state_idx, $states{"myself"}[$self_state_idx]." ".sprintf("%6s",$values{"myself"}[$self_state_idx]));
	    }
	});

	$self_modificator->bind('<Return>' => sub{
	    if (defined $self_state_idx){
		$states{"myself"}[$self_state_idx] = join "", @self_curr_flags;
		$self_states_Ls->delete($self_state_idx);
		$self_states_Ls->insert($self_state_idx, $states{"myself"}[$self_state_idx]." ".sprintf("%6s",$values{"myself"}[$self_state_idx]));
	    }
	});
	
	$self_modificator->bind('<FocusOut>' => sub{
	    if (defined $self_state_idx){
		$states{"myself"}[$self_state_idx] = join "", @self_curr_flags;
		$self_states_Ls->delete($self_state_idx);
		$self_states_Ls->insert($self_state_idx, $states{"myself"}[$self_state_idx]." ".sprintf("%6s",$values{"myself"}[$self_state_idx]));
	    }
	});

#################

	$self_new_state_Bt->configure(-command => sub{ 
	    push(@{ $states{"myself"} },"");
	    push(@{ $values{"myself"} },1);
	    $self_state_idx = $#{ $states{"myself"} };
	    for (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
		$self_curr_flags[$_] = '?';
		$self_rbt0[$_]->configure(-variable => \$self_curr_flags[$_],
					  -state    => 'normal');
		$self_rbtQ[$_]->configure(-variable => \$self_curr_flags[$_],
					  -state    => 'normal');
		$self_rbt1[$_]->configure(-variable => \$self_curr_flags[$_],
					  -state    => 'normal');
	    }
	    $states{"myself"}[$self_state_idx] = join "", @self_curr_flags;
	    $self_states_Ls->insert('end', $states{"myself"}[$self_state_idx]." ".sprintf("%6s",$values{"myself"}[$self_state_idx]));
	    $self_modificator->configure(-textvariable => \$values{"myself"}[$self_state_idx],
					 -state        => 'normal'
					 );
	});
	
	for (0..$#self_curr_flags){
	    $self_rbt0[$_]->configure(-command  => sub{
		$states{"myself"}[$self_state_idx] = join "", @self_curr_flags;
		$self_states_Ls->delete($self_state_idx);
		$self_states_Ls->insert($self_state_idx, $states{"myself"}[$self_state_idx]." ".sprintf("%6s",$values{"myself"}[$self_state_idx]));
	    });
	    $self_rbtQ[$_]->configure(-command  => sub{
		$states{"myself"}[$self_state_idx] = join "", @self_curr_flags;
		$self_states_Ls->delete($self_state_idx);
		$self_states_Ls->insert($self_state_idx, $states{"myself"}[$self_state_idx]." ".sprintf("%6s",$values{"myself"}[$self_state_idx]));
	    });
	    $self_rbt1[$_]->configure(-command  => sub{
		$states{"myself"}[$self_state_idx] = join "", @self_curr_flags;
		$self_states_Ls->delete($self_state_idx);
		$self_states_Ls->insert($self_state_idx, $states{"myself"}[$self_state_idx]." ".sprintf("%6s",$values{"myself"}[$self_state_idx]));
	    });
	}

#----------------------------------------------------------------------------------------------------
#                                             end of page "self" 
#----------------------------------------------------------------------------------------------------

	if ($total_cplxs{$symbol}{'neighboursensitive'} == 1){
	    my $neighbour = $states_Nb->add("neighbour", 
					    -label => "neighbour",
					    );
	    
#----------------------------------------------------------------------------------------------------
#                                             Page "neighbour" 
#----------------------------------------------------------------------------------------------------

	    my $neighb_descr = $neighbour->LabEntry(-label        => "Description",
						    -textvariable => \$react{"neighbour"}{"Description"},
						    -background   => $look{"entrybackground"},
						    -foreground   => $look{"entryforeground"}
						    )->pack(-side   => 'bottom',
							    -fill   => 'both',
							    -expand => '1');
	    
	    my $neighb_desc_L = $neighb_descr->Subwidget('label');
	    $neighb_desc_L->configure(-background => $neighbour->cget(-background));
	    
#------------------------------------------- Relative rates ------------------------------------------
	    
	    my $neighb_rate_Fr = $neighbour->LabFrame(-borderwidth    => 1, 
						      -label          => 'Relative rates',
						      -labelside      => 'acrosstop'
						      )->pack(-side   => 'right',
							      -expand => '1', 
							      -fill   => 'both');
	
	    
#------------------------------------------- All states Frame ----------------------------------------
	    
	    my $neighb_states_Fr = $neighb_rate_Fr->Frame()->pack(-side   => 'top',
								  -expand => '1',
								  -fill   => 'both');
	
	    my $neighb_states_Ls = $neighb_states_Fr->Scrolled('Listbox',
							       -background   => $look{"entrybackground"},
							       -height => '4',
#							       -width => '15',
							       )->pack(-side   => 'top',
								       -expand => '1',
								       -fill   => 'x');	
	    
	    $balloon->attach($neighb_states_Ls, -msg => $messages{"RemoveState"});
	    
	    my $neighb_new_state_Bt = $neighb_states_Fr->Button(-text             => 'new',
								-activeforeground => $look{"activeforeground"},
							    -activebackground => $look{"activebackground"},
								)->pack(-side   => 'right',
									-fill   => 'x',
									-expand => '1',
									-anchor => 'w');
	    
	    $balloon->attach($neighb_new_state_Bt, -msg => $messages{"NewState"});
	    
#--------------------------------------------- Set state Frame ---------------------------------------
	    
	    my $neighb_set_Fr = $neighb_rate_Fr->Frame()->pack(-side   => 'top',
							       -expand => '1',
							       -fill   => 'both');
	    
	    
	    my $neighb_flags_Fr = $neighb_set_Fr->Frame->pack(-side => 'left');
	    
	    $balloon->attach($neighb_flags_Fr, -msg => $messages{"SetStateModifReac"});
	    
	    $neighb_flags_Fr->Label(-text => '0')->grid(-row    => '0',
							-column => '1');
	    $neighb_flags_Fr->Label(-text => '?')->grid(-row    => '0',
							-column => '2');
	    $neighb_flags_Fr->Label(-text => '1')->grid(-row    => '0',
							-column => '3');
	    
	    my (@neighb_rbt0,@neighb_rbtQ,@neighb_rbt1);
	    for (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
		$neighb_flags_Fr->Label(-text =>$sim->{"ms_complexes"}{$symbol}{"Flags"}[$_]
					# because first flag is zero and first row is 1 (cause the 0 ? 1)
					)->grid(-row    => $_+1, 
						-column => '0');
		$neighb_rbt0[$_] = $neighb_flags_Fr->Radiobutton(-value    => '0',
								 -variable => \$neighb_curr_flags[$_],
								 -state    => 'disabled',
								 )->grid(-row    => $_+1,
									 -column => '1');
		$neighb_rbtQ[$_] = $neighb_flags_Fr->Radiobutton(-value    => '?',
								 -variable => \$neighb_curr_flags[$_],
								 -state    => 'disabled',
								 )->grid(-row    => $_+1,
									 -column => '2');
		$neighb_rbt1[$_] = $neighb_flags_Fr->Radiobutton(-value    => '1',
								 -variable => \$neighb_curr_flags[$_],
								 -state    => 'disabled',
								 )->grid(-row    => $_+1,
									 -column => '3');
	    }
	    my $neighb_rate_mod = $neighb_set_Fr->Frame->pack(-side => 'right');
	    
	    $neighb_rate_mod->Label(-text => "Relative rate:"
				    )->pack(-side => 'top');
	    my $neighb_modificator = $neighb_rate_mod->Entry(-state        => 'disabled',
							     -foreground   => $look{"entryforeground"},
							     -background   => $look{"entrybackground"},
							     )->pack(-side   => 'top',
								     -expand => 1,
								     -fill   => 'x');
	    # FIXME: CHECK THAT BETWEEN 0 AND 1
	    $balloon->attach($neighb_modificator, -msg => $messages{"Modificator"});
	    
	    
#------------------------------------------- identity of the neighbour affected ---------------------
	    # NOTE: If we want to implement different NS per arrays, an action on those 
	    # radiobuttons has to redraw the page according to the correct flags
	    
	    my $neighb_id_Fr = $neighbour->LabFrame(-borderwidth    => 1, 
						    -label          => 'which neighbour?',
						    -labelside      => 'acrosstop'
						    )->pack(-side   => 'bottom', 
							    -expand => '1', 
							    -fill   => 'both');

	    $balloon->attach($neighb_id_Fr, -msg => $messages{"ChooseNeighbour"});

	    if ($sim->{"ns_complexes"}{$symbol}{"NumNeighbours"} == 3){

		$neighb_id_Fr->Radiobutton(-value    => '0',
					   -variable => \$react{"neighbour"}{"ReactNeighbourID"},
					   )->grid(-row    => 0,
						   -column => 1);
		$neighb_id_Fr->Radiobutton(-value    => '1',
					   -variable => \$react{"neighbour"}{"ReactNeighbourID"},
					   )->grid(-row    => 2,
						   -column => 2);
		$neighb_id_Fr->Radiobutton(-value    => '2',
					   -variable => \$react{"neighbour"}{"ReactNeighbourID"},
					   )->grid(-row    => 2,
						   -column => 0);
		my $triskel_L = $neighb_id_Fr->Label()->grid(-row    => 1,
							   -column => 1,
							   -sticky => 'w'
							   );
		my $triskel = $triskel_L->Pixmap(-file => $DIRLIB.$sep."triskel.xpm");
		$triskel_L->configure(-image => $triskel);

	    } elsif ($sim->{"ns_complexes"}{$symbol}{"NumNeighbours"} == 4){

		$neighb_id_Fr->Radiobutton(-value    => '0',
					   -variable => \$react{"neighbour"}{"ReactNeighbourID"},
					   )->grid(-row    => 0,
						   -column => 1);
		$neighb_id_Fr->Radiobutton(-value    => '1',
					   -variable => \$react{"neighbour"}{"ReactNeighbourID"},
					   )->grid(-row    => 1,
						   -column => 2);
		$neighb_id_Fr->Radiobutton(-value    => '2',
					   -variable => \$react{"neighbour"}{"ReactNeighbourID"},
					   )->grid(-row    => 3,
						   -column => 1);
		$neighb_id_Fr->Radiobutton(-value    => '3',
					   -variable => \$react{"neighbour"}{"ReactNeighbourID"},
					   )->grid(-row    => 1,
						   -column => 0);
		my $cross_L = $neighb_id_Fr->Label()->grid(-row    => 1,
							   -column => 1,
							   -sticky => 'w'
							   );
		my $cross = $cross_L->Pixmap(-file => $DIRLIB.$sep."cross.xpm");
		$cross_L->configure(-image => $cross);
		
	     } elsif ($sim->{"ns_complexes"}{$symbol}{"NumNeighbours"} == 6){
		 
		$neighb_id_Fr->Radiobutton(-value    => '0',
					   -variable => \$react{"neighbour"}{"ReactNeighbourID"},
					   )->grid(-row    => 0,
						   -column => 1);
		$neighb_id_Fr->Radiobutton(-value    => '1',
					   -variable => \$react{"neighbour"}{"ReactNeighbourID"},
					   )->grid(-row    => 0,
						   -column => 3);
		$neighb_id_Fr->Radiobutton(-value    => '2',
					   -variable => \$react{"neighbour"}{"ReactNeighbourID"},
					   )->grid(-row    => 1,
						   -column => 4);
		$neighb_id_Fr->Radiobutton(-value    => '3',
					   -variable => \$react{"neighbour"}{"ReactNeighbourID"},
					   )->grid(-row    => 2,
						   -column => 3);
		$neighb_id_Fr->Radiobutton(-value    => '4',
					   -variable => \$react{"neighbour"}{"ReactNeighbourID"},
					   )->grid(-row    => 2,
						   -column => 1);
		$neighb_id_Fr->Radiobutton(-value    => '5',
					   -variable => \$react{"neighbour"}{"ReactNeighbourID"},
					   )->grid(-row    => 1,
						   -column => 0);

		my $star_L = $neighb_id_Fr->Label()->grid(-row    => 1,
							   -column => 2,
							   -sticky => 'w'
							   );

		my $star = $star_L->Pixmap(-file => $DIRLIB.$sep."star.xpm");
		$star_L->configure(-image => $star);
		
	     } else {}
#-------------------------------------------- Clear/Set flags ----------------------------------------
	    
	    my $neighb_setclear_Fr = $neighbour->LabFrame(-borderwidth    => 1, 
							  -label          => 'Set/Clear flags',
							  -labelside      => 'acrosstop'
							  )->pack(-side   => 'left', 
								  -expand => '1', 
								  -fill   => 'both');
	    
	    $balloon->attach($neighb_setclear_Fr, -msg => $messages{"SetClearNeighb"});
	    
	    $neighb_setclear_Fr->Label(-text => 'set')->grid(-row    => '0',
							     -column => '1');
	    $neighb_setclear_Fr->Label(-text => 'clear')->grid(-row    => '0',
							       -column => '2');
	    $neighb_setclear_Fr->Label(-text => 'none')->grid(-row    => '0',
							      -column => '3');
	    
	    	    for my $flag_idx (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
		$neighb_setclear_Fr->Label(-text => $sim->{"ms_complexes"}{$symbol}{"Flags"}[$flag_idx]
					   )->grid(-row    => $flag_idx+1, 
						   -column => '0');
		
		# If the flag is controlled by a rapid equilibrium, we disable the possible control by a reaction.
		my $bingo2 = 0;
		foreach my $re (@{ $sim->{"ms_complexes"}{$symbol}{"Rapid_equilibriums"} }){
		    $bingo2++ if ($re->{"Flag"} eq $sim->{"ms_complexes"}{$symbol}{"Flags"}[$flag_idx]);
		}
		if ($sim->{"general_config"}{"UseSpatialExtensions"} and defined $sim->{"ns_complexes"}{$symbol}){
		    foreach my $re (@{ $sim->{"ns_complexes"}{$symbol}{"Rapid_equilibriums"} }){
			$bingo2++ if ($re->{"Flag"} eq $sim->{"ms_complexes"}{$symbol}{"Flags"}[$flag_idx]);
		    }
		}
		if ($bingo2){
		    $neighb_setclear_Fr->Radiobutton(-value    => "",
						     -variable => \$neighb_flags_effect[$flag_idx],
						     )->grid(-row    => $flag_idx+1,
							     -column => '3');
		} else {
		    $neighb_setclear_Fr->Radiobutton(-value    => '+',
						     -variable => \$neighb_flags_effect[$flag_idx],
						     )->grid(-row    => $flag_idx+1,
							     -column => '1');
		    $neighb_setclear_Fr->Radiobutton(-value    => '-',
						     -variable => \$neighb_flags_effect[$flag_idx],
						     )->grid(-row    => $flag_idx+1,
							     -column => '2');
		    $neighb_setclear_Fr->Radiobutton(-value    => "",
						     -variable => \$neighb_flags_effect[$flag_idx],
						     )->grid(-row    => $flag_idx+1,
							     -column => '3');
		}	
	    }
	    
#-----------------------------------------------------------------------------------------------------
#                                           Functionnal code
#-----------------------------------------------------------------------------------------------------
	    
	    for my $i (0..$#{ $states{"neighbour"} }){
		$neighb_states_Ls->insert('end', $states{"neighbour"}[$i]." ".sprintf("%6s",$values{"neighbour"}[$i]));
	    }
	    
	    $neighb_states_Ls->bind('<Button-1>' => sub{ 
		$neighb_state_idx = $neighb_states_Ls->curselection; 
		if (defined $neighb_state_idx){
		    $neighb_modificator->configure(-textvariable => \$values{"neighbour"}[$neighb_state_idx],
						   -state        => 'normal');
		    @neighb_curr_flags = split "",$states{"neighbour"}[$neighb_state_idx];
		    for (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
			$neighb_rbt0[$_]->configure(-variable => \$neighb_curr_flags[$_],
						    -state    => 'normal');
			$neighb_rbtQ[$_]->configure(-variable => \$neighb_curr_flags[$_],
						    -state    => 'normal');
			$neighb_rbt1[$_]->configure(-variable => \$neighb_curr_flags[$_],
						    -state    => 'normal');
		    }
		}
	    });
	    
	    $neighb_states_Ls->bind('<Control-Button-1>' => sub{ 
		# IMPORTANT: test $states_Ls->curselection and NOT $neighb_state_idx
		# Because $neighb_state_idx remains defined after usage. Here we require
		# a new selection before deletion. 
		if (defined $neighb_states_Ls->curselection){
		    splice (@{ $states{"neighbour"} },$neighb_state_idx,1);
		    splice (@{ $values{"neighbour"} },$neighb_state_idx,1);
		    $neighb_states_Ls->delete('active');
		    undef $neighb_state_idx;
		    for (0..$#self_curr_flags){
			$self_rbt0[$_]->configure(-state => 'disabled');
			$self_rbt0[$_]->deselect;
			$self_rbtQ[$_]->configure(-state => 'disabled');
			$self_rbtQ[$_]->deselect;
			$self_rbt1[$_]->configure(-state => 'disabled');
			$self_rbt1[$_]->deselect;
		}
		}
	    });

############# Fuse

	    $neighb_modificator->bind('<Leave>' => sub{
		if (defined $neighb_state_idx){
		    $states{"neighbour"}[$neighb_state_idx] = join "", @neighb_curr_flags;
		    $neighb_states_Ls->delete($neighb_state_idx);
		    $neighb_states_Ls->insert($neighb_state_idx, $states{"neighbour"}[$neighb_state_idx]." ".sprintf("%6s",$values{"neighbour"}[$neighb_state_idx]));
	    }
	    });
	    
	    $neighb_modificator->bind('<FocusOut>' => sub{
		if (defined $neighb_state_idx){
		    $states{"neighbour"}[$neighb_state_idx] = join "", @neighb_curr_flags;
		    $neighb_states_Ls->delete($neighb_state_idx);
		    $neighb_states_Ls->insert($neighb_state_idx, $states{"neighbour"}[$neighb_state_idx]." ".sprintf("%6s",$values{"neighbour"}[$neighb_state_idx]));
		}
	    });
	    
	    $neighb_modificator->bind('<Return>' => sub{
		if (defined $neighb_state_idx){
		    $states{"neighbour"}[$neighb_state_idx] = join "", @neighb_curr_flags;
		    $neighb_states_Ls->delete($neighb_state_idx);
		    $neighb_states_Ls->insert($neighb_state_idx, $states{"neighbour"}[$neighb_state_idx]." ".sprintf("%6s",$values{"neighbour"}[$neighb_state_idx]));
		}
	    });

###############

	    $neighb_new_state_Bt->configure(-command => sub{ 
		push(@{	$states{"neighbour"} },""); 
		push(@{ $values{"neighbour"} },1);
		$neighb_state_idx = $#{ $states{"neighbour"} }; 
		for (0..$#{ $sim->{"ms_complexes"}{$symbol}{"Flags"} }){
		    $neighb_curr_flags[$_] = '?';
		    $neighb_rbt0[$_]->configure(-variable => \$neighb_curr_flags[$_], 
						-state => 'normal');
		    $neighb_rbtQ[$_]->configure(-variable => \$neighb_curr_flags[$_], 
						-state => 'normal');
		    $neighb_rbt1[$_]->configure(-variable => \$neighb_curr_flags[$_], 
						-state => 'normal'); 
		}
		$states{"neighbour"}[$neighb_state_idx] = join "", @neighb_curr_flags; 
		$neighb_states_Ls->insert('end', $states{"neighbour"}[$neighb_state_idx]." ".sprintf("%6s",$values{"neighbour"}[$neighb_state_idx]));
		$neighb_modificator->configure(-textvariable =>	\$values{"neighbour"}[$neighb_state_idx], 
					       -state => 'normal');
	    }); 

	    for (0..$#neighb_curr_flags){
		$neighb_rbt0[$_]->configure(-command => sub{
		    $states{"neighbour"}[$neighb_state_idx] = join "", @neighb_curr_flags;
		    $neighb_states_Ls->delete($neighb_state_idx);
		    $neighb_states_Ls->insert($neighb_state_idx, $states{"neighbour"}[$neighb_state_idx]." ".sprintf("%6s",$values{"neighbour"}[$neighb_state_idx]));
		});
		$neighb_rbtQ[$_]->configure(-command => sub{
		    $states{"neighbour"}[$neighb_state_idx] = join "", @neighb_curr_flags;
		    $neighb_states_Ls->delete($neighb_state_idx);
		    $neighb_states_Ls->insert($neighb_state_idx, $states{"neighbour"}[$neighb_state_idx]." ".sprintf("%6s",$values{"neighbour"}[$neighb_state_idx]));
 		});
		$neighb_rbt1[$_]->configure(-command => sub{
		    $states{"neighbour"}[$neighb_state_idx] = join "", @neighb_curr_flags;
		    $neighb_states_Ls->delete($neighb_state_idx);
		    $neighb_states_Ls->insert($neighb_state_idx,$states{"neighbour"}[$neighb_state_idx]." ".sprintf("%6s",$values{"neighbour"}[$neighb_state_idx])); 
		});
		
		} 
	} 
    } else { 
	$react_states_W->deiconify();
	$react_states_W->raise(); 
    } 
}

#####################################################################################################
# Delete the effect of a multistate on a reaction
#####################################################################################################

sub deleteMS{
    my ($sim,$symbol,$ms) = @_;

    for my $react_idx (0..$#{ $sim->{"ms_complexes"}{$ms}{"Reactions"} }){
	if ( $sim->{"ms_complexes"}{$ms}{"Reactions"}[$react_idx]{"Symbol"} eq  $symbol){
	    splice (@{ $sim->{"ms_complexes"}{$ms}{"Reactions"} },$react_idx,1);
	    last;
	}
    }
}

#####################################################################################################
# Delete the effect of a neighbour-sensitive on a reaction
#####################################################################################################

sub deleteNS{
    my ($sim,$symbol,$ns) = @_;

    for my $react_idx (0..$#{ $sim->{"ns_complexes"}{$ns}{"Reactions"} }){
	if ( $sim->{"ns_complexes"}{$ns}{"Reactions"}[$react_idx]{"Symbol"} eq  $symbol){
	    splice (@{ $sim->{"ns_complexes"}{$ns}{"Reactions"} },$react_idx,1);
	    last;
	}
    } 
}

######################################################################################################
# Configuration of the dynamic values
#
# Structure:
# ----------
# $dyn_values_W     Toplevel
#     Button "accept"     2 0
#     Button "close"      2 1
#     values_Fr Frame     1 0/1
#         Value Label         i 0
#         numeric Entry       i j
#         del Button          i n         
#         del Button          m j
#     new_time_Fr   Frame 0 0
#     new_value_Fr   Frame 0 1
#
######################################################################################################

sub dynamic_values{
    my ($sim) = @_;
    my %dyn_values; # local version
    my %times;      # record of all timepoints
    my ($newtime,$newvalue);
    
    foreach my $dynvalue (keys %{ $sim->{"dynamic_values"} }){
	foreach my $timepoint (keys %{ $sim->{"dynamic_values"}{$dynvalue} }){
	    $times{$timepoint} = undef; # we don't care of the value
	    $dyn_values{$dynvalue}{$timepoint} =  $sim->{"dynamic_values"}{$dynvalue}{$timepoint};
	}
    }

    if(!Exists(my $dyn_values_W)){

#-----------------------------------------------------------------------------------------------------
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------

	$dyn_values_W = $mw->Toplevel();
#	$dyn_values_W->resizable(0,0);
	$dyn_values_W->geometry("+".$look{"xsecond"}."+".$look{"ysecond"}); 
	$dyn_values_W->title("Dynamic values");
	# users cannot kill the window via decorations
	$dyn_values_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$dyn_values_W->transient($dyn_values_W->Parent->toplevel);

#---------------------------------------- close Frame ------------------------------------------------
	$dyn_values_W->Button(
			      -text             => "accept",
			      -activeforeground => $look{"activeforeground"},
			      -activebackground => $look{"activebackground"},
			      -command          => sub{
				  
				  $sim->{"dynamic_values"} = {};
				  foreach my $dynvalue (keys %dyn_values){
				  %{ $sim->{"dynamic_values"}{$dynvalue} } = %{ $dyn_values{$dynvalue} };
			      }				  
				  $dyn_values_W->grabRelease();
				  $dyn_values_W->destroy;
			      }
			      )->grid(
				      -row        => 2,
				      -column     => 0,
				      );
	
	$dyn_values_W->Button(
			      -text             => "cancel",
			      -activeforeground => $look{"activeforeground"},
			      -activebackground => $look{"activebackground"},
			      -command          => sub{ 
				  $dyn_values_W->grabRelease();
				  $dyn_values_W->destroy }
			      )->grid(
				      -row        => 2,
				      -column     => 1,
				      );
	
#------------------------------------ configuration of existing entries ------------------------------

	my $values_Fr = $dyn_values_W->LabFrame(
						-borderwidth => 1, 
						)->grid(-row        => 1,
							-column     => 0,
							-columnspan => 2);

	$values_Fr = make_dynvalues_list(\%dyn_values,\%times,$dyn_values_W,$values_Fr);

#------------------------------------ creation of new entries ----------------------------------------

	my $new_time_Fr = $dyn_values_W->Frame()->grid(
						       -row        => 0,
						       -column     => 0
						       );
	my $new_time_En = $new_time_Fr->Entry(
					      -width            => 8,
					      -foreground   => $look{"entryforeground"},
					      -background   => $look{"entrybackground"},
					      -textvariable => \$newtime,
					      )->pack(-side => 'left');
	
	my $new_time_Bt = $new_time_Fr->Button(-width            => 6,
					       -text             => "add time",
						-activeforeground => $look{"activeforeground"},
						-activebackground => $look{"activebackground"},
						-command          => sub{
						    if ($newtime =~ /[\d\.eE\-\+]+/){   # FIXME: check if $time < simulation length
							$times{$newtime} = undef;
							foreach my $dynvalue (keys %dyn_values){
							    $dyn_values{$dynvalue}{$newtime} = undef;
							}
							$values_Fr = make_dynvalues_list(\%dyn_values,\%times,$dyn_values_W,$values_Fr);
							$newtime = "";
						    }
						}
						)->pack(-side => 'left');

	$balloon->attach($new_time_Fr, -msg => $messages{"NewTimepoint"});

	my $new_value_Fr = $dyn_values_W->Frame()->grid(
							-row        => 0,
							-column     => 1
							);

	my $new_value_En = $new_value_Fr->Entry(
						-width            => 8,
						-foreground   => $look{"entryforeground"},
						-background   => $look{"entrybackground"},
						textvariable => \$newvalue,
						)->pack(-side => 'left');
	
	my $new_value_Bt = $new_value_Fr->Button(-width            => 6,
						 -text             => "add value",
						 -activeforeground => $look{"activeforeground"},
						 -activebackground => $look{"activebackground"},
						 -command          => sub{
						     if ($newvalue =~ /\w+/){
							 $dyn_values{$newvalue} = {};
							 foreach my $timepoint (keys %times){
							     $dyn_values{$newvalue}{$timepoint} = undef;
							 }
							 $values_Fr = make_dynvalues_list(\%dyn_values,\%times,$dyn_values_W,$values_Fr);
							 $newvalue = "";
						     }
						 }
						 )->pack(-side => 'left');

	$balloon->attach($new_value_Bt, -msg => $messages{"NewDynValue"});

#-----------------------------------------------------------------------------------------------------
#                                           Functionnal code
#-----------------------------------------------------------------------------------------------------

	
	$new_time_En->bind('<Return>' => sub{
	    if ($newtime =~ /[\d\.eE\-\+]+/){   # FIXME: check if $time < simulation length
		$times{$newtime} = undef;
		foreach my $dynvalue (keys %dyn_values){
		    $dyn_values{$dynvalue}{$newtime} = undef;
		}
		$values_Fr = make_dynvalues_list(\%dyn_values,\%times,$dyn_values_W,$values_Fr);		
		$newtime = "";
	    }
	});
	
	$new_value_En->bind('<Return>' => sub{
	    if ($newvalue =~ /\w+/){
		$dyn_values{$newvalue} = {};
		foreach my $timepoint (keys %times){
		    $dyn_values{$newvalue}{$timepoint} = undef;
		}
		$values_Fr = make_dynvalues_list(\%dyn_values,\%times,$dyn_values_W,$values_Fr);
		$newvalue;
	    }
	});
    
    } else {
	$dyn_values_W->deiconify();
	$dyn_values_W->raise();
    }
}

######################################################################################################
# draw the matrix of dynamic values-timepoints
######################################################################################################

sub make_dynvalues_list{
    my ($dyn_values,$times,$dyn_values_W,$values_Fr) = @_;
    my ($row,$column);

# Strange, here values_Fr is destroyed as if it was one of its own child
    my @kids = $values_Fr->children;
    foreach (@kids){
	$_->destroy;
    }

    $values_Fr = $dyn_values_W->LabFrame(
					    -borderwidth => 1, 
					    )->grid(-row        => 1,
						    -column     => 0,
						    -columnspan => 2);

    $column = 1;   # begins at 1 to leave place for the value label
    foreach my $timepoint (sort {$a <=> $b } keys %$times){ # sort by increasing value rather than alphabetical
	$values_Fr->Label(-text => $timepoint
			  )->grid(-row    => 0,
				  -column => $column);
	$column++;
    }
    $row = 1; 
    foreach my $dynvalue (sort keys %$dyn_values){
	$values_Fr->Label(-text => $dynvalue,
			  )->grid(-row    => $row,
				  -column => 0);
	$column = 1;
	foreach my $timepoint (sort {$a <=> $b } keys %{ $dyn_values->{$dynvalue} }){ # sort by increasing value rather than alphabetical
	    $values_Fr->Entry(-width        => 10,
			      -textvariable => \$dyn_values->{$dynvalue}{$timepoint},
			      -foreground   => $look{"entryforeground"},
			      -background   => $look{"entrybackground"},
			      )->grid(-row    => $row,
				      -column => $column);
	    $column++;
	}
	$values_Fr->Button(-text    => 'del',
			   -command => sub{
			       delete $dyn_values->{$dynvalue};
			       # Problem here: the window doesn't shrink when I deacrease the number of widgets
			       $values_Fr = make_dynvalues_list($dyn_values,$times,$dyn_values_W,$values_Fr);
			   } 
			   )->grid(-row    => $row,
				   -column => $column);
	$row++;
    }
    $column = 1;   # begins at 1 to leave place for the value label
    foreach my $timepoint (sort {$a <=> $b } keys %$times){ # sort by increasing value rather than alphabetical
	$values_Fr->Button(-text => 'del',
			   -command => sub{
			       foreach my $dynvalue (keys %$dyn_values){
				   delete $dyn_values->{$dynvalue}{$timepoint};
			       }
			       delete $times->{$timepoint};
			       $values_Fr = make_dynvalues_list($dyn_values,$times,$dyn_values_W,$values_Fr);
			   } 
			   )->grid(-row    => $row,
				   -column => $column);
	$column++;
    }
#    $values_Fr->geometry("");
    return $values_Fr;
}

######################################################################################################
#            This function opens a window and display the messages required by argument 
######################################################################################################

sub message{

    my $title = shift;          # chops the title as the first argument
    my $bitmap = shift;         # chops the bitmap as the second argument
    my $message_key;
    my $Text;
 
    foreach $message_key (@_){  # chops the other arguments as messages
        $Text .= $message_key;
    }

    my $message_W = $mw->messageBox(-title   => $title,
				    -icon    => $bitmap,
				    -message => $Text,
				    -type    => 'OK',
#				    -default => 'OK',
# default is obligatory 'OK', and does not work properly under Windows anyway.
				    );
}

######################################################################################################
#                                          Launch the simulation
######################################################################################################

sub run_sim{
    my ($sim)= @_ ;
    my @stochsim_out;    # The output of stochsim on STDOUT

    if (defined $sim->{"main_config_dir"}){ # Save INI files 
	print " writing configuration files ...\n";
	save_file($sim);
    } else {save_file_as($sim);}        # "Save as ..." INI files
    
    if (defined $sim->{"main_config_dir"} and $sim->{"main_config_dir"} ne ""){
	eval {
	    chdir $sim->{"main_config_dir"}.$sep."Input" 
		or die "can't change current directory to $sim->{main_config_dir}${sep}Input: $!"; 
	    print " Trying to launch $stochsim in the directory ",
	    $sim->{"main_config_dir"}.$sep."Input"," ...\n";	    
	  LAUNCH: {
	      if ($^O eq "unix" or $^O eq "linux" or $^O eq "dec_osf" or $^O eq "darwin" or $^O eq "aix"){
		  # I fork to launch StochSim and get the PID
		  if ($stochsim_pid = fork){
		      # I am supposed to take care of zombies. But everything seems fine ...
		      $stop_mi->configure(-state => 'normal');
		      $run_mi->configure(-state => 'disabled');
		      $run_Bt->configure(-state => 'disabled');
		      $stop_Bt->configure(-state => 'normal');
		  } else { 
		      die "cannot fork: $!" unless defined $stochsim_pid;
		      exec("$stochsim") or die "Couldn't run $stochsim: $!";
		  }
	      } elsif ($^O eq "MSWin32"){
		  if (system("$stochsim")  != 0){
		      die "cannot launch $stochsim\n";
		  }
	      } else {
		  die "I do not recognise the operating system\n";
	      }
	      print " $stochsim launched.";
	      sleep($look{"launch"});
	      if (-e "$sim->{main_config_dir}${sep}Output${sep}VAR.OUT"){ 
		  if (defined $vd_Bt){ $vd_Bt->configure(-state => 'normal') }
		  if (defined $m_Bt){ $m_Bt->configure(-state => 'normal') }
		  if (defined $pl_Bt){ $pl_Bt->configure(-state => 'normal') }
	      } else {
		  print " I do not find the file $sim->{main_config_dir}${sep}Output${sep}VAR.OUT",
		  " I wait a little longer.\n";
		  sleep(5 * $look{"launch"});
		  if (-e "$sim->{main_config_dir}${sep}Output${sep}VAR.OUT"){ 
		      if (defined $vd_Bt){ $vd_Bt->configure(-state => 'normal') }
		      if (defined $m_Bt){ $m_Bt->configure(-state => 'normal') }
		      if (defined $pl_Bt){ $pl_Bt->configure(-state => 'normal') }
		  } else {
		      print " I do not find the file $sim->{main_config_dir}${sep}Output${sep}VAR.OUT",
		      " I wait a little longer.\n";
		      sleep(10 * $look{"launch"});
		      if (-e "$sim->{main_config_dir}${sep}Output${sep}VAR.OUT"){ 
			  if (defined $vd_Bt){ $vd_Bt->configure(-state => 'normal') }
			  if (defined $m_Bt){ $m_Bt->configure(-state => 'normal') }
			  if (defined $pl_Bt){ $pl_Bt->configure(-state => 'normal') }
		      } else {
			  print " I do not find the file $sim->{main_config_dir}${sep}Output${sep}VAR.OUT\n",
			  " There is maybe a problem. Launch StochSim on the \n",
			  " command-line and check the file LOG.OUT.\n";
		      }
		  }
	      }
	  }
	};
	message("error","error",$@) if $@;
    }
    return $stochsim_pid
}

######################################################################################################
#                                             View the results
######################################################################################################

sub view_data{
    my ($maindir) = @_; 
    
    if(!Exists(my $View_W)){

	$View_W = $mw->Toplevel();
	$View_W->title($maindir.$sep."Output${sep}VAR.OUT");
	$View_W->Button(-text    => "close",
			-command => sub{ $View_W->destroy }
			)->pack;
	
	my $ViewText = $View_W->Scrolled
	    ("ROText", 
	     -font => "-adobe-courier-medium-r-normal--0-0-0-0-m-0-iso8859-1"
	     )->pack(-expand => '1',
		     -fill   => 'both');
	eval {
	    open(VH,$maindir.$sep."Output${sep}VAR.OUT") 
		or die "unable to open $maindir${sep}Output${sep}VAR.OUT";
	    while (<VH>){
		$ViewText->insert('end', $_);
	    }
	    close(VH);
	};
	message("error","error",$@) if $@;
    } else {
	$View_W->deiconify();
	$View_W->raise();
    }
}

######################################################################################################
#                                             Well ...
######################################################################################################

sub quittkstochsim{
    my @stochsimrc; 
    my ($pdfreader,$confdir) = (0,0); # booleans
    
    open (WRITERC, ">$DIRCONF${sep}stochsimrc") or die "can't open $DIRCONF${sep}stochsimrc: $!";
    print WRITERC "##############################################################################\n",
                  "#                       perl/Tk interface for stochsim                       #\n",
                  "# FILE: stochsimrc                                                            \n",
                  "#                                                                             \n",
                  "# AIM: Contains the configuration of the frontend                             \n",
                  "#                                                                             \n",
                  "# AUTHOR: Nicolas Le Novère                                                   \n",
                  "#                                                                             \n",
                  "##############################################################################\n",
                  "#                                                                             \n",
                  "#     This program is free software; you can redistribute it and/or modify    \n",
                  "#     it under the terms of the GNU General Public License as published by    \n",
                  "#     the Free Software Foundation; either version 2 of the License, or       \n",
                  "#     (at your option) any later version.                                     \n",
                  "#                                                                             \n",
                  "#     This program is distributed in the hope that it will be useful,         \n",
                  "#     but WITHOUT ANY WARRANTY; without even the implied warranty of          \n",
                  "#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           \n",
                  "#     GNU General Public License for more details.                            \n",
                  "#                                                                             \n",
                  "#     You should have received a copy of the GNU General Public License       \n",
                  "#     along with this program; if not, write to the Free Software             \n",
                  "#     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n",
                  "#\n",
                  "#     Nicolas Le Novère \n",
                  "#     Department of Zoology, University of Cambridge, Downing street, \n",
		      "#     CB2 3EJ Cambridge, UK. e-mail: nl223\@cus.cam.ac.uk\n\n";
    # More control on the format of each item would not be a bad idea
    print WRITERC "# Pause during the initialisation of StochSim simulations\n";
    print WRITERC "launch=",$look{"launch"} ? $look{"launch"} : 1,"\n\n";
    print WRITERC "# Location of PDF reader\n";
    print WRITERC "pdfreader=",$look{"pdfreader"},"\n\n";
    print WRITERC "# Default location of configuration files\n";
    print WRITERC "confdir=",$look{"confdir"},"\n\n";
    print WRITERC "# interval between progress bar refreshment, in seconds\n";
    print WRITERC "refresh=",$look{"refresh"} ? $look{"refresh"} : 2,"\n\n";
    print WRITERC "# Width of the explanation balloons\n";
    print WRITERC "BalloonWidth=",$look{"BalloonWidth"} ? $look{"BalloonWidth"} : 10,"\n\n";
    print WRITERC "# Are the balloon present at start-up?\n";
    print WRITERC "balloon=",$look{"balloon"} ? $look{"balloon"} : 'on',"\n\n";
    print WRITERC "# The following colors can be defined by odes names such as \"\#FF0000\" or \"\#153D4F\" \n",
    "# the '#' means hexadecimal. The following digits are the specification of the \n",
    "# color by its Red Green Blue content, each value between 0 and 255, i.e. 00 to\n",
    "# FF in hexadecimal. Some common colors:\n",
    "# black       = \#000000\n",
    "# white       = \#FFFFFF\n",
    "# red         = \#FF0000\n",
    "# light green = \#00FF00\n",
    "# blue        = \#0000FF\n",
    "# yellow      = \#FFFF00  (red + green)\n",
    "# cyan        = \#00FFFF  (green + blue)\n",
    "# orange      = \#FFAA00  (red + light green\n",
    "# gray        = \#AFAFAF\n",
    "# dark green  = \#009900 \n\n";
    print WRITERC "# background and text colors when the mouse is over a menu item\n";
    print WRITERC "activebackground=",$look{"activebackground"} ? $look{"activebackground"} : "#FF0000" ,"\n";
    print WRITERC "activeforeground=",$look{"activeforeground"} ? $look{"activeforeground"} : "#FFFFFF" ,"\n\n";
    print WRITERC "# background and text colors when the mouse is over a simulation item \n";
    print WRITERC "activeselectionbackground =",$look{"activeselectionbackground"} ? $look{"activeselectionbackground"} : "#0000FF","\n";
    print WRITERC "activeselectionforeground =",$look{"activeselectionforeground"} ? $look{"activeselectionforeground"} : "#FFFFFF","\n\n";
    print WRITERC "# background and text color of the entry widgets\n";
    print WRITERC "entrybackground=",$look{"entrybackground"} ? $look{"entrybackground"} : "#FFFFFF","\n";
    print WRITERC "entryforeground=",$look{"entryforeground"} ? $look{"entryforeground"} : "#000000","\n\n";
    print WRITERC "# position of the main window in pixels from the left and the top \n";
    print WRITERC "xmain=",$look{"xmain"} ? $look{"xmain"} : 50,"\n";
    print WRITERC "ymain=",$look{"ymain"} ? $look{"ymain"} : 50,"\n\n";
    print WRITERC "# position of the window of second level in pixels from the left and the top \n";
    print WRITERC "xsecond=",$look{"xsecond"} ? $look{"xsecond"} : 250,"\n";
    print WRITERC "ysecond=",$look{"ysecond"} ? $look{"ysecond"} : 50,"\n\n";
    print WRITERC "# position of the window of third level in pixels from the left and the top \n";
    print WRITERC "xthird=",$look{"xthird"} ? $look{"xthird"} : 450,"\n";
    print WRITERC "ythird=",$look{"ythird"} ? $look{"ythird"} : 50,"\n\n";
    print WRITERC "# position of the window of fourth level in pixels from the left and the top \n";
    print WRITERC "xfourth=",$look{"xfourth"} ? $look{"xfourth"} : 650,"\n";
    print WRITERC "yfourth=",$look{"yfourth"} ? $look{"yfourth"} : 50,"\n\n";
    print WRITERC "# number of components appearing in the scrolled component list \n";
    print WRITERC "heightcomponentlist=",$look{"heightcomponentlist"} ? $look{"heightcomponentlist"} : 5,"\n\n";
    print WRITERC "# thickness of the focus highlight \n";
    print WRITERC "highlightthickness=",$look{"highlightthickness"} ? $look{"highlightthickness"} : 2,"\n\n";
    print WRITERC "# width and height of the plotting canvas \n";
    print WRITERC "plotwidth=",$look{"plotwidth"} ? $look{"plotwidth"}: 500,"\n";
    print WRITERC "plotheight=",$look{"plotheight"} ? $look{"plotheight"}: 300,"\n\n";
    print WRITERC "# number of ticks on the ordinate and abscissa axis\n";
    print WRITERC "numord=",$look{"numord"} ? $look{"numord"}: 10,"\n";
    print WRITERC "numabs=",$look{"numabs"} ? $look{"numabs"}: 5,"\n\n";
    print WRITERC "# type of the plot, points (0) or lines linking points (1) \n";
    print WRITERC "plottype=",$look{"plottype"} ? $look{"plottype"}: 0,"\n\n";
    print WRITERC "# Background color of the plotting canvas \n";
    print WRITERC "canvasbackground=",$look{"canvasbackground"} ? $look{"canvasbackground"} : "#FFFFFF","\n\n";
    print WRITERC "# number of points which will be plotted \n";
    print WRITERC "numpoints=",$look{"numpoints"} ? $look{"numpoints"} : 1000,"\n\n";
    print WRITERC "# colors used in the plotting tool. They overide the corresponding default values \n";
    print WRITERC "# (i.e. the first replaces the first and so on.) \n";
    print WRITERC "graphcolors=",$look{"graphcolors"} ? $look{"graphcolors"} : "#000000,#FF0000,#00FF00,#0000FF,#FFFF00,#00FFFF,#FFAA00,#AFAFAF,#009900","\n\n";
    
    my $index = 0;
    while (@previous){ 
	print WRITERC "SIM$index=",shift @previous,"\n";
	$index++;
    }		       
    close(WRITERC);
    exit;
}

######################################################################################################
#                                  Plot the variable to display
######################################################################################################

sub plotsim{
    my ($sim) = @_;
    my ($Xscale,$Yscale) = (1,1);
    my ($abs,$ord) = ($look{"plotwidth"}/2,$look{"plotheight"}/2); # The center of the visible part
    my $c;         # The drawing canvas
    my @colors = ("#000000",  # black
		  "#FF0000",  # red
		  "#00FF00",  # light green
		  "#0000FF",  # blue
		  "#FFFF00",  # yellow
		  "#00FFFF",  # cyan
		  "#FFAA00",  # orange
		  "#AFAFAF",  # gray
		  "#009900"   # dark green
		  );

    my $i = 0;
    foreach (@{ [ split /,/,$look{"graphcolors"} ] }){
	$colors[$i] = $_;
	$i++;
    }
    my $lines;
    $look{"plottype"} ? $lines = $look{"plottype"} : $lines = 0;   

    # Define the axis. I leave space for labels and legends
    my %axis = ("minabs" => 50,
		"maxabs" => $look{"plotwidth"} - 50,
		"minord" => $look{"plotheight"} - 30,
		"maxord" => 10,
		);
    $axis{"width"}  = abs( $axis{"maxabs"} - $axis{"minabs"} );
    $axis{"height"} = abs( $axis{"maxord"} - $axis{"minord"} );
    # DO NOT FORGET: THE Y ORIGIN IS ON THE TOP

    if(!Exists(my $plot_W)){

	$plot_W = $mw->Toplevel();
	$plot_W->geometry("+".$look{"xsecond"}."+".$look{"ysecond"}); 
	my $title = basename($sim->{"main_config_dir"});
	$plot_W->title("results: $title");
	$plot_W->resizable(0,0);
# Let the user choose the origin of the zoom
# Bind the shift-click to capture of mouse coordinates

	$c = $plot_W->Scrolled('Canvas',
			       -width      => $look{"plotwidth"}        ? $look{"plotwidth"}        : 600,
			       -height     => $look{"plotheight"}       ? $look{"plotheight"}       : 300,
			       -background => $look{"canvasbackground"} ? $look{"canvasbackground"} : 'white',
			       )->pack(-side => 'bottom');
	$balloon->attach($c, -msg => $messages{"ChooseOrigin"});

	my $real_canvas = $c->Subwidget('canvas');
	$real_canvas->Tk::bind('<Control-Button-1>' => [ \&print_xy, Ev('x'), Ev('y'), \$abs, \$ord ]);
	
	my $close_Bt = $plot_W->Button(-text             => "close",
				       -activeforeground => $look{"activeforeground"},
				       -activebackground => $look{"activebackground"},
				       -command          => sub{ $plot_W->destroy }
				       )->pack(-side => 'left');
	$balloon->attach($close_Bt, -msg => $messages{"ClosePlot"});

	my $drawing_Bt = $plot_W->Button(-text             => $lines ? "points?" : "lines?",
					 -activeforeground => $look{"activeforeground"},
					 -activebackground => $look{"activebackground"},					 
					 )->pack(-side => 'left');
	
	$drawing_Bt->configure(-command => sub{ 
	    if ($lines) {
		$balloon->attach($drawing_Bt, -msg => $messages{"PointDrawing"});
		$lines = 0;
		$drawing_Bt->configure(-text => "lines?");
		updateplot($sim,$c,\%axis,\@colors,$Xscale,$Yscale,$abs,$ord,$lines);
#		$drawing_Bt->update;
	    } else {
		$balloon->attach($drawing_Bt, -msg => $messages{"LineDrawing"});
		$lines = 1;
		$drawing_Bt->configure(-text => "points?");
		updateplot($sim,$c,\%axis,\@colors,$Xscale,$Yscale,$abs,$ord,$lines);
#		$drawing_Bt->update;
	    }
	}
			       );
	my $update_Bt = $plot_W->Button(-text            => "update",
				       -activeforeground => $look{"activeforeground"},
				       -activebackground => $look{"activebackground"},
					-command         => sub { 
					    updateplot($sim,$c,\%axis,\@colors,$Xscale,$Yscale,$abs,$ord,$lines) 
					    },
					)->pack(-side => 'left');
	$balloon->attach($update_Bt, -msg => $messages{"UpdatePlot"});

	my $print_Bt = $plot_W->Button(-text             => "print",
				       -activeforeground => $look{"activeforeground"},
				       -activebackground => $look{"activebackground"},
				       -command          => sub { 
					   my $psfile;
					   $psfile = $mw->getSaveFile
					       (-title => "Save plot as",
						-initialdir  => $sim->{"main_config_dir"}.$sep."Output",
						-initialfile => "plot.ps" );
					   $c->postscript(-file => $psfile); 			   
				       }
				       )->pack(-side => 'left');
	$balloon->attach($print_Bt, -msg => $messages{"PrintPlot"});

	my $zoominX_Bt = $plot_W->Button(-text             => "zoom in X",
					 -activeforeground => $look{"activeforeground"},
					 -activebackground => $look{"activebackground"},
					 -command          => sub { 
					     $c->scale("all",$abs,$ord,2,1);
					     $Xscale *= 2;
					     $c->configure(-scrollregion => [ $c->bbox("all") ]);
					 }
					 )->pack(-side => 'left');

	$balloon->attach($zoominX_Bt, -msg => $messages{"ZoomInX"});

	my $zoominY_Bt = $plot_W->Button(-text             => "zoom in Y",
					 -activeforeground => $look{"activeforeground"},
					 -activebackground => $look{"activebackground"},
					 -command          => sub { 
					     $c->scale("all",$abs,$ord,1,2);
					     $Yscale *= 2;
					     $c->configure(-scrollregion => [ $c->bbox("all") ]);
					 }
					 )->pack(-side => 'left');
	$balloon->attach($zoominY_Bt, -msg => $messages{"ZoomInY"});

	my $zoomoutX_Bt = $plot_W->Button(-text             => "zoom out X",
					  -activeforeground => $look{"activeforeground"},
					  -activebackground => $look{"activebackground"},
					  -command          => sub { 
					      $c->scale("all",$abs,$ord,0.5,1);
					      $Xscale *= 0.5;
					      $c->configure(-scrollregion => [ $c->bbox("all") ]);
					  }
					  )->pack(-side => 'left');
	$balloon->attach($zoomoutX_Bt, -msg => $messages{"ZoomOutX"});

	my $zoomoutY_Bt = $plot_W->Button(-text             => "zoom out Y",
					  -activeforeground => $look{"activeforeground"},
					  -activebackground => $look{"activebackground"},
					  -command          => sub { 
					      $c->scale("all",$abs,$ord,1,0.5);
					      $Yscale *= 0.5;
					      $c->configure(-scrollregion => [ $c->bbox("all") ]);
					  }
					  )->pack(-side => 'left');
	$balloon->attach($zoomoutY_Bt, -msg => $messages{"ZoomOutY"});

#------------------------------ The axis -------------------------------------------------------------

	$c->createLine
	    ($axis{"minabs"}, 
	     $axis{"minord"}, 
	     $axis{"minabs"}, 
	     $axis{"maxord"}, 
	     -width => 2);
	$c->createLine
	    ($axis{"minabs"}, 
	     $axis{"minord"}, 
	     $axis{"maxabs"}, 
	     $axis{"minord"}, 
	     -width => 2);

#----------------------------- The labels ------------------------------------------------------------
# They have to change with the progression. See the function updateplot. Only the
# marks stay there.
	
	my $valdiv = $look{"numord"} ? $look{"numord"} : 10;

	for (1..$valdiv){
	    $c->createLine($axis{"minabs"},
			   $axis{"minord"} - $axis{"height"} / $valdiv * $_, # minus because Y VERS LE BAS
			   $axis{"minabs"} - 5,
			   $axis{"minord"} - $axis{"height"} / $valdiv * $_,
			   );
	}

	my $timediv = $look{"numabs"} ? $look{"numabs"} : 5;
	
	for (1..$timediv){
	    $c->createLine($axis{"minabs"} + $axis{"width"} / $timediv * $_,
			   $axis{"minord"},
			   $axis{"minabs"} + $axis{"width"} / $timediv * $_,
			   $axis{"minord"} + 5,
			   );
	}
	$c->configure(-scrollregion => [ $c->bbox("all") ]);
	updateplot($sim,$c,\%axis,\@colors,$Xscale,$Yscale,$abs,$ord,$lines);

    } else {
	$plot_W->deiconify();
	$plot_W-> raise();
    }
}

sub print_xy {
    my ($canv, $x, $y, $r_abs, $r_ord) = @_;

    $$r_abs = $canv->canvasx($x);
    $$r_ord = $canv->canvasy($y);
}


######################################################################################################
#                                      Refresh the drawing canvas
######################################################################################################

sub updateplot{
    my ($sim,$graph,$axis,$colors,$Xscale,$Yscale,$abs,$ord,$lines) = @_;
    my @row;
    my @times;     # The recorded times
    my @sampled;   # the sampled times
    my @data;      # 2D array containing the data
    my @subset;    # data actually printed
    my @variables; # The variable names
    my @maxima;    # contains maximal value for each timeslice
    my @minima;    # contains minimal value for each timeslice
    my $line;
    my $frequency; # frequency of sampling from @data to @subset 
#------------------------------------------ reading the data -----------------------------------------
# We read the datafile, and then we extract the relevant subset of result
# There is definitively a problem when the file does not exist

    eval {
	open(PH, "<$sim->{main_config_dir}${sep}Output${sep}VAR.OUT") 
	    or die "unable to open $sim->{main_config_dir}${sep}Output${sep}VAR.OUT";

	# Read the first line
	if (defined ($line = <PH>)){
	    chomp($line);
	    @variables = split " ", $line;
	    # remove the word time
	    shift @variables;  
	}
	my $i = 0;
	while (<PH>){
	    chomp;
	    @row = split " ";
	    push(@times, shift @row);
	    $data[$i] = [ @row ];
	    # NOT 'push @data, \@row' or all entries of $data point 
            # to the content of @row, i.e. the last values
	    $i++;
	}
	close PH or die "unable to close $sim->{main_config_dir}${sep}Output${sep}VAR.OUT";

    };
    if ($@) {
	message("error","error",$@);
	return;
    }   

    # get the desired subset
    if (scalar(@data) < $look{"numpoints"}){
	@subset  = @data;
	@sampled = @times;
    } else {
	if ( not $look{"numpoints"} ){ $look{"numpoints"} = 1000 };
	$frequency = scalar(@data) / $look{"numpoints"};
	for (0..$look{"numpoints"}-1){
	    $subset[$_]  = $data[sprintf("%.0f",$_ * $frequency)];
	    $sampled[$_] = $times[sprintf("%.0f",$_ * $frequency)];
	}
    }


#--------------------------- The legend --------------------------------------------------------------
    $graph->delete("legend");
    for (0 .. $#variables){
	$graph->createLine( $abs - ($abs - ($axis->{"maxabs"}+2)) * $Xscale, 
			    $ord - ($ord - ($axis->{"maxord"}+(12 * $_)))*$Yscale, 
			    $abs - ($abs - ($axis->{"maxabs"}+10))* $Xscale,
			    $ord - ($ord - ($axis->{"maxord"}+(12 * $_)))*$Yscale,
			    -width => 2,
			    -fill  => $colors->[$_] ? $colors->[$_] : 'black',
			    -tag    => "legend");
	$graph->createText( $abs - ($abs - ($axis->{"maxabs"}+12))*$Xscale, 
			    $ord - ($ord - ($axis->{"maxord"}+(12*$_)))*$Yscale, 
			   -text   => $variables[$_],
			   -anchor => 'w',
			   -tag    => "legend"
			   );
    }

#-------------------------------------------- Rescaling the values -----------------------------------
# We compute the minimum and maximum and then rescale all the values. 

    foreach my $row (@subset){
	push @maxima, max( $row ); 
	push @minima, min( $row );
    }
    my $maxval = max(\@maxima);
    if ($maxval <= 0){ $maxval = 1 }   # Strange, no particles ????
    
    my $minval = min(\@minima);
    if ($minval < 0){ $minval = 0 }   # ??!

    my $maxord = $minval + (int( ($maxval-$minval)/10 ) + 1) * 10;
    
    foreach my $row (@subset){
        map { $_ = $axis->{"height"} * ( 1 - ($maxord - $_) / ($maxord - $minval )) } @$row;
    }

#------------------------------------------- Tracing the label of values -----------------------------
    
    my $valdiv = $look{"numord"} ? $look{"numord"} : 10;
    my $val_elt = ($maxord - $minval) / $valdiv;
    
    # We delete all the old labels (tagged with the "labelval" tag), and then
    # we trace the new ones.
    $graph->delete("labelval");
    for (1..$valdiv){
	$graph->createText($abs - ($abs - ($axis->{"minabs"} - 6)) * $Xscale, 
			   $ord - ($ord - ($axis->{"minord"} - $axis->{"height"} / $valdiv * $_))*$Yscale,
			   -text   =>  $minval + $val_elt * $_,
			   -anchor => 'e',
			   -tag    => "labelval"
			   );
    }
    
    # We compute the maximum and then rescale all the sampled
    
    my $maxtime = max(\@sampled);
    if ($maxtime == 0){ $maxtime = 1 }
    map { $_ = $_ * $axis->{"width"} / $maxtime } @sampled;    
    
#----------------------------------------- Tracing the label of values -------------------------------
    
    my $timediv = $look{"numabs"} ? $look{"numabs"} : 5;
    my $time_elt = $maxtime / $timediv; # not int, because time can be inferior to 1 second

    # We delete all the old labels (tagged with the "labelval" tag), and then
    # we trace the new ones.
    $graph->delete("timeval");
    for (0..$timediv){
	$graph->createText($abs - ($abs - ($axis->{"minabs"} + $axis->{"width"} / $timediv * $_))*$Xscale, 
			   $ord - ($ord - ($axis->{"minord"} + 6)) * $Yscale,
			   -text   =>  sprintf("%.1e",$time_elt * $_),
			   -anchor => 'n',
			   -tag    => "timeval"
			   );
    }

    # We delete all the old points (tagged with the "point" tag), and then
    # we trace the new ones.
    $graph->delete("values");
    if ($lines){
	my @oldvalues;
	for (0..scalar @variables){
	    $oldvalues[$_] = $subset[0][$_];
	}
	my $oldtime = $sampled[0];
	foreach my $row (@subset){
	    my $currtime = shift @sampled;
	    my $icolor = 0;
	    my @newvalues;
	    while (@$row){
		push @newvalues, shift @$row;
		$graph->createLine($abs - ($abs - ($axis->{"minabs"} + $oldtime ))*$Xscale, 
				   $ord - ($ord - ($axis->{"minord"} - $oldvalues[$#newvalues] ))*$Yscale, 
                                   # that is not an error $#newvalues and not $#oldvalues ...
				   $abs - ($abs - ($axis->{"minabs"} + $currtime))*$Xscale, 
				   $ord - ($ord - ($axis->{"minord"} - $newvalues[$#newvalues]))*$Yscale,
				   -fill => $colors->[$icolor] ? $colors->[$icolor] : 'black',
				   -tag  => "values"
				   );
		$icolor++;
	    }
	    $oldtime = $currtime;
	    @oldvalues = @newvalues;
	}
    } else {
	foreach my $row (@subset){
	    my $currtime = shift @sampled;
	    my $icolor = 0;
	    while (@$row){
		my $value = shift @$row;
		$graph->createOval($abs - ($abs - ($axis->{"minabs"} + $currtime - 1))*$Xscale, 
				   $ord - ($ord - ($axis->{"minord"} - $value - 1))*$Yscale,
				   $abs - ($abs - ($axis->{"minabs"} + $currtime + 1))*$Xscale, 
				   $ord - ($ord - ($axis->{"minord"} - $value + 1))*$Yscale,
				   -fill    => $colors->[$icolor] ? $colors->[$icolor] : 'black',
				   -outline => $colors->[$icolor] ? $colors->[$icolor] : 'black',
				   -tag     => "values"
				   );
		$icolor++;
	    }
	}
    }

    # We change the scrolling area
    $graph->configure(-scrollregion => [ $graph->bbox("all") ]);
}

######################################################################################################
#               This function return the biggest value found into an array
######################################################################################################

sub max{
    my ($input) = @_;
    my $maximum = $input->[0];

    for (1..$#{ $input }){
	if ($input->[$_] > $maximum){ $maximum = $input->[$_] }
    }

    return $maximum;
}

######################################################################################################
#               This function return the smallest value found into an array
######################################################################################################

sub min{
    my ($input) = @_;
    my $minimum = $input->[0];

    for (1..$#{ $input }){
	if ($input->[$_] < $minimum){ $minimum = $input->[$_] }
    }

    return $minimum;
}


#####################################################################################################
#                  Draw a progress bar permitting to follow the simulation
#####################################################################################################

sub updatevalue {
    my ($value,$time,@file);

    open(MH, "$sim->{main_config_dir}${sep}Output${sep}VAR.OUT") 
	or die "unable to open $sim->{main_config_dir}${sep}Output${sep}VAR.OUT";
    
    @file = <MH>;
    $time = @{ [split " ", pop @file] }[0];
    $value = 100 * ( $time / $sim->{"general_config"}{"SimulationDuration"} );
    
    close(MH) or die "unable to close $sim->{main_config_dir}${sep}Output${sep}VAR.OUT";
    
    $prog_value = $value;
    if ($prog_value == 100){
	$stop_mi->configure(-state => 'disabled');
	$run_mi->configure(-state => 'normal');
	$run_Bt->configure(-state => 'normal');
	$stop_Bt->configure(-state => 'disabled');
	$mw->afterCancel($barId);
    }
    $bar->update;
}

#####################################################################################################
#                                  Configuration of the interface
#####################################################################################################

sub preferences {
    my %prefs; # local copy

    foreach (keys %look){
	$prefs{$_} = $look{$_};
    }
    $prefs{"graphcolors"} = [ split /\s*,\s*/,$look{"graphcolors"} ];
    
    if(!Exists(my $pref_W)){
	
#---------------------------------------------------------------------------------------------------- 
#                                         Main Widget 
#-----------------------------------------------------------------------------------------------------
	
	$pref_W = $mw->Toplevel();
	$pref_W->resizable(0,0);
	$pref_W->geometry("+".$look{"xsecond"}."+".$look{"ysecond"}); 
	$pref_W->title("Preferences");
	# users cannot kill the window via decorations
	$pref_W->protocol('WM_DELETE_WINDOW' => sub {});
	# suppression of the iconifying decoration
	$pref_W->transient($pref_W->Parent->toplevel);

#---------------------------------------------- close Frame ------------------------------------------

	my $close_Fr = $pref_W->Frame->pack(-side=>'bottom');

	$close_Fr->Button(-text             => "accept",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{ 
			      foreach (keys %prefs){
				  $look{$_} = $prefs{$_};
			      }
			      $look{"graphcolors"} = join ",",@{ $prefs{"graphcolors"} };
			      $pref_W->grabRelease();
			      $pref_W ->destroy }
			  )->pack(-side => 'left');

	$close_Fr->Button(-text             => "cancel",
			  -activeforeground => $look{"activeforeground"},
			  -activebackground => $look{"activebackground"},
			  -command          => sub{ $pref_W->grabRelease();
						    $pref_W ->destroy }
			  )->pack(-side => 'left');

#------------------------------------------- The NoteBook ------------------------------------------- 
	
	my $pref_Nb = $pref_W->NoteBook()->pack(-side => 'top');

#----------------------------------------- Plot -----------------------------------------------------

	my $plot = $pref_Nb->add("plot", -label => "plot");	
	
	$plot->Label(-text => "Width of the plot",
		     )->grid(-row    => 0,
			     -column => 0);

	$plot->Entry(-textvariable => \$prefs{"plotwidth"},
		     -foreground   => $look{"entryforeground"},
		     -background   => $look{"entrybackground"},
		     )->grid(-row        => 0,
			     -column     => 1,
			     -columnspan => 2);

	$plot->Label(-text => "Height of the plot",
		     )->grid(-row    => 1,
			     -column => 0);

	$plot->Entry(-textvariable => \$prefs{"plotheight"},
		     -foreground   => $look{"entryforeground"},
		     -background   => $look{"entrybackground"},
		     )->grid(-row        => 1,
			     -column     => 1,
			     -columnspan => 2);

	$plot->Label(-text => "Nb ticks on the value axis",
		     )->grid(-row    => 2,
			     -column => 0);

	$plot->Entry(-textvariable => \$prefs{"numord"},
		     -foreground   => $look{"entryforeground"},
		     -background   => $look{"entrybackground"},
		     )->grid(-row        => 2,
			     -column     => 1,
			     -columnspan => 2);

	$plot->Label(-text => "Nb of ticks on the time axis",
		     )->grid(-row    => 3,
			     -column => 0);

	$plot->Entry(-textvariable => \$prefs{"numabs"},
		     -foreground   => $look{"entryforeground"},
		     -background   => $look{"entrybackground"},
		     )->grid(-row        => 3,
			     -column     => 1,
			     -columnspan => 2);

	$plot->Label(-text => "Type of plot",
		      )->grid(-row        => 4,
			      -column     => 0,
			      );
	
	$plot->Radiobutton(-text     => 'points',
			   -variable => \$prefs{"plottype"},
			   -value    => '0'
			   )->grid(-row    => 4,
				   -column => 1,
				   );

	$plot->Radiobutton(-text     => 'lines',
			   -variable => \$prefs{"plottype"},
			   -value    => '1'
			   )->grid(-row    => 4,
				   -column => 2,
				   );

	$plot->Label(-text => "Number of points to plot",
		     )->grid(-row    => 5,
			     -column => 0);

	$plot->Entry(-textvariable => \$prefs{"numpoints"},
		     -foreground   => $look{"entryforeground"},
		     -background   => $look{"entrybackground"},
		     )->grid(-row        => 5,
			     -column     => 1,
			     -columnspan => 2);
	
	$plot->Label(-text => "Color of the background",
		     )->grid(-row    => 6,
			     -column => 0);
	
	my $canvback_Bt = $plot->Button(-background       => $prefs{"canvasbackground"},
					-activebackground => $prefs{"canvasbackground"},					
					)->grid(-row        => 6,
						-column     => 1,
						-columnspan => 2,
						-sticky     => 'nsew');
	
	$canvback_Bt->configure(-command => sub{
	    my $color = $canvback_Bt->chooseColor(-initialcolor => $prefs{"canvasbackground"});
	    if ($color){
		$prefs{"canvasbackground"} = $color;
		$canvback_Bt->configure(-background       => $prefs{"canvasbackground"},
					-activebackground => $prefs{"canvasbackground"},
					);
	    }
	});
				
	$plot->Label(-text => "Color of the successive plots",
		     )->grid(-row    => 7,
			     -column => 0,
			     );

	my $colors_Fr = $plot->Frame()->grid(-row        => 8,
					     -column     => 0,
					     -columnspan => 3
					     );
	
	my $colors_Bt = [];
	print_colors($prefs{"graphcolors"},$colors_Fr,$colors_Bt);

	$plot->Button(-text             => 'more',
		      -activeforeground => $look{"activeforeground"},
		      -activebackground => $look{"activebackground"},
		      -command          => sub{
			  push @{ $prefs{"graphcolors"} }, '#000000'; 
			  for (0..$#$colors_Bt){$colors_Bt->[$_]->destroy}
			  $colors_Bt = [];
			  print_colors($prefs{"graphcolors"},$colors_Fr,$colors_Bt);
		      }
		      )->grid(-row    => 7,
			      -column => 1,
			      -sticky => 'nsew'
			     );	
	$plot->Button(-text             => 'less',
		      -activeforeground => $look{"activeforeground"},
		      -activebackground => $look{"activebackground"},
		      -command          => sub{
			  pop @{ $prefs{"graphcolors"} }; 
			  for (0..$#$colors_Bt){$colors_Bt->[$_]->destroy}
			  $colors_Bt = [];
			  print_colors($prefs{"graphcolors"},$colors_Fr,$colors_Bt);
		      }
		      )->grid(-row    => 7,
			      -column => 2,
			      -sticky => 'nsew');	

#----------------------------------------- Windows --------------------------------------------------

	my $windows = $pref_Nb->add("windows", -label => "windows");	

	$windows->Label(-text => "Position of the windows \n(X then Y in pixels from the upper-left corner)",
			)->grid(-row        => 0,
				-column     => 0,
				-columnspan => 3);
	
	$windows->Label(-text => "First level",
			)->grid(-row        => 1,
				-column     => 0,
				);
	
	$windows->Entry(-textvariable => \$prefs{"xmain"},
			-foreground   => $look{"entryforeground"},
			-background   => $look{"entrybackground"},
			)->grid(-row    => 1,
				-column => 1);
	
	$windows->Entry(-textvariable => \$prefs{"ymain"},
			-foreground   => $look{"entryforeground"},
			-background   => $look{"entrybackground"},
			)->grid(-row    => 1,
				-column => 2);
	
	$windows->Label(-text => "Second level",
			)->grid(-row        => 2,
				-column     => 0,
				);
	
	$windows->Entry(-textvariable => \$prefs{"xsecond"},
			-foreground   => $look{"entryforeground"},
			-background   => $look{"entrybackground"},
			)->grid(-row    => 2,
				-column => 1);
	
	$windows->Entry(-textvariable => \$prefs{"ysecond"},
			-foreground   => $look{"entryforeground"},
			-background   => $look{"entrybackground"},
			)->grid(-row    => 2,
				-column => 2);
	
	$windows->Label(-text => "Third level",
			)->grid(-row        => 3,
				-column     => 0,
				);
	
	$windows->Entry(-textvariable => \$prefs{"xthird"},
			-foreground   => $look{"entryforeground"},
			-background   => $look{"entrybackground"},
			)->grid(-row    => 3,
				-column => 1);
	
	$windows->Entry(-textvariable => \$prefs{"ythird"},
			-foreground   => $look{"entryforeground"},
			-background   => $look{"entrybackground"},
			)->grid(-row    => 3,
				-column => 2);
	
	$windows->Label(-text => "Fourth level",
			)->grid(-row        => 4,
				-column     => 0,
				);
	
	$windows->Entry(-textvariable => \$prefs{"xfourth"},
			-foreground   => $look{"entryforeground"},
			-background   => $look{"entrybackground"},
			)->grid(-row    => 4,
				-column => 1);
	
	$windows->Entry(-textvariable => \$prefs{"yfourth"},
			-foreground   => $look{"entryforeground"},
			-background   => $look{"entrybackground"},
			)->grid(-row    => 4,
				-column => 2);
	
#----------------------------------------- Paths ----------------------------------------------------
	
	my $paths = $pref_Nb->add("paths", -label => "paths");	
	
	$paths->Label(-text => "main configuration directory",
		      )->grid(-row        => 0,
			      -column     => 0,
			      -columnspan => 2
			      );
	
	$paths->Entry(-textvariable => \$prefs{"confdir"},
		      -foreground   => $look{"entryforeground"},
		      -background   => $look{"entrybackground"},
		      )->grid(-row        => 1,
			      -column     => 0,
			      -columnspan => 2,
			      -sticky     => 'ew'
			      );
	
	$paths->Label(-text => "favourite PDF reader",
		      )->grid(-row        => 3,
			      -column     => 0,
			      -columnspan => 2
			      );
	
	$paths->Entry(-textvariable => \$prefs{"pdfreader"},
		      -foreground   => $look{"entryforeground"},
		      -background   => $look{"entrybackground"},
		      )->grid(-row    => 4,
			      -column => 0,
			      -sticky => 'ew'
			      );

	$paths->Button(-text =>          => 'browse', 
		       -activeforeground => $look{"activeforeground"},
		       -activebackground => $look{"activebackground"},
		       -command          => sub{ 
			   (my $initdir = $prefs{"pdfreader"}) =~ s/$sep[^$sep]+$//;
			   my $answer = $mw->getOpenFile(-title      => "Choose your favourite PDF reader",
							 -initialdir => $initdir
							 );
			   $prefs{"pdfreader"} = $answer if $answer;
		       }
		       )->grid(-row    => 4,
			       -column => 1,
			       -sticky => 'ew',
			       );

#----------------------------------------- Misc -----------------------------------------------------

	my $misc = $pref_Nb->add("misc", -label => "misc");

	$misc->Label(-text => "Default balloon state",
		      )->grid(-row        => 0,
			      -column     => 0,
			      );
	
	$misc->Radiobutton(-text     => 'on',
			   -variable => \$prefs{"balloon"},
			   -value    => 'on'
			   )->grid(-row        => 0,
				   -column     => 1,
				   );

	$misc->Radiobutton(-text     => 'off',
			   -variable => \$prefs{"balloon"},
			   -value    => 'off'
			   )->grid(-row        => 0,
				   -column     => 2,
				   );

	$misc->Label(-text => "Balloon width",
		     )->grid(-row        => 1,
			     -column     => 0,
			     );

	$misc->Entry(-textvariable => \$prefs{"BalloonWidth"},
		     -foreground   => $look{"entryforeground"},
		     -background   => $look{"entrybackground"},
		     )->grid(-row        => 1,
			     -column     => 1,
			     -columnspan => 2
			     );

	$misc->Label(-text => "Progress bar refreshment interval (in sec)",
		     )->grid(-row        => 2,
			     -column     => 0,
			     );

	$misc->Entry(-textvariable => \$prefs{"refresh"},
		     -foreground   => $look{"entryforeground"},
		     -background   => $look{"entrybackground"},
		     )->grid(-row        => 2,
			     -column     => 1,
			     -columnspan => 2
			     );

    } else { 
	$pref_W->deiconify();
	$pref_W->raise(); 
    } 
}

sub print_colors{
    my ($graphcolors,$colors_Fr,$colors_Bt) = @_;

    for my $idx (0..$#$graphcolors){
	$colors_Bt->[$idx] = $colors_Fr->Button(-background       => $graphcolors->[$idx],
						-activebackground => $graphcolors->[$idx],
						-command          => sub{
						    my $color = $colors_Fr->chooseColor(-initialcolor => $graphcolors->[$idx]);
						    if ($color){
							$graphcolors->[$idx] = $color;
							$colors_Bt->[$idx]->configure(-background       => $graphcolors->[$idx],
										      -activebackground => $graphcolors->[$idx],
										      );
						    }
						}
						)->pack(-side => 'left');	
    }
}
