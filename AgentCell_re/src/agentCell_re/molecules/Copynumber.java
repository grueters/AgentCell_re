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
 * Created on Jun 9, 2004
 *
 * 
 * 
 */
package agentCell_re.molecules;


/**
 * @author emonet
 *
 *  Copynumbers store the MolecularType from Molecular and the level
 */
public class Copynumber extends Molecule {
    private long level = 0;

    /**
     *
     */
    public Copynumber(String newMolecularType) {
        this(newMolecularType, 0);
    }

    public Copynumber(String newMolecularType, long newLevel) {
        this.setMolecularType(newMolecularType);
        level = newLevel;
    }

    /**
     * Given a volume, returns the concentration in Mol
     * @param volume (in liters)
     * @return [C] = copynumber / (vol * avogadro)
     */
    public Concentration getConcentration(double volume) {
        return new Concentration(this.getMolecularType(),
            level / (volume * Molecule.AVOGADRO));
    }

    /**
     * @return
     */
    public long getLevel() {
        return level;
    }

    /**
     * @param l
     */
    public void setLevel(long l) {
        level = l;
    }
}
