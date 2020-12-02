#!/usr/bin/perl -w

###############################################################################
#                          perl interface for stochsim                       
#
# File: readsbml.pm
#
# Aim:  Reads a simulation under the SBML format
# (see http://www.sbml.org)
# determine from the file itself if it an SBML level1 or level2 file.
#
# Author: Nicolas Le Novère
#
# Date of last modification: 29/MAR/2004
#
###############################################################################

#     This program is free software; you can redistribute it and/or modify it 
#     under the terms of the GNU General Public License as published by the 
#     Free Software Foundation; either version 2 of the License, or (at your 
#     option) any later version.
#
#     This program is distributed in the hope that it will be useful, but
#     WITHOUT ANY WARRANTY; without even the implied warranty of 
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
#     Public License  for more details.
#
#     You should have received a copy of the GNU General Public License along 
#     with this program; if not, write to the Free Software Foundation, Inc., 
#     59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#    Nicolas LE NOVÈRE,  Computational Neurobiology, 
#    EMBL-EBI, Wellcome-Trust Genome Campus, Hinxton, Cambridge, CB10 1SD, UK
#    Tel: +44(0)1223 494 521,  Fax: +44(0)1223 494 468,  Mob: +33(0)689218676
#    http://www.ebi.ac.uk/~lenov

package readsbml;
use strict;
use XML::Simple;
use Data::Dumper;
use vars qw($VERSION);

## set the version for version checking;
$VERSION = 0.016;

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
#              BoundaryCondition     => scalar,  integer: 0 is toroïdal, 1 otherwise
#              EquilibrationInterval => scalar   
#              Snapshot_variables    => { Name => { States          => [],      states shown in the snapshots} 
#                                                   StoreInterval   => scalar,  
#                                                   AverageOutput   => scalar,
#                                                   AverageInterval => scalar,
#                                                   SampleInterval  => scalar,
#                                                   Description     => scalar  } } } }
#};

###############################################################################
#  Entry point. Automatically detect Level 1 and Level 2
###############################################################################

sub readsbml{
    my ($sbml,$sim) = @_;

    # wipe out the previous simulation. DO NOT UNDEF THE LISTS. 
    $sim->{"main_config_dir"}   = undef;
    $sim->{"components"}        = {};           
    $sim->{"complexes"}         = {};            
    $sim->{"reactions"}         = [];            
    $sim->{"display_variables"} = [];    
    $sim->{"ms_complexes"}      = {}; 
    $sim->{"ns_complexes"}      = {};
    undef %{ $sim->{"array_config"} };   
    $sim->{"arrays"}            = {};
    undef %{ $sim->{"dynamic_values"} };   

    my $struct = XMLin($sbml,
		       forcearray    => 1,  # Force an enumeration as a list 
# even if only one element (compartment for instance)
		       suppressempty => undef,
		       keyattr       => []); # disable the array folding on 
# 'name', 'id', or 'key' attributes

    print Dumper($struct); # For debug purposes
    if ($struct->{"level"} == 1){
	warn "The file ",$sbml," declares itself as SBML level ",$struct->{"level"},".\n";
	readsbml1($struct,$sim);
    } elsif ($struct->{"level"} == 2){
	warn "The file ",$sbml," declares itself as SBML level ",$struct->{"level"},".\n";
	warn "CAUTION: the parsing of SBML level 2 is still in its infancy!\n";
	readsbml2($struct,$sim);
    } else {
	die "The file ",$sbml," declares itself as SBML level ",$struct->{"level"},". Only level 1 and 2 are supported.\n";
    }
}

###############################################################################
#                     Parse SBML Level 1 files
###############################################################################

sub readsbml1 {
    my ($struct,$sim) = @_;

###############################################################################
#                     Population of the general configuration
###############################################################################
 
    # NB: there is only one compartment in StochSim.
    $sim->{"general_config"}{"ReactionVolume"} = $struct->{"model"}[0]{"listOfCompartments"}[0]{"compartment"}[0]{"volume"};
    $sim->{"general_config"}{"StoreInterval"}  = $struct->{"annotations"}[0]{"stsim:StoreInterval"};
    $sim->{"general_config"}{"TimeIncrement"}  = $struct->{"annotations"}[0]{"stsim:TimeIncrement"};
    $sim->{"general_config"}{"SimulationDuration"}  = $struct->{"annotations"}[0]{"stsim:SimulationDuration"};
    $sim->{"general_config"}{"MaximumNumberComplexes"}  = $struct->{"annotations"}[0]{"stsim:MaximumNumberComplexes"};

###############################################################################
#                           Parsing of listOfSpecies
###############################################################################

    # StochSim needs the composition of each complex, which is stored inside 
    # the "annotations" element of the SBML file. 
    
    my %listcpnts;
    foreach (split /\s*;\s*/, $struct->{"annotations"}[0]{"stsim:listcpnts"}){
	my ($cplx,$cpnts) = split /\s*:\s*/, $_;
	$listcpnts{$cplx} = [ split /\s*,\s*/,$cpnts ];
    };

    foreach my $species (@{ $struct->{"model"}[0]{"listOfSpecies"}[0]{"species"} }){
	$sim->{"complexes"}{$species->{"name"}}{"InitLevel"} = $species->{"initialAmount"};

	if (exists $listcpnts{$species->{"name"}}){
	    # there exists an annotations field with composition in components 
	    for (0 .. $#{ $listcpnts{$species->{"name"}} }){
		my $cut_cpnt_name = substr($listcpnts{$species->{"name"}}[$_],0,$main::limits{"MAX_COMPONENT_SYMBOL_LENGTH"});
		$sim->{"complexes"}{$species->{"name"}}{"Components"}[$_] = $cut_cpnt_name;
		$sim->{"components"}{$cut_cpnt_name}{"Name"} = $cut_cpnt_name; 
	    }
	} else {
	    # If no components have been found in annotations we create of one component 
	    # per complex. The length of a component symbol is limited to MAX_COMPONENT_SYMBOL_LENGTH
	    my $cpnt_name = substr($species->{"name"},0,$main::limits{"MAX_COMPONENT_SYMBOL_LENGTH"});
	    $sim->{"components"}{$cpnt_name}{"Name"} = $cpnt_name; 
	    # name and symbol of StochSim are equivalent in SBML
	    push @{ $sim->{"complexes"}{$species->{"name"}}{"Components"} }, $cpnt_name;
	}
	
	# FIXME: Solve the problem of overlapping component names. Components a and ab cannot coexist in stochsim.

# A serious parsing of 'units' would be useful for many area of SBML parsing. Think about it 

	if (not exists $species->{"units"} 
	    or not defined $species->{"units"}
	    or $species->{"units"} eq "mole"){ # units is not specified => SBML1 says 'moles'
	    $sim->{"complexes"}{$species->{"name"}}{"InitLevel"} *= 6.0225e23;
	} elsif ($species->{"units"} eq "numberof"){
	    # do nothing, right units
	} else {
	    warn "the unit ",$species->{"units"}," of the species ",$species->{"name"}," was not recognized.\n",
	    "I assume it is mole (default of SBML1 definition).\n";
	    $sim->{"complexes"}{$species->{"name"}}{"InitLevel"} *= 6.0225e23;
	}

	if (defined $species->{"boundaryCondition"} and $species->{"boundaryCondition"} eq "true"){
	    $sim->{"complexes"}{$species->{"name"}}{"Constant"} = 1;
	} else {
	    $sim->{"complexes"}{$species->{"name"}}{"Constant"} = 0;
	}

	if (defined $species->{"notes"}[0]{"body"}[0]{"p"}){
	    $sim->{"complexes"}{$species->{"name"}}{"Description"} = join " ", @{ $species->{"notes"}[0]{"body"}[0]{"p"} };
	}
    }

###############################################################################
#                         Parsing of listOfReactions
###############################################################################
#
# Maybe accept reactions which are not uni or bi-molecular and convert?
# 

    my $react_idx = 0;
    foreach my $react (@{ $struct->{"model"}[0]{"listOfReactions"}[0]{"reaction"} }){
	if (defined $react->{"notes"}[0]{"body"}[0]{"p"}){
	    $sim->{"reactions"}[$react_idx]{"Description"} = join " ", @{ $react->{"notes"}[0]{"body"}[0]{"p"} };
	}

	@{ $sim->{"reactions"}[$react_idx]{"Substrates"} } = map{ $_->{"species"} }@{ $react->{"listOfReactants"}[0]{"speciesReference"} };
	if (@{ $sim->{"reactions"}[$react_idx]{"Substrates"} } > 2){
	    warn "The reaction ", $react->{"name"}," contains more than two substrates.\n",
	    "StochSim accepts only uni- and bimolecular reactions.\n";
	}
	@{ $sim->{"reactions"}[$react_idx]{"Products"} } = map{ $_->{"species"} }@{ $react->{"listOfProducts"}[0]{"speciesReference"} };
	if (@{ $sim->{"reactions"}[$react_idx]{"Products"} } > 2){
	    warn "The reaction ", $react->{"name"}," contains more than two products.\n",
	    "StochSim accepts only uni- and bimolecular reactions (the reversible reaction should have no more than two reactants).\n";
	}

# ! Assume a StochSim-like definition (kf, kr and so on). Think to something more flexible.
# Parse the formula, for each parameter, look in the internal list, else look in the general list, 
# else generate a warning?
	
	# This is far to be a true parser. What about some serious work someday, hum?  
	my ($first,$second);
	# CAUTION: there are intermingled braces: the second part of the formula is kept into $3 and not $2
	if ( $react->{"kineticLaw"}[0]{"formula"} =~ /([+-]?\s*\w+\s*(\*\s*\w+\s*)+)(([+-]?\s*\w+\s*(\*\s*\w+\s*)+)?)/ ){
	    $first = $1 and $second = $3;
	    my ($forward,$reverse);
	    if ($first =~ /^\s*-/){
		($reverse) = split /\s*\*\s*/, $first;	    
		$reverse =~ s/\s*-\s*//;
	    } else {
		($forward) = split /\s*\*\s*/, $first;	    
	    }
	    if ($second =~ /^\s*-/){
		($reverse) = split /\s*\*\s*/, $second;	
		$reverse =~ s/\s*-\s*//;
	    } else {
		($forward) = split /\s*\*\s*/, $second;	    
	    }

	    # analyse the general parameters, if any
	    foreach my $param (@{ $struct->{"model"}[0]{"listOfParameters"}[0]{"parameter"} }){
		if ($param->{"name"} =~ /^\s*$forward\s*$/i){
		    $sim->{"reactions"}[$react_idx]{"Kf"} = $param->{"value"};
		} elsif ($param->{"name"} =~ /^\s*$reverse\s*$/i){
		    $sim->{"reactions"}[$react_idx]{"Kr"} = $param->{"value"};
		}
	    } 

	    # replace by the local prameters, if any
	    foreach my $param (@{ $react->{"kineticLaw"}[0]{"listOfParameters"}[0]{"parameter"} }){
		if ($param->{"name"} =~ /^\s*$forward\s*$/i){
		    $sim->{"reactions"}[$react_idx]{"Kf"} = $param->{"value"};
		} elsif ($param->{"name"} =~ /^\s*$reverse\s*$/i){
		    $sim->{"reactions"}[$react_idx]{"Kr"} = $param->{"value"};
		}
	    }
	} else {
	    die "the kinetic law of reaction between ",join(" and ", @{ $sim->{"reactions"}[$react_idx]{"Substrates"} })," does not
	    looklike a simple mass action law.";
	}
 
	$react_idx++;
    }    

###############################################################################
#                         Parsing of listOfRules
###############################################################################

    # came from an idiotic ignorance of "boundaryCondition". Does not hurt anyway.

    foreach my $speciesRule (@{ $struct->{"model"}[0]{"listOfRules"}[0]{"speciesConcentrationRule"} }){
	if ($speciesRule->{"RuleTypeEnum"} eq 'rate' and $speciesRule->{"formula"} == 0){

	    # Caution. Complexes have to be defined first
	    if (exists $sim->{"complexes"}{$speciesRule->{"species"}}){
		$sim->{"complexes"}{$speciesRule->{"species"}}{"Constant"} = 1;
	    } else {
		print "a speciesConcentrationRule is defined for an undefined species.\n";
	    }
	}
    }

}

###############################################################################
#                     Parse SBML Level 2 files
###############################################################################

sub readsbml2 {
    my ($struct,$sim) = @_;

###############################################################################
#                     Population of the general configuration
###############################################################################
 
    if (@{ $struct->{"model"}[0]{"listOfCompartments"}[0]{"compartment"} } > 1){
	die "The SBML file contains several compartments. StochSim supports only one compartment.\n";
    } 
    if ($struct->{"model"}[0]{"listOfCompartments"}[0]{"compartment"}[0]{"constant"} eq "false"){
	die "The compartment ",$struct->{"model"}[0]{"listOfCompartments"}[0]{"compartment"}[0]{"name"}," is not constant, but StochSim understand constant volumes.\n";
    } else {
	warn "This dumb parser assume the size is in litre ...\n"; # FIXME - convert at least cubic meters!
	$sim->{"general_config"}{"ReactionVolume"} = $struct->{"model"}[0]{"listOfCompartments"}[0]{"compartment"}[0]{"size"};
    }
    if ($struct->{"model"}[0]{"listOfCompartments"}[0]{"compartment"}[0]{"spatialDimensions"} != 3){
	die "spatialDimensions = ",$struct->{"model"}[0]{"listOfCompartments"}[0]{"compartment"}[0]{"spatialDimensions"}," but StochSim understand only the three-dimentional compartments.\n";
    } else {
	warn "This dumb parser assume the size is in litre ...\n"; # FIXME - convert at least cubic meters!
	$sim->{"general_config"}{"ReactionVolume"} = $struct->{"model"}[0]{"listOfCompartments"}[0]{"compartment"}[0]{"size"};
    }
    $sim->{"general_config"}{"StoreInterval"}  = $struct->{"annotations"}[0]{"stsim:StoreInterval"};
    $sim->{"general_config"}{"TimeIncrement"}  = $struct->{"annotations"}[0]{"stsim:TimeIncrement"};
    $sim->{"general_config"}{"SimulationDuration"}  = $struct->{"annotations"}[0]{"stsim:SimulationDuration"};
    $sim->{"general_config"}{"MaximumNumberComplexes"}  = $struct->{"annotations"}[0]{"stsim:MaximumNumberComplexes"};

###############################################################################
#                           Parsing of listOfSpecies
###############################################################################

    # StochSim needs the composition of each complex, which is stored inside 
    # the "annotations" element of the SBML file. 
    
    my %listcpnts;
    foreach (split /\s*;\s*/, $struct->{"annotations"}[0]{"stsim:listcpnts"}){
	my ($cplx,$cpnts) = split /\s*:\s*/, $_;
	$listcpnts{$cplx} = [ split /\s*,\s*/,$cpnts ];
    };

    foreach my $species (@{ $struct->{"model"}[0]{"listOfSpecies"}[0]{"species"} }){

	if (exists $listcpnts{$species->{"name"}}){
            # there exists an annotations field with composition in components 
	    for (0 .. $#{ $listcpnts{$species->{"name"}} }){
		my $cut_cpnt_name = substr($listcpnts{$species->{"name"}}[$_],0,$main::limits{"MAX_COMPONENT_SYMBOL_LENGTH"});
		$sim->{"complexes"}{$species->{"name"}}{"Components"}[$_] = $cut_cpnt_name;
		$sim->{"components"}{$cut_cpnt_name}{"Name"} = $cut_cpnt_name; 
	    }
	} else {
	    # If no components have been found in annotations we create of one component 
	    # per complex. The length of a component symbol is limited to MAX_COMPONENT_SYMBOL_LENGTH
	    my $cpnt_name = substr($species->{"name"},0,$main::limits{"MAX_COMPONENT_SYMBOL_LENGTH"});
	    $sim->{"components"}{$cpnt_name}{"Name"} = $cpnt_name; 
	    # name and symbol of StochSim are equivalent in SBML
	    push @{ $sim->{"complexes"}{$species->{"name"}}{"Components"} }, $cpnt_name;
	}
	
	# FIXME: Solve the problem of overlapping component names. Components a and ab cannot coexist in stochsim.

# A serious parsing of 'units' would be useful for many area of SBML parsing. Think about it 
	
	if ($species->{"hasOnlySubstanceUnits"} eq 'true'){ # unit is substance
	    if ($species->{"substanceUnits"} eq "item"){
		$sim->{"complexes"}{$species->{"name"}}{"InitLevel"} = $species->{"initialAmount"};
	    } elsif ($species->{"substanceUnits"} eq 'mole'){
		$sim->{"complexes"}{$species->{"name"}}{"InitLevel"} = $species->{"initialAmount"} * 6.0225e23;
	    } else {
		warn "the unit ",$species->{"substanceUnits"}," of the species ",$species->{"name"}," was not recognized.\n",
		"I assume it is mole (default of SBML2 definition).\n";
		$sim->{"complexes"}{$species->{"name"}}{"InitLevel"} = $species->{"initialAmount"} * 6.0225e23;
	    }
	} else {                                            # unit is substance / size	 
	    warn "I assule the unit of the species ",$species->{"name"}," is mole per litre.\n",
	    $sim->{"complexes"}{$species->{"name"}}{"InitLevel"} = $species->{"initialConcentration"} * 6.0225e23 * $sim->{"general_config"}{"ReactionVolume"};	    
	}

	if (defined $species->{"boundaryCondition"} and $species->{"boundaryCondition"} eq "true"){
	    $sim->{"complexes"}{$species->{"name"}}{"Constant"} = 1;
	} else {
	    $sim->{"complexes"}{$species->{"name"}}{"Constant"} = 0;
	}

	if (defined $species->{"notes"}[0]{"body"}[0]{"p"}){
	    $sim->{"complexes"}{$species->{"name"}}{"Description"} = join " ", @{ $species->{"notes"}[0]{"body"}[0]{"p"} };
	}
    }

###############################################################################
#                         Parsing of listOfReactions
###############################################################################
#
# Maybe accept reactions which are not uni or bi-molecular and convert?
# 

    my $react_idx = 0;
    foreach my $react (@{ $struct->{"model"}[0]{"listOfReactions"}[0]{"reaction"} }){
	if (defined $react->{"notes"}[0]{"body"}[0]{"p"}){
	    $sim->{"reactions"}[$react_idx]{"Description"} = join " ", @{ $react->{"notes"}[0]{"body"}[0]{"p"} };
	}

	@{ $sim->{"reactions"}[$react_idx]{"Substrates"} } = map{ $_->{"species"} }@{ $react->{"listOfReactants"}[0]{"speciesReference"} };
	if (@{ $sim->{"reactions"}[$react_idx]{"Substrates"} } > 2){
	    warn "The reaction ", $react->{"name"}," contains more than two substrates.\n",
	    "StochSim accepts only uni- and bimolecular reactions.\n";
	}
	@{ $sim->{"reactions"}[$react_idx]{"Products"} } = map{ $_->{"species"} }@{ $react->{"listOfProducts"}[0]{"speciesReference"} };
	if (@{ $sim->{"reactions"}[$react_idx]{"Products"} } > 2){
	    warn "The reaction ", $react->{"name"}," contains more than two products.\n",
	    "StochSim accepts only uni- and bimolecular reactions (the reversible reaction should have no more than two reactants).\n";
	}

# ! Assume a standard  kf * subs * subst - kr * prod * prod.
# Parse the formula, for each parameter, look in the internal list, else look in the general list, 
# else generate a warning?
	
	if (exists $react->{"listOfProducts"}[0]{'kineticLaw'}[0]{'apply'}[0]{'minus'}){
	    # the equation is of the form v = X - Y
	    if (@{ $react->{"listOfProducts"}[0]{'kineticLaw'}[0]{'apply'}[0]{'apply'} } > 2){
		# the reaction is of the form v = X - Y +/- Z
		die "The equation of reaction ",$react->{'name'}," is not of the form v = kf*A*b [- kr*C*D]\n";
	    } else {
		foreach my $member (@{ $react->{"listOfProducts"}[0]{'kineticLaw'}[0]{'apply'}[0]{'apply'} }){
		    if (exists $member->{'times'}){
			# map the whole member of equation. CAUTION A+A->A => kf*A*A - kr*A
			# seek the value, localy, then globaly. If not found die
		    } else {
			if (@{ $member->{'ci'} } > 1){
			    die "One member of the equation of the reaction ",$react->{'name'}," is unusable by StochSim\n";
			} else {
			    # if the ci is a reactant -> kf = 1
			    # if the ci is a product -> kr = 1
			    # else die
			}
		    }
		}
	    }
	} else {
	    # the equation if of the form v = X or v = - Y; transform it into v = X - Y with one of the kinetic constants nul 
	}

#	my ($first,$second);
	# CAUTION: there are intermingled braces: the second part of the formula is kept into $3 and not $2
#	if ( $react->{"kineticLaw"}[0]{"formula"} =~ /([+-]?\s*\w+\s*(\*\s*\w+\s*)+)(([+-]?\s*\w+\s*(\*\s*\w+\s*)+)?)/ ){
#	    $first = $1 and $second = $3;
#	    my ($forward,$reverse);
#	    if ($first =~ /^\s*-/){
#		($reverse) = split /\s*\*\s*/, $first;	    
#		$reverse =~ s/\s*-\s*//;
#	    } else {
#		($forward) = split /\s*\*\s*/, $first;	    
#	    }
#	    if ($second =~ /^\s*-/){
#		($reverse) = split /\s*\*\s*/, $second;	
#		$reverse =~ s/\s*-\s*//;
#	    } else {
#		($forward) = split /\s*\*\s*/, $second;	    
#	    }

	    # analyse the general parameters, if any
#	    foreach my $param (@{ $struct->{"model"}[0]{"listOfParameters"}[0]{"parameter"} }){
#		if ($param->{"name"} =~ /^\s*$forward\s*$/i){
#		    $sim->{"reactions"}[$react_idx]{"Kf"} = $param->{"value"};
#		} elsif ($param->{"name"} =~ /^\s*$reverse\s*$/i){
#		    $sim->{"reactions"}[$react_idx]{"Kr"} = $param->{"value"};
#		}
#	    } 

	    # replace by the local prameters, if any
#	    foreach my $param (@{ $react->{"kineticLaw"}[0]{"listOfParameters"}[0]{"parameter"} }){
#		if ($param->{"name"} =~ /^\s*$forward\s*$/i){
#		    $sim->{"reactions"}[$react_idx]{"Kf"} = $param->{"value"};
#		} elsif ($param->{"name"} =~ /^\s*$reverse\s*$/i){
#		    $sim->{"reactions"}[$react_idx]{"Kr"} = $param->{"value"};
#		}
#	    }
#	} else {
#	    die "the kinetic law of reaction between ",join(" and ", @{ $sim->{"reactions"}[$react_idx]{"Substrates"} })," does not
#	    looklike a simple mass action law.";
#	}
 
	$react_idx++;
    }    

###############################################################################
#                         Parsing of listOfRules
###############################################################################

    # came from an idiotic ignorance of "boundaryCondition". Does not hurt anyway.

#    foreach my $speciesRule (@{ $struct->{"model"}[0]{"listOfRules"}[0]{"speciesConcentrationRule"} }){
#	if ($speciesRule->{"RuleTypeEnum"} eq 'rate' and $speciesRule->{"formula"} == 0){

	    # Caution. Complexes have to be defined first
#	    if (exists $sim->{"complexes"}{$speciesRule->{"species"}}){
#		$sim->{"complexes"}{$speciesRule->{"species"}}{"Constant"} = 1;
#	    } else {
#		print "a speciesConcentrationRule is defined for an undefined species.\n";
#	    }
#	}
#    }

}

1;
