/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*

AgentCell is an agent-based simulation of bacterial chemotaxis.
Copyright (c) 2005 Trustees of the University of Chicago

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Authors: Thierry Emonet (emonet@uchicago.edu) and Michael J. North (north@anl.gov)

	Thierry Emonet
	920 E. 58th Street
	University of Chicago
	Chicago, IL 60637

	Michael J. North
	9700 S. Cass Avenue
	Argonne, IL 60439

*/
/*
 * Created on Jun 8, 2004
 *
 * 
 * 
 */
package agentCell_re.molecules;


/**
 * @author emonet
 *
 * Base clase for molecular objects. Defines methods to get and set the molecular type.
 */
public class Molecule {
    public static final double AVOGADRO = 6.022142E+23;
    public static final String UNDEFINED = "Undefined";

    /**
     * Comment for <code>ASPARTATE</code>
     * String that contains the name used by stochsim for 
     * the occupancy number [asp]/([asp] + Kd), 
     * where Kd is the receptor-ligand disssoc. const. for 
     * receptors in the INACTIVE conformation. 
     */
    public static final String ASPARTATE = "asp";

    /**
     * Comment for <code>ACTIVE_RECEPTORS_OCCUPANCY</code>
     * String that contains the name used by stochsim for 
     * the occupancy number [asp]/([asp] + Kd*), 
     * where Kd* is the receptor-ligand disssoc. const. for 
     * receptors in the ACTIVE conformation. 
     */
    public static final String ACTIVE_RECEPTORS_OCCUPANCY = "asp*";
    public static final String CHEYP = "Yp";
    public static final String TAR = "Tar";
    private String molecularType = UNDEFINED;

    public String getMolecularType() {
        return molecularType;
    }

    public void setMolecularType(String newMolecularType) {
        molecularType = newMolecularType;
    }
}
