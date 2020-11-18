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
 * 
 */
public class AveragedCheYpThresholdMotor extends ThresholdMotor {
    private Copynumber threshold;
    private long boxcar[];

    /**
     * Extends ThresholdMotor by defining isAboveThreshold = true
     * when a BOXCAR AVERAGE of CheYp is above the threshold.
     * Note that the step method in ThresholdMotor must be called at regular
     * time intervals for this average to make sense!!!
     *
     * @param newCell
     * @param newState
     * @param newMinCWDuration
     * @param newMinCCWDuration
     * @param newThreshold
     * @param boxcarWidth
     */
    public AveragedCheYpThresholdMotor(ChemotacticCell newCell, int newState,
        double newMinCWDuration, double newMinCCWDuration,
        Copynumber newThreshold, int boxcarWidth) {
        super(newCell, newState, newMinCWDuration, newMinCCWDuration);
        threshold = newThreshold;

        if (boxcarWidth < 1) {
            System.err.println(
                "Error in creating the object. The argument boxcarWidth must be >= 1");
            System.err.flush();
            System.exit(1);
        }

        boxcar = new long[boxcarWidth];

        //set all the elements equal to the initial level of CheYp
        for (int i = 0; i < boxcar.length; i++) {
            boxcar[i] = this.getCell().getCheYp().getLevel();
        }
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.motor.ThresholdMotor#isAboveThreshold()
     * Calculate if threshold is crossed using a sliding average of CheYp
     * instead of the instantaneous value
     */
    public boolean isAboveThreshold() {
        //Shift all elements of boxcar right by one
        System.arraycopy(boxcar, 0, boxcar, 1, boxcar.length - 1);

        //fill in first element with the new value
        boxcar[0] = this.getCell().getCheYp().getLevel();

        //calculate average
        double ypAverage = 0;

        for (int i = 0; i < boxcar.length; i++)
            ypAverage += boxcar[i];

        ypAverage = ypAverage / boxcar.length;

        //System.out.println(boxcar[0]+" "+ypAverage);
        //check if average is above threshold or not
        return ypAverage > threshold.getLevel();
    }
}
