###############################################################################
#                          perl interface for stochsim                       
# Original author of stochsim (Win32 version) and its MS Windows graphical   
# interface                                   1998 : Carl Morton-Firth                
# port to Linux and spatial extensions   1999-2002 : Tom Simon Shimizu                
# Perl interface                         2000-2002 : Nicolas Le Novère                
#
# File: writeinifile.pm
#
# Aim:  Writes the configuration files of stochsim
#
# Author: Nicolas Le Novère
#
# Date of last modification: 13/oct/2003
#
###############################################################################

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

package writeinifile;
use strict;
use vars qw($VERSION);

## set the version for version checking;
$VERSION = 0.063;

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
#                                          Flag      => scalar,
#                                          _bitstring_ => scalar } ],
#                Display_variables  => [ { Name   => scalar,
#                                          States => [] } ]
#                Reactions          => [ { Symbol      => scalar,
#                                          Description => scalar,
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
#                                        CoupledStates => [],
#                                        _bitstring_   => scalar } ]
#              Reactions          => [ { Description => scalar,
#                                        EffectOnNeighbour => [],
#                                        ReactNeighbourID  => scalar,
#                                        _bitstring_       => scalar } ] }
#
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
#};

sub SaveFile{
    my ($sim) = @_;                    # a whole simulation 

################################################################################
# IMPORTANT NOTICE: The general hash contains keys pointing to defined values
# though sometimes empty. No need to test the definition. However, from the
# second level of structuration, the definition is not ensured. WE NEED TO TEST.
################################################################################

    # I destroy all the files belonging to the previous simulation

    opendir(DIR,"$sim->{main_config_dir}/Input") or die "can't opendir $sim->{main_config_dir}/Input: $!\n";
    while(defined(my $file = readdir(DIR))){
	if ($file =~ /\.INI[~]?$/i){
	    unlink("$sim->{main_config_dir}/Input/$file") or die "can't delete $file: $!\n";
	}
    }
    closedir (DIR) or die "can't closedir $sim->{main_config_dir}/Input: $!\n";

                              ###############
                              # STCHSTC.INI #
                              ############### 
 
    open (SCF, ">$sim->{main_config_dir}/Input/STCHSTC.INI") 
	or die "can't open the general configuration file $sim->{main_config_dir}/Input/STCHSTC.INI: $!";
                                #
                                # [Simulation Parameters]
                                #
    print SCF "[Simulation Parameters]\n";
    print SCF "DisplayInterval        = ", defined $sim->{"general_config"}{"StoreInterval"}          ? $sim->{"general_config"}{"StoreInterval"}          : 0, "\n";
    print SCF "StoreInterval          = ", defined $sim->{"general_config"}{"StoreInterval"}          ? $sim->{"general_config"}{"StoreInterval"}          : 0, "\n";
    print SCF "DumpInterval           = ", defined $sim->{"general_config"}{"DumpInterval"}           ? $sim->{"general_config"}{"DumpInterval"}           : 0, "\n";
    print SCF "TimeIncrement          = ", defined $sim->{"general_config"}{"TimeIncrement"}          ? $sim->{"general_config"}{"TimeIncrement"}          : 0, "\n";
    print SCF "SimulationDuration     = ", defined $sim->{"general_config"}{"SimulationDuration"}     ? $sim->{"general_config"}{"SimulationDuration"}     : 0, "\n";
    print SCF "ReactionVolume         = ", defined $sim->{"general_config"}{"ReactionVolume"}         ? $sim->{"general_config"}{"ReactionVolume"}         : 0, "\n";
    print SCF "MaximumNumberComplexes = ", defined $sim->{"general_config"}{"MaximumNumberComplexes"} ? $sim->{"general_config"}{"MaximumNumberComplexes"} : 0, "\n";
    print SCF "\n";
                                #
                                # [Options]
                                #
    print SCF "[Options]\n";
    print SCF "AbortOnResolutionErr   = ", defined $sim->{"general_config"}{"AbortOnResolutionErr"}  ? $sim->{"general_config"}{"AbortOnResolutionErr"}  : 1, "\n";
    print SCF "DisplayAllLevels       = ", defined $sim->{"general_config"}{"DisplayAllLevels"}      ? $sim->{"general_config"}{"DisplayAllLevels"}      : 1, "\n";
    print SCF "OptimiseTimeIncrement  = ", defined $sim->{"general_config"}{"OptimiseTimeIncrement"} ? $sim->{"general_config"}{"OptimiseTimeIncrement"} : 1, "\n";
    print SCF "RandomNumberGenerator  = ", defined $sim->{"general_config"}{"RandomNumberGenerator"} ? $sim->{"general_config"}{"RandomNumberGenerator"} : 3, "\n";
    print SCF "TimeUnits              = ", defined $sim->{"general_config"}{"TimeUnits"}             ? $sim->{"general_config"}{"TimeUnits"}             : 1, "\n";
    print SCF "CreateDumpFile         = ", defined $sim->{"general_config"}{"CreateDumpFile"}        ? $sim->{"general_config"}{"CreateDumpFile"}        : 0, "\n";
    print SCF "UseSpatialExtensions   = ", defined $sim->{"general_config"}{"UseSpatialExtensions"}  ? $sim->{"general_config"}{"UseSpatialExtensions"}  : 0, "\n";
    print SCF "\n";
                               #
                               # [File Names]
                               # Note that I plan to freeze the working directories of STOCHSIM
                               # In the future put things like  print SCONFIGFILE "ComplexINIFile         = $sim->{main_config_dir}/Input/COMPLEX.INI\n";
                               # Humm, do that in main program only? 
    print SCF "[File Names]\n";
    print SCF "ComplexINIFile         = ", defined $sim->{"general_config"}{"ComplexINIFile"}    ? $sim->{"general_config"}{"ComplexINIFile"}    : "", "\n";
    print SCF "DumpOut                = ", defined $sim->{"general_config"}{"DumpOut"}           ? $sim->{"general_config"}{"DumpOut"}           : "", "\n";
    print SCF "DynamicValues          = ", defined $sim->{"general_config"}{"DynamicValues"}     ? $sim->{"general_config"}{"DynamicValues"}     : "", "\n";
    print SCF "LogFile                = ", defined $sim->{"general_config"}{"LogFile"}           ? $sim->{"general_config"}{"LogFile"}           : "", "\n";
    print SCF "MessageINIFile         = ", defined $sim->{"general_config"}{"MessageINIFile"}    ? $sim->{"general_config"}{"MessageINIFile"}    : "", "\n";
    print SCF "ReactionMatrixOut      = ", defined $sim->{"general_config"}{"ReactionMatrixOut"} ? $sim->{"general_config"}{"ReactionMatrixOut"} : "", "\n";
    print SCF "ReactionINIFile        = ", defined $sim->{"general_config"}{"ReactionINIFile"}   ? $sim->{"general_config"}{"ReactionINIFile"}   : "", "\n";
    print SCF "SeedsIn                = ", defined $sim->{"general_config"}{"SeedsIn"}           ? $sim->{"general_config"}{"SeedsIn"}           : "", "\n";
    print SCF "SeedsOut               = ", defined $sim->{"general_config"}{"SeedsOut"}          ? $sim->{"general_config"}{"SeedsOut"}          : "", "\n";
    print SCF "VariablesOut           = ", defined $sim->{"general_config"}{"VariablesOut"}      ? $sim->{"general_config"}{"VariablesOut"}      : "", "\n";
    print SCF "ArrayINIFile           = ", defined $sim->{"general_config"}{"ArrayINIFile"}      ? $sim->{"general_config"}{"ArrayINIFile"}      : "", "\n";
    print SCF "ArrayOutPrefix         = ", defined $sim->{"general_config"}{"ArrayOutPrefix"}    ? $sim->{"general_config"}{"ArrayOutPrefix"}    : "", "\n";
    close(SCF) or die "can't close the general configuration file $sim->{main_config_dir}/Input/STCHSTC.INI: $!";

                              ###############
                              # COMPLEX.INI #
                              ############### 
    
    open (SCPLX, ">$sim->{main_config_dir}/Input/COMPLEX.INI") 
	or die "can't open the complex file $sim->{main_config_dir}/Input/COMPLEX.INI: $!";
                              #
                              # [General]
                              # 
    print SCPLX "[General]\n";
    print SCPLX "Symbols                   = ", join(',', keys %{ $sim->{"components"} }), "\n";
    if ($sim->{"display_variables"}){   
	print SCPLX "NumDisplayVariables       = ", scalar @{ $sim->{"display_variables"} },"\n";
    } else { print SCPLX "NumDisplayVariables       = 0\n"; }
    if ($sim->{"general_config"}{"InitialLevelsFromDumpFile"} == 1){
	print SCPLX "InitialLevelsFromDumpFile = ", $sim->{"general_config"}{"InitialLevelsFromDumpFile"}, "\n";
	print SCPLX "InitialLevelsFile         = ", $sim->{"general_config"}{"InitialLevelsFile"}, "\n";
	print SCPLX "InitialLevelsFromTime     = ", $sim->{"general_config"}{"InitialLevelsFromTime"}, "\n";
    } else {
	print SCPLX "InitialLevelsFromDumpFile = 0\n";
    }
    print SCPLX "\n";
                              #
                              # [Component  ]
                              # 
    print SCPLX "; *********************************************\n";
    print SCPLX "; Component information\n";
    print SCPLX "; *********************************************\n";
    print SCPLX "\n";
    foreach my $component (sort keys %{ $sim->{"components"} }){
	print SCPLX "[Component $component]\n";
	print SCPLX "Name        = ", defined $sim->{"components"}{$component}{"Name"} ? $sim->{"components"}{$component}{"Name"} : "","\n";
	if (defined($sim->{"components"}{$component}{"Description"})){     # The description is not mandatory
	    print SCPLX "Description = ", defined $sim->{"components"}{$component}{"Description"} ? $sim->{"components"}{$component}{"Description"} : "","\n";
	}
	print SCPLX "\n";
    }
                              #
                              # [Complex Type  ]
                              # 
    print SCPLX "; *********************************************\n";
    print SCPLX "; Complex Type information\n";
    print SCPLX "; *********************************************\n";
    print SCPLX "\n";
    print SCPLX "[Complex Types]\n";
    print SCPLX "NumRapidEqm  = 0\n"; # TO BE REMOVED WHEN STOCHSIM IS FIXED
    print SCPLX "NumberOfSets = ", scalar(keys %{ $sim->{"complexes"} }),"\n";
    my $count = 1;
    foreach my $cplx (sort keys %{ $sim->{"complexes"} }){
	print SCPLX "Set", $count, "         = ",$cplx, "\n";
#       Suppressed momentarily (parsing the complex symbol instead)
#	print SCPLX "Comp", $count, "        = ", join (",",@{ $sim->{"complexes"}{$cplx}{"Components"} }), "\n"; 
	if (defined($sim->{"complexes"}{$cplx}{"Description"})){ 
	    print SCPLX "Description", $count, " = ", $sim->{"complexes"}{$cplx}{"Description"}, "\n";
	}
	$count++;
    }
    print SCPLX "\n";
                              #
                              # [Initial Levels]
                              # 
    print SCPLX "; *********************************************\n";
    print SCPLX "; Initial Levels information\n";
    print SCPLX "; *********************************************\n";
    print SCPLX "\n";
    print SCPLX "[Initial Levels]\n";
    my @constants;
    foreach my $cplx (sort keys %{ $sim->{"complexes"} }){
	print SCPLX "$cplx = ", defined $sim->{"complexes"}{$cplx}{"InitLevel"} ? $sim->{"complexes"}{$cplx}{"InitLevel"} : 0, "\n";
	if ($sim->{"complexes"}{$cplx}{"Constant"}){
	    push(@constants,$cplx);
	}
    }
    
    print SCPLX "Constants = ",join(",",@constants),"\n" if @constants;
    print SCPLX "\n";
                              #
                              # [Special Complex Types]
                              # 
    print SCPLX "; *********************************************\n";
    print SCPLX "; Special Complex Types information\n";
    print SCPLX "; *********************************************\n";
    print SCPLX "\n";
    print SCPLX "[Special Complex Types]\n";
    if ( scalar(keys %{ $sim->{"ms_complexes"} }) > 0){ 
	print SCPLX "SpecialTypes       = Multistate_Complex\n";
	print SCPLX "Multistate_Complex = ", join(",", sort keys %{ $sim->{"ms_complexes"} }),"\n";
	foreach (sort keys %{ $sim->{"ms_complexes"} }){
#	print SCPLX "${ms}INIFile = ",$sim->{"main_config_dir"},"/Input/",$sim->{"ms_complexes"}{$_}{"File"},"\n";
	    print SCPLX "${_}INIFile = ",$sim->{"ms_complexes"}{$_}{"File"},"\n";
	}
    } else{ print SCPLX "SpecialTypes       = \n"; }

    print SCPLX "\n";
                              #
                              # [Neighbour Sensitive Complex Types]
                              # 
    print SCPLX "; *********************************************\n";
    print SCPLX "; Neighbour Sensitive Complex Types information\n";
    print SCPLX "; *********************************************\n";
    print SCPLX "\n";
    print SCPLX "[Neighbour Sensitive Complex Types]\n";
    if ( scalar(keys %{ $sim->{"ns_complexes"} }) > 0){ 
	print SCPLX "NeighbourSensitiveComplexes = ", join(",", sort keys %{ $sim->{"ns_complexes"} }), "\n";
	foreach (keys %{ $sim->{"ns_complexes"} }){
#		print SCPLX $_,"_NS_INIFile = ",$sim->{"main_config_dir"},"/Input/",$sim->{"ns_complexes"}{$_}{"File"},"\n";
	    print SCPLX $_,"_NS_INIFile = ",$sim->{"ns_complexes"}{$_}{"File"},"\n";
	} 
    } else { print SCPLX "NeighbourSensitiveComplexes = \n"; }
    
    print SCPLX "\n";
                              #
                              # [Display Variable X]
                              #
    print SCPLX "; *********************************************\n";
    print SCPLX "; Information on the variables to display\n";
    print SCPLX "; *********************************************\n";
    print SCPLX "\n";
    my $index = 1;
    foreach my $displvar (@{ $sim->{"display_variables"} }){
	print SCPLX "[Display Variable $index]\n";
	print SCPLX "Name  = ", defined $displvar->{"Name"} ? $displvar->{"Name"} : "", "\n";
	print SCPLX "Types = ", join(',', @{$displvar->{"Types"}}), "\n";
	print SCPLX "\n";
	$index++;
    }
    close(SCPLX) or die "can't close the complex file $sim->{main_config_dir}/Input/COMPLEX.INI: $!";

                              ################
                              # REACTION.INI #
                              ################ 

    open (SREACT, ">$sim->{main_config_dir}/Input/REACTION.INI") 
	or die "can't open the reaction file $sim->{main_config_dir}/Input/REACTION.INI: $!";
                              #
                              # [General]
                              #
    print SREACT "[General]\n";
    print SREACT "NumberOfReactions = ", scalar @{ $sim->{reactions} },"\n";
    print SREACT "\n";
                              #
                              # [Reaction ]
                              #
    $index = 1;
    foreach my $reaction (@{ $sim->{"reactions"} }){
	print SREACT "[Reaction $index]\n";
	print SREACT "Description = ", defined $reaction->{"Description"} ? $reaction->{"Description"} : "","\n"; 
	my @CstSubst;
	my @Subst;
	foreach my $subst (@{ $reaction->{"Substrates"} }){
	    if ($sim->{"complexes"}{$subst}{"Constant"}){
		push(@CstSubst,$subst);
	    } else {
		push(@Subst,$subst);
	    }
	}
	print SREACT "ConstantSubstrates = ", join(",", @CstSubst),"\n" if @CstSubst;
	print SREACT "Substrates = ", join(",", @Subst),"\n";
	my @CstProd;
	my @Prod;
	foreach my $prod (@{ $reaction->{"Products"} }){
	    if ($sim->{"complexes"}{$prod}{"Constant"}){
		push(@CstProd,$prod);
	    } else {
		push(@Prod,$prod);
	    }
	}
	print SREACT "ConstantProducts = ", join(",", @CstProd),"\n" if @CstProd; 
	print SREACT "Products = ", join(",", @Prod),"\n";
	$reaction->{"Kf"} =~ /([\d\.eE\-\+]+)(@(\S+))?/;
	my $KF = $1;
	my $Fdyn = $3; 
	foreach my $subst (@{ $reaction->{"Substrates"} }){
	    if ($sim->{"complexes"}{$subst}{"Constant"}){ 
		$KF *= $sim->{"complexes"}{$subst}{"InitLevel"} / ($sim->{"general_config"}{"ReactionVolume"} * 6.0225e23);
	    }
	}
	print SREACT $Fdyn ? "kf = $KF\@$Fdyn\n" : "kf = $KF\n";
	$reaction->{"Kr"} =~ /([\d\.eE\-\+]+)(@(\S+))?/;
	my $KR = $1;
	my $Rdyn = $3; 
	foreach my $prod (@{ $reaction->{"Products"} }){
	    if ($sim->{"complexes"}{$prod}{"Constant"}){ 
		$KR *= $sim->{"complexes"}{$prod}{"InitLevel"} / ($sim->{"general_config"}{"ReactionVolume"} * 6.0225e23);
	    }
	}
	print SREACT $Rdyn ? "kr = $KR\@$Rdyn\n" : "kr = $KR\n";
	print SREACT "\n";
	$index++;
    }
    close(SREACT) or die "can't close the reaction file $sim->{main_config_dir}/Input/REACTION.INI: $!";

                              ###############
                              # DYNAMIC.INI #
                              ###############
    my %dynvalues;   # internal copy of dynamic values. I need to copy $sim->{"dynamic_values"} and to work 
                     # on a local copy otherwise the interface values are also changed.
    my %modified;    # Check that a dynamic_value is modified only once.

    foreach my $value (keys %{ $sim->{"dynamic_values"} }){
	$modified{$value} = 0;
	foreach my $timepoint (keys %{ $sim->{"dynamic_values"}{$value} }){
	    $dynvalues{$value}{$timepoint} = $sim->{"dynamic_values"}{$value}{$timepoint};
	}
    }

    foreach my $react (@{ $sim->{"reactions"} }){
	if ($react->{"Kf"} =~ /@(\S+)/){
	    my $fdyn = $1;
	    my $found = 0;
	    foreach my $dyn (keys %dynvalues){
		if ($fdyn eq $dyn){
		    $found++;
		    last if $modified{$dyn};
		    foreach my $cplx (@{ $react->{"Substrates"} }){
                        # CAUTION: Need to read COMPLEX.INI _before_ DYNAMIC.INI
			if ($sim->{"complexes"}{$cplx}{"Constant"}){  			    
			    $modified{$dyn}++;
			    foreach my $timepoint (keys %{ $dynvalues{$dyn} }){
				$dynvalues{$dyn}{$timepoint} *= $sim->{"complexes"}{$cplx}{"InitLevel"} / ($sim->{"general_config"}{"ReactionVolume"} * 6.0225e23) ;
			    }
			}
		    }
		    last;
		}
	    }
	    if (not $found){
		# This is not possible if no flaw in the interface
		$react->{"Kf"} =~ s/@\S+//;
		warn "The dynamic value $fdyn required by one of the reaction is not defined in the file DYNAMIC.INI. I suppress the call.";
	    }
	} 
	if ($react->{"Kr"} =~ /@(\S+)/){
	    my $rdyn = $1;
	    my $found = 0;
	    foreach my $dyn (keys %dynvalues){
		if ($rdyn eq $dyn){
		    $found++;
		    last if $modified{$dyn};
		    foreach my $cplx (@{ $react->{"Products"} }){
			# CAUTION: Need to read COMPLEX.INI _before_ DYNAMIC.INI
			if ($sim->{"complexes"}{$cplx}{"Constant"}){  
			    $modified{$dyn}++;
			    foreach my $timepoint (keys %{ $sim->{"dynamic_values"}{$dyn} }){
				$dynvalues{$dyn}{$timepoint} *= $sim->{"complexes"}{$cplx}{"InitLevel"} / ($sim->{"general_config"}{"ReactionVolume"} * 6.0225e23) ;
			    }
			}
		    }
		    last;
		}
	    }
	    if (not $found){
                # This is not possible if no flaw in the interface
		$react->{"Kr"} =~ s/@\S+//;
		warn "The dynamic value $rdyn required by one of the reaction is not defined in the file DYNAMIC.INI. I suppress the call.";
	    }
	} 
    }
	
    open (SDYN, ">$sim->{main_config_dir}/Input/DYNAMIC.INI") 
	or die "can't open the dynamic values file $sim->{main_config_dir}/Input/DYNAMIC.INI: $!";
    #
    # [General]
    #
    print SDYN "[General]\n";
    my $anykey;
    foreach (keys %dynvalues ){ $anykey = $_; }
    if (defined $anykey){
	print SDYN "NumberOfSets = ", scalar keys %{ $dynvalues{$anykey} },"\n";
	my $dyn_set = 1;
	foreach my $dyn_time (sort keys %{ $dynvalues{$anykey} } ){
	    print SDYN "Set$dyn_set = $dyn_time \n";             
	    $dyn_set++;
	}
	print SDYN "\n";
	#
	# [Time  ]
	#
	foreach my $dyn_time (sort keys %{ $dynvalues{$anykey} }){
	    print SDYN "[Time $dyn_time]\n";
	    foreach my $dyn_value ( sort keys %dynvalues ){
		print SDYN $dyn_value," = ",$dynvalues{$dyn_value}{$dyn_time},"\n";
	    }
	    print SDYN "\n";
	}
    } else {
	print SDYN "NumberOfSets = 0\n";
    }
    close(SDYN) or die "can't open the dynamic values file $sim->{main_config_dir}/Input/DYNAMIC.INI: $!";

                              ###############
                              # MULTISTATES #
                              ###############

    foreach my $ms (keys %{ $sim->{"ms_complexes"} }){
	open (SMS, ">$sim->{main_config_dir}/Input/$sim->{ms_complexes}{$ms}{File}") 
	    or die "can't open the multistate file $sim->{main_config_dir}/Input/$sim->{ms_complexes}{$ms}{File} to write configuration: $!";
	print SMS "; This file contains details for the multistate complex type $ms\n";
	print SMS "\n";
                              #
                              # [General]
                              #
	print SMS "[General]\n";
	print SMS "Description         = ", defined $sim->{"ms_complexes"}{$ms}{"Description"}        ? $sim->{"ms_complexes"}{$ms}{"Description"}                                    : "","\n";
	print SMS "StateFlags          = ", defined $sim->{"ms_complexes"}{$ms}{"Flags"}              ? join(",",@{ $sim->{"ms_complexes"}{$ms}{"Flags"} })                           : "","\n";
	print SMS "NumRapidEqm         = ", defined $sim->{"ms_complexes"}{$ms}{"Rapid_equilibriums"} ? scalar @{$sim->{"ms_complexes"}{$ms}{"Rapid_equilibriums"}}                   : 0,"\n";
	print SMS "Reactions           = ", defined $sim->{"ms_complexes"}{$ms}{"Reactions"}          ? join(",", map{ $_->{"Symbol"} }@{ $sim->{"ms_complexes"}{$ms}{"Reactions"} }) : "","\n";
	print SMS "NumDisplayVariables = ", defined $sim->{"ms_complexes"}{$ms}{"Display_variables"}  ? scalar @{ $sim->{"ms_complexes"}{$ms}{"Display_variables"} }                  : 0,"\n";
	print SMS "\n";
                              #
                              # [Initial States]
                              #
	print SMS "[Initial States]\n";
	print SMS "Wildcards           = ";
	my @levels_wc; # list of the wildcards used in the configuration of initial levels	
	foreach my $level (sort keys %{ $sim->{"ms_complexes"}{$ms}{"InitLevel"} }){
	    if ($level =~ /\?/){ push (@levels_wc,$level); }
	}
	print SMS join(",",@levels_wc),"\n";
	foreach my $level (sort keys %{ $sim->{"ms_complexes"}{$ms}{"InitLevel"} }){
	    print SMS "$level = $sim->{ms_complexes}{$ms}{InitLevel}{$level}\n";
	}
	print SMS "\n";
                              #
                              # [Rapid Equilibrium  ]
                              #
	my $equ_number = 0;
	foreach my $equilibrium (@{ $sim->{"ms_complexes"}{$ms}{"Rapid_equilibriums"} }){
	    my @states_wc; # list of the wildcards used in the configuration of rapid equilibriums
	    print SMS "[Rapid Equilibrium ",$equ_number+1,"]\n";
	    print SMS "Description         = ", defined $equilibrium->{"Description"} ? $equilibrium->{"Description"} : "" ,"\n";
	    print SMS "Flag                = ", $equilibrium->{"Flag"},"\n";
	    foreach my $state (sort keys %$equilibrium){
		if ($state =~ /^(0|1|\?)+$/){ 
		    print SMS $state," = ", $equilibrium->{$state},"\n"; 
		    if ($state =~ /\?/){ push (@states_wc,$state); }
		}
	    }
	    print SMS "Wildcards           = ", join(",", @states_wc),"\n";
	    $equ_number++;
	    print SMS "\n";
	}
	                      #
                              # [Reaction  ]
                              #
	foreach my $reaction (@{ $sim->{"ms_complexes"}{$ms}{"Reactions"} }){
	    my @reactions_wc; # list of the wildcards used in the configuration of reactions
	    print SMS "[Reaction ", $reaction->{"Symbol"},"]\n";
	    print SMS "Description         = ", defined $reaction->{"Description"} ? $reaction->{"Description"} : "" ,"\n";
	    print SMS "Effect              = ", join(",", @{ $reaction->{"Effect"} }),"\n" if defined $reaction->{"Effect"};
	    foreach my $state (sort keys %$reaction){
		if ($state =~ /^(0|1|\?)+$/){ 
		    print SMS $state," = ", $reaction->{$state},"\n"; 
		    if ($state =~ /\?/){ push (@reactions_wc,$state); }
		}
	    }
	    print SMS "Wildcards           = ", join(",", @reactions_wc),"\n";
	    print SMS "\n";
	}
	                      #
                              # [Display Variable  ]
                              #
	$index = 0;
	foreach my $displ_var (@{ $sim->{"ms_complexes"}{$ms}{"Display_variables"} }){
	    print SMS "[Display Variable ", $index+1, "]\n";
	    print SMS "Name   = ", $displ_var->{"Name"},"\n";
	    print SMS "States = ", join(",", @{ $displ_var->{"States"} }),"\n";
	    $index++;	
	    print SMS "\n";
	}
	close(SMS) or die "can't close the multistate file $sim->{main_config_dir}/Input/$sim->{ms_complexes}{$ms}{File} to write configuration: $!";
    }

    if ($sim->{"general_config"}{"UseSpatialExtensions"}){
	#############
	# ARRAY.INI #
	############# 
	
	open (SA, ">$sim->{main_config_dir}/Input/ARRAY.INI") 
	    or die "can't open the array configuration file $sim->{main_config_dir}/Input/ARRAY.INI: $!";
	#
	# [General]
	#
	print SA "[General]\n";
	print SA "Arrays                 = ", join(",", sort keys %{ $sim->{"arrays"} }), "\n";
	print SA "DumpInterval           = ", defined $sim->{"array_config"}{"DumpInterval"}         ? $sim->{"array_config"}{"DumpInterval"}         : 0, "\n";
	my $numsnap = 0;
	foreach my $array (keys %{ $sim->{"arrays"} }){
	    $numsnap += scalar keys %{ $sim->{"arrays"}{$array}{"Snapshot_variables"} };
	}
	print SA "NumSnapshotVariables   = $numsnap\n";
	print SA "\n";
	
	foreach my $array ( sort keys %{ $sim->{"arrays"} }){
	    print SA "[$array]\n";
	    print SA "Description           = ", defined $sim->{"arrays"}{$array}{"Description"} ? $sim->{"arrays"}{$array}{"Description"} : "", "\n";
	    print SA "Complex               = ", defined $sim->{"arrays"}{$array}{"Complex"} ? $sim->{"arrays"}{$array}{"Complex"} : "", "\n";
#	print SA "NeighbourSensitive    = ", defined $sim->{"arrays"}{$array}{"NeighbourSensitive"} ? $sim->{"arrays"}{$array}{"NeighbourSensitive"} : 1, "\n";
	    print SA "NeighbourSensitive    = 1\n";  # Not used in the current version of StochSim (NLN 26/JUN/2001)
	    print SA "Geometry              = ", defined $sim->{"arrays"}{$array}{"Geometry"} ? $sim->{"arrays"}{$array}{"Geometry"} : "", "\n";
	    print SA "XDimension            = ", defined $sim->{"arrays"}{$array}{"XDimension"} ? $sim->{"arrays"}{$array}{"XDimension"} : 0, "\n";
	    print SA "YDimension            = ", defined $sim->{"arrays"}{$array}{"YDimension"} ? $sim->{"arrays"}{$array}{"YDimension"} : 0, "\n";
	    print SA "CreateDumpFile        = ", defined $sim->{"arrays"}{$array}{"CreateDumpFile"} ? $sim->{"arrays"}{$array}{"CreateDumpFile"} : 0, "\n";
	    print SA "BoundaryCondition     = ", defined $sim->{"arrays"}{$array}{"BoundaryCondition"} ? $sim->{"arrays"}{$array}{"BoundaryCondition"} : 1, "\n";
	    print SA "EquilibrationInterval = ", defined $sim->{"arrays"}{$array}{"EquilibrationInterval"} ? $sim->{"arrays"}{$array}{"EquilibrationInterval"} : 1, "\n";
	    print SA "InitialState          = ", defined $sim->{"arrays"}{$array}{"InitialState"} ? $sim->{"arrays"}{$array}{"InitialState"} : "FromConcentration", "\n";
	    if ($sim->{"arrays"}{$array}{"InitialState"} eq "FromDumpFile"){
		print SA "InitialStateFile      = ", defined $sim->{"arrays"}{$array}{"InitialStateFile"} ? $sim->{"arrays"}{$array}{"InitialStateFile"} : "", "\n";
		print SA "InitialStateFromTime  = ", defined $sim->{"arrays"}{$array}{"InitialStateFromTime"} ? $sim->{"arrays"}{$array}{"InitialStateFromTime"} : "", "\n";
	    }

	    print SA "\n";
	}
	my $snapcount = 1;
	foreach my $array ( sort keys %{ $sim->{"arrays"} }){	   
	    foreach my $snap ( sort keys %{ $sim->{"arrays"}{$array}{"Snapshot_variables"} }){
		print SA "[Snapshot Variable $snapcount]\n";
		print SA "Name                  = ",$snap,"\n";
		print SA "Description           = ",defined $sim->{"arrays"}{$array}{"Snapshot_variables"}{$snap}{"Description"} ? $sim->{"arrays"}{$array}{"Snapshot_variables"}{$snap}{"Description"} : "","\n";
		print SA "Array                 = ",$array,"\n";
		print SA "StoreInterval         = ",$sim->{"arrays"}{$array}{"Snapshot_variables"}{$snap}{"StoreInterval"},"\n"; 
		print SA "AveragedOutput        = ",$sim->{"arrays"}{$array}{"Snapshot_variables"}{$snap}{"AveragedOutput"},"\n"; 
		print SA "AverageInterval       = ",$sim->{"arrays"}{$array}{"Snapshot_variables"}{$snap}{"AverageInterval"},"\n";
		print SA "SampleInterval        = ",$sim->{"arrays"}{$array}{"Snapshot_variables"}{$snap}{"SampleInterval"},"\n";
		print SA "States                = ", join(",",@{ $sim->{"arrays"}{$array}{"Snapshot_variables"}{$snap}{"States"} }),"\n";
		print SA "\n";
		$snapcount++;
	    } 
	}
	close(SA)
	    or die "can't close the array configuration file $sim->{main_config_dir}/Input/ARRAY.INI: $!";
	
	foreach my $ns (keys %{ $sim->{"ns_complexes"} }){
	    open(SNS,">$sim->{main_config_dir}/Input/$sim->{ns_complexes}{$ns}{File}") 
		or die "can't open the file $sim->{main_config_dir}/Input/$sim->{ns_complexes}{$ns}{File}";
	    #
	    # [General]
	    #
	    print SNS "[General]\n";
	    print SNS "Description   = ", defined $sim->{"ns_complexes"}{$ns}{"Description"} ? $sim->{"ns_complexes"}{$ns}{"Description"} : "","\n";   
	    print SNS "NumNeighbours = ";
	    if (defined $sim->{"ns_complexes"}{$ns}{"NumNeighbours"}){
		print SNS $sim->{"ns_complexes"}{$ns}{"NumNeighbours"},"\n";
	    } else {
		print SNS "4\n"; 
		warn "NumNeighbours was undefined. I assume 4 in $sim->{main_config_dir}/Input/$sim->{ns_complexes}{$ns}{File}\n";
	    }   
	    print SNS "NumRapidEqm   = ", defined $sim->{"ns_complexes"}{$ns}{"Rapid_equilibriums"} ? scalar @{ $sim->{"ns_complexes"}{$ns}{"Rapid_equilibriums"} } : 0,"\n";
	    print SNS "Reactions     = ", join(",", map{ $_->{"Symbol"} }@{ $sim->{"ns_complexes"}{$ns}{"Reactions"} }),"\n";
	    print SNS "\n";
	    my $ns_equ_nb = 1;
	    foreach my $ns_equ ( @{ $sim->{ns_complexes}{$ns}{"Rapid_equilibriums"} } ){
		my @states_wc; # list of the wildcards used in the configuration of rapid equilibriums
		print SNS "[Rapid Equilibrium ", $ns_equ_nb,"]\n";
		print SNS "Description   = ", defined $ns_equ->{"Description"} ? $ns_equ->{"Description"} : "","\n";
		print SNS "Flag          = ", $ns_equ->{"Flag"},"\n";
		print SNS "CoupledStates = ", defined $ns_equ->{"CoupledStates"} ? join(",", @{ $ns_equ->{"CoupledStates"} }) : "","\n";		
		foreach my $state ( sort keys %$ns_equ ){
		    if ($state =~ /^(0|1|\?)+$/){ 
			print SNS $state," = ", join(",",@{ $ns_equ->{$state} }),"\n"; 
			if ($state =~ /\?/){ push (@states_wc,$state); }
		    }
		}
		print SNS "Wildcards     = ", join(",", @states_wc),"\n";
		print SNS "\n";
		$ns_equ_nb++;
	    }
	    #
	    # [Reaction  ]
	    #
	    foreach my $reaction (@{ $sim->{ns_complexes}{$ns}{"Reactions"} }){
		my @reactions_wc; # list of the wildcards used in the configuration of reactions
		print SNS "[Reaction ", $reaction->{"Symbol"},"]\n";
		foreach my $toto (sort keys %$reaction){
		    if ( $toto !~ /^(Symbol|EffectOnNeighbour|Description)$/ ){
			print SNS $toto, " = ", $reaction->{$toto},"\n";
			if ($toto =~ /\?/){ 
			    push (@reactions_wc,$toto); 
			}
		    } elsif ($toto eq "EffectOnNeighbour") {
			print SNS "EffectOnNeighbour = ", join(",", @{ $reaction->{"EffectOnNeighbour"} } ),"\n";
		    } elsif ($toto eq "Description"){
			print SNS "Description = ", defined $reaction->{"Description"} ? $reaction->{"Description"} : "" ,"\n";
		    }
		}
		print SNS "Wildcards = ", join(",", @reactions_wc)," ; \n";
		print SNS "\n";
	    }
	    close SNS or  die "can't close the file $sim->{main_config_dir}/Input/$ns->{File}";
	}
    }    
}

1;














