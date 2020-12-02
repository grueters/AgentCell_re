################################################################################
#                          perl interface for stochsim                       
# Original author of stochsim (Win32 version) and its MS Windows graphical   
# interface                              1998 : Carl Morton-Firth         
# Original port to Linux                 1999 : Tom Simon Shimizu         
# Corrections and perl textuel interface 2000 : Nicolas Le Novère        
#
# File: readinifile.pm
#
# Aim:  Reads the configuration files of stochsim
#
# Author: Nicolas Le Novère
#
# Date of last modification: 13/OCT/2003
#
################################################################################

#     This program is free software; you can redistribute it and/or modify it 
#     under the terms of the GNU General Public License as published by the Free
#     Software Foundation; either version 2 of the License, or (at your option) 
#     any later version.
#
#     This program is distributed in the hope that it will be useful, but
#     WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#     or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
#     for more details.
#
#     You should have received a copy of the GNU General Public License along 
#     with this program; if not, write to the Free Software Foundation, Inc., 59
#     Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#     Nicolas Le Novère 
#     Department of Zoology, University of Cambridge, Downing street, 
#     CB2 3EJ Cambridge, UK. e-mail: nl223@cus.cam.ac.uk

package readinifile;

use strict;
use File::Basename;
use vars qw($VERSION);
use Data::Dumper;

## set the version for version checking;
$VERSION = 0.061;

# This is a description of a reference to a hash describing a simulation

#$sim = {
#    "main_config_dir"      => "",  
# contains the address of the main directory containing the configuration files

#    "general_config"       => {}, 
# hash containing the substance of the file STCHSTC.INI

#    "components"           => {}, 
# hash of hashes containing the elementary blocks of the simulation each 
# element possesses three keys 'Name', 'Symbol' and 'Description'.
# { Symbol => { Name        => scalar,
#               Description => scalar } }

#    "complexes"            => {}, 
# Hash of hashes containing the objects of the reactions. Each element 
# possesses four mandatory keys 'Symbol', 'InitLevel', Constant and 'Components'. 
# 'Components' points to an array containing the symbols of the components forming 
# the complex. In addition the optional key 'Description' point to a short 
# description of the complex 
# { Symbol => { InitLevel   => scalar,
#               Constant    => scalar (0|1)
#               Description => scalar
#               Components  => [] } }

#    "display_variables"    => [], 
# array of hashes containing the variables to be displayed (Useful only if 
# DisplayAllLevels = 0). Each element possesses two keys 'Name' and 'Types'.
# 'Types' is an array containing the complexes included in the variable.
# { Name  => scalar,
#   Types => [] }

#    "reactions"            => [], 
# array of hashes containing description of reactions in the system. Keys are 
# 'Substrates', 'Products', 'Kf', 'Kr', 'Description'. The two formers are 
# arrays
# { Description => scalar,
#   Kf          => scalar,
#   Kr          => scalar,
#   Substrates  => [],
#   Products    => [] }

#    "dynamic_values"       => {}, 
# hash, which keys are the codes of the dynamic values. Each point to a hash 
# which keys are the timepoints  and the values are ... the values.
#
#    "ms_complexes"         => {}, 
# Hash of hashes containing description of the multistates.  The keys
# are the 'Symbol' representing the complex.  Each complex hash
# possess the keys 'File','Description', 'Flags', 'InitLevel',
# 'Rapid_equilibriums', 'Display_variables', 'Reactions'. 'File' is
# the address of the file containing the specification of this
# ns_complex. 'Flags' is an array containing the name of the various
# flags. 'InitLevel' is a hash, which keys are the states (including
# wildcards). 'Rapid_equilibriums' is an array of hashes which keys
# are 'Flag', which value is the name of the flag affected by this
# rapid equilibrium and the states, which values are
# probabilities. 'Display_variables' is an array of hashes which keys
# are 'Name' and 'States'. States is an array of bitstring
# representing the states handled by this variable. 'Reactions' is an
# array of hashes which keys are 'Symbol', 'Effect' and the
# states. 'Effect' point to an array of values composed of + or -
# followed by one of the flags.
#  { Symbol => { File               => scalar,
#                Description        => scalar,
#                Flags              => []
#                InitLevel          => { _bitstring_ => scalar },
#                Rapid_equilibriums => [ { Description => scalar,
#                                          Flag        => scalar,
#                                          _bitstring_ => scalar } ],
#                Display_variables  => [ { Name   => scalar,
#                                          States => [] } ]
#                Reactions          => [ { Description => scalar,
#                                          Symbol      => scalar,
#                                          Effect      => [], 
#                                          _bitstring_ => scalar } ] }

#    "ns_complexes"         => {}, 
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
#                                        CoupledStates => scalar,
#                                        _bitstring_   => [] } ]
#              Reactions          => [ { Description       => scalar,
#                                        EffectOnNeighbour => [],
#                                        ReactNeighbourID  => scalar,
#                                        _bitstring_       => scalar } ] }

#    "array_config"         => {}, 
# hash containing the general configuration read in ARRAY.INI
# { DumpInterval       => scalar,
# }
#
#    "arrays"               => {}, 
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
#              BoundaryCondition     => scalar,  integer: 0 is toroïdal, 0 otherwise
#              EquilibrationInterval => scalar,   
#              InitialState          => scalar,
#              InitialStateFile      => scalar,
#              InitialStateFromTime  => scalar
#              Snapshot_variables    => { Name => { States          => [],      states shown in the snapshots} 
#                                                   StoreInterval   => scalar,  
#                                                   AverageOutput   => scalar,
#                                                   AverageInterval => scalar,
#                                                   SampleInterval  => scalar } } } }
#};

sub OpenFile{
    my ($sim) = @_; # a whole simulation
    my $line;       # read a line of an infile

    # wipe out the previous simulation. DO NOT UNDEF THE LISTS. 

    undef %{ $sim->{"general_config"} };
    undef %{ $sim->{"components"} };           
    undef %{ $sim->{"complexes"} };    
    $sim->{"reactions"}         = [];            
    $sim->{"display_variables"} = [];    
    undef %{ $sim->{"ms_complexes"} }; 
    undef %{ $sim->{"ns_complexes"} };
    undef %{ $sim->{"array_config"} };   
    undef %{ $sim->{"arrays"} };
    undef %{ $sim->{"dynamic_values"} };   
                                 
    ########################################
    # Read the the main configuration file
    ########################################
    
    open (OCF, "$sim->{main_config_dir}/Input/STCHSTC.INI") 
	or die "can't open the general configuration file $sim->{main_config_dir}/Input/STCHSTC.INI: $!";
    
    while (defined($line = <OCF>)){
	$line =~ s/\r?\n$//;
	if ($line =~ /^\w/){    
	    %{ $sim->{"general_config"} } = (%{ $sim->{"general_config"} },read_ini_line($line));
	}
    }
    close(OCF) 
	or die "can't close the general configuration file $sim->{main_config_dir}/Input/STCHSTC.INI: $!";

    # the following lines convert old INI files, such as those generated by StochSim 1.0
    
#    if (exists $sim->{"general_config"}{"DisplayFrequency"}){
#	$sim->{"general_config"}{"DisplayInterval"} = $sim->{"general_config"}{"DisplayFrequency"};
#	delete $sim->{"general_config"}{"DisplayFrequency"};
#    }
    if (exists $sim->{"general_config"}{"StoreFrequency"}){
	$sim->{"general_config"}{"StoreInterval"} = $sim->{"general_config"}{"StoreFrequency"};
	delete $sim->{"general_config"}{"StoreFrequency"};
    }

# This is a purely arbitrary decision

    if (exists $sim->{"general_config"}{"StoreInterval"}){	
	$sim->{"general_config"}{"DisplayInterval"} = $sim->{"general_config"}{"StoreInterval"};
    } 

    #######################################
    # Read the complex configuration file
    #######################################
    
    my ($var_nb,$react_nb) = (0,0); # indices of read objects
    my ($component,$complex,@read_complexes);
    my %complexes;

    open (OCPLX, "$sim->{main_config_dir}/Input/COMPLEX.INI") 
	or die "can't open the complex initialisation file $sim->{main_config_dir}/Input/COMPLEX.INI: $!";

  COMPLEX: while (defined($line = <OCPLX>)){ # Read the lines until the end of the file
      $line =~ s/\r?\n$//;
      
      #-------------------
      # [General] section
      #-------------------

      if ($line =~ /^(\[General)/){
	  $line = <OCPLX>;
	  $line =~ s/\r?\n$//;	      
	  while (defined $line and $line !~ /^\[/){ 
	      if ($line =~ /^InitialLevelsFromDumpFile/){
		  $sim->{"general_config"}{"InitialLevelsFromDumpFile"} = (read_ini_line($line))[1];
	      } elsif ($line =~ /^InitialLevelsFile/){
		  $sim->{"general_config"}{"InitialLevelsFile"} = (read_ini_line($line))[1];
	      }	elsif ($line =~ /^InitialLevelsFromTime/){
		  $sim->{"general_config"}{"InitialLevelsFromTime"} = (read_ini_line($line))[1];
	      }	  
	      $line = <OCPLX>;
	      $line =~ s/\r?\n$//;
	  }
	  redo; # Quit the General section and run again without acquiring another line
      #-----------------------
      # [Component X] section
      #-----------------------

      } elsif ($line =~ /^(\[Component\s+)(\w+)/){
	  # FIXME: check the component symbol => separate subfunction
	  $component = $2;
	  $sim->{"components"}{$component} = {};
	  $line = <OCPLX>;
	  $line =~ s/\r?\n$//;
	  while (defined $line and $line !~ /^\[/){ 
	      if ($line =~ /^Name/){
		  $sim->{"components"}{$component}{"Name"} = (read_ini_line($line))[1];
	      } elsif ($line =~ /^Description/){
		  $sim->{"components"}{$component}{"Description"} = (read_ini_line($line))[1];
	      } else {} # Do nothing
	      $line = <OCPLX>;
	      $line =~ s/\r?\n$//;
	  }
	  redo; # Quit the Component section and run again without acquiring another line
	  
	  #-------------------------
	  # [Complex Types] section
	  #-------------------------
	  
      } elsif ($line =~ /^\[Complex\sTypes/){
	  $line = <OCPLX>;
	  $line =~ s/\r?\n$//;	  
	  while (defined $line and $line !~ /^\[/){
	      # NumRapidEqm and NumOfSets are determined by the remaining configuration items
	      if ($line =~ /^Set(\d+)/){ 
		  $complexes{$1}{"Symbol"} = (read_ini_line($line))[1]; 
	      } elsif ($line =~ /^Comp(\d+)/){ 
		  $complexes{$1}{"Components"} = [ split /\s*,\s*/,(read_ini_line($line))[1] ]; 
	      } elsif ($line =~ /^Description(\d+)/){
		  $complexes{$1}{"Description"} = (read_ini_line($line))[1]; 
	      } else {} # Do nothing
	      $line = <OCPLX>;
	      $line =~ s/\r?\n$//;
	  }	
	  while ( my ($key, $value) = each (%complexes) ){
	      # Caution. %complexes is not $sim->{"complexes"}. The symbol of the complex
	      # is $value->{"Symbol"} and not $key
	      ParseCplxSymb($sim,$value);
 	      $sim->{"complexes"}{$value->{"Symbol"}}{"Description"} = $value->{"Description"};
	  }
	  redo; # Quit the Complex Types section and run again without acquiring another line
	  
	  #-------------------------
	  # [Initial Levels] section
	  #-------------------------
	  
      } elsif ($line =~ /^\[Initial\sLevels/){
	  $line = <OCPLX>;
	  $line =~ s/\r?\n$//;
	  while (defined $line and $line !~ /^\[/){ 
	      if ($line =~ /^\s*(;+.*)?$/){
		  $line = <OCPLX>;
		  $line =~ s/\r?\n$//; 
		  next;
	      }
	      if ($line =~ /^Constants/){
		  my @constants = split /\s*,\s*/,(read_ini_line($line))[1];
		  foreach my $cplx (@constants){
		      $sim->{"complexes"}{$cplx}{"Constant"} = 1;
		  }; 
	      } else {
		  # Be careful, here we read all the lines. There is no keyword test before treatment
		  my ($complex_symb,$complex_conc) = read_ini_line($line);
		  $sim->{"complexes"}{$complex_symb}{"InitLevel"} = $complex_conc;
	      }
	      $line = <OCPLX>;
	      $line =~ s/\r?\n$//;
	  }	 
	  redo; # Quit the Initial Levels section and run again without acquiring another line
	  
	  #---------------------------------
	  # [Special Complex Types] section	  
          #---------------------------------
	  
      } elsif ($line =~ /^\[Special\sComplex\sTypes/){
	  $line = <OCPLX>;
	  $line =~ s/\r?\n$//;
	  while (defined $line and $line !~ /^\[/){
	      if ($line =~ /^(\w+)(?=INIFile)/){
		  $sim->{"ms_complexes"}{$1} = {};
		  # I do not want the path, then I strip it
		  $sim->{"ms_complexes"}{$1}{"File"} = basename((read_ini_line($line))[1],".INI").".INI";
	      } else {} # do nothing. The symbol is determined from the filename line
	      $line = <OCPLX>;
	      $line =~ s/\r?\n$//;
	  }
	  redo; # Quit the Special Complex Types section and run again without acquiring another line

	  #---------------------------------------------
	  # [Neighbour Sensitive Complex Types] section
	  #---------------------------------------------
	  # This could change soon. For the moment it is redundant whith the entry in the ARRAY.INI file

      } elsif ($line =~ /^\[Neighbour\sSensitive\sComplex\sTypes/
	       and $sim->{"general_config"}{"UseSpatialExtensions"} == 1){
	  $line = <OCPLX>;
	  $line =~ s/\r?\n$//;
	  while (defined $line and $line !~ /^\[/){
	      if ($line =~ /^(\w+)(?=_NS_INIFile)/){
		  $sim->{"ns_complexes"}{$1} = {};
		  # I do not want the path, then I strip it
		  $sim->{"ns_complexes"}{$1}{"File"} = basename((read_ini_line($line))[1],".INI").".INI";
	      } else {} # do nothing. The symbol is determined from the filename line
	      $line = <OCPLX>;
	      $line =~ s/\r?\n$//;
	  }
	  redo; # Quit the Special Complex Types section and run again without acquiring another line

	  #----------------------------
	  # [Display Variable] section
	  #----------------------------
	  
      } elsif ($line =~ /^\[Display\sVariable/){
	  $line = <OCPLX>;
	  $line =~ s/\r?\n$//;
	  while (defined $line and $line !~ /^\[/){
	      if ($line =~ /^Name/){
		  $sim->{"display_variables"}[$var_nb]{"Name"} = (read_ini_line($line))[1];
	      } elsif ($line =~ /^Types/){
		  $sim->{"display_variables"}[$var_nb]{"Types"} = [ split /\s*,\s*/,(read_ini_line($line))[1] ]; 
	      } else {}	  # Do nothing
	      $line = <OCPLX>;
	      $line =~ s/\r?\n$//;
	  }
	  $var_nb++;
	  redo;  # Quit the Display Variable section and run again without acquiring another line
      } elsif (not defined ($line)){
	  last COMPLEX;
      } else {}  # Do nothing. If we do not know what is on the line, we ignore.   
  }
    close(OCPLX) 
	or die "can't close the complex initialisation file $sim->{main_config_dir}/Input/COMPLEX.INI: $!";

    # Check that all non constant complexes are tagged as such
    while( my ($key,$value) = each %{ $sim->{"complexes"} }){
	$sim->{"complexes"}{$key}{"Constant"} = 0 if (not defined $sim->{"complexes"}{$key}{"Constant"});
    }

    #######################################
    # Read the reaction configuration file
    #######################################

    open (OREACT,"$sim->{main_config_dir}/Input/REACTION.INI") 
	or die "can't open the reaction initialisation file $sim->{main_config_dir}/Input/REACTION.INI: $!";
    
    $sim->{"reactions"} = [];
  REACTION: while (defined($line = <OREACT>)){   # Read the lines until the end of the file
      $line =~ s/\r?\n$//;
      
      #-------------------
      # [General] section
      #-------------------

# The general section of the complex.ini file does not contain anything 
# informative. Its content will be determined from the remaining configuration 
# items.

      #----------------------
      # [Reaction X] section
      #----------------------

      if ($line =~ /^\[Reaction/){
	  my @CstSubst;
	  my @CstProd;
	  $line = <OREACT>;
	  $line =~ s/\r?\n$//;
	  while (defined $line and $line !~ /^\[/){
	      if ($line =~ /^Description/){
		  $sim->{"reactions"}[$react_nb]{"Description"} = (read_ini_line($line))[1];
	      } elsif ($line =~ /^Substrates/){
		  push(@{ $sim->{"reactions"}[$react_nb]{"Substrates"} }, @{[ split /\s*,\s*/,(read_ini_line($line))[1] ]}); 
	      } elsif ($line =~ /^ConstantSubstrates/){
		  @CstSubst = split /,/,(read_ini_line($line))[1];
		  push(@{ $sim->{"reactions"}[$react_nb]{"Substrates"} }, @CstSubst); 
	      } elsif ($line =~ /^Products/){
		  push(@{ $sim->{"reactions"}[$react_nb]{"Products"} }, @{[ split /\s*,\s*/,(read_ini_line($line))[1] ]});
 	      } elsif ($line =~ /^ConstantProducts/){
		  @CstProd = split /\s*,\s*/,(read_ini_line($line))[1];
		  push(@{ $sim->{"reactions"}[$react_nb]{"Products"} }, @CstProd); 
	      } elsif ($line =~ /^kf/){
		  $sim->{"reactions"}[$react_nb]{"Kf"} = (read_ini_line($line))[1]; 
		  $sim->{"reactions"}[$react_nb]{"Kf"} =~ /([\d\.eE\-\+]+)(@(\S+))?/;
		  my $KF = $1;
		  my $Fdyn = $3;
		  foreach my $CstSubst (@CstSubst){
		      # CAUTION: Need to read COMPLEX.INI _before_ REACTION.INI
		      $KF *= ($sim->{"general_config"}{"ReactionVolume"} * 6.0225e23)/ $sim->{"complexes"}{$CstSubst}{"InitLevel"};
		  }
		  $Fdyn ? ( $sim->{"reactions"}[$react_nb]{"Kf"} = "$KF\@$Fdyn" ) : ( $sim->{"reactions"}[$react_nb]{"Kf"} = $KF ); 
	      } elsif ($line =~ /^kr/){
		  $sim->{"reactions"}[$react_nb]{"Kr"} = (read_ini_line($line))[1];
		  $sim->{"reactions"}[$react_nb]{"Kr"} =~ /([\d\.eE\-\+]+)(@(\S+))?/;
		  my $KR = $1;
		  my $Rdyn = $3;
		  foreach my $CstProd (@CstProd){
		      # CAUTION: Need to read COMPLEX.INI _before_ REACTION.INI
		      $KR *= ($sim->{"general_config"}{"ReactionVolume"} * 6.0225e23)/ $sim->{"complexes"}{$CstProd}{"InitLevel"};
		  }
		  $Rdyn ? ( $sim->{"reactions"}[$react_nb]{"Kr"} = "$KR\@$Rdyn" ) : ( $sim->{"reactions"}[$react_nb]{"Kr"} = "$KR" );
	      } else {}
	      $line = <OREACT>;
	      $line =~ s/\r?\n$//;
	  }
	  $react_nb++;                # We go to the next reaction
	  redo; # Quit the Reaction section and run again without acquiring another line
      } elsif (not defined ($line)){
	  last REACTION;
      } else {}	  # Do nothing. If we do not know what is on the line, we ignore.   
  }
    close(OREACT) or die "can't open the reaction initialisation file $sim->{main_config_dir}/Input/REACTION.INI: $!";

    ############################################
    # Read the dynamic value configuration file
    ############################################
    my ($time,$dyn_value,$change);  
    my %times; # to collect all times
    
    open (ODYN, "$sim->{main_config_dir}/Input/DYNAMIC.INI") 
	or die "can't open the dynamic value configuration file $sim->{main_config_dir}/Input/DYNAMIC.INI: $!";


  DYNAMIC: while (defined($line=<ODYN>)){ # Read the lines until the end of the file
      $line =~ s/\r?\n$//;

      #-------------------
      # [General] section
      #-------------------

# The general section of the complex.ini file does not contain anything 
# informative. Its content will be determined from the remaining configuration 
# items.

      #------------------
      # [Time X] section
      #------------------

      if ($line =~ /^\[Time\s*((?=\d|\.\d)\d*(\.\d*)?([eE]([+-]?\d+))?)/){          # Here is an instant to change dynamic values	      

	  $time = $1;
	  $times{$1} = undef; # we don't care of the value
	  $line = <ODYN>;
	  $line =~ s/\r?\n$//;
	  while (defined $line and $line !~ /^\[/){
	      if ($line =~ /^\s*(;+.*)?$/){
		  $line = <ODYN>;
		  $line =~ s/\r?\n$//;
		  next;
	      }
	      ($dyn_value,$change) = (read_ini_line($line));
	      if (defined $change and $change =~ /\S+/){
		  $sim->{"dynamic_values"}{$dyn_value}{$time} = $change;
	      } 
	      $line = <ODYN>;
	      $line =~ s/\r?\n$//;
	  }
	  redo; # Quit the instant section and run again without acquiring another line
      } elsif (not defined ($line)){
	  last DYNAMIC;
      } else {
	  # Do nothing
      }
  }

    close(ODYN) or die "can't open the dynamic value configuration file $sim->{main_config_dir}/Input/DYNAMIC.INI: $!";

    my %modified; # ensure that a dynamic value is modified only once
    foreach my $dynvalue (keys %{ $sim->{"dynamic_values"}}){
	$modified{$dynvalue} = 0;
	foreach (keys %times){
	    $sim->{"dynamic_values"}{$dynvalue}{$_} = undef if not exists $sim->{"dynamic_values"}{$dynvalue}{$_};
	}
    }
    
    # CAUTION: Need to read REACTION.INI _before_ DYNAMIC.INI
    foreach my $react (@{ $sim->{"reactions"} }){
	if ($react->{"Kf"} =~ /@(\S+)/){
	    my $fdyn = $1;
	    my $found = 0;
	    foreach my $dyn (keys %{ $sim->{"dynamic_values"} }){
		if ($fdyn eq $dyn){
		    $found++;
		    last if $modified{$dyn};
		    foreach my $cplx (@{ $react->{"Substrates"} }){
                        # CAUTION: Need to read COMPLEX.INI _before_ DYNAMIC.INI
			if ($sim->{"complexes"}{$cplx}{"Constant"}){  
			    $modified{$dyn}++;
			    foreach my $timepoint (keys %{ $sim->{"dynamic_values"}{$dyn} }){
				$sim->{"dynamic_values"}{$dyn}{$timepoint} *= ($sim->{"general_config"}{"ReactionVolume"} * 6.0225e23)/ $sim->{"complexes"}{$cplx}{"InitLevel"};
			    }
			}
		    }
		    last;
		}
	    }
	    if (not $found){
		$react->{"Kf"} =~ s/@\S+//;
		warn "The dynamic value $fdyn required by one of the reaction is not defined in the file DYNAMIC.INI. I suppress the call.";
	    }
	} 
	if ($react->{"Kr"} =~ /@(\S+)/){
	    my $rdyn = $1;
	    my $found = 0;
	    foreach my $dyn (keys %{ $sim->{"dynamic_values"} }){
		if ($rdyn eq $dyn){
		    $found++;
		    last if $modified{$dyn};
		    foreach my $cplx (@{ $react->{"Products"} }){
                        # CAUTION: Need to read COMPLEX.INI _before_ DYNAMIC.INI
			if ($sim->{"complexes"}{$cplx}{"Constant"}){ 
			    $modified{$dyn}++;
			    foreach my $timepoint (keys %{ $sim->{"dynamic_values"}{$dyn} }){
				$sim->{"dynamic_values"}{$dyn}{$timepoint} *= ($sim->{"general_config"}{"ReactionVolume"} * 6.0225e23)/ $sim->{"complexes"}{$cplx}{"InitLevel"};
			    }
			}
		    }
		    last;
		}
	    }
	    if (not $found){
		$react->{"Kr"} =~ s/@\S+//;
		warn "The dynamic value $rdyn required by one of the reaction is not defined in the file DYNAMIC.INI. I suppress the call.";
	    }
	} 
    }

    # FIXME: check that all the dynamic values called in the REACTION.INI file are defined,
    # if not, warning and remove te Dyn value from Kf anf Kr

    ##################################################
    # Read the multistate complex configuration files
    ##################################################

    foreach my $ms (keys %{ $sim->{"ms_complexes"} }){
	my ($equ_number,$var_nb,$react_number) = (0,0,0);
	$sim->{"ms_complexes"}{$ms}{"Description"}        = undef;
	$sim->{"ms_complexes"}{$ms}{"Flags"}              = [];
	$sim->{"ms_complexes"}{$ms}{"InitLevel"}          = {};
	$sim->{"ms_complexes"}{$ms}{"Rapid_equilibriums"} = [];
	$sim->{"ms_complexes"}{$ms}{"Reactions"}          = [];

	open (OMS, "$sim->{main_config_dir}/Input/$sim->{ms_complexes}{$ms}{File}") 
	    or die "can't open the multistate complex configuration file $sim->{main_config_dir}/Input/$sim->{ms_complexes}{$ms}{File}: $!";

      MULTISTATE: while (defined($line = <OMS>)){ # Read the lines until the end of the file
	  $line =~ s/\015?\012$//;

	  #-------------------
	  # [General] section
	  #-------------------
	  if ($line =~ /^\[General/){
	      $line = <OMS>;
	      $line =~ s/\r?\n$//;	      
	      while (defined $line and $line !~ /^\[/){ 
		  if ($line =~ /^StateFlags/){
		      $sim->{"ms_complexes"}{$ms}{"Flags"} = [ split /\s*,\s*/,(read_ini_line($line))[1] ];
		  } elsif ($line =~ /^Description/){
		      $sim->{"ms_complexes"}{$ms}{"Description"} = (read_ini_line($line))[1];
		  }
		  $line = <OMS>;
		  $line =~ s/\r?\n$//;
	      }
	      redo; # Quit the general section and run again without acquiring another line
	      
	      #--------------------------
	      # [Initial States] section
	      #--------------------------
	      
	  } elsif ($line =~ /^\[Initial States/){
	      $line = <OMS>;
	      $line =~ s/\r?\n$//;	 
	      while (defined $line and $line !~ /^\[/){ 
		  if ($line =~ /^\s*(;+.*)?$/){
		      $line = <OMS>;
		      $line =~ s/\r?\n$//;
		      next;
		  } elsif ($line =~ /^[01\?]+\s*=/){       
                      # DOES not take the lines with "wildcards". Will be computed from the others
		      my ($key,$value) = (read_ini_line($line));
		      $sim->{"ms_complexes"}{$ms}{"InitLevel"}{$key}=$value;
		  } else {}; 	  
		  $line = <OMS>;
		  $line =~ s/\r?\n$//;
	      }
	      redo; # Quit the Initial States section and run again without acquiring another line

	      #-----------------------------
	      # [Rapid Equilibrium] section
	      #-----------------------------
	      
	  } elsif ($line =~ /^\[Rapid Equilibrium/){
	      $line = <OMS>;
	      $line =~ s/\r?\n$//;
	      while (defined $line and $line !~ /^\[/){ 
		  if ($line =~ /^Description/){
		      $sim->{"ms_complexes"}{$ms}{"Rapid_equilibriums"}[$equ_number]{"Description"} =  (read_ini_line($line))[1];
		  } elsif ($line =~ /^(State|Flag)/){ # 'State' for backward compatibility
		      $sim->{"ms_complexes"}{$ms}{"Rapid_equilibriums"}[$equ_number]{"Flag"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^[01\?]+\s*=/){     
                      # does NOT take the line with "Wildcards". Will be computed from the others
		      # CAUTION FIXME This assumes that all other lines are bitstrings. Whatif bullshit lines ? 
		      my ($key,$value) = (read_ini_line($line));
		      $sim->{"ms_complexes"}{$ms}{"Rapid_equilibriums"}[$equ_number]{$key} = $value;
		  } else {}; 		
		  $line = <OMS>;
		  $line =~ s/\r?\n$//;
	      }	      
	      $equ_number++;
	      redo; # Quit the Rapid Equilibrium section and run again without acquiring another line
	      

	      #----------------------------
	      # [Reaction] section
	      #----------------------------
	      
	  } elsif ($line =~ /^(\[Reaction\s+)(\w+)/){
	      $sim->{"ms_complexes"}{$ms}{"Reactions"}[$react_number]{"Symbol"} = $2;
	      $line = <OMS>;
	      $line =~ s/\r?\n$//;
	      while (defined $line and $line !~ /^\[/){ 
		  if ($line =~ /^Description/){
		      $sim->{"ms_complexes"}{$ms}{"Reactions"}[$react_number]{"Description"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^Effect/){
		      $sim->{"ms_complexes"}{$ms}{"Reactions"}[$react_number]{"Effect"} = [ split /\s*,\s*/,(read_ini_line($line))[1] ]; 
		  } elsif ($line =~ /^[01\?]+\s*=/){    
                      # does NOT take the lines with "wildcards". Will be computed from the others
		      # CAUTION FIXME This assumes that all other lines are bitstrings. Whatif bullshit lines ? 
		      my ($key,$value) = (read_ini_line($line));
		      $sim->{"ms_complexes"}{$ms}{"Reactions"}[$react_number]{$key} = $value;
		  } else {}; 		
		  $line = <OMS>;
		  $line =~ s/\r?\n$//;
	      }
	      $react_number++;
	      redo; # Quit the Reaction section and run again without acquiring another line

	      #----------------------------
	      # [Display Variable] section
	      #----------------------------

	  } elsif ($line =~ /^\[Display Variable\s+/){
	      $line = <OMS>;
	      $line =~ s/\r?\n$//;
	      while (defined $line and $line !~ /^\[/){
		  if ($line =~ /^Name/){
		      $sim->{"ms_complexes"}{$ms}{"Display_variables"}[$var_nb]{"Name"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^States/){
		      $sim->{"ms_complexes"}{$ms}{"Display_variables"}[$var_nb]{"States"} = [ split /\s*,\s*/,(read_ini_line($line))[1] ];
		  } else {}; 		
		  $line = <OMS>;
		  $line =~ s/\r?\n$//;
	      }
	      $var_nb++;
	      redo; # Quit the Display Variable section and run again without acquiring another line
	  } elsif (not defined ($line)){
	      last MULTISTATE;
	  } else {}	  
      };
	close(OMS) or die "can't open the multistate complex configuration file $sim->{main_config_dir}/Input/$sim->{ms_complexes}{$ms}{File}: $!";
    }

    ########################### 
    # Read the ARRAY.INI file
    ###########################

    if ($sim->{"general_config"}{"UseSpatialExtensions"} == 1){
	
	open (OA, "$sim->{main_config_dir}/Input/ARRAY.INI") 
	    or die  "can't open the array initialisation file $sim->{main_config_dir}/Input/ARRAY.INI: $!";

      ARRAY: while (defined($line = <OA>)){
	  $line =~ s/\015?\012$//;
      
	  #-------------------
	  # [General] section
	  #-------------------
	  
	  if ($line =~ /^\[General/){
	      $line = <OA>;
	      $line =~ s/\r?\n$//;
	      while (defined $line and $line !~ /^\[/){
		  if ($line =~ /^DumpInterval/){
		      $sim->{"array_config"}{"DumpInterval"} = (read_ini_line($line))[1];
		  } else {}  # paramater 'Arrays' is determined from array sections, 
		             # and 'NumSnapshotVariables' from the Snapshot Variable section
		  $line = <OA>;
		  $line =~ s/\r?\n$//;
	      }
	      redo; # Quit the General section and run again without acquiring another line

	      #---------------------------------------------
	      # [Neighbour Sensitive Complex Types] section
	      #---------------------------------------------

	  } elsif ($line =~ /^\[Neighbour\sSensitive\sComplex\sTypes/){
	      $line = <OA>;
	      $line =~ s/\r?\n$//;
	      while (defined $line and $line !~ /^\[/){ 
		  if ($line =~ /^(\w+)(?=_NS_INIFile)/){
		      $sim->{"ns_complexes"}{$1} = {};
		      # I do not want the path, then I strip it
		      $sim->{"ns_complexes"}{$1}{"File"} = basename((read_ini_line($line))[1],".INI").".INI";
		  } else {} # do nothing. The symbol is determined from the filename line
		  $line = <OA>;
		  $line =~ s/\r?\n$//;
	      }
	      redo; # Quit the Neighbour Sensitive Complex Types

	      #-----------------------------
	      # [Snapshot Variable] section
	      #-----------------------------

	  } elsif ($line =~ /^\[Snapshot\sVariable/){
	      my %snapshot;
	      $line = <OA>;
	      $line =~ s/\r?\n$//;
	      while (defined $line and $line !~ /^\[/){
		  if ($line =~ /^Array/){
		      $snapshot{"Array"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^Name/){
		      $snapshot{"Name"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^StoreInterval/){
		      $snapshot{"StoreInterval"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^AveragedOutput/){
		      $snapshot{"AveragedOutput"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^AverageInterval/){
		      $snapshot{"AverageInterval"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^SampleInterval/){
		      $snapshot{"SampleInterval"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^States/){
		      $snapshot{"States"} = [ split /\s*,\s*/,(read_ini_line($line))[1] ];
		  } else {}	  # Do nothing
		  $line = <OA>;
		  $line =~ s/\r?\n$//;
	      }
	      $sim->{"arrays"}{$snapshot{"Array"}}{"Snapshot_variables"}{$snapshot{"Name"}}{"StoreInterval"}   = $snapshot{"StoreInterval"};
	      $sim->{"arrays"}{$snapshot{"Array"}}{"Snapshot_variables"}{$snapshot{"Name"}}{"AveragedOutput"}  = $snapshot{"AveragedOutput"};
	      $sim->{"arrays"}{$snapshot{"Array"}}{"Snapshot_variables"}{$snapshot{"Name"}}{"AverageInterval"} = $snapshot{"AverageInterval"};
	      $sim->{"arrays"}{$snapshot{"Array"}}{"Snapshot_variables"}{$snapshot{"Name"}}{"SampleInterval"}  = $snapshot{"SampleInterval"};
	      foreach my $foo (@{ $snapshot{"States"} }){
		  push @{ $sim->{"arrays"}{$snapshot{"Array"}}{"Snapshot_variables"}{$snapshot{"Name"}}{"States"} }, $foo;
	      }
	      redo;  # Quit the Snapshot Variable section and run again without acquiring another line
	      
	      #-----------------------
	      # [ARRAY_NAME] section
	      #-----------------------
	      
	  } elsif ($line =~ /^\[(\w+)/ and $line !~ /(General|Neighbour\sSensitive\sComplex\sTypes|Snapshot\sVariable)/){
	      my $name = $1;
	      $line = <OA>;
	      $line =~ s/\r?\n$//;
	      while (defined $line and $line !~ /^\[/){ 
		  if ($line =~ /^Description/){
		      $sim->{"arrays"}{$name}{"Description"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^Complex/){
		      $sim->{"arrays"}{$name}{"Complex"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^NeighbourSensitive/){
		      $sim->{"arrays"}{$name}{"NeighbourSensitive"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^Geometry/){
		      $sim->{"arrays"}{$name}{"Geometry"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^XDimension/){
		      $sim->{"arrays"}{$name}{"XDimension"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^YDimension/){
		      $sim->{"arrays"}{$name}{"YDimension"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^CreateDumpFile/){
		      $sim->{"arrays"}{$name}{"CreateDumpFile"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^BoundaryCondition/){
		      $sim->{"arrays"}{$name}{"BoundaryCondition"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^EquilibrationInterval/){
		      $sim->{"arrays"}{$name}{"EquilibrationInterval"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^InitialState\b/){
		      $sim->{"arrays"}{$name}{"InitialState"} = (read_ini_line($line))[1];
		  } elsif ($line =~ /^InitialStateFile/){
		      $sim->{"arrays"}{$name}{"InitialStateFile"} = (read_ini_line($line))[1];
		      print "DEBUG-->readinifile: $sim->{arrays}{$name}{InitialStateFile}\n";
		  } elsif ($line =~ /^InitialStateFromTime/){
		      $sim->{"arrays"}{$name}{"InitialStateFromTime"} = (read_ini_line($line))[1];
	  } else {}
		  $line = <OA>;
		  $line =~ s/\r?\n$//;
	      }
	      redo; # Quit the General section and run again without acquiring another line
	  } elsif (not defined ($line)){
	      last ARRAY;
	  } else {}  # Do nothing. If we do not know what is on the line, we ignore.   
      }
	close (OA) or die "can't close the array initialisation file $sim->{main_config_dir}/Input/ARRAY.INI: $!";
	
	while( my ($ns,$snips) = each %{ $sim->{"ns_complexes"} }){
	    my ($ns_equ_nb,$ns_react_nb) = (0,0);
	    $sim->{"ns_complexes"}{$ns}{"NumNeighbours"}      = undef; 
	    $sim->{"ns_complexes"}{$ns}{"Description"}        = undef;
	    $sim->{"ns_complexes"}{$ns}{"Rapid_equilibriums"} = [];
	    $sim->{"ns_complexes"}{$ns}{"Reactions"}          = [];
	    
	    open (ONS, "$sim->{main_config_dir}/Input/$sim->{ns_complexes}{$ns}{File}") 
		or die"can't open the neighbour sensitive complex configuration file $sim->{main_config_dir}/Input/$sim->{ns_complexes}{$ns}{File}: $!";
	    
	  NEIGHBOUR: while (defined($line = <ONS>)){ # Read the lines until the end of the file
	      $line =~ s/\015?\012$//;
	      
	      #-------------------
	      # [General] section
	      #-------------------
	      
	      if ($line =~ /^\[General/){
		  $line = <ONS>;
		  $line =~ s/\r?\n$//;
		  while (defined $line and $line !~ /^\[/){
		      if ($line =~ /^NumNeighbours/){
			  $sim->{"ns_complexes"}{$ns}{"NumNeighbours"} = (read_ini_line($line))[1];
		      } elsif ($line =~ /^Description/){
			  $sim->{"ns_complexes"}{$ns}{"Description"} = (read_ini_line($line))[1];
		      } else {}  # other paramater are determined from the remaining sections
		      $line = <ONS>;
		      $line =~ s/\r?\n$//;
		  }
		  redo; # Quit the General section and run again without acquiring another line
		  
		  #-----------------------------
		  # [Rapid Equilibrium] section
		  #-----------------------------
		  
	      } elsif ($line =~ /^\[Rapid Equilibrium/){
		  $sim->{"ns_complexes"}{$ns}{"Rapid_equilibriums"}[$ns_equ_nb]{"CoupledStates"} = [];
		  $line = <ONS>;
		  $line =~ s/\r?\n$//;
		  while (defined $line and $line !~ /^\[/){
		      if ($line =~ /^Description/){
			  $sim->{"ns_complexes"}{$ns}{"Rapid_equilibriums"}[$ns_equ_nb]{"Description"} = (read_ini_line($line))[1];
		      } elsif ($line =~ /^(State|Flag)/){
			  $sim->{"ns_complexes"}{$ns}{"Rapid_equilibriums"}[$ns_equ_nb]{"Flag"} = (read_ini_line($line))[1];
		      } elsif ($line =~ /^CoupledStates/){
			  $sim->{"ns_complexes"}{$ns}{"Rapid_equilibriums"}[$ns_equ_nb]{"CoupledStates"} = [ split /\s*,\s*/,(read_ini_line($line))[1] ];
		      } elsif ($line =~ /^[01\?]+\s*=/){     
			  # does NOT take the line with "Wildcards". Will be computed from the others
			  # CAUTION FIXME This assumes that all other lines are bitstrings. Whatif bullshit lines ? 
			  my ($key,$value) = (read_ini_line($line));
			  $sim->{"ns_complexes"}{$ns}{"Rapid_equilibriums"}[$ns_equ_nb]{$key} = [ split /\s*,\s*/,$value ];
			  if (not defined $sim->{"ns_complexes"}{$ns}{"NumNeighbours"}){
			      $sim->{"ns_complexes"}{$ns}{"NumNeighbours"} = scalar @{ $sim->{"ns_complexes"}{$ns}{"Rapid_equilibriums"}[$ns_equ_nb]{$key} } - 1;
			  }
		      } else {}; 		
		      $line = <ONS>;
		      $line =~ s/\r?\n$//;
		  }
		  $ns_equ_nb++;
		  redo; # Quit the Rapid Equilibrium section and run again without acquiring another line
		  
		  #----------------------------
		  # [Reaction] section
		  #----------------------------
		  
	      } elsif ($line =~ /^(\[Reaction\s+)(\w+)/){
		  $sim->{"ns_complexes"}{$ns}{"Reactions"}[$ns_react_nb]{"Symbol"} = $2;
		  $line = <ONS>;
		  $line =~ s/\r?\n$//;
		  while (defined $line and $line !~ /^\[/){
		      if ($line =~ /^Description/){
			  $sim->{"ns_complexes"}{$ns}{"Reactions"}[$ns_react_nb]{"Description"} = (read_ini_line($line))[1];
		      } elsif ($line =~ /^EffectOnNeighbour/){
			  $sim->{"ns_complexes"}{$ns}{"Reactions"}[$ns_react_nb]{"EffectOnNeighbour"} = [ split /\s*,\s*/,(read_ini_line($line))[1] ];
		      } elsif ($line =~ /^ReactNeighbourID/){
			  $sim->{"ns_complexes"}{$ns}{"Reactions"}[$ns_react_nb]{"ReactNeighbourID"} = (read_ini_line($line))[1];
		      } elsif ($line =~ /^[01\?]+\s*=/){    
			  # does NOT not take the lines with "wildcards". Will be computed from the others
			  # CAUTION FIXME This assumes that all other lines are bitstrings. Whatif bullshit lines ? 
			  my ($key,$value) = (read_ini_line($line));
			  $sim->{"ns_complexes"}{$ns}{"Reactions"}[$ns_react_nb]{$key} = $value;
		      } else {}; 		
		      $line = <ONS>;
		      $line =~ s/\r?\n$//;
		  }
		  $ns_react_nb++;
		  redo; # Quit the Reaction section and run again without acquiring another line
	      } elsif (not defined ($line)){
		  last NEIGHBOUR;
	      } else {}  
	  }
	    close (ONS) or die "can't close the neighbour sensitive complex configuration file $sim->{main_config_dir}/Input/$sim->{ns_complexes}{$ns}{File}: $!";
	}
    } # End of reading conditioned to $sim->{"general_config"}{"UseSpatialExtensions"}
}

    #########################################
    # Reads a line of an initialisation file
    #########################################

sub read_ini_line{
    my %content;

# removes the spaces at the beginning of the line
    $_[0] =~ s/^\s+//; 
# removes the spaces at the end of the line
    $_[0] =~ s/\s+$//; 
# removes the comments (following one ';') and the spaces before
    $_[0] =~ s/\s*;+.*//;                  
# removes spaces surrounding the equal sign and grab the values
    $_[0] =~ /(.*?)\s*=\s*(.*)/ and $content{$1} = $2; 
    return %content;

# Note that a list of values with commas as separators is taken as a whole.
}

###########################################################
# Parse the symbol of a complex to extract its components #
###########################################################

sub ParseCplxSymb{
    my ($sim,$value) = @_;
    my $symbol = $value->{"Symbol"};

    while ($symbol !~ /^\s*$/){ 
	my $bingo = 0;
	foreach my $component (keys %{ $sim->{"components"} }){
	    if ($symbol =~ /^$component/){
		$bingo = 1;
		push (@{ $sim->{"complexes"}{$value->{"Symbol"}}{"Components"} },$component);
		$symbol =~ s/^$component//;
		last;
	    }
	}
	if (not $bingo){
	    die "The symbol of the complex: ",$value->{"Symbol"},"cannot be decomposed into components. The remaining bit is $symbol. Fix that by creating the correct component(s).";
	    last;
	}
    }
}


#END { } 

1;

