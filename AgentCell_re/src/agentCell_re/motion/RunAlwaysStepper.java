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
 * Created on Feb 24, 2004
 *
 * 
 * 
 */
package agentCell_re.motion;

import agentCell_re.cells.Cell;
import agentCell_re.cells.ChemotacticCell;


/**
 * @author emonet
 *
 */
public class RunAlwaysStepper implements MotionStepper {
    private ChemotacticCell cell;
    private Motion run;

    /**
     * Create a run and tumble motionStepper that is stuck on RUN independently
     * of the state of the flagella.
     *
     * @param newCell
     */
    public RunAlwaysStepper(ChemotacticCell newCell) {
        cell = newCell;
    }

    /*
     * (non-Javadoc)
     *
     * @see edu.uchicago.agentcell.motion.MotionStepper#step(double)
     */
    public void step(double dt) {
        run.step(dt);
        cell.getWorld().getBoundaryConditions().apply(cell);
    }

    /*
     * (non-Javadoc)
     *
     * @see edu.uchicago.agentcell.motion.MotionStepper#getCell()
     */
    public Cell getCell() {
        return cell;
    }

    /*
     * (non-Javadoc)
     *
     * @see edu.uchicago.agentcell.motion.MotionStepper#setCell(edu.uchicago.agentcell.cells.ChemotacticCell)
     */
    public void setCell(Cell cell) {
        this.cell = (ChemotacticCell) cell;
    }

    /**
     * @return Returns the run.
     */
    public Motion getRun() {
        return run;
    }

    /**
     * @param run The run to set.
     */
    public void setRun(Motion run) {
        this.run = run;
    }
}
