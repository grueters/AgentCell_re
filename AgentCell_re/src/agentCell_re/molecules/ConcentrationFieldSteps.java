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
 * Created on Jul 21, 2004
 *
 * 
 * 
 */
package agentCell_re.molecules;

import agentCell_re.math.Vect;


/**
 * @author emonet
 *
 * 
 * 
 */
public class ConcentrationFieldSteps extends ConcentrationField {
    private Vect start1;
    private double level1;
    private Vect start2;
    private double level2;
    private Vect start3;
    private double level3;
    private Vect start4;
    private double level4;
    private Vect start5;
    private double level5;

    /**
     * @param newMolecularType
     * @param start1
     * @param level1
     * @param start2
     * @param level2
     * @param start3
     * @param level3
     * @param start4
     * @param level4
     * @param start5
     * @param level5
     */
    public ConcentrationFieldSteps(String newMolecularType, Vect start1,
        double level1, Vect start2, double level2, Vect start3, double level3,
        Vect start4, double level4, Vect start5, double level5) {
        this.setMolecularType(newMolecularType);
        this.start1 = start1;
        this.level1 = level1;
        this.start2 = start2;
        this.level2 = level2;
        this.start3 = start3;
        this.level3 = level3;
        this.start4 = start4;
        this.level4 = level4;
        this.start5 = start5;
        this.level5 = level5;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.molecules.ConcentrationField#getConcentrationLevelAt(edu.uchicago.agentcell.math.Vect)
     */
    public double getConcentrationLevelAt(Vect position) {
        if (position.getElement(2) < start1.getElement(2)) {
            return 0.0;
        } else if ((position.getElement(2) >= start1.getElement(2)) &&
                (position.getElement(2) < start2.getElement(2))) {
            return level1;
        } else if ((position.getElement(2) >= start2.getElement(2)) &&
                (position.getElement(2) < start3.getElement(2))) {
            return level2;
        } else if ((position.getElement(2) >= start3.getElement(2)) &&
                (position.getElement(2) < start4.getElement(2))) {
            return level3;
        } else if ((position.getElement(2) >= start4.getElement(2)) &&
                (position.getElement(2) < start5.getElement(2))) {
            return level4;
        } else {
            return level5;
        }
    }
}
