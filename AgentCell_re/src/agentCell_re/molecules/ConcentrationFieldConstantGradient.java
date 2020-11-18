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

import agentCell_re.math.Vect;


/**
 * @author emonet
 *
 * Constant concentration gradient:
 *       concentration = gradient . position + levelAtOrigin
 * where gradient is a vector and levelAtOrigin is a scalar
 */
public class ConcentrationFieldConstantGradient extends ConcentrationField {
    private Vect origin;
    private double levelAtOrigin;
    private Vect gradient;

    /**
     *
     */
    public ConcentrationFieldConstantGradient(String newMolecularType,
        Vect newOrigin, double newLevelAtOrigin, Vect newGradient) {
        this.setMolecularType(newMolecularType);
        this.origin = newOrigin;
        this.levelAtOrigin = newLevelAtOrigin;
        this.gradient = newGradient;
    }

    /**
     * @return
     */
    public double getLevelAtOrigin() {
        return levelAtOrigin;
    }

    /**
     * @return
     */
    public Vect getGradient() {
        return gradient;
    }

    /**
     * @param d
     */
    public void setLevelAtOrigin(double d) {
        levelAtOrigin = d;
    }

    /**
     * @param vect
     */
    public void setGradient(Vect vect) {
        gradient = vect;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.molecules.ConcentrationField#getConcentrationValueAt(edu.uchicago.agentcell.math.Vect)
     * Returns the concentration. Return zero in case the the concentration becomes negative (e.g. if gradient is y = a * (x-x0) and x <x0)
     */
    public double getConcentrationLevelAt(Vect position) {
        double value = gradient.mult(position) - gradient.mult(origin) +
            levelAtOrigin;

        if (value <= 0) {
            return 0;
        } else {
            return value;
        }
    }
}
