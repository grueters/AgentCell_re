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
 * Created on Jul 30, 2004
 *
 * 
 * 
 */
package agentCell_re.motion;

import agentCell_re.cells.Cell;
import agentCell_re.cells.ChemotacticCell;
import agentCell_re.flagella.Flagella;


/**
 * @author emonet
 *
 * 
 * 
 */
public class RunTumbleStepper implements MotionStepper {
    private ChemotacticCell cell;
    private Motion run;
    private Motion tumble;
    private boolean previousStateWasAPART;

    /**
     * Create a run and tumble motionStepper that exactly reflects the
     * the states of the flagella.
     * @param newCell
     */
    public RunTumbleStepper(ChemotacticCell newCell) {
        cell = newCell;

        //if the first state is TUMBLE this will make sure that we tumble. If
        // not it has no effects.
        previousStateWasAPART = true;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.motion.MotionStepper#step(double)
     */
    public void step(double dt) {
        if (this.cell.getFlagella().getState() == Flagella.APART) {
            // we only call the tumble step on the first step of a series of CW.
            if (previousStateWasAPART) {
        	// if (true) {
                tumble.step(dt); //TODO: Change the tumble step to change the Repast-Model to a Tumble-Model
                previousStateWasAPART = false;
            }
        } else {
            run.step(dt); //TODO: Change the run step to change the Repast-Model to a Run-Model
            cell.getWorld().getBoundaryConditions().apply(cell);
            previousStateWasAPART = true;
        }
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.motion.MotionStepper#getCell()
     */
    public Cell getCell() {
        return cell;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.motion.MotionStepper#setCell(edu.uchicago.agentcell.cell.ChemotacticCell)
     */
    public void setCell(Cell newCell) {
        cell = (ChemotacticCell) newCell;
    }

    /**
     * @return
     */
    public Motion getRun() {
        return run;
    }

    /**
     * @return
     */
    public Motion getTumble() {
        return tumble;
    }

    /**
     * @param motion
     */
    public void setRun(Motion motion) {
        run = motion;
    }

    /**
     * @param motion
     */
    public void setTumble(Motion motion) {
        tumble = motion;
    }
}
