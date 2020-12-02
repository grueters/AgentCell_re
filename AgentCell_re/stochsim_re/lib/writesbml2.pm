#!/usr/bin/perl -w
###############################################################################
#                          perl interface for stochsim                       
#
# File: writesbml2.pm
#
# Aim:  Writes the simulation under the SBML format level 2 
# (see http://www.sbml.org)
#
# Author: Nicolas Le Novère
#
# Date of last modification: 28/APR/2004
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
#    Nicolas LE NOVÈRE,  Computational Neurobiology, 
#    EMBL-EBI, Wellcome-Trust Genome Campus, Hinxton, Cambridge, CB10 1SD, UK
#    Tel: +44(0)1223 494 521,  Fax: +44(0)1223 494 468,  Mob: +33(0)689218676
#    http://www.ebi.ac.uk/~lenov

package writesbml2;
use strict;
use XML::Writer;
use use IO::File;
use Data::Dumper;
use vars qw($VERSION);

## set the version for version checking;
$VERSION = 0.010;

# This is a description of a reference to a hash describing a simulation

#$sim = {
#    "main_config_dir"      => "",  
# contains the address of the main directory containing the configuration files

#    "general_config"       => {}, 
# hash containing the substance of the file STCHSTC.INI

#    "components"           => {}, 
# hash of hashes containing the elementary blocks of the simulation. Each 
# entry is a hash possessing two keys 'Name' and 'Description'.
# { Symbol => { Name        => scalar,
#               Description => scalar } }

#    "complexes"            => {}, 
# Hash of hashes containing the objects of the reactions. Each entry is a hash
# possessing three mandatory keys 'InitLevel', Constant and 'Components'. 
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
#
#    "reactions"            => [], 
# array of hashes containing description of reactions in the system. Keys are 
# 'Substrates', 'Products', 'Kf', 'Kr', 'Description'. The two formers are 
# arrays
# { Description => scalar,
#   Kf          => scalar,
#   Kr          => scalar,
#   Substrates  => [],
#   Products    => [] }
#
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
# The keys are the 'Symbol' representing the complex. Each complex hash possess 
# the keys 'Description', 'Flags', 'InitLevel', 'Rapid_equilibriums', 'Display_variables', 
# 'Reactions'. 'Flags' is an array containing the name of the various flags.
# 'File' is the  address of the file containing the specification of this ns_complex. 
# 'Rapid_equilibriums' is an array of hashes which keys are 'Flag', 'CoupledStates' 
# and a set of bitstrings. 
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
#              EquilibrationInterval => scalar   
#              InitialState          => scalar,
#              InitialStateFile      => scalar,
#              InitialStateFromTime  => scalar
#              Snapshot_variables    => { Name => { States          => [],      states shown in the snapshots} 
#                                                   StoreInterval   => scalar,  
#                                                   AverageOutput   => scalar,
#                                                   AverageInterval => scalar,
#                                                   SampleInterval  => scalar } } } }
#};

sub writesbml2 {
    my ($sim) = @_;
    
# store a hash where keys are dynamic value codes and values are arrays of parameters affected by the dynamic value.
    my %dynvalues;

# store the timepoints. CAUTION the key is numerical, while the value is the name of the timepoint
    my %timepoints;

    my @dynvalue = keys %{ $sim->{"dynamic_values"} };
    my $instant = 0;
    if (scalar (keys %{ $sim->{"dynamic_values"} })){
	foreach my $timepoint (sort {$a <=> $b } keys %{ $sim->{"dynamic_values"}{$dynvalue[0]} }){
	    $timepoints{$timepoint} = "t$instant";
	    $instant++;	
	}
    }

# remove the timepoint 0. Use the default values for parameters 
    delete $timepoints{0};

    $sim->{"main_config_dir"} =~ m!/([^/]+)$! and my $dirname = $1;

    my $currenttime = scalar gmtime()." GMT";

    my $output = new IO::File(">$sim->{main_config_dir}/$dirname.xml");
    my $writer = new XML::Writer( 
				  OUTPUT      => $output,
				  DATA_MODE   => 'true',
				  DATA_INDENT => 4
				  );
    
    $writer->xmlDecl();
    $writer->startTag('sbml',
		      'xmlns'   => 'http://www.sbml.org/sbml/level2',
		      'level'   => 2,
		      'version' => 1
		      );
#
##########################################################################
#         composition of each complex is stored in annotations
# That way, we can export a simulation containing multicomponent complexes
# and retain their compositions. It is mandatory since the symbols of
# components can't overlap. The result looklikes:
# stsim:listcpnts="A:A;B:B;AB:A,B"
###########################################################################

    my @listcpnts;
    my $listcpnts;

    foreach (keys %{ $sim->{"complexes"} }){
	push @listcpnts ,"$_:".join(",", @{ $sim->{"complexes"}{$_}{"Components"} });
    }

    $writer->startTag('model',
		     'id'   => 'mod',
		     'name' => $dirname
		     ); # I cannot use $dirname cause there could be forbidden characters 
    $writer->startTag('annotation',
		      'xmlns:stsim'                  => 'http://www.sbml.org/2001/ns/stochsim',
		      );
    $writer->dataElement('stsim:disclaimer',"model definition generated on $currenttime by TkStochSim. (c) Nicolas Le Novere 2000-2003");
    $writer->dataElement('stsim:listcpnts', join(";",@listcpnts));
    $writer->dataElement('stsim:StoreInterval', $sim->{'general_config'}{'StoreInterval'});
    $writer->dataElement('stsim:TimeIncrement', $sim->{'general_config'}{'TimeIncrement'});
    $writer->dataElement('stsim:SimulationDuration', $sim->{'general_config'}{'SimulationDuration'});
    $writer->dataElement('stsim:MaximumNumberComplexes' => $sim->{'general_config'}{'MaximumNumberComplexes'});
    $writer->endTag; # end annotation

######## Put a description of the model here.

# StochSim uses only unimolecular and bimolecular reaction. We have only two units to define for the whole SBML file

    $writer->startTag('listOfUnitDefinitions');
    $writer->startTag('unitDefinition',
		      'id' => 'unimolRate'
		      );
    $writer->startTag('listOfUnits');
    $writer->emptyTag('unit',
		      'kind'     => 'second',
		      'exponent' => '-1'
 		      );
    $writer->endTag(); # end listOfunits
    $writer->endTag(); # end unitDefinition
    $writer->startTag('unitDefinition',
		      'id' => 'bimolRate'
		      );
    $writer->startTag('listOfUnits');
    $writer->emptyTag('unit',
		      'kind'     => 'mole',
		      'exponent' => '-1'
 		      );
    $writer->emptyTag('unit',
		      'kind'     => 'litre',
		      'exponent' => '-1'
 		      );
    $writer->emptyTag('unit',
		      'kind'     => 'second',
		      'exponent' => '-1'
 		      );
    $writer->endTag(); # end listOfunits
    $writer->endTag(); # end unitDefinition
    $writer->endTag(); # end listOfUnitDefinitions

    $writer->startTag('listOfCompartments');
    $writer->startTag('compartment',
		     'id'                => 'comp1',
		     'name'              => 'compartment1',
		     'spatialDimensions' => '3',
		     'size'              => $sim->{'general_config'}{'ReactionVolume'},
		     'units'             => 'litre'
		     );

    $writer->endTag(); # end compartment
    $writer->endTag(); # end listOfCompartments

    $writer->startTag('listOfSpecies');

###########################################################################
#                     Generation of listOfSpecies
###########################################################################
    
# scrolls complexes. Then if a complex is a multistate, 
# we'll take care of the relevant states

    my $cplx_idx = 0;
    for my $cplx (keys %{ $sim->{"complexes"} }){

	if (defined($sim->{"complexes"}{$cplx}{"Description"}) 
	    and ($sim->{"complexes"}{$cplx}{"Description"} =~ /\w/)){

	    if ($sim->{"complexes"}{$cplx}{"Constant"}){
		
		$writer->startTag('species',
				  'id'                    => 'Cplx'.$cplx_idx,
				  'name'                  => $cplx,
				  'compartment'           => 'comp1',	    
				  'initialAmount'         => $sim->{'complexes'}{$cplx}{'InitLevel'}/6.022e23,
				  'substanceUnits'        => "mole",
				  'hasOnlySubstanceUnits' => 'true',
				  'constant'              => 'true',
				  'boundaryCondition'     => 'true'
				  );

	    } else {
	    
		$writer->startTag('species',
				  'id'                    => 'Cplx'.$cplx_idx,
				  'name'                  => $cplx,
				  'compartment'           => 'comp1',	    
				  'initialAmount'         => $sim->{'complexes'}{$cplx}{'InitLevel'}/6.022e23,
				  'substanceUnits'        => "mole",
				  'hasOnlySubstanceUnits' => 'true'
				  );

	    }

	    $writer->startTag('notes');
	    $writer->startTag('body',
			      "xmlns" => 'http://www.w3.org/1999/xhtml'
			      );
	    $writer->dataElement('p', $sim->{'complexes'}{$cplx}{'Description'} );

	    $writer->endTag(); # end body
	    $writer->endTag(); # end notes
	    $writer->endTag(); # end species
	    
	} else {	

	    if ($sim->{'complexes'}{$cplx}{'Constant'}){

		$writer->emptyTag('species',
				  'id'                    => 'Cplx'.$cplx_idx,
				  'name'                  => $cplx,
				  'compartment'           => 'comp1',	    
				  'initialAmount'         => $sim->{'complexes'}{$cplx}{'InitLevel'}/6.022e23,
				  'substanceUnits'        => "mole",
				  'hasOnlySubstanceUnits' => 'true',
				  'constant'              => 'true',
				  'boundaryCondition'     => 'true'
				  );

	    } else {

		$writer->emptyTag('species',
				  'id'                    => 'Cplx'.$cplx_idx,
				  'name'                  => $cplx,
				  'compartment'           => 'comp1',	    
				  'initialAmount'         => $sim->{'complexes'}{$cplx}{'InitLevel'}/6.022e23,
				  'substanceUnits'        => "mole",
				  'hasOnlySubstanceUnits' => 'true'
				  );		

	    }
	    
	}

	$cplx_idx++;
    }
    
    $writer->endTag(); # end listOfSpecies


###########################################################################
#              Generation of the general listOfParameters
###########################################################################

    $writer->startTag('listOfParameters');
    
    for my $react_idx (0..$#{ $sim->{"reactions"} }){
	
	# we will always specify the 'constant' attribute event if it is true by default
	
	my $constant;
	my $unit;
	my $kf;
	if ($sim->{"reactions"}[$react_idx]{"Kf"} =~ /@(\S+)/){
	    $constant = 'false';
	    ($kf = $sim->{"reactions"}[$react_idx]{"Kf"}) =~ s/@(\S+)//;
# record that kf of the current reaction is controlled by a dynamic value
# useful for event assignments
	    push(@{ $dynvalues{$1} },"kf_$react_idx"); 
	} else {
	    $constant = 'true';
	    $kf = $sim->{"reactions"}[$react_idx]{"Kf"};
	}
	if (scalar @{ $sim->{"reactions"}[$react_idx]{"Substrates"} } > 1){
	    $unit = 'bimolRate';
	} else {
	    $unit = 'unimolRate';
	}
	$writer->emptyTag('parameter',
			  'id'       => "kf_$react_idx",  
			  'value'    => $kf,
			  'units'    => $unit,
			  'constant' => $constant,
			  );
	my $kr;
	if ($sim->{"reactions"}[$react_idx]{"Kr"} =~ /@(\S+)/){
	    $constant = 'false';
	    ($kr = $sim->{"reactions"}[$react_idx]{"Kr"}) =~ s/@(\S+)//;
# record that kr of the current reaction is controlled by a dynamic value
# useful for event assignments
	    push(@{ $dynvalues{$1} },"kr_$react_idx");
	} else {
	    $constant = 'true';
	    $kr = $sim->{"reactions"}[$react_idx]{"Kr"};
	}
	if (scalar @{ $sim->{"reactions"}[$react_idx]{"Products"} } > 1){
	    $unit = 'bimolRate';
	} else {
	    $unit = 'unimolRate';
	}
	$writer->emptyTag('parameter',
			  "id"       => "kr_$react_idx",  
			  "value"    => $kr,
			  'units'    => $unit,
			  'constant' => $constant
			  );
    }
    
    foreach (sort {$a <=> $b } keys %timepoints){
	$writer->emptyTag('parameter',
			  "id"       => $timepoints{$_},  
			  "value"    => $_,
			  'units'    => 'second',
			  'constant' => 'true'
			  );
    }
    $writer->endTag(); # end listOfParameters

###########################################################################
#                     Generation of listOfReactions
###########################################################################
    
    
    $writer->startTag('listOfReactions');
    
    for my $react_idx (0..$#{ $sim->{"reactions"} }){
	
	$writer->startTag('reaction',
			  'id'   => 'React'.$react_idx,
			  'name' => 'React'.$react_idx,
			  );
	
	if (defined $sim->{"reactions"}[$react_idx]{"Description"} 
	    and $sim->{"reactions"}[$react_idx]{"Description"} =~ /\w/){
	    
	    $writer->startTag('notes');
	    $writer->startTag('body',
			      "xmlns" => 'http://www.w3.org/1999/xhtml'
			      );
	    $writer->dataElement('p', $sim->{reactions}[$react_idx]{Description} );
	    $writer->endTag(); # end body
	    $writer->endTag(); # end notes
	}
	
	
	$writer->startTag('listOfReactants');
	foreach my $substrate (@{ $sim->{"reactions"}[$react_idx]{"Substrates"} }){
	    $writer->emptyTag('speciesReference',
			      'species' => $substrate
			      );
	}
	$writer->endTag(); # end listOfReactants
	$writer->startTag('listOfProducts');
	foreach my $product (@{ $sim->{"reactions"}[$react_idx]{"Products"} }){
	    $writer->emptyTag('speciesReference',
			      'species' => $product
			      );
	}
	$writer->endTag(); # end listOfProducts
	$writer->startTag('kineticLaw');
	$writer->startTag('notes');
	$writer->startTag('body',
			  "xmlns" => 'http://www.w3.org/1999/xhtml'
			  );
	my $formula = "(kf_$react_idx * ".
	    join(" * ", @{ $sim->{"reactions"}[$react_idx]{"Substrates"} }).
		" - kr_$react_idx * ".
		    join(" * ", @{ $sim->{"reactions"}[$react_idx]{"Products"} }).") * comp1";
	
	$writer->dataElement('p', $formula );
	$writer->endTag(); # end body
	$writer->endTag(); # end notes
	$writer->startTag('math',
			  "xmlns" => 'http://www.w3.org/1998/Math/MathML'
			  );
	$writer->startTag('apply');
	$writer->emptyTag('minus');
	$writer->startTag('apply');
	$writer->emptyTag('times');
	$writer->dataElement('ci',"kf_$react_idx");
	foreach my $substrate2 (@{ $sim->{"reactions"}[$react_idx]{"Substrates"} }){
	    $writer->dataElement('ci',$substrate2);
	}
	$writer->endTag(); # end apply
	$writer->startTag('apply');
	$writer->emptyTag('times');
	$writer->dataElement('ci',"kr_$react_idx");
	foreach my $product2 (@{ $sim->{"reactions"}[$react_idx]{"Products"} }){
	    $writer->dataElement('ci',$product2);
	}
	$writer->endTag(); # end apply
	$writer->endTag(); # end apply
	$writer->endTag(); # end math
	$writer->endTag(); # end kineticLaw
	$writer->endTag(); # end reaction
	
    }
    
    $writer->endTag(); # end listOfReactions

###########################################################################
#                       Generation of listOfEvents
###########################################################################

    # there exist at least one dynamic value
    if ( scalar (keys %{ $sim->{"dynamic_values"} }) ){
	$writer->startTag('listOfEvents'); 
	foreach my $timepoint (sort {$a <=> $b} keys %timepoints){
	    $writer->startTag('event'); 
	    $writer->startTag('trigger');
	    $writer->startTag('math',
			      "xmlns" => 'http://www.w3.org/1998/Math/MathML'
			      );
	    $writer->startTag('apply');
	    $writer->emptyTag('gt');
	    $writer->startTag('csymbol',
			      'encoding'      => 'text',
			      'definitionURL' => "http://www.sbml.org/sbml/symbols/time"
			      );
	    $writer->characters('time');
	    $writer->endTag(); # end csymbol
	    $writer->dataElement('ci',$timepoints{$timepoint});
	    $writer->endTag(); # end apply
	    $writer->endTag(); # end math
	    $writer->endTag(); # end trigger
	    $writer->startTag('listOfEventAssignments'); 
	    foreach my $dyncode (keys %{ $sim->{"dynamic_values"} }){
		foreach (@{ $dynvalues{$dyncode} }){
		    $writer->startTag('eventAssignment',
				      'variable' => $_);
		    $writer->startTag('math',
				      "xmlns" => 'http://www.w3.org/1998/Math/MathML'
				      );
		    $writer->dataElement('cn',$sim->{"dynamic_values"}{$dyncode}{$timepoint});
		    $writer->endTag; # end math
		    $writer->endTag; # end eventAssignment
		} 
	    }
	    $writer->endTag(); # end listOfEventAssignments
	    $writer->endTag(); # end event
	}
	$writer->endTag(); # end listOfEvents
	
    }
    
    $writer->endTag(); # end model
    $writer->endTag(); # end sbml
    $writer->end();
    $output->close();
}
1;

