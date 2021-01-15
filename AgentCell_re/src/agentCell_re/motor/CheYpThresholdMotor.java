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
 * Created on Jun 11, 2004
 *
 * 
 * 
 */
package agentCell_re.motor;

import agentCell_re.cells.ChemotacticCell;
import agentCell_re.molecules.Copynumber;


/**
 * @author emonet
 *
 *
 */
public class CheYpThresholdMotor extends ThresholdMotor {
    private Copynumber threshold;

    /**
     * Extends ThresholdMotor by defining isAboveThreshold = true
     * when the INSTANTANEOUS cell value of CheYp is above the threshold
     *
     * @param newCell
     * @param newState
     * @param newMinCWDuration
     * @param newMinCCWDuration
     * @param newThreshold
     */
    public CheYpThresholdMotor(ChemotacticCell newCell, int newState,
        double newMinCWDuration, double newMinCCWDuration,
        Copynumber newThreshold) {
        super(newCell, newState, newMinCWDuration, newMinCCWDuration);
        threshold = newThreshold;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.motor.ThresholdMotor#isAboveThreshold()
     */
    public boolean isAboveThreshold() {
        //check if yp is above threshold or not
        return this.getCell().getCheYp().getLevel() > threshold.getLevel();
    }

	@Override
	public double getCheYpAverage() {
		return 0.0;
	}

}
